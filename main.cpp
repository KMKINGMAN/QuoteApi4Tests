#include "crow_all.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <nlohmann/json.hpp>
#include <exception>

using namespace std;

bool isExist(const char* filename) {
    ifstream file(filename);
    return file.good();
}

int main() {
    sqlite3* db;
    const char* DBFile = "KMCodersQM.db";

    if (!isExist(DBFile)) {
        cout << "Database file does not exist. Creating a new one..." << endl;
        int rc = sqlite3_open(DBFile, &db);

        if (rc) {
            cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
            return 1;
        }

        const char* createTableSQL = "CREATE TABLE IF NOT EXISTS posts ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "content TEXT,"
                                     "author TEXT,"
                                     "date DATETIME"
                                     ");";

        rc = sqlite3_exec(db, createTableSQL, 0, 0, 0);

        if (rc != SQLITE_OK) {
            cerr << "Error creating table: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return 1;
        }

        cout << "Database created successfully." << endl;
    } else {
        cout << "Database file already exists." << endl;

        int rc = sqlite3_open(DBFile, &db);

        if (rc) {
            cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
            return 1;
        }
    }

    crow::SimpleApp app;

    nlohmann::json posts = nlohmann::json::array();

    CROW_ROUTE(app, "/")([&db]() {
        return "<h1>KMCoders Quote Manager API</h1>";
    });

    CROW_ROUTE(app, "/all").methods("GET"_method)
        ([&db, &posts](const crow::request& req, crow::response& res) {
            sqlite3_stmt* stmt;
            const char* selectSQL = "SELECT id, content, author, date FROM posts;";
            int rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, 0);

            if (rc != SQLITE_OK) {
                cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
                res.code = 500;
                res.write("Internal Server Error");
                res.end();
                return;
            }

            posts.clear();

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const char* content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                const char* author = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                nlohmann::json post;
                post["id"] = id;
                post["content"] = content;
                post["author"] = author;
                post["date"] = date;

                posts.push_back(post);
            }

            sqlite3_finalize(stmt);

            res.set_header("Content-Type", "application/json");
            res.write(posts.dump());
            res.end();
        });

    CROW_ROUTE(app, "/add").methods("POST"_method)
        ([&db](const crow::request& req) {
            try {
                nlohmann::json requestData = nlohmann::json::parse(req.body); 
                string content = requestData["content"];
                string author = requestData["author"];
                time_t now = time(0);
                char dateBuffer[100];
                strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
                string date = dateBuffer;
                sqlite3_stmt* stmt;
                const char* insertSQL = "INSERT INTO posts (content, author, date) VALUES (?, ?, ?);";
                int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, 0);

                if (rc != SQLITE_OK) {
                    cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
                    return crow::response(500, "Internal Server Error");
                }

                sqlite3_bind_text(stmt, 1, content.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, author.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);

                rc = sqlite3_step(stmt);
                sqlite3_finalize(stmt);

                if (rc != SQLITE_DONE) {
                    cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
                    return crow::response(500, "Internal Server Error");
                }
                return crow::response(201);
            } catch (const exception& e) {
                cerr << "Exception: " << e.what() << endl;
                return crow::response(400, "Bad Request");
            }
        });

    app.port(18080).run();

    return 0;
}

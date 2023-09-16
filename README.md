# KMCoders Quote Manager

This is a simple web API for managing quotes. It uses the Crow C++ web framework and SQLite for data storage. The API allows you to add quotes and retrieve a list of all quotes.

## Prerequisites

Before running this application, make sure you have the following dependencies installed on your system:

- CMake
- Clang++ (C++ compiler)
- SQLite3
- Crow C++ Web Framework
- nlohmann/json Library

You can install these dependencies using your package manager or build tools like Nix.

## Building and Running

To build and run the application, follow these steps:

1. Clone this repository to your local machine.

2. Navigate to the project directory.

3. Create a build directory and navigate to it:

   ```bash
   mkdir build
   cd build
   ```

4. Run CMake to generate the build files:

   ```bash
   cmake ..
   ```

5. Build the application:

   ```bash
   make
   ```

6. Run the application:

   ```bash
   ./main
   ```

The API will be available at `http://localhost:18080`.

## API Endpoints

### GET /

Returns a simple message indicating the API's presence.

### GET /all

Retrieves a list of all quotes in JSON format.

### POST /add

Adds a new quote. You must provide the quote content and author in the request body as JSON.

Example request body:

```json
{
  "content": "This is a sample quote.",
  "author": "Muhammad Kurkar"
}
```

## Database

The application uses SQLite as its database. The database file is named "KMCodersQM.db" and will be created in the project directory if it doesn't already exist. The database schema includes a "posts" table for storing quotes.

## License

This project is licensed under the BSL License

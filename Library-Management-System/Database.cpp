#include "database.h"
#include <iostream>

using namespace std;

// Constructor: Initializes the database pointer to null.
Database::Database() {
    db = nullptr;
}

// Destructor: Ensures the database connection is closed when the object is destroyed.
Database::~Database() {
    close();
}

/**
 * Opens a connection to 'library.db'.
 * If the file doesn't exist, SQLite will create it.
 * Also ensures the 'books' table exists with the required schema.
 */
bool Database::open() {
    // Attempt to open/create the database file
    if (sqlite3_open("library.db", &db) != SQLITE_OK) {
        std::cout << "Cannot open database\n";
        return false;
    }

    // SQL statement to initialize the table structure if it's a new database
    std::string sql =
        "CREATE TABLE IF NOT EXISTS books ("
        "Id INTEGER PRIMARY KEY,"    // Unique Identifier
        "Title TEXT,"                // Book Title
        "Author TEXT,"               // Author Name
        "Year INTEGER,"              // Publication Year
        "isBorrowed INTEGER,"        // Boolean flag (0 or 1)
        "Borrower TEXT"              // Name of the person who borrowed the book
        ");";

    return execute(sql);
}

/**
 * Executes a standard SQL query (INSERT, UPDATE, DELETE, CREATE).
 * This wrapper handles error reporting and memory management for error messages.
 */
bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;

    // Execute the SQL string
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cout << "SQL error: " << errMsg << std::endl;

        // sqlite3_exec allocates memory for errMsg if an error occurs; it must be freed.
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Closes the SQLite connection and releases the handle.
void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

// Returns the raw SQLite handle for advanced operations (like prepared statements).
sqlite3* Database::getDbHandle() {
    return db;
}
#pragma once
#include <string>
#include "sqlite3.h"

/**
 * @class Database
 * @brief A wrapper class for SQLite3 database operations.
 * * This class manages the connection lifecycle (opening/closing) and
 * provides a simple interface to execute SQL commands.
 */
class Database {
private:
    sqlite3* db; // Pointer to the SQLite database structure

public:
    /**
     * @brief Constructor that initializes the database pointer.
     */
    Database();

    /**
     * @brief Destructor that ensures the database connection is closed.
     */
    ~Database();

    /**
     * @brief Opens the connection to the library database file.
     * @return true if connection is successful, false otherwise.
     */
    bool open();

    /**
     * @brief Closes the active SQLite database connection.
     */
    void close();

    /**
     * @brief Executes a non-query SQL statement (INSERT, UPDATE, DELETE).
     * @param sql The SQL command string to execute.
     * @return true if execution is successful, false if an error occurs.
     */
    bool execute(const std::string& sql);

    /**
     * @brief Returns the raw handle for the SQLite connection.
     * Useful for prepared statements and data fetching in other classes.
     */
    sqlite3* getDbHandle();
};
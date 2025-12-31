#include "database.h"
#include <iostream>

using namespace std;

Database::Database() {
    db = nullptr;
}

Database::~Database() {
    close();
}

bool Database::open() {
    if (sqlite3_open("library.db", &db) != SQLITE_OK) {
        std::cout << "Cannot open database\n";
        return false;
    }

    std::string sql =
        "CREATE TABLE IF NOT EXISTS books ("
        "Id INTEGER PRIMARY KEY,"
        "Title TEXT,"
        "Author TEXT,"
        "Year INTEGER,"
        "isBorrowed INTEGER,"
        "Borrower TEXT"
        ");";

    return execute(sql);
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cout << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

sqlite3* Database::getDbHandle() {
    return db;
}

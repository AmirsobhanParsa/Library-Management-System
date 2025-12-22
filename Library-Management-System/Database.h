#pragma once

#include <string>
#include "sqlite3.h"

class Database {
private:
    sqlite3* db;

public:
    Database();
    ~Database();
    bool open();
    void close();
    bool execute(const std::string& sql);
};


#pragma once

#include <string>
#include "book.h"
#include "database.h"

class BookManager {
private:
    Database db;

public:
    BookManager();
    bool addBook(const Book& book);
    bool removeBook(int bookId);
    bool borrowBook(int bookId, const std::string& borrower);
    bool returnBook(int bookId);
    void listAllBooks();
};

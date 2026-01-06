#pragma once
#include <string>
#include "book.h"
#include "database.h"
#include <vector>
#include <algorithm>

class BookManager {
private:
    Database db;

public:
    BookManager();
    bool addBook(const Book& book);
    bool removeBook(int bookId);
    bool borrowBook(int bookId, const std::string& borrower);
    bool returnBook(int bookId);
    vector<Book> getAllBooks();
    void printBooks(const vector<Book>& books);
    void sortBooksByYear(vector<Book>& books);
    void sortBooksById(vector<Book>& books);
    vector<Book> searchBooksById(const vector<Book>& books, int searchId);
    int generateUniqueId();
    bool idExists(int id);
};

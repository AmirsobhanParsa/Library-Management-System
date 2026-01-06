#include "BookManager.h"
#include <iostream>
#include <iomanip>
#include "sqlite3.h"
#include <random>
#include <ctime>

using namespace std;

BookManager::BookManager() {
    if (!db.open()) {
        cout << "Database connection failed\n";
    }
}

bool BookManager::addBook(const Book& BookItem) {
    int finalId = generateUniqueId();
    string sql =
        "INSERT INTO books (Id, Title, Author, Year, isBorrowed, Borrower) VALUES (" +
        to_string(finalId) + ", '" +
        BookItem.Title + "', '" +
        BookItem.Author + "', " +
        to_string(BookItem.Year) + ", 0, NULL);";

    if (db.execute(sql)) {
        cout << "Book added successfully with ID: " << finalId << endl;
        return true;
    }
    return false;
}

bool BookManager::removeBook(int BookId) {
    string sql =
        "DELETE FROM books WHERE Id = " + to_string(BookId) + ";";

    return db.execute(sql);
}

vector<Book> BookManager::getAllBooks() {
    vector<Book> booksList;
    sqlite3_stmt* stmt;
    string query = "SELECT Id, Title, Author, Year, isBorrowed, Borrower FROM books";

    sqlite3* rawDB = db.getDbHandle();

    if (sqlite3_prepare_v2(rawDB, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error fetching books: " << sqlite3_errmsg(rawDB) << endl;
        return booksList;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Book b;
        b.Id = sqlite3_column_int(stmt, 0);

        const unsigned char* t = sqlite3_column_text(stmt, 1);
        b.Title = t ? string(reinterpret_cast<const char*>(t)) : "";

        const unsigned char* a = sqlite3_column_text(stmt, 2);
        b.Author = a ? string(reinterpret_cast<const char*>(a)) : "";

        b.Year = sqlite3_column_int(stmt, 3);
        b.isBorrowed = sqlite3_column_int(stmt, 4);

        const unsigned char* borrower = sqlite3_column_text(stmt, 5);
        b.Borrower = borrower ? string(reinterpret_cast<const char*>(borrower)) : "";

        booksList.push_back(b);
    }

    sqlite3_finalize(stmt);
    return booksList;
}

void BookManager::printBooks(const vector<Book>& books) {
    cout << "-------------------------------------------------\n";
    cout << left << setw(5) << "ID" << setw(20) << "Title" << setw(15) << "Author" << setw(6) << "Year" << endl;
    cout << "-------------------------------------------------\n";
    for (const auto& b : books) {
        cout << left << setw(5) << b.Id
            << setw(20) << b.Title
            << setw(15) << b.Author
            << setw(6) << b.Year << endl;
    }
    cout << "-------------------------------------------------\n";
}

void BookManager::sortBooksByYear(vector<Book>& books) {
    sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.Year < b.Year;
        });
}

void BookManager::sortBooksById(vector<Book>& books) {
    sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.Id < b.Id;
        });
}

vector<Book> BookManager::searchBooksById(const vector<Book>& books, int searchId) {
    vector<Book> results;

    for (const auto& item : books) {
        if (item.Id == searchId) {
            results.push_back(item);
            break;
        }
    }

    return results;
}

bool BookManager::idExists(int id) {
    sqlite3_stmt* stmt;
    string query = "SELECT COUNT(*) FROM books WHERE Id = " + to_string(id);
    int count = 0;

    if (sqlite3_prepare_v2(db.getDbHandle(), query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count > 0;
}

int BookManager::generateUniqueId() {
    static mt19937 gen(static_cast<unsigned int>(time(0)));
    uniform_int_distribution<> dis(10000, 99999);

    int newId;
    bool exists = true;

    while (exists) {
        newId = dis(gen);
        exists = idExists(newId);
    }

    return newId;
}
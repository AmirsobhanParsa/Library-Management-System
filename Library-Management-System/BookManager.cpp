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
        Action act;
        act.type = ADD;
        act.targetId = finalId; // آیدی که جنریت شده رو نگه می‌داریم
        historyStack.push(act);

        return true;
    }
    return false;
}

bool BookManager::removeBook(int BookId) {
    Book b = getBookById(BookId); // قبل از حذف، بک‌آپ می‌گیریم
    if (b.Id == -1) return false;
    
    string sql =
        "DELETE FROM books WHERE Id = " + to_string(BookId) + ";";

    if (db.execute(sql)) {
        Action act;
        act.type = REMOVE;
        act.bookData = b; // کل آبجکت کتاب را ذخیره می‌کنیم تا بتوانیم برش گردانیم
        historyStack.push(act);
        return true;
    }
    return false;
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

Book BookManager::getBookById(int id) {
    sqlite3_stmt* stmt;
    string query = "SELECT * FROM books WHERE Id = " + to_string(id);
    Book b = { -1, "", "", 0, false, "" };

    if (sqlite3_prepare_v2(db.getDbHandle(), query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            b.Id = sqlite3_column_int(stmt, 0);
            b.Title = (char*)sqlite3_column_text(stmt, 1);
            b.Author = (char*)sqlite3_column_text(stmt, 2);
            b.Year = sqlite3_column_int(stmt, 3);
            b.isBorrowed = sqlite3_column_int(stmt, 4);
            const char* borrower = (const char*)sqlite3_column_text(stmt, 5);
            b.Borrower = borrower ? borrower : "";
        }
    }
    sqlite3_finalize(stmt);
    return b;
}

// -------------------- Borrow Book --------------------
bool BookManager::borrowBook(int bookId, const string& borrower) {
    Book b = getBookById(bookId);
    if (b.Id == -1) { cout << "Book not found!\n"; return false; }

    if (b.isBorrowed) {
        cout << "Book is currently borrowed. Added " << borrower << " to the waiting queue.\n";
        requestQueue.enqueue(bookId, borrower);
        return true;
    }

    string sql = "UPDATE books SET isBorrowed = 1, Borrower = '" + borrower + "' WHERE Id = " + to_string(bookId);
    if (db.execute(sql)) {
        Action act;
        act.type = BORROW;
        act.targetId = bookId;
        act.oldBorrower = "";
        act.bookData.Borrower = borrower;
        historyStack.push(act);

        cout << "Book borrowed successfully by " << borrower << ".\n";
        return true;
    }
    return false;
}

// -------------------- Return Book --------------------
bool BookManager::returnBook(int bookId) {
    Book b = getBookById(bookId);
    if (!b.isBorrowed) { cout << "Book is not borrowed.\n"; return false; }

    Action act;
    act.type = RETURN;
    act.targetId = bookId;
    act.oldBorrower = b.Borrower;

    string sql = "UPDATE books SET isBorrowed = 0, Borrower = NULL WHERE Id = " + to_string(bookId);
    db.execute(sql);

    string nextBorrower = requestQueue.dequeueRequestForBook(bookId);

    if (nextBorrower != "") {
        cout << "Queue Alert: Book passed to next person in queue: " << nextBorrower << endl;
        string sql2 = "UPDATE books SET isBorrowed = 1, Borrower = '" + nextBorrower + "' WHERE Id = " + to_string(bookId);
        db.execute(sql2);
    }
    else {
        cout << "Book returned successfully.\n";
    }

    historyStack.push(act);
    return true;
}

void BookManager::undoLastAction() {
    Action lastAct;
    if (!historyStack.pop(lastAct)) {
        cout << "Nothing to undo (History is empty).\n";
        return;
    }

    string sql;
    switch (lastAct.type) {
    case ADD:
        cout << "Undoing Add Book (ID: " << lastAct.targetId << ")...\n";
        sql = "DELETE FROM books WHERE Id = " + to_string(lastAct.targetId);
        db.execute(sql);
        break;

    case REMOVE:
        cout << "Undoing Remove Book (Restoring: " << lastAct.bookData.Title << ")...\n";
        sql = "INSERT INTO books (Id, Title, Author, Year, isBorrowed, Borrower) VALUES (" +
            to_string(lastAct.bookData.Id) + ", '" +
            lastAct.bookData.Title + "', '" +
            lastAct.bookData.Author + "', " +
            to_string(lastAct.bookData.Year) + ", " +
            (lastAct.bookData.isBorrowed ? "1" : "0") + ", '" +
            lastAct.bookData.Borrower + "')";
        db.execute(sql);
        break;

    case BORROW:
        cout << "Undoing Borrow...\n";
        sql = "UPDATE books SET isBorrowed = 0, Borrower = NULL WHERE Id = " + to_string(lastAct.targetId);
        db.execute(sql);
        break;

    case RETURN:
        cout << "Undoing Return (Giving back to " << lastAct.oldBorrower << ")...\n";
        sql = "UPDATE books SET isBorrowed = 1, Borrower = '" + lastAct.oldBorrower + "' WHERE Id = " + to_string(lastAct.targetId);
        db.execute(sql);
        break;
    }
}

void BookManager::showLastOperation() {
    Action lastAct;

    if (!historyStack.peek(lastAct)) {
        cout << "\n[Status]: No operations recorded in history.\n";
        return;
    }

    cout << "\n--- Last Operation Status ---\n";
    switch (lastAct.type) {
    case ADD:
        cout << "Action: Added a book\n";
        cout << "Details: ID " << lastAct.targetId << " was added to the library.\n";
        break;

    case REMOVE:
        cout << "Action: Removed a book\n";
        cout << "Details: '" << lastAct.bookData.Title << "' (ID: " << lastAct.bookData.Id << ") was removed.\n";
        break;

    case BORROW:
        cout << "Action: Borrowed a book\n";
        cout << "Details: Book ID " << lastAct.targetId
            << " was borrowed by " << lastAct.bookData.Borrower << ".\n";
        break;

    case RETURN:
        cout << "Action: Returned a book\n";
        cout << "Details: Book ID " << lastAct.targetId << " was returned from " << lastAct.oldBorrower << ".\n";
        break;
    }
    cout << "-----------------------------\n";
}
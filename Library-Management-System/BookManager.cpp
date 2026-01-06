#include "BookManager.h"
#include <iostream>
#include <iomanip>
#include "sqlite3.h"
#include <random>
#include <ctime>
#include <string>

using namespace std;

// Helper function to truncate strings for table display
// This ensures that long titles/authors do not break the table alignment.
string truncate(string str, size_t width) {
    if (str.length() > width) {
        return str.substr(0, width - 3) + "...";
    }
    return str;
}

// Constructor: Initializes the BookManager and attempts to open the database.
BookManager::BookManager() {
    if (!db.open()) {
        cerr << "[!] Critical Error: Database connection failed.\n";
    }
}

// Adds a new book to the database and records the action in the history stack.
bool BookManager::addBook(const Book& BookItem) {
    // Generate a unique 5-digit ID for the book
    int finalId = generateUniqueId();

    // Construct SQL query to insert the book
    string sql =
        "INSERT INTO books (Id, Title, Author, Year, isBorrowed, Borrower) VALUES (" +
        to_string(finalId) + ", '" +
        BookItem.Title + "', '" +
        BookItem.Author + "', " +
        to_string(BookItem.Year) + ", 0, NULL);";

    // Execute the query
    if (db.execute(sql)) {
        cout << "\n[+] Book Added Successfully.\n";
        cout << "    ID: " << finalId << " | Title: " << BookItem.Title << endl;

        // Push action to history stack for Undo functionality
        Action act;
        act.type = ADD;
        act.targetId = finalId;
        historyStack.push(act);

        return true;
    }
    return false;
}

// Removes a book from the database by ID.
bool BookManager::removeBook(int BookId) {
    // Retrieve book details before deletion (needed for Undo)
    Book b = getBookById(BookId);
    if (b.Id == -1) return false; // Book not found

    string sql =
        "DELETE FROM books WHERE Id = " + to_string(BookId) + ";";

    if (db.execute(sql)) {
        // Log the removal action with the deleted book's data
        Action act;
        act.type = REMOVE;
        act.bookData = b;
        historyStack.push(act);
        return true;
    }
    return false;
}

// Retrieves all books from the database and returns them as a vector.
vector<Book> BookManager::getAllBooks() {
    vector<Book> booksList;
    sqlite3_stmt* stmt;
    string query = "SELECT Id, Title, Author, Year, isBorrowed, Borrower FROM books";

    sqlite3* rawDB = db.getDbHandle();

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(rawDB, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[!] Error fetching books: " << sqlite3_errmsg(rawDB) << endl;
        return booksList;
    }

    // Iterate through the result rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Book b;
        b.Id = sqlite3_column_int(stmt, 0);

        // Safely cast text columns (handle NULLs)
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

    // Finalize statement to release memory
    sqlite3_finalize(stmt);
    return booksList;
}

// Displays the list of books in a formatted table.
void BookManager::printBooks(const vector<Book>& books) {
    if (books.empty()) {
        cout << "\n[i] No books found in the library.\n";
        return;
    }

    // Table Header
    cout << "\n";
    cout << " +-------+----------------------+----------------------+------+----------------------+" << endl;
    cout << " | " << left << setw(5) << "ID"
        << " | " << setw(20) << "Title"
        << " | " << setw(20) << "Author"
        << " | " << setw(4) << "Year"
        << " | " << setw(20) << "Status" << " |" << endl;
    cout << " +-------+----------------------+----------------------+------+----------------------+" << endl;

    // Table Body
    for (const auto& b : books) {
        string status = b.isBorrowed ? ("Borrowed: " + b.Borrower) : "Available";

        cout << " | " << left << setw(5) << b.Id
            << " | " << setw(20) << truncate(b.Title, 20)
            << " | " << setw(20) << truncate(b.Author, 20)
            << " | " << setw(4) << b.Year
            << " | " << setw(20) << truncate(status, 20) << " |" << endl;
    }
    cout << " +-------+----------------------+----------------------+------+----------------------+" << endl;
}

// Sorts the vector of books based on publication year (Ascending).
void BookManager::sortBooksByYear(vector<Book>& books) {
    sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.Year < b.Year;
        });
}

// Sorts the vector of books based on ID (Ascending).
void BookManager::sortBooksById(vector<Book>& books) {
    sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.Id < b.Id;
        });
}

// Performs a linear search to find books matching a specific ID.
vector<Book> BookManager::searchBooksById(const vector<Book>& books, int searchId) {
    vector<Book> results;

    for (const auto& item : books) {
        if (item.Id == searchId) {
            results.push_back(item);
            break; // IDs are unique, so we can stop after finding one
        }
    }

    return results;
}

// Checks if a specific ID already exists in the database.
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

// Generates a random 5-digit ID and ensures it doesn't collide with existing IDs.
int BookManager::generateUniqueId() {
    static mt19937 gen(static_cast<unsigned int>(time(0))); // Mersenne Twister RNG
    uniform_int_distribution<> dis(10000, 99999);

    int newId;
    bool exists = true;

    // Loop until a unique ID is found
    while (exists) {
        newId = dis(gen);
        exists = idExists(newId);
    }

    return newId;
}

// Fetches a single book's details from the database by ID.
Book BookManager::getBookById(int id) {
    sqlite3_stmt* stmt;
    string query = "SELECT * FROM books WHERE Id = " + to_string(id);
    Book b = { -1, "", "", 0, false, "" }; // Default invalid book

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
// Handles borrowing logic. If the book is taken, adds the user to a waiting queue.
bool BookManager::borrowBook(int bookId, const string& borrower) {
    Book b = getBookById(bookId);
    if (b.Id == -1) {
        cout << "\n[!] Error: Book not found with ID " << bookId << ".\n";
        return false;
    }

    // Check if the book is already borrowed
    if (b.isBorrowed) {
        cout << "\n[i] Book is currently unavailable.\n";
        cout << "    -> Added '" << borrower << "' to the waiting queue.\n";
        requestQueue.enqueue(bookId, borrower); // Add to Queue
        return true;
    }

    // Update database to mark as borrowed
    string sql = "UPDATE books SET isBorrowed = 1, Borrower = '" + borrower + "' WHERE Id = " + to_string(bookId);
    if (db.execute(sql)) {
        Action act;
        act.type = BORROW;
        act.targetId = bookId;
        act.oldBorrower = "";
        act.bookData.Borrower = borrower;
        historyStack.push(act);

        cout << "\n[+] Success: Book borrowed by " << borrower << ".\n";
        return true;
    }
    return false;
}

// -------------------- Return Book --------------------
// Returns a book. If there is a queue, automatically assigns it to the next person.
bool BookManager::returnBook(int bookId) {
    Book b = getBookById(bookId);
    if (!b.isBorrowed) {
        cout << "\n[!] This book is not currently borrowed.\n";
        return false;
    }

    Action act;
    act.type = RETURN;
    act.targetId = bookId;
    act.oldBorrower = b.Borrower;

    // Reset status in DB
    string sql = "UPDATE books SET isBorrowed = 0, Borrower = NULL WHERE Id = " + to_string(bookId);
    db.execute(sql);

    // Check the waiting queue for this book
    string nextBorrower = requestQueue.dequeueRequestForBook(bookId);

    if (nextBorrower != "") {
        cout << "\n[!] QUEUE ALERT: Book automatically assigned to next user: " << nextBorrower << endl;
        // Automatically borrow for the next user in line
        string sql2 = "UPDATE books SET isBorrowed = 1, Borrower = '" + nextBorrower + "' WHERE Id = " + to_string(bookId);
        db.execute(sql2);
    }
    else {
        cout << "\n[+] Book returned successfully. Now available.\n";
    }

    historyStack.push(act);
    return true;
}

// Reverts the last operation performed (Add, Remove, Borrow, Return).
void BookManager::undoLastAction() {
    Action lastAct;
    if (!historyStack.pop(lastAct)) {
        cout << "\n[i] History is empty. Nothing to undo.\n";
        return;
    }

    string sql;
    cout << "\n[UNDO] Reverting last action... ";

    switch (lastAct.type) {
    case ADD:
        // Undo Add -> Delete the book
        cout << "(Removing Book ID: " << lastAct.targetId << ")\n";
        sql = "DELETE FROM books WHERE Id = " + to_string(lastAct.targetId);
        db.execute(sql);
        break;

    case REMOVE:
        // Undo Remove -> Re-insert the book
        cout << "(Restoring Book: " << lastAct.bookData.Title << ")\n";
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
        // Undo Borrow -> Return the book
        cout << "(Canceling Borrow)\n";
        sql = "UPDATE books SET isBorrowed = 0, Borrower = NULL WHERE Id = " + to_string(lastAct.targetId);
        db.execute(sql);
        break;

    case RETURN:
        // Undo Return -> Restore the previous borrower
        cout << "(Restoring loan to " << lastAct.oldBorrower << ")\n";
        sql = "UPDATE books SET isBorrowed = 1, Borrower = '" + lastAct.oldBorrower + "' WHERE Id = " + to_string(lastAct.targetId);
        db.execute(sql);
        break;
    }
    cout << "       -> Done.\n";
}

// Peeks at the top of the history stack to show the last executed action.
void BookManager::showLastOperation() {
    Action lastAct;

    if (!historyStack.peek(lastAct)) {
        cout << "\n[Status] No operations recorded in history.\n";
        return;
    }

    cout << "\n================ LAST OPERATION ================\n";
    switch (lastAct.type) {
    case ADD:
        cout << " Action : Added Book\n";
        cout << " Details: ID [" << lastAct.targetId << "] entered the library.\n";
        break;

    case REMOVE:
        cout << " Action : Removed Book\n";
        cout << " Details: '" << lastAct.bookData.Title << "' (ID: " << lastAct.bookData.Id << ") deleted.\n";
        break;

    case BORROW:
        cout << " Action : Borrow Book\n";
        cout << " Details: ID [" << lastAct.targetId << "] borrowed by " << lastAct.bookData.Borrower << ".\n";
        break;

    case RETURN:
        cout << " Action : Return Book\n";
        cout << " Details: ID [" << lastAct.targetId << "] returned from " << lastAct.oldBorrower << ".\n";
        break;
    }
    cout << "================================================\n";
}
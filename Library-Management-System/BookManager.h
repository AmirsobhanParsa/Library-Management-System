#pragma once
#include <string>
#include "book.h"
#include "database.h"
#include <vector>
#include <algorithm>
#include "DataStructures.h"

/**
 * @class BookManager
 * @brief Coordinates the business logic between the Database, Queue, and Stack.
 * * This class handles library operations such as adding/removing books,
 * managing the waitlist for borrowed books, and tracking history for undo operations.
 */
class BookManager {
private:
    Database db;                 // Handles low-level SQLite operations
    ManualQueue requestQueue;    // Manages the waitlist for unavailable books
    ManualStack historyStack;    // Stores previous actions to support the Undo feature

public:
    // Constructor: Initializes the manager and database connection
    BookManager();

    // --- Core Management Operations ---

    // Adds a new book entry to the library and the database
    bool addBook(const Book& book);

    // Deletes a book from the library records using its ID
    bool removeBook(int bookId);

    // Retrieves all book records stored in the database
    vector<Book> getAllBooks();

    // Prints a formatted table of books to the console
    void printBooks(const vector<Book>& books);

    // --- Sorting and Searching ---

    // Sorts a given list of books by publication year (Ascending)
    void sortBooksByYear(vector<Book>& books);

    // Sorts a given list of books by their unique ID (Ascending)
    void sortBooksById(vector<Book>& books);

    // Searches for a specific book by ID within a vector of books
    vector<Book> searchBooksById(const vector<Book>& books, int searchId);

    // --- Utility and Validation ---

    // Generates a random, non-duplicate 5-digit ID
    int generateUniqueId();

    // Checks if a specific ID is already taken in the database
    bool idExists(int id);

    // Finds and returns a Book object from the database by its ID
    Book getBookById(int id);

    // --- Borrowing Logic ---

    // Marks a book as borrowed or adds the user to the queue if currently unavailable
    bool borrowBook(int bookId, const std::string& borrower);

    // Returns a book and automatically assigns it to the next person in the queue
    bool returnBook(int bookId);

    // --- History and State Management ---

    // Reverts the most recent library operation (ADD, REMOVE, BORROW, RETURN)
    void undoLastAction();

    // Displays details of the most recent action pushed to the history stack
    void showLastOperation();
};
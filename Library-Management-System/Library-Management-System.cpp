#include <iostream>
#include <limits>
#include "BookManager.h"

using namespace std;

// Helper function to clear the input buffer.
// This is crucial when mixing 'cin >>' (which leaves newlines in the buffer)
// and 'getline' (which reads until a newline).
void clearInput() {
    cin.clear(); // Reset any error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard characters until newline
}

int main() {
    // Instantiate the core logic manager
    BookManager manager;
    int choice;

    // Main application loop
    while (true) {
        // Display Main Menu
        cout << "\n========= LIBRARY MANAGEMENT SYSTEM =========" << endl;
        cout << "1. Add New Book (Auto ID)" << endl;
        cout << "2. List All Books (from Database)" << endl;
        cout << "3. Search Book by ID" << endl;
        cout << "4. Borrow Book (Queue System)" << endl;
        cout << "5. Return Book (Auto-Transfer)" << endl;
        cout << "6. Undo Last Action (Max 5)" << endl;
        cout << "7. Show Last Operation Status" << endl;
        cout << "0. Exit" << endl;
        cout << "=============================================" << endl;
        cout << "Enter your choice: ";

        // Input Validation: Check if the user entered a valid integer
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            clearInput(); // Clear the invalid input from buffer
            continue;     // Restart the loop
        }

        // Exit condition
        if (choice == 0) break;

        switch (choice) {
        case 1: { // Add Book
            Book b;
            clearInput(); // Clear buffer before taking string input

            cout << "Enter Title: "; getline(cin, b.Title);
            cout << "Enter Author: "; getline(cin, b.Author);
            cout << "Enter Year: "; cin >> b.Year;

            manager.addBook(b);
            break;
        }
        case 2: { // List and Sort Books
            // Fetch fresh data from the database
            vector<Book> books = manager.getAllBooks();

            if (books.empty()) {
                cout << "Library is empty." << endl;
                break;
            }

            // Ask user for sorting preference
            cout << "Sort options: [1] None, [2] ID, [3] Year: ";
            int s; cin >> s;

            // Sort in memory based on user choice
            if (s == 2) manager.sortBooksById(books);
            else if (s == 3) manager.sortBooksByYear(books);

            manager.printBooks(books);
            break;
        }
        case 3: { // Search Book
            int id;
            cout << "Enter Book ID to search: "; cin >> id;

            vector<Book> all = manager.getAllBooks();
            vector<Book> found = manager.searchBooksById(all, id);

            if (found.empty()) cout << "Book not found." << endl;
            else manager.printBooks(found);
            break;
        }
        case 4: { // Borrow Book
            int id;
            string name;

            cout << "Enter Book ID: "; cin >> id;
            clearInput(); // Clear buffer for name input
            cout << "Enter Borrower Name: "; getline(cin, name);

            // Manager handles logic for queueing or borrowing
            manager.borrowBook(id, name);
            break;
        }
        case 5: { // Return Book
            int id;
            cout << "Enter Book ID: "; cin >> id;

            // Manager handles logic for transferring to next user in queue
            manager.returnBook(id);
            break;
        }
        case 6: { // Undo Action
            manager.undoLastAction();
            break;
        }
        case 7: { // Show Status
            manager.showLastOperation();
            break;
        }
        default:
            cout << "Invalid choice! Try again." << endl;
        }
    }

    cout << "Exiting... Goodbye!" << endl;
    return 0;
}
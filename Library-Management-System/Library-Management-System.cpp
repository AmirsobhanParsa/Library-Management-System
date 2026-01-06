#include <iostream>
#include <limits>
#include "BookManager.h"

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    BookManager manager;
    int choice;

    while (true) {
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

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            clearInput();
            continue;
        }

        if (choice == 0) break;

        switch (choice) {
        case 1: { // Add Book
            Book b;
            clearInput();
            cout << "Enter Title: "; getline(cin, b.Title);
            cout << "Enter Author: "; getline(cin, b.Author);
            cout << "Enter Year: "; cin >> b.Year;
            manager.addBook(b);
            break;
        }
        case 2: { // Sort
            vector<Book> books = manager.getAllBooks();
            if (books.empty()) {
                cout << "Library is empty." << endl;
                break;
            }

            cout << "Sort options: [1] None, [2] ID, [3] Year: ";
            int s; cin >> s;
            if (s == 2) manager.sortBooksById(books);
            else if (s == 3) manager.sortBooksByYear(books);

            manager.printBooks(books);
            break;
        }
        case 3: { // Search
            int id;
            cout << "Enter Book ID to search: "; cin >> id;
            vector<Book> all = manager.getAllBooks();
            vector<Book> found = manager.searchBooksById(all, id);
            if (found.empty()) cout << "Book not found." << endl;
            else manager.printBooks(found);
            break;
        }
        case 4: { // Borrow
            int id;
            string name;
            cout << "Enter Book ID: "; cin >> id;
            clearInput();
            cout << "Enter Borrower Name: "; getline(cin, name);
            manager.borrowBook(id, name);
            break;
        }
        case 5: { // Return
            int id;
            cout << "Enter Book ID: "; cin >> id;
            manager.returnBook(id);
            break;
        }
        case 6: { // Undo
            manager.undoLastAction();
            break;
        }
        case 7: { // Last Status
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
#include <iostream>
#include "BookManager.h"

int main() {
    BookManager manager;
    vector<Book> allBooks = manager.getAllBooks();

    int idToSearch;
    cout << "Enter Book ID to search: ";
    cin >> idToSearch;

    vector<Book> result = manager.searchBooksById(allBooks, idToSearch);

    if (result.empty()) {
        cout << "No book found with ID: " << idToSearch << endl;
    }
    else {
        cout << "Book Found:" << endl;
        manager.printBooks(result);
    }

    return 0;
}

#include <iostream>
#include "BookManager.h"

int main() {
    BookManager manager;

    Book newBook;
    cout << "Enter Title: "; getline(cin, newBook.Title);
    cout << "Enter Author: "; getline(cin, newBook.Author);
    cout << "Enter Year: "; cin >> newBook.Year;

    manager.addBook(newBook);

    manager.printBooks(manager.getAllBooks());

    return 0;
}

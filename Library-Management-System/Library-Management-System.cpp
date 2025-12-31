#include <iostream>
#include "BookManager.h"

int main() {
    BookManager manager;

     //manager.addBook({101, "C++ Programming", "Stroustrup", 2013, false, ""});
     //manager.addBook({102, "Clean Code", "Robert Martin", 2008, false, ""});
     //manager.addBook({103, "The Pragmatic", "Andrew Hunt", 1999, false, ""});

    cout << "Loading books from database...\n";
    vector<Book> allBooks = manager.getAllBooks();

    manager.printBooks(allBooks);

    return 0;
}

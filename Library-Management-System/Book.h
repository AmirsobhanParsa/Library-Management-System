#pragma once
#include <string>

using namespace std;

/**
 * @struct Book
 * @brief Represents a single book entity within the library system.
 * * This structure maps directly to the 'books' table in the database
 * and is used for passing data between the UI, Manager, and Storage layers.
 */
struct Book
{
    int Id;             // Unique 5-digit identifier for the book
    string Title;       // The title/name of the book
    string Author;      // The author of the book
    int Year;           // The publication year

    bool isBorrowed;    // Status flag: true if checked out, false if available
    string Borrower;    // Name of the person currently holding the book (empty if available)
};
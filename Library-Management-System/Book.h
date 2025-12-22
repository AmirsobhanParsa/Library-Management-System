#pragma once
#include <string>

using namespace std;
struct Book
{
	int Id;
	string Title;
	string Author;
	int Year;
	bool isBorrowed;
	string Borrower;
};

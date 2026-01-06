# 📚 Library Management System (LMS)

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![SQLite](https://img.shields.io/badge/SQLite-07405E?style=for-the-badge&logo=sqlite&logoColor=white)

A robust, high-performance **Library Management System** built with **C++** and **SQLite**. This project showcases the implementation of custom data structures and database integration without relying on standard STL containers like `std::stack` or `std::queue`.

---

## 🌟 Key Features

* **💾 Persistent Storage:** Powered by **SQLite** for reliable, serverless data management.
* **🧠 Custom Data Structures:**
    * **Manual Queue:** Linked-List based implementation for managing book reservations (FIFO).
    * **Manual Stack:** Array-based stack for a 5-step **Undo** system (LIFO).
* **🆔 Unique ID Generator:** Automatic 5-digit unique ID generation for every new book with database collision check.
* **🔍 Advanced Search & Sort:** * Search by **ID** with $O(n)$ efficiency.
    * Sort by **Year of Publication** or **ID** using IntroSort ($O(n \log n)$).
* **🔄 Automated Borrowing Logic:** Real-time status tracking and automatic transfer to the next person in the queue upon book return.
* **⏪ Action History:** Revert the last 5 actions (Add, Remove, Borrow, Return) with a custom Undo system.

---

## 🏗️ Technical Architecture

The system follows a clean architectural approach:

1.  **Database Layer:** Handles raw SQL execution and database connectivity.
2.  **Logic Layer (`BookManager`):** Acts as a bridge, syncing the SQLite database with memory-based structures.
3.  **Data Structure Layer:** Pure C++ implementation of Queue and Stack to demonstrate low-level memory management.

---

## 📊 Complexity Analysis

| Operation | Data Structure | Time Complexity |
| :--- | :--- | :--- |
| **Search by ID** | Vector / Linear | $O(n)$ |
| **Sorting** | IntroSort | $O(n \log n)$ |
| **Waiting Queue** | Manual Linked List | $O(1)$ |
| **Undo Stack** | Manual Array Stack | $O(1)$ |

---

## 🛠️ Installation & Setup

### Prerequisites
* A C++ compiler (GCC, Clang, or MSVC).
* `sqlite3.h` and `sqlite3.c` (included in the source files).

### Build Instructions
```bash
# Clone the repository
git clone [https://github.com/yourusername/library-management-system.git](https://github.com/yourusername/library-management-system.git)

# Navigate to the project folder
cd library-management-system

# Compile the project
g++ main.cpp BookManager.cpp database.cpp sqlite3.c -o LibraryApp

# Run the application
./LibraryApp

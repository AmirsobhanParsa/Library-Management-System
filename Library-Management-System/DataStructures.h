#pragma once
#include <string>
#include <iostream>
#include "book.h"

using namespace std;

// --- Queue Section: Linked List implementation for Borrowing Requests ---

// Node structure to store individual borrowing requests in the queue
struct RequestNode {
    int bookId;
    string borrowerName;
    RequestNode* next;
};

/**
 * @class ManualQueue
 * @brief A linked-list based queue that manages waiting lists for borrowed books.
 * It supports specific removal of requests when a book becomes available.
 */
class ManualQueue {
private:
    RequestNode* front, * rear;

public:
    // Constructor: Initializes an empty queue
    ManualQueue() { front = rear = nullptr; }

    // Adds a new borrower request to the end of the queue
    void enqueue(int id, string name) {
        RequestNode* temp = new RequestNode();
        temp->bookId = id;
        temp->borrowerName = name;
        temp->next = nullptr;

        // If queue is empty, both front and rear point to the new node
        if (rear == nullptr) {
            front = rear = temp;
            return;
        }

        // Link the current rear to the new node and update the rear pointer
        rear->next = temp;
        rear = temp;
    }

    /**
     * Searches for the first request matching a specific book ID.
     * If found, removes that specific node from the list and returns the borrower name.
     */
    string dequeueRequestForBook(int id) {
        if (front == nullptr) return "";

        RequestNode* temp = front;
        RequestNode* prev = nullptr;

        while (temp != nullptr) {
            if (temp->bookId == id) {
                string foundUser = temp->borrowerName;

                // Case: The node to remove is the front node
                if (prev == nullptr) {
                    front = temp->next;
                    if (front == nullptr) rear = nullptr;
                }
                // Case: The node to remove is in the middle or end
                else {
                    prev->next = temp->next;
                    if (prev->next == nullptr) rear = prev;
                }

                delete temp; // Free memory
                return foundUser;
            }
            prev = temp;
            temp = temp->next;
        }
        return ""; // Return empty string if no request exists for this book
    }

    // Checks if there are any pending requests in the queue
    bool isEmpty() { return front == nullptr; }
};



// --- Stack Section: Fixed-size implementation for Undo history ---

// Defined types of actions that can be stored in history
enum ActionType { ADD, REMOVE, BORROW, RETURN };

// Data structure representing a single user action for the Undo feature
struct Action {
    ActionType type;
    Book bookData;
    int targetId;
    string oldBorrower;
};

/**
 * @class ManualStack
 * @brief A fixed-size stack (capacity: 5) to track recent operations.
 * When the limit is reached, it shifts older actions out to make room for new ones.
 */
class ManualStack {
private:
    Action actions[5]; // Static array to store the last 5 actions
    int top;           // Index of the most recent action
    int count;         // Current number of actions stored

public:
    // Constructor: Initializes empty stack status
    ManualStack() { top = -1; count = 0; }

    /**
     * Pushes a new action onto the stack.
     * If the stack is full, it performs a left-shift to remove the oldest element.
     */
    void push(Action act) {
        if (count < 5) {
            top++;
            actions[top] = act;
            count++;
        }
        else {
            // Shift all elements left (effectively deleting actions[0])
            for (int i = 0; i < 4; i++) {
                actions[i] = actions[i + 1];
            }
            actions[4] = act; // Add the new action at the final position
            top = 4;
        }
    }

    // Removes and returns the most recent action from the stack
    bool pop(Action& outAction) {
        if (count == 0) return false;
        outAction = actions[top];
        top--;
        count--;
        return true;
    }

    // Returns the most recent action without removing it
    bool peek(Action& outAction) {
        if (count == 0) return false;
        outAction = actions[top];
        return true;
    }
};
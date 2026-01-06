#pragma once
#include <string>
#include <iostream>
#include "book.h"

using namespace std;

// ------------------- ??? ?? (Queue) ???? ?????? ????? -------------------

struct RequestNode {
    int bookId;
    string borrowerName;
    RequestNode* next;
};

class ManualQueue {
private:
    RequestNode* front, * rear;

public:
    ManualQueue() { front = rear = nullptr; }

    void enqueue(int id, string name) {
        RequestNode* temp = new RequestNode();
        temp->bookId = id;
        temp->borrowerName = name;
        temp->next = nullptr;

        if (rear == nullptr) {
            front = rear = temp;
            return;
        }
        rear->next = temp;
        rear = temp;
    }

    string dequeueRequestForBook(int id) {
        if (front == nullptr) return "";

        RequestNode* temp = front;
        RequestNode* prev = nullptr;

        while (temp != nullptr) {
            if (temp->bookId == id) {
                string foundUser = temp->borrowerName;

                if (prev == nullptr) {
                    front = temp->next;
                    if (front == nullptr) rear = nullptr;
                }
                else {
                    prev->next = temp->next;
                    if (prev->next == nullptr) rear = prev;
                }

                delete temp;
                return foundUser;
            }
            prev = temp;
            temp = temp->next;
        }
        return "";
    }

    bool isEmpty() { return front == nullptr; }
};

// ------------------- ??? ???? (Stack) ???? Undo -------------------

enum ActionType { ADD, REMOVE, BORROW, RETURN };

struct Action {
    ActionType type;
    Book bookData;
    int targetId;
    string oldBorrower;
};

class ManualStack {
private:
    Action actions[5];
    int top;
    int count;

public:
    ManualStack() { top = -1; count = 0; }

    void push(Action act) {
        if (count < 5) {
            top++;
            actions[top] = act;
            count++;
        }
        else {
            for (int i = 0; i < 4; i++) {
                actions[i] = actions[i + 1];
            }
            actions[4] = act;
            top = 4;
        }
    }

    bool pop(Action& outAction) {
        if (count == 0) return false;
        outAction = actions[top];
        top--;
        count--;
        return true;
    }

    bool peek(Action& outAction) {
        if (count == 0) return false;
        outAction = actions[top];
        return true;
    }
};
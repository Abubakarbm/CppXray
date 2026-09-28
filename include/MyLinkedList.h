#pragma once
#include "Errors.h"
#include <iostream>

template <typename T>
class MyLinkedList {
public:
 // Public so Visualizer can walk the list for rendering
    struct Node {
        T value;
        Node* prev;
        Node* next;

        explicit Node(const T& v) : value(v), prev(nullptr), next(nullptr) {}
    };
    //struct for my linked list.

private:
    Node* head;   
    Node* tail;  
    int   sz;

    Node* nodeAt(int index) const {
        Node* cur;
        if(index < sz / 2) {
            cur = head;
            for(int i = 0; i < index; i++) cur = cur->next;
        } else {
            cur = tail;
            for(int i = sz - 1; i > index; i--) cur = cur->prev;
        }
        return cur;
    }
    // This is a helper function to find the node at a specific index.
    // It optimizes the search by starting from the head if the index is in the first half of the list, 
    //and from the tail if it's in the second half.

public:
   
    void push_back(const T& val) {
        Node* node = new Node(val);
        if(!tail) {                 
            head = tail = node;
        } else {
            node->prev = tail;        
            tail->next = node;        
            tail = node;    
        }
        sz++;
    }
    //a funtion that adds a new node with given values to the end of the list.
    //if the list is empty, it sets both head and tail to the new node.

    void clear() {
        Node* cur = head;
        while(cur) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        sz = 0;
    }
    // removes all nodes from the list and resets it to an empty state.

    MyLinkedList() : head(nullptr), tail(nullptr), sz(0) {}

    MyLinkedList(const MyLinkedList& other) : head(nullptr), tail(nullptr), sz(0) {
        Node* cur = other.head;
        while(cur) {
            push_back(cur->value);
            cur = cur->next;
        }
    }
    // Copy constructor to create a new list as a copy of an existing one.

    ~MyLinkedList() {
        clear();
    }
    // Destructor to clean up memory when the list is destroyed.
    //delets every node individually

    MyLinkedList& operator=(const MyLinkedList& other) {
        if(this == &other) return *this;
        clear();
        Node* cur = other.head;
        while(cur) { push_back(cur->value); cur = cur->next; }
        return *this;
    }
    //this is an overloaded = operator which copies the contents of one list to another.

    void push_front(const T& val) {
        Node* node = new Node(val);
        if(!head) {
            head = tail = node;
        } else {
            node->next = head;
            head->prev = node;
            head = node;
        }
        sz++;
    }
    //adds a new node with the given value to the front of the list.

    void pop_back() {
        if(!tail) throw EmptyContainerError("MyLinkedList");
        Node* toDelete = tail;
        if(tail->prev) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {                       
            head = tail = nullptr;
        }
        delete toDelete;               
        sz--;
    }
    //pops last node from list. if list is empty, it throws an exception.

    void pop_front() {
        if(!head) throw EmptyContainerError("MyLinkedList");
        Node* toDelete = head;
        if(head->next) {
            head = head->next;
            head->prev = nullptr;
        } else {
            head = tail = nullptr;
        }
        delete toDelete;
        sz--;
    }
    // pops first node from the list. if empty, throws an exception.

    void insert(int index, const T& val) {
        if(index < 0 || index > sz) throw OutOfRangeError(index, sz + 1);
        if(index == 0) { push_front(val); return; }
        if(index == sz) { push_back(val);  return; }

        Node* cur = nodeAt(index);      // find node at position
        Node* node = new Node(val);

        node->prev = cur->prev;
        node->next = cur;
        cur->prev->next = node;
        cur->prev = node;
        sz++;
    }
    // inserts a new node with given value at a specific index in the list.
    //if the index is out of range, it htrows an exception.

    void remove(int index) {
        if(index < 0 || index >= sz) throw OutOfRangeError(index, sz);
        if(index == 0) { pop_front(); return; }
        if(index == sz - 1) { pop_back();  return; }

        Node* cur = nodeAt(index);
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        delete cur;
        sz--;
    }
    // removes the node at a specific index from the list.

    T& front() {
        if(!head) throw EmptyContainerError("MyLinkedList");
        return head->value;
    }
    // returns a reference to the value of the first node in the list.

    T& back() {
        if(!tail) throw EmptyContainerError("MyLinkedList");
        return tail->value;
    }
    // returns a reference to the value of the last node in the list.

    int find(const T& val) const {
        Node* cur = head;
        int idx = 0;

        while(cur) {
            if(cur->value == val) return idx;
            cur = cur->next;
            idx++;
        }

        return -1;   // not found
    }
    // searches for the first occurrence of a value in the list and returns its index, or -1 if not found.

    int  size()  const { return sz; }
    // returns the number of nodes in the list.

    bool empty() const { return sz == 0; }
    // returns true if the list is empty, false otherwise.

    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }
    // These provide access to the head and tail nodes for visualization purposes.

    bool operator==(const MyLinkedList& other) const {
        if(sz != other.sz) return false;

        Node* a = head;
        Node* b = other.head;

        while(a) {
            if(!(a->value == b->value)) return false;
            a = a->next;
            b = b->next;
        }
        return true;
    }
    //overloaded == operator to compare two lists based on their contents. 
    //It checks if the szs are equal and then compares each corresponding node's value for equality.

    bool operator!=(const MyLinkedList& other) const {
        return !(*this == other);
    }
    //overloaded != operator to compare two lists based on their contents.
    //it is just the inverse of the == operator.

    friend std::ostream& operator<<(std::ostream& os, const MyLinkedList<T>& list) {
        os << "MyLinkedList[" << list.sz << "] { ";
        Node* cur = list.head;

        while(cur) {
            os << cur->value;
            if(cur->next) os << " <-> ";
            cur = cur->next;
        }
        os << " }";
        return os;
    }
    //overloaded << operator to print the contents of the list in a readable format.

};

// This header defines a template class MyLinkedList which implements a doubly linked list data structure.
// It includes methods for adding, removing, and accessing elements, as well as error handling.
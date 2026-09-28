#pragma once
#include "MyVector.h"
#include "MyLinkedList.h"
#include "Errors.h"
#include <iostream>

template <typename T>
class MyStack {
    MyVector<T> vec;   // internal storage

public:
    void push(const T& val) {
        vec.push_back(val);    
    }
    // adds an element to the top of the stack by pushung it to the back of the vector.

    void pop() {
        if(vec.empty()) throw EmptyContainerError("MyStack");
        vec.pop_back();
    }
    // removes the top element of the stack by popping the back of the vector.

    T& top() {
        if(vec.empty()) throw EmptyContainerError("MyStack");
        return vec.back();
    }
    // returns a reference to the top element of the stack, which is the last element of the vector.

    const T& top() const {
        if(vec.empty()) throw EmptyContainerError("MyStack");
        return vec.back();
    }
    //const version for const stacks.

    bool empty() const { return vec.empty(); }
    int  size()  const { return static_cast<int>(vec.size()); }
    void clear() { vec.clear(); }

    const MyVector<T>& raw() const { return vec; }
    // This provides access to the internal vector for visualization purposes.

    bool operator==(const MyStack& o) const { return vec == o.vec; }
    bool operator!=(const MyStack& o) const { return !(*this == o); }

    friend std::ostream& operator<<(std::ostream& os, const MyStack<T>& s) {
        os << "MyStack[" << s.size() << "] top->{ ";
        for(int i = static_cast<int>(s.vec.size()) - 1; i >= 0; i--) {
            os << s.vec[i];
            if(i > 0) os << ", ";
        }
        os << " }";
        return os;
    }
};
// This is a simple stack implementation using MyVector as the underlying storage.

template <typename T>
class MyQueue {
    MyLinkedList<T> list;

public:
    void enqueue(const T& val) {
        list.push_back(val);   
    }
    //adds element to the back of the queue.

    void dequeue() {
        if(list.empty()) throw EmptyContainerError("MyQueue");
        list.pop_front();         
    }
    // removes the front element of the queue.

    T& front() {
        if(list.empty()) throw EmptyContainerError("MyQueue");
        return list.front();
    }
    T& back() {
        if(list.empty()) throw EmptyContainerError("MyQueue");
        return list.back();
    }
    //these return the refrences to the front and back elements of the queue resp.

     const T& front() const {
        if(list.empty()) throw EmptyContainerError("MyQueue");
        return list.front();
    }
    const T& back() const {
        if(list.empty()) throw EmptyContainerError("MyQueue");
        return list.back();
    }
    //const versions for const queues.

    bool empty() const { return list.empty(); }
    int  size()  const { return list.size(); }
    void clear()       { list.clear(); }

    const MyLinkedList<T>& raw() const { return list; }
    //provides access to the internal linked list for visualization purposes.

    bool operator==(const MyQueue& o) const { return list == o.list; }

    friend std::ostream& operator<<(std::ostream& os, const MyQueue<T>& q) {
        os << "MyQueue[" << q.size() << "] front->{ ";
        auto* cur = q.list.getHead();
        while(cur) {
            os << cur->value;
            if(cur->next) os << ", ";
            cur = cur->next;
        }
        os << " }<-back";
        return os;
    }
};
// This is a simple queue implementation using MyLinkedList as the underlying storage.

template <typename T>
class MyMinHeap {
    MyVector<T> vec;   // heap stored as array — index math gives tree structure

    void heapifyUp(int i) {
        while(i > 0) {
            int parent = (i - 1) / 2;
            if(vec[parent] > vec[i]) {
                T tmp       = vec[parent];
                vec[parent] = vec[i];
                vec[i]      = tmp;
                i = parent;
            } else break;
        }
    }
    //this helps maintain heap structure by 'heapifying' after inserting a new element.

    void heapifyDown(int i) {
        int n = static_cast<int>(vec.size());
        while(true) {
            int smallest = i;
            int left     = 2 * i + 1;
            int right    = 2 * i + 2;
            if(left  < n && vec[left]  < vec[smallest]) smallest = left;
            if(right < n && vec[right] < vec[smallest]) smallest = right;
            if(smallest == i) break;
            T tmp         = vec[i];
            vec[i]        = vec[smallest];
            vec[smallest] = tmp;
            i = smallest;
        }
    }
    //maintains heap structure after removing min elem by 'heapifying' down from the root.

public:
    void insert(const T& val) {
        vec.push_back(val);
        heapifyUp(static_cast<int>(vec.size()) - 1);
    }
    // adds a new element to the heap.

    T extractMin() {
        if(vec.empty()) throw EmptyContainerError("MyMinHeap");
        T minVal  = vec[0];         // save the minimum
        vec[0]    = vec.back();     // move last element to root
        vec.pop_back();             // remove the last slot
        if(!vec.empty()) heapifyDown(0);  // restore heap property
        return minVal;
    }
    //gets and removes min elem (root) from the heap

    T& peek() {
        if(vec.empty()) throw EmptyContainerError("MyMinHeap");
        return vec[0];
    }

    const T& peek() const {
        if(vec.empty()) throw EmptyContainerError("MyMinHeap");
        return vec[0];
    }
    // returns a reference to the minimum element (root) without removing it.


    bool empty() const { return vec.empty(); }
    int  size() const { return static_cast<int>(vec.size()); }
    void clear() { vec.clear(); }

    const MyVector<T>& raw() const { return vec; }

    friend std::ostream& operator<<(std::ostream& os,
                                     const MyMinHeap<T>& h) {
        os << "MyMinHeap[" << h.size() << "] { ";
        for(int i = 0; i < static_cast<int>(h.vec.size()); i++) {
            os << h.vec[i];
            if(i < static_cast<int>(h.vec.size()) - 1) os << ", ";
        }
        os << " }";
        return os;
    }
};
// This is a simple min-heap implementation using MyVector as the underlying storage.
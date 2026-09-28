# pragma once 
#include "Errors.h"
#include <iostream>
#include <initializer_list>
using namespace std;

template <typename T>
class MyVector{
private:
    T* data;
    // pointer to my heap array
    size_t elemsize;
    // no of elements stored
    size_t space;
    // no of elements we have room for

private:
    void resize(size_t newSpace){
        if(newSpace < 1) newSpace = 1;
        T* newData = new T[newSpace];

        for(size_t i = 0; i < elemsize; ++i)
            newData[i] = data[i];

        delete[] data;
        data = newData;
        space = newSpace;
    }
    // when we run out of space, we allcate a new array then copy everything then delete the old one.
    // this is push back

public: 
    MyVector(): data(new T[5]), elemsize(0), space(5){}
    // this is my default constructor which sets the array and capacity to 5.

    void push_back(const T& val){
        if(elemsize == space) resize(space*2);
        data[elemsize++] = val;
    }
    // this is the push back function which adds an element to the end of the vector.
    // if we run out of space, we call resize to double the capacity.

    void pop_back(){
        if(elemsize == 0) throw EmptyContainerError("MyVector");
        elemsize--;
        // this is the pop back function which removes the last element of the vector.

        if(elemsize>0 && elemsize < space/4 && space/2 >= 5) 
            resize(space/2);
        // this shrinks the array if it's using less than 25% of its capacity and the new capacity is at least 5.
        // this is to save memory when we have a lot of unused space.
    }

    void remove(int i){
        if(i < 0 || static_cast<size_t>(i) >= elemsize) throw OutOfRangeError(i, static_cast<int>(elemsize));
        for(int j = i; static_cast<size_t>(j) < elemsize - 1; ++j)
            data[j] = data[j+1];
        elemsize--;
        // this removes an element at a specific index by shifting all the elements after it to the left
    }

    explicit MyVector(int n): data(nullptr), elemsize(0), space(0){
        if(n < 0) throw AllocationError("Sorry! the size must be >= 0");
        size_t initialSpace = static_cast<size_t>(n) < 1 ? 1 : static_cast<size_t>(n) * 2;
        data = new T[initialSpace];
        elemsize = static_cast<size_t>(n);
        space = initialSpace;
        for(size_t i = 0; i < elemsize; ++i)
            data[i] = T{};
    }
    //this is the pre-sized costructor which creates n default elements

    MyVector(initializer_list<T> list): data(new T[list.size() * 2 + 1]),
        elemsize(0), space(list.size() * 2 + 1){
            for(const T& val : list) push_back(val);
        }
    // this initialises my list: MyVector<int> v={1,2,3}

    MyVector(const MyVector& other): data(new T[other.space]), elemsize(other.elemsize), space(other.space){
        for(size_t i = 0; i < elemsize; ++i)
            data[i] = other.data[i];
    }
    // this is our deep copy constructor.
    // if we did a shallow copy, wed have two obj pointing to same array, 
    // and if one is destroyed, the other would just be a pointer pointing to nowhere.

    MyVector(MyVector&& other) noexcept: data(other.data), elemsize(other.elemsize), space(other.space){
        other.data = nullptr;
        other.elemsize = 0;
        other.space = 0;
    }
    // this is a move costructor which we can use when returning MyVector. 
    // with this, we can move MyVector instead of coping it.

    ~MyVector(){
        delete[] data;
    }
    // a destructor to free up the heap

    MyVector& operator=(const MyVector& other){
        if(this == &other) return *this;
        delete[] data;
        space = other.space;
        elemsize = other.elemsize;
        data = new T[space];

        for(size_t i = 0; i < elemsize; ++i)
            data[i] = other.data[i];
        return *this;
    }
    // here we overloaded the = operator to do deep copy assignment.
    // if we didnt do this, we would have the same problem as the copy constructor with shallow copy.

    T& operator[](int i){
        if(i < 0 || static_cast<size_t>(i) >= elemsize)
            throw OutOfRangeError(i, static_cast<int>(elemsize));
        return data[i];
    }
    const T& operator[](int i) const {
        if(i < 0 || static_cast<size_t>(i) >= elemsize)
            throw OutOfRangeError(i, static_cast<int>(elemsize));
        return data[i];
    }
    // const overload
    // here we overloaded the [] operator to check boundaries and return the data or an error where necessary

    MyVector operator+(const MyVector& other) const{
        MyVector result;
        for(size_t i = 0; i < elemsize; ++i)
            result.push_back(data[i]);

        for(size_t i = 0; i < other.elemsize; ++i)
            result.push_back(other.data[i]);

        return result;
    }
    // here, we overloaded the + operator to concatenate two MyVectors together.
    // we create a new MyVector, then we push back all the elements of the first MyVector, then we push back all the elements of the second MyVector, and then we return the result.

    bool operator==(const MyVector& other) const{
        if(elemsize != other.elemsize) return false;

        for(size_t i = 0; i < elemsize; ++i)
            if(!(data[i] == other.data[i])) return false;
        
        return true;
    }

    bool operator!=(const MyVector& other) const{
        return !(*this==other);
    }
    // Overloads the == operator to return true if the vectors are equal, false otherwise.
    // Overloads the != operator to return true if the vectors are not equal, using the == operator for comparison.

    friend ostream& operator<<(ostream& os, const MyVector<T>& v){
        os << "MyVector[" << v.space << "/" << v.elemsize << "] { ";
        for(size_t i = 0; i < v.elemsize; ++i){
            os << v.data[i];
            if(i != v.elemsize - 1) os << ", ";
        }
        os << " }";
        return os;
    }
    // Overloads the << operator to print the contents of the MyVector in a readable format, showing its capacity and size.

    T& front(){
        if(elemsize == 0) throw EmptyContainerError("MyVector");
        return data[0];
    }
    // this returns the first element of the vector, and throws an error if the vector is empty.

    T& back(){
        if(elemsize == 0) throw EmptyContainerError("MyVector");
        return data[elemsize-1];
    }
    // this returns the last element of the vector, and throws an error if the vector is empty.

    int find(const T& val) const{
        for(size_t i = 0; i < elemsize; ++i)
            if(data[i] == val) return static_cast<int>(i);
        return -1;
    }
    // this finds the index of a value in the vector, and returns -1 if it is not found.

    size_t size() const { return elemsize; }
    // returns no of elements stored in the vector

    size_t capacity() const { return space; }
    // returns the total capacity of the vector

    bool empty()    const { return elemsize == 0; }
    // returns true if the vector is empty, false otherwise

    void clear(){
        elemsize = 0;
        resize(5);
    }
    // this clears the vector by setting the size to 0 and resizing it back to the default capacity of 5.

    T* begin() { return data; }
    // returns a pointer to the first element of the vector
    T* end() { return data + elemsize; }
    // returns a pointer to the last element of the vector
    
    const T* begin() const { return data; }
    // returns a const pointer to the first element of the vector

    const T* end()   const { return data + elemsize; }
    // returns a const pointer to the last element of the vector

    // All these pointers would be used for iteration.
};


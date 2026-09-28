#pragma once
// this tells the compiler to include this file exactly once even if it would be included by multiple files//
#include <stdexcept>
#include<string>
using namespace std;

class LibraryError : public exception{
protected:
    string msg;
public:
    explicit LibraryError(const string& m):msg(m){}
    const char* what() const noexcept override{return msg.c_str();}
};
// this is a base class for all our Error handlers, all the handlers inherit from this. 
// this class also inherits from std::exceptions which provides the interface to handle errors//
// we can use it to catch all Errors at once or catch specific ones// 

class OutOfRangeError : public LibraryError {
public:
    explicit OutOfRangeError(int index, int size): 
        LibraryError("index " + to_string(index) + ": out of range [0, " + to_string(size-1) + "]"){}
}; 
//This throws errors when we try to access an index that is outside the valid range of a container.

class EmptyContainerError : public LibraryError{
public:
    explicit EmptyContainerError(const string& name):
        LibraryError("Sorry empty container: "+ name){}
};
// this throws errors when we try to access an element from an empty container, like a vector or a list. 
// It takes the name of the container as a parameter to provide more specific error messages.

class AllocationError : public LibraryError{
public:
    explicit AllocationError(const string& detail): LibraryError("Memory allocation failed: "+ detail){}
};
// This throws errors when the program fails to allocate memory, 
// which can happen when the system runs out of memory or when there is a problem with the memory management.

class FileNotFoundError: public LibraryError{
public:
    explicit FileNotFoundError(const string& filename) : LibraryError(filename+ " Not found!!"){}
}; 
// This throws errors when we try to access a file that does not exist 
// or cannot be found in the specified location in our program.

class CorruptFileError : public LibraryError{
    public: 
    explicit CorruptFileError(const string& filename) : LibraryError(filename+" is corrupted or unreadable!!"){}
};
// This throws errors when we try to read a file that is corrupted or unreadable,

class InvalidGraphError : public LibraryError{
public:
    explicit InvalidGraphError(const string reason) : LibraryError("Invalid graph: "+ reason){}
};
// This throws errors when we encounter an invalid graph structure, 
// such as a graph with negative edge weights in some algs, self-loops, or disconnected components.

class NoPathError : public LibraryError{
public:
    explicit NoPathError(int src, int dstn) : 
        LibraryError("No path from node "+ to_string(src) + (" to node " + to_string(dstn))){}
};
// This throws errors when we try to find a path which doesnt exist between two nodes in a graph,

class EmptyQuizError : public LibraryError{
public: 
    explicit EmptyQuizError(const string& topic) : LibraryError("Sorry, there are no questions for the topic: "+ topic){}
};
// This throws errors when we try to access a quiz topic that has no questions available.

// with these custome errors, we can catch specific errors precisely, or catch all the errors atonce using the base class:
//catch(LibraryError&). 

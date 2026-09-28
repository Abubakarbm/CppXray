#pragma once
#include "MyVector.h"
#include <string>
#include <iostream>
using namespace std;

enum class NodeState {
    UNVISITED,// grey 
    SOURCE,// green 
    CURRENT,// amber 
    VISITED,// teal  
    IN_QUEUE,// purple
    PATH // yellow
};
//this shows the state of the graph mode during traversal which the visualizer uses to know how exactly to colour the graph.

struct NodeSnapshot {
    int id;
    int dist;
    NodeState state;
    
    NodeSnapshot() : id(0), dist(2147483647), state(NodeState::UNVISITED) {}
    NodeSnapshot(int i, NodeState s, int d = 2147483647) : id(i), dist(d), state(s) {}
};
//this is a snapshot of the graph node during traversal 
//which is used to know how exactly to clour the node and the distance from the source node in the case of Dijkstra's algorithm.

struct BarSnapshot {
    int  value;
    bool highlighted;  // being compared right now
    bool sorted;       // in its final position
    bool isPivot;      // the chosen pivot (Quick Sort)

    BarSnapshot() : value(0), highlighted(false), sorted(false), isPivot(false) {}
    BarSnapshot(int v, bool hl=false, bool s=false, bool pv=false) : value(v), highlighted(hl), sorted(s), isPivot(pv) {}
};
//this is the snapshot of a bar in the sorting visualizer. detaermines the colours and values of the bars during sorting.

enum class MemCellType { 
    STACK_FRAME, // red
    HEAP_ALLOC, // green
    HEAP_FREED, // blue
    EMPTY  // gray
};
//this enum represents the type of a memory cell in the memory visualizer, 
//this can be a stack frame, a heap allocation, a freed heap cell, or empty.

struct MemCell {
    string label;
    string address;
    string value;
    MemCellType type;

    MemCell() : type(MemCellType::EMPTY) {}
    MemCell(const string& a, const string& l, const string& v, MemCellType t)
        : label(l), address(a), value(v), type(t) {}
};
//this is a struct for a mem cell in the visualizer with address, label (variable name or function name), value and type(stack, frame..)

struct ComplexityData {
    int comparisons = 0;
    int swaps = 0;
    int theoretical = 0;
    int n = 0;
    string algoName;
};
// This struct holds data about the complexity of a sorting algorithm, including the number of comparisons and swaps made, 
// the theoretical complexity, the size of the input (n), and the name of the algorithm.

struct GraphEdge {
    int u, v, w;
};
//struct for an edge in the graph.

// ── Abstract base — one frame of animation ────────────────────
class AlgorithmState {
public:
    string explanation; // text shown below the visual
    int activeLine; // code line to highlight
    ComplexityData cxData; // filled in by sort algorithms

    AlgorithmState(const string& exp, int line) : explanation(exp), activeLine(line) {}

    virtual ~AlgorithmState() = default;

    virtual void render() const = 0;
    // virtual so each derived class knows how to draw itself.
    virtual string typeName() const = 0;
};
//An abstract class showing the state of an alg at a given time with explanations and code line highlight.

class GraphState : public AlgorithmState {
public:
    MyVector<NodeSnapshot> nodes;
    MyVector<GraphEdge> edges;
    int nodeCount;

    GraphState(const string& exp, int line, int nc) : AlgorithmState(exp, line), nodeCount(nc) {}

    string typeName() const override { return "graph"; }
    void render() const override;   // implemented in Renderer.cpp
};
// This class represents the state of a graph algorithm at a specific point in time, 
//including the state of each node and the edges between them. for dijkstra, bfs and dfs.

class SortState : public AlgorithmState {
public:
    MyVector<BarSnapshot> bars;
    ComplexityData cx;

    SortState(const string& exp, int line) : AlgorithmState(exp, line) {}

    string typeName() const override { return "sort"; }
    void render() const override;
};
// This class represents the state of a sorting algorithm at a specific point in time,
//for bubble, merge and quick sort.

class TreeState : public AlgorithmState {
public:
    MyVector<int> values;       // heap-array representation of the tree
    int insertedVal;  // the most recently inserted value
    bool isMinHeap;    // true = heap coloring, false = BST coloring

    TreeState(const string& exp, int line, bool heap = false)
        : AlgorithmState(exp, line), insertedVal(-1), isMinHeap(heap) {}

    string typeName() const override { return isMinHeap ? "heap" : "bst"; }
    void render() const override;
};
// This class represents the state of a tree-based alg at a specific point in time,
//for bst and minheap.

class MemoryState : public AlgorithmState {
public:
    MyVector<MemCell> stackCells;   // stack frames, bottom-up
    MyVector<MemCell> heapCells;    // heap allocations

    MemoryState(const string& exp, int line) : AlgorithmState(exp, line) {}

    string typeName() const override { return "memory"; }
    void render() const override;
};
// This class represents the state of memory during the execution of an algorithm,
// showing the stack frames and heap allocations at a specific point in time.


//this files sole purpose in this project is to define the various states the visualizer could be in.
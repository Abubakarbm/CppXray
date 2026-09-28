#pragma once
#include "Errors.h"
#include "MyVector.h"
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

//FILE MANAGER.
class FileManager {
public:

    static void saveSession(const string& filename,
                            const string& algoName,
                            int stepIdx,
                            int totalSteps) {
        ofstream f(filename);
        if(!f.is_open()) throw FileNotFoundError(filename);

        f << "algo="  << algoName   << "\n";
        f << "step="  << stepIdx    << "\n";
        f << "total=" << totalSteps << "\n";
        f << "saved=" << timestamp() << "\n";
        f.close();
    }
    //session saved to a text file with the algorithm name, current step index, total steps and a timestamp for when it was saved.

    static void loadSession(const string& filename, string& algoName, int& stepIdx) {
        ifstream f(filename);
        if(!f.is_open()) throw FileNotFoundError(filename);

        string line;
        while(getline(f, line)) {
            auto pos = line.find('=');
            if(pos == string::npos) continue;
            string key = line.substr(0, pos);
            string val = line.substr(pos + 1);
            if(key == "algo") algoName = val;
            if(key == "step") {
                try { stepIdx = stoi(val); }
                catch(...) { throw CorruptFileError(filename); }
            }
        }
        f.close();
    }
    //session loaded from a text file by reading the lines and parsing the key-value pairs for algorithm name and step index. It throws an error if the file is not found or if the contents are corrupted.

    static void appendLog(const string& filename, const string& algoName,
                          int comparisons, int swaps, int theoretical, int n) {
        ofstream f(filename, ios::app);
        if(!f.is_open()) return;   // silent fail for logs
        f << "[" << timestamp() << "] "
          << algoName
          << "  n="    << n
          << "  cmp="  << comparisons
          << "  swaps=" << swaps
          << "  theory=" << theoretical
          << "  pct="  << (theoretical > 0 ? comparisons * 100 / theoretical : 0) << "%\n";
        f.close();
    }
    //this appends a complexity run to a log file. 

    static bool exists(const string& filename) {
        ifstream f(filename);
        return f.good();
    }
    //this checks if a file exists by trying to open it and checking if the stream is good.

private:
    static string timestamp() {
        time_t t = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return string(buf);
    }
};

//GRAPH BUILDER
struct UserEdge { int u, v, w; };

class GraphBuilder {
public:
    int nodeCount;
    int source;
    MyVector<UserEdge> edges;

    GraphBuilder() : nodeCount(0), source(0) {}

    void setNodes(int n) {
        if(n < 2 || n > 12)
            throw InvalidGraphError("Node count must be between 2 and 12");
        nodeCount = n;
    }
    //set the number of nodes for the graph with validation.

    void setSource(int s) {
        if(s < 0 || s >= nodeCount)
            throw InvalidGraphError("Source node " + to_string(s) + " out of range [0," +
                to_string(nodeCount-1) + "]");
        source = s;
    }
    //set the number of nodes and the source node for the graph with validation.

    void addEdge(int u, int v, int w) {
        if(u < 0 || u >= nodeCount || v < 0 || v >= nodeCount)
            throw InvalidGraphError("Edge nodes out of range");
        if(u == v)
            throw InvalidGraphError("Self-loops not supported");
        if(w < 1 || w > 99)
            throw InvalidGraphError("Weight must be 1-99");
        edges.push_back({u, v, w});
    }
    //adds an edge to the graph with validation for node indices, self-loops and weight range.

    void removeEdge(int idx) {
        if(idx < 0 || idx >= static_cast<int>(edges.size()))
            throw OutOfRangeError(idx, static_cast<int>(edges.size()));
        edges.remove(idx);
    }
    //removes an edge at a specific index from the edge list.

    void clear() { edges.clear(); }
    //clears all edges from the graph.

    void saveToFile(const string& filename) const {
        ofstream f(filename);
        if(!f.is_open()) throw FileNotFoundError(filename);
        f << nodeCount << " " << source << "\n";
        for(int i = 0; i < static_cast<int>(edges.size()); i++)
            f << edges[i].u << " " << edges[i].v << " " << edges[i].w << "\n";
        f.close();
    }
    //saves graph definition to a text file.

    void loadFromFile(const string& filename) {
        ifstream f(filename);
        if(!f.is_open()) throw FileNotFoundError(filename);
        clear();

        string line;
        bool firstLine = true;
        while(getline(f, line)) {
            if(line.empty() || line[0] == '#') continue;
            istringstream ss(line);
            if(firstLine) {
                int n, s;
                if(!(ss >> n >> s))
                    throw CorruptFileError(filename);
                setNodes(n);
                setSource(s);
                firstLine = false;
            } else {
                int u, v, w;
                if(!(ss >> u >> v >> w))
                    throw CorruptFileError(filename);
                addEdge(u, v, w);
            }
        }
        f.close();
    }
    // Loads graph definition from file
    // Format: first line = "nodeCount source"
    //         each next line = "u v weight"
};

//QUIZ ENGINE
struct QuizQuestion {
    string question;
    string options[4];
    int correctIndex;
    string explanation;
};
//struct for the quiz question.

class QuizEngine {
    MyVector<QuizQuestion*> questions;
    int score;
    int current;
    string topic;

public:
    QuizEngine() : score(0), current(0) {}

    ~QuizEngine() {
        for(int i = 0; i < static_cast<int>(questions.size()); i++)
            delete questions[i];
    }

    // ── Load questions for a given algorithm topic ──
    void loadTopic(const string& t) {
        topic = t;
        for(int i = 0; i < static_cast<int>(questions.size()); i++)
            delete questions[i];
        questions.clear();
        score = 0;
        current = 0;

        if(t == "dijkstra") {
            add("Dijkstra uses a priority queue. What does "
                "it guarantee about extracted nodes?",
                "They are always unvisited",
                "They have the minimum tentative distance",
                "They are adjacent to the source",
                "They have been visited before",
                1, "The PQ always gives the node with the "
                   "smallest known distance — the greedy choice.");

            add("When do we update dist[v] during relaxation?",
                "We always update dist[v]",
                "Only if dist[u]+w < dist[v]",
                "Only if v is unvisited",
                "Never update manually",
                1, "Relaxation: only update if we found a "
                   "strictly shorter path.");

            add("Why does Dijkstra FAIL on negative edges?",
                "It crashes immediately",
                "A finalized node could have a shorter path "
                "via a negative edge discovered later",
                "It runs in an infinite loop",
                "The priority queue overflows",
                1, "Once popped, a node is final. Negative "
                   "edges can invalidate this assumption.");

            add("Time complexity with a binary heap?",
                "O(V²)",
                "O(E log V)",
                "O((V+E) log V)",
                "O(V log E)",
                2, "O((V+E) log V) — each relaxation costs "
                   "O(log V) for the heap update.");
        }
        else if(t == "bubble") {
            add("After k passes, what is guaranteed?",
                "First k elements are sorted",
                "Last k elements are in their final position",
                "The array is fully sorted",
                "Nothing is guaranteed",
                1, "Each pass bubbles the largest unsorted "
                   "element to its final position at the end.");

            add("Best-case complexity with early-exit?",
                "O(n²)", "O(n log n)", "O(n)", "O(1)",
                2, "If no swaps occur in a pass the array is "
                   "already sorted — early exit gives O(n).");

            add("Why is bubble sort considered slow?",
                "Too much memory",
                "O(n²) comparisons in worst/average case",
                "Cannot handle duplicates",
                "It is unstable",
                1, "O(n²) makes it impractical for n > ~1000.");
        }
        else if(t == "quick") {
            add("What is the role of the pivot?",
                "It is always the median",
                "It partitions elements into <=pivot and >pivot",
                "It marks the sorted portion",
                "It is always the minimum element",
                1, "Pivot divides: elements <= go left, "
                   "> go right.");

            add("Quick Sort worst case occurs when?",
                "Input is random",
                "Input is already sorted (last-element pivot)",
                "Input is reverse sorted",
                "Input has all duplicates",
                1, "Sorted input + last-element pivot → O(n²) "
                   "because every partition is 0 vs n-1.");

            add("Quick Sort is NOT stable. What does that mean?",
                "It can crash on equal elements",
                "Equal elements may change their relative order",
                "It cannot sort negative numbers",
                "It uses extra memory",
                1, "Unstable sorts may reorder equal elements — "
                   "matters when sorting records by one key.");
        }
        else if(t == "bst") {
            add("Where does a value go if less than current node?",
                "Right subtree", "Left subtree",
                "Replace the node", "Root",
                1, "BST property: left < parent < right.");

            add("Worst-case BST height with sorted input?",
                "O(log n)",
                "O(n) — degenerates to a linked list",
                "O(n²)", "O(1)",
                1, "Inserting sorted values creates a "
                   "degenerate one-directional tree.");

            add("Average-case BST search complexity?",
                "O(1)", "O(log n)", "O(n)", "O(n log n)",
                1, "O(log n) for balanced tree. "
                   "O(n) worst case (degenerate).");
        }
        else if(t == "memory") {
            add("Where does 'int x = 5;' live?",
                "Heap", "Stack",
                "Data segment", "CPU register only",
                1, "Local variables go on the stack — "
                   "fast, auto-freed when scope ends.");

            add("What happens if you call new but never delete?",
                "Compile error", "Stack overflow",
                "Memory leak — heap never freed",
                "Automatic garbage collection",
                2, "C++ has no GC. Heap memory leaks until "
                   "the process terminates.");

            add("A dangling pointer is:",
                "A pointer that was never initialized",
                "A pointer to memory that has been freed",
                "Two pointers to the same object",
                "A pointer to stack memory",
                1, "After 'delete p', p still holds the old "
                   "address. Dereferencing it is undefined "
                   "behavior — classic C++ bug.");

            add("What is RAII?",
                "A sorting algorithm",
                "Resource Acquisition Is Initialization — "
                "destructor automatically frees memory",
                "A type of smart pointer",
                "A heap allocation strategy",
                1, "RAII ties resource lifetime to object "
                   "lifetime. Destructor auto-frees. "
                   "Basis of unique_ptr and shared_ptr.");
        }
        else {
            throw EmptyQuizError(t);
        }

        if(questions.size() == 0)
            throw EmptyQuizError(t);
    }

    bool hasNext() const { return current < static_cast<int>(questions.size()); }
    int getScore() const { return score; }
    int getCurrent() const{ return current; }
    int getTotal() const { return static_cast<int>(questions.size()); }
    const string& getTopic() const { return topic; }

    const QuizQuestion* currentQuestion() const {
        if(current >= static_cast<int>(questions.size())) return nullptr;
        return questions[current];
    }

    bool answer(int choice) {
        if(current >= static_cast<int>(questions.size())) return false;
        bool correct = (choice == questions[current]->correctIndex);
        if(correct) score++;
        current++;
        return correct;
    }
    // Returns true if the answer was correct

    void reset() { score = 0; current = 0; }

private:
    void add(const string& q, const string& o0, const string& o1,
         const string& o2, const string& o3, int ans, const string& exp) {
        QuizQuestion* qq = new QuizQuestion();
        qq->question     = q;
        qq->options[0]   = o0;  qq->options[1] = o1;
        qq->options[2]   = o2;  qq->options[3] = o3;
        qq->correctIndex = ans;
        qq->explanation  = exp;
        questions.push_back(qq);
    }
};
//the QuizEngine owns QuizQuestion pointers and is responsible for deleting them in the destructor. 
//It loads questions based on the topic, tracks the current question and score, and provides methods to answer questions and reset the quiz.

#pragma once
#include "AlgorithmState.h"
#include "MyVector.h"
#include "Containers.h"
#include "Errors.h"
#include <string>
#include <climits>
#include <cmath>

using namespace std;

class AlgorithmBase {
public:
    // ── Steps own their memory — destructor cleans up ─────
    virtual ~AlgorithmBase() {
        for(int i = 0; i < static_cast<int>(steps.size()); i++)
            delete steps[i];
    }
    //to avoid memory leaks, we loop through to delete all the steps in the destructor of the algorithm base class.

    virtual void generateSteps() = 0;
    virtual string name() const = 0;
    virtual string subtitle() const = 0;
    virtual string timeComplexity() const = 0;
    virtual string spaceComplexity() const = 0;
    virtual string algoType() const = 0;

    struct CodeLine { int ln; string code; };
    virtual MyVector<CodeLine> codeLines() const = 0;
    // this is a struct for a line of code in the algorithm, with its line number and the code itself.

    // All algorithms store their steps here
    MyVector<AlgorithmState*> steps;

protected:
    // ── Shared helper: clear and rebuild steps ─────────────
    void clearSteps() {
        for(int i = 0; i < static_cast<int>(steps.size()); i++)
            delete steps[i];
        steps.clear();
    }
    //this functions clears steps by deleting all step vectors and clearing the vectors.
};

// ── Build a GraphState snapshot from current algorithm state ──
static GraphState* makeGraphState(
    const string& explanation,
    int activeLine,
    const MyVector<int>& dist,
    const MyVector<NodeState>& nodeStates,
    const MyVector<GraphEdge>& edges){
    auto* s = new GraphState(explanation, activeLine, static_cast<int>(nodeStates.size()));

    for(int i = 0; i < static_cast<int>(nodeStates.size()); i++)
        s->nodes.push_back(NodeSnapshot(i, nodeStates[i], dist[i]));

    for(int i = 0; i < static_cast<int>(edges.size()); i++)
        s->edges.push_back(edges[i]);

    return s;
}
// this is a helper function that creates a GraphState from the current state of the algorithm, 
//returns a pointer to the new graphstate.
//this also serves as the base class for all the graphs in the algorithms and how they are visualized in the renderer.

//Algorithm 1: DIJKSTRA.
class DijkstraAlgorithm : public AlgorithmBase {
public:
    struct Edge { int u, v, w; };

    int            nodeCount;
    int            src;
    MyVector<Edge> edgeList;

    DijkstraAlgorithm(int n, int source) : nodeCount(n), src(source) {
        if(n < 2)
            throw InvalidGraphError("Node count must be >= 2");
        if(source < 0 || source >= n)
            throw InvalidGraphError("Source node out of range");
    }
    // constructor for Dijkstra's algorithm which takes the number of nodes and the source node as parameters.

    void addEdge(int u, int v, int w) {
        if(u < 0 || u >= nodeCount || v < 0 || v >= nodeCount)
            throw InvalidGraphError("Edge endpoints out of range");
        edgeList.push_back({u, v, w});
    }
    //this adds an edge to the graph.

    string name() const override { return "Dijkstra's Shortest Path"; }
    string subtitle() const override {
        return "source: " + to_string(src) + "  nodes: " + to_string(nodeCount);
    }
    string timeComplexity() const override { return "O((V+E) log V)"; }
    string spaceComplexity() const override { return "O(V)"; }
    string algoType() const override { return "Greedy / Priority Queue"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1,  "void dijkstra(Graph& g, int src) {"},
            {2,  "  priority_queue<pair<int,int>> pq;"},
            {3,  "  vector<int> dist(V, INT_MAX);"},
            {4,  "  dist[src]=0; pq.push({0, src});"},
            {5,  "  while (!pq.empty()) {"},
            {6,  "    auto [d,u] = pq.top(); pq.pop();"},
            {7,  "    for (auto [v,w] : g[u]) {"},
            {8,  "      if (dist[u]+w < dist[v]) {"},
            {9,  "        dist[v] = dist[u]+w;"},
            {10, "        pq.push({dist[v], v});"},
            {11, "      }"},
            {12, "    }"},
            {13, "  }"},
            {14, "}"}
        };
    }

    void generateSteps() override {
        clearSteps();

        // Initialises distances and states
        MyVector<int> dist(nodeCount);
        MyVector<NodeState> state(nodeCount);
        MyVector<bool> visited(nodeCount);

        for(int i = 0; i < nodeCount; i++) {
            dist[i] = (i == src) ? 0 : INT_MAX;
            state[i] = (i == src) ? NodeState::SOURCE : NodeState::UNVISITED;
            visited[i] = false;
        }

        // Builds edge list once for all snapshots
        MyVector<GraphEdge> gEdges;
        for(int i = 0; i < static_cast<int>(edgeList.size()); i++)
            gEdges.push_back({edgeList[i].u, edgeList[i].v, edgeList[i].w});

        // Step 0 — initial state
        steps.push_back(makeGraphState("Init: dist[" + to_string(src) + "]=0, "
            "all others=INF. Push source into priority queue.", 4, dist, state, gEdges));

        // Runs Dijkstra using the custom MyMinHeap
        // pair<int,int> = {distance, nodeId}
        MyMinHeap<pair<int,int>> pq;
        pq.insert({0, src});
        // this is the main loop of Dijkstra's algorithm which continues until the priority queue is empty.

        while(!pq.empty()) {
            auto [d, u] = pq.extractMin();

            if(visited[u]) continue;
            visited[u] = true;
            state[u]   = NodeState::CURRENT;
            // Skips stale entries which are the nodes already finalized.

            steps.push_back(makeGraphState(
                "Pop node " + to_string(u) +
                " (dist=" + (d == INT_MAX ? "INF" : to_string(d)) +
                "). Now relax its neighbors.",
                6, dist, state, gEdges));

            // Relax all edges incident to u
            for(int i = 0; i < static_cast<int>(edgeList.size()); i++) {
                auto& e = edgeList[i];
                int nb = -1, nw = 0;
                if(e.u == u) { nb = e.v; nw = e.w; }
                else if(e.v == u) { nb = e.u; nw = e.w; }
                if(nb < 0 || visited[nb]) continue;

                if(dist[u] != INT_MAX && dist[u] + nw < dist[nb]) {
                    dist[nb]  = dist[u] + nw;
                    state[nb] = NodeState::IN_QUEUE;
                    pq.insert({dist[nb], nb});

                    steps.push_back(makeGraphState(
                        "Relax edge (" +
                        to_string(u) + "->" + to_string(nb) +
                        ", w=" + to_string(nw) + "). dist[" +
                        to_string(nb) + "]=" + to_string(dist[nb]) + "  Updated!",
                        9, dist, state, gEdges));
                }
                else {
                    steps.push_back(makeGraphState(
                        "Relax edge (" +
                        to_string(u) + "->" + to_string(nb) +
                        ", w=" + to_string(nw) + "). No update needed.",
                        9, dist, state, gEdges));
                }
            }

            state[u] = NodeState::VISITED;
        }

        steps.push_back(makeGraphState(
            "Dijkstra complete! All shortest paths from source " +
            to_string(src) + " found.", 13, dist, state, gEdges));
    }

    // The default 6-node weighted graph used on startup
    static DijkstraAlgorithm* defaultGraph() {
        auto* d = new DijkstraAlgorithm(6, 0);
        d->addEdge(0,1,4); d->addEdge(0,2,2);
        d->addEdge(1,3,5); d->addEdge(1,2,1);
        d->addEdge(2,4,8); d->addEdge(3,5,2);
        d->addEdge(4,5,6); d->addEdge(3,4,3);
        return d;
    }
};

//Algorithm 2: BFS.
class BFSAlgorithm : public AlgorithmBase {
public:
    string name() const override { return "BFS — Breadth-First Search"; }
    string subtitle() const override { return "source: 0  explores level by level"; }
    string timeComplexity() const override { return "O(V + E)"; }
    string spaceComplexity()const override { return "O(V)"; }
    string algoType() const override { return "Queue-based traversal"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1,  "void bfs(Graph& g, int src) {"},
            {2,  "  queue<int> q;"},
            {3,  "  vector<bool> visited(V, false);"},
            {4,  "  visited[src]=true; q.push(src);"},
            {5,  "  while (!q.empty()) {"},
            {6,  "    int u = q.front(); q.pop();"},
            {7,  "    for (int v : g[u])"},
            {8,  "      if (!visited[v]) {"},
            {9,  "        visited[v]=true; q.push(v);"},
            {10, "      }"},
            {11, "  }"},
            {12, "}"}
        };
    }
    // this is the code for BFS algorithm which uses a queue to explore the graph level by level starting from the source node. 

    void generateSteps() override {
        clearSteps();

        // Fixed 6-node undirected graph
        // adj[u] lists neighbors of u
        int adj[6][4] = {
            {1, 2, -1, -1},
            {0, 2,  3, -1},
            {0, 1,  4, -1},
            {1, 5, -1, -1},
            {2, 5, -1, -1},
            {3, 4, -1, -1}
        };
        MyVector<GraphEdge> edges;
        edges.push_back({0,1,0}); edges.push_back({0,2,0});
        edges.push_back({1,2,0}); edges.push_back({1,3,0});
        edges.push_back({2,4,0}); edges.push_back({3,5,0});
        edges.push_back({4,5,0});

        MyVector<int> dist(6);
        MyVector<NodeState> state(6);
        MyVector<bool> vis(6);
        for(int i = 0; i < 6; i++) {
            dist[i]  = INT_MAX;
            state[i] = NodeState::UNVISITED;
            vis[i] = false;
        }

        // Mark source
        state[0] = NodeState::SOURCE;
        vis[0] = true;
        dist[0] = 0;

        steps.push_back(makeGraphState( "Init BFS from node 0. Mark visited. "
            "Queue: [0]",
            4, dist, state, edges));

        // BFS using our custom MyQueue — level tracked by counting frontier size
        MyQueue<int> q;
        q.enqueue(0);
        int level        = 0;
        int levelSize    = 1;   // nodes remaining in current BFS level
        int nextLevelSz  = 0;   // nodes queued for next level

        while(!q.empty()) {
            int u = q.front();
            q.dequeue();
            levelSize--;
            state[u] = NodeState::CURRENT;

            // Build queue string for explanation
            string qStr = "Queue: [";
            auto* node = q.raw().getHead();
            bool first = true;
            while(node) {
                if(!first) qStr += ",";
                qStr += to_string(node->value);
                first = false;
                node  = node->next;
            }
            qStr += "]";

            steps.push_back(makeGraphState(
                "Dequeue " + to_string(u) +
                " (level " + to_string(level) +
                "). Process its unvisited neighbors.  " + qStr,
                6, dist, state, edges));

            // Enqueue unvisited neighbors
            for(int k = 0; k < 4 && adj[u][k] != -1; k++) {
                int v = adj[u][k];
                if(!vis[v]) {
                    vis[v]   = true;
                    dist[v]  = level + 1;
                    state[v] = NodeState::IN_QUEUE;
                    q.enqueue(v);
                    nextLevelSz++;

                    steps.push_back(makeGraphState(
                        "Enqueue neighbor " + to_string(v) +
                        ". BFS level " + to_string(level + 1) +
                        ", dist=" + to_string(level + 1) + ".  " + qStr,
                        9, dist, state, edges));
                }
            }

            state[u] = NodeState::VISITED;

            // Advance BFS level when current frontier is exhausted
            if(levelSize == 0) {
                level++;
                levelSize   = nextLevelSz;
                nextLevelSz = 0;
            }
        }

        steps.push_back(makeGraphState(
            "BFS complete! All nodes visited level by level.",
            11, dist, state, edges));
    }
};

//Algoritm 3: DFS
class DFSAlgorithm : public AlgorithmBase {
public:
    string name() const override { return "DFS — Depth-First Search"; }
    string subtitle() const override { return "source: 0 recurse before width"; }
    string timeComplexity() const override { return "O(V + E)"; }
    string spaceComplexity() const override { return "O(V)"; }
    string algoType() const override { return "Recursive / Call Stack"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "void dfs(Graph& g, int u,"},
            {2, "         vector<bool>& vis) {"},
            {3, "  vis[u] = true;"},
            {4, "  for (int v : g[u])"},
            {5, "    if (!vis[v])"},
            {6, "      dfs(g, v, vis);"},
            {7, "}"}
        };
    }
    // this is the code for DFS algorithm which uses recursion to explore the graph depth by depth starting from the source node.

    void generateSteps() override {
        clearSteps();

        MyVector<GraphEdge> edges;
        edges.push_back({0,1,0}); edges.push_back({0,2,0});
        edges.push_back({1,3,0}); edges.push_back({2,4,0});
        edges.push_back({3,5,0}); edges.push_back({4,5,0});

        MyVector<int>       dist(6);
        MyVector<NodeState> state(6);
        MyVector<bool>      vis(6);
        for(int i = 0; i < 6; i++) {
            dist[i]  = INT_MAX;
            state[i] = NodeState::UNVISITED;
            vis[i]   = false;
        }

        // Pre-computed DFS order for this fixed graph
        int  dfsOrder[] = {0, 1, 3, 5, 2, 4};
        string callStacks[] = {
            "[ main() → dfs(0) ]",
            "[ dfs(0) → dfs(1) ]",
            "[ dfs(1) → dfs(3) ]",
            "[ dfs(3) → dfs(5) ]",
            "[ dfs(0) → dfs(2) ]",
            "[ dfs(2) → dfs(4) ]"
        };

        for(int step = 0; step < 6; step++) {
            int u     = dfsOrder[step];
            vis[u]    = true;
            state[u]  = NodeState::CURRENT;

            // Mark previous current nodes as visited
            for(int k = 0; k < step; k++)
                if(state[dfsOrder[k]] == NodeState::CURRENT)
                    state[dfsOrder[k]] = NodeState::VISITED;

            steps.push_back(makeGraphState(
                "dfs(" + to_string(u) + "). Mark visited. "
                "Call stack: " + callStacks[step],
                step == 0 ? 3 : 6,
                dist, state, edges));
        }

        for(int i = 0; i < 6; i++) state[i] = NodeState::VISITED;
        steps.push_back(makeGraphState("DFS complete! All 6 nodes visited. "
            "Backtracking unwound the call stack.",
            7, dist, state, edges));
    }
};

//Algorithm 4: BubbleSort
class BubbleSortAlgorithm : public AlgorithmBase {
    MyVector<int> inputArr;
    ComplexityData cx;

public:
    explicit BubbleSortAlgorithm(MyVector<int> arr) : inputArr(arr) {}

    string name() const override { return "Bubble Sort"; }
    string subtitle() const override { return "adjacent comparisons  stable  in-place"; }
    string timeComplexity() const override { return "O(n²)"; }
    string spaceComplexity() const override { return "O(1)"; }
    string algoType() const override { return "Comparison Sort"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "void bubbleSort(vector<int>& a) {"},
            {2, "  int n = a.size();"},
            {3, "  for (int i=0; i<n-1; i++)"},
            {4, "    for (int j=0; j<n-1-i; j++)"},
            {5, "      if (a[j] > a[j+1])"},
            {6, "        swap(a[j], a[j+1]);"},
            {7, "}"}
        };
    }
    // this is the code for Bubble Sort algorithm which uses adjacent comparisons to sort the array in place. It has a time complexity of O(n^2) and space complexity of O(1).

    void generateSteps() override {
        clearSteps();
        MyVector<int> arr = inputArr;
        int n = static_cast<int>(arr.size());

        cx.n = n;
        cx.comparisons = 0;
        cx.swaps = 0;
        cx.theoretical = n * (n - 1) / 2;
        cx.algoName = "bubble";

        auto snap = [&](int hl1, int hl2, int sortedFrom,
                        const string& exp, int line) {
            auto* s = new SortState(exp, line);
            for(int i = 0; i < n; i++) {
                bool hl     = (i == hl1 || i == hl2);
                bool sorted = (sortedFrom >= 0 && i >= sortedFrom);
                s->bars.push_back(BarSnapshot(arr[i], hl, sorted, false));
            }
            s->cx = cx;
            steps.push_back(s);
        };
        // Helper lambda: snapshot current array state

        snap(-1, -1, -1, "Bubble Sort: n=" + to_string(n) +
             "  theoretical max comparisons = " + to_string(cx.theoretical), 1);

        for(int i = 0; i < n - 1; i++) {
            for(int j = 0; j < n - 1 - i; j++) {
                cx.comparisons++;
                bool swapped = arr[j] > arr[j + 1];
                if(swapped) {
                    cx.swaps++;
                    int tmp = arr[j]; arr[j] = arr[j+1]; arr[j+1] = tmp;
                }
                snap(j, j + 1, n - i,
                     "Compare a[" + to_string(j) +
                     "]=" + to_string(arr[j]) +
                     " vs a[" + to_string(j+1) +
                     "]=" + to_string(arr[j+1]) +
                     (swapped ? "  → SWAP!  swaps=" + to_string(cx.swaps)
                         : "  → In order.") + "  cmp=" + to_string(cx.comparisons),
                     swapped ? 6 : 5);
            }
        }

        auto* fin = new SortState(
            "Sorted! " +
            to_string(cx.comparisons) + " comparisons, " +
            to_string(cx.swaps) + " swaps. " +
            "Theory said max " + to_string(cx.theoretical) + ".", 7);
        for(int i = 0; i < n; i++)
            fin->bars.push_back(BarSnapshot(arr[i], false, true, false));
        fin->cx = cx;
        steps.push_back(fin);
    }
};

//Algorithm 5: Merge Sort
class MergeSortAlgorithm : public AlgorithmBase {
    MyVector<int> inputArr;
    ComplexityData cx;
    int n;

public:
    explicit MergeSortAlgorithm(MyVector<int> arr) : inputArr(arr), n(static_cast<int>(arr.size())) {}

    string name() const override { return "Merge Sort"; }
    string subtitle() const override { return "divide & conquer  stable  O(n log n)"; }
    string timeComplexity() const override { return "O(n log n)"; }
    string spaceComplexity() const override { return "O(n)"; }
    string algoType() const override { return "Divide & Conquer"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "void mergeSort(vec& a, int l, int r) {"},
            {2, "  if (l >= r) return;"},
            {3, "  int m = (l+r)/2;"},
            {4, "  mergeSort(a, l, m);"},
            {5, "  mergeSort(a, m+1, r);"},
            {6, "  merge(a, l, m, r);"},
            {7, "}"}
        };
    }

    void generateSteps() override {
        clearSteps();
        MyVector<int> arr = inputArr;
        cx.n = n;
        cx.comparisons = 0;
        cx.swaps = 0;
        cx.theoretical = (int)(n * log2(n + 1));
        cx.algoName = "merge";

        addSnap(arr, {}, {}, "Merge Sort: divide into halves, merge sorted. "
                "O(n log n) = ~" + to_string(cx.theoretical) + " ops.", 1);
        msort(arr, 0, n - 1);

        auto* fin = new SortState(
            "Sorted! " + to_string(cx.comparisons) +
            " comparisons vs theory " + to_string(cx.theoretical) + ".", 7);
        for(int i = 0; i < n; i++)
            fin->bars.push_back(BarSnapshot(arr[i], false, true, false));

        fin->cx = cx;
        steps.push_back(fin);
    }

private:
    void addSnap(MyVector<int>& arr, MyVector<int> highlight, MyVector<int> sortedIdx,
        const string& exp, int line) {
        auto* s = new SortState(exp, line);
        for(int i = 0; i < n; i++) {
            bool hl = (highlight.find(i) >= 0);
            bool sorted = (sortedIdx.find(i) >= 0);
            s->bars.push_back(BarSnapshot(arr[i], hl, sorted, false));
        }

        s->cx = cx;
        steps.push_back(s);
    }

    void msort(MyVector<int>& arr, int l, int r) {
        if(l >= r) return;
        int m = (l + r) / 2;

        // Show the divide step
        MyVector<int> leftHL, rightHL;
        for(int i = l; i <= m;   i++) leftHL.push_back(i);
        for(int i = m+1; i <= r; i++) rightHL.push_back(i);

        addSnap(arr, leftHL, {},
            "Divide [" + to_string(l) + "," + to_string(r) +
            "] → left[" + to_string(l) + "," + to_string(m) +
            "] right[" + to_string(m+1) + "," + to_string(r) + "]", 3);

        msort(arr, l,   m);
        msort(arr, m+1, r);

        // Merge step
        MyVector<int> temp;
        int i = l, j = m + 1;
        while(i <= m && j <= r) {
            cx.comparisons++;
            if(arr[i] <= arr[j]) temp.push_back(arr[i++]);
            else { temp.push_back(arr[j++]); cx.swaps++; }
        }

        while(i <= m) temp.push_back(arr[i++]);
        while(j <= r) temp.push_back(arr[j++]);
        for(int k = l; k <= r; k++) arr[k] = temp[k - l];

        MyVector<int> merged;
        for(int k = l; k <= r; k++) merged.push_back(k);
        addSnap(arr, merged, merged,
            "Merged [" + to_string(l) + "," + to_string(r) +
            "]. cmp=" + to_string(cx.comparisons), 6);
    }
};

//Algorithm 6: Quick Sort
class QuickSortAlgorithm : public AlgorithmBase {
    MyVector<int> inputArr;
    ComplexityData cx;
    int n;
    MyVector<bool> sortedMask;

public:
    explicit QuickSortAlgorithm(MyVector<int> arr)
        : inputArr(arr), n(static_cast<int>(arr.size())), sortedMask(static_cast<int>(arr.size())) {}

    string name() const override { return "Quick Sort"; }
    string subtitle() const override { return "pivot partitioning  in-place  O(n log n) avg"; }
    string timeComplexity() const override { return "O(n log n) avg, O(n²) worst"; }
    string spaceComplexity() const override { return "O(log n)"; }
    string algoType() const override { return "Divide & Conquer"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "int partition(vec& a, int l, int r) {"},
            {2, "  int pivot=a[r], i=l-1;"},
            {3, "  for (int j=l; j<r; j++)"},
            {4, "    if (a[j]<=pivot) swap(a[++i],a[j]);"},
            {5, "  swap(a[i+1], a[r]);"},
            {6, "  return i+1;"},
            {7, "}"}
        };
    }

    void generateSteps() override {
        clearSteps();
        MyVector<int> arr = inputArr;
        for(int i = 0; i < n; i++) sortedMask[i] = false;

        cx.n = n;
        cx.comparisons = 0;
        cx.swaps = 0;
        cx.theoretical = (int)(n * log2(n + 1));
        cx.algoName = "quick";

        addSnap(arr, -1, -1, -1, "Quick Sort: pick last element as pivot, "
                "partition around it.", 1);
        qsort(arr, 0, n - 1);

        auto* fin = new SortState("Sorted! " + to_string(cx.comparisons) +
            " comparisons, " + to_string(cx.swaps) + " swaps.", 7);

        for(int i = 0; i < n; i++)
            fin->bars.push_back(BarSnapshot(arr[i], false, true, false));

        fin->cx = cx;
        steps.push_back(fin);
    }

private:
    void addSnap(MyVector<int>& arr, int hl1, int hl2, int pivotIdx, const string& exp, int line){
        auto* s = new SortState(exp, line);
        for(int i = 0; i < n; i++) {
            bool isPiv = (i == pivotIdx);
            bool isHL = (i == hl1 || i == hl2) && !isPiv;
            s->bars.push_back(BarSnapshot(arr[i], isHL, sortedMask[i], isPiv));
        }
        s->cx = cx;
        steps.push_back(s);
    }
    //adds a snapshot of the current state of the array, highlighting the pivot and the elements being compared.

    void qsort(MyVector<int>& arr, int l, int r) {
        if(l >= r) {
            if(l == r) sortedMask[l] = true;
            return;
        }

        int pivot = arr[r];
        addSnap(arr, l, r, r,
            "Pivot = a[" + to_string(r) + "]=" + to_string(pivot) +
            "  Partition range [" + to_string(l) + "," +
            to_string(r) + "]", 2);

        int idx = l - 1;

        for(int j = l; j < r; j++) {
            cx.comparisons++;
            if(arr[j] <= pivot) {
                idx++;
                cx.swaps++;
                int tmp = arr[idx]; arr[idx] = arr[j]; arr[j] = tmp;
                addSnap(arr, idx, j, r,
                    "a[" + to_string(j) + "]=" + to_string(arr[j]) +
                    " <= pivot(" + to_string(pivot) +
                    ")  swap a[" + to_string(idx) +
                    "] ↔ a[" + to_string(j) + "]", 4);
            }
        }

        // Place pivot in final position
        cx.swaps++;
        int tmp = arr[idx+1]; arr[idx+1] = arr[r]; arr[r] = tmp;
        sortedMask[idx+1] = true;
        addSnap(arr, idx+1, -1, idx+1,
            "Pivot " + to_string(arr[idx+1]) +
            " placed at index " + to_string(idx+1) +
            ". Recurse on left and right halves.", 5);

        qsort(arr, l, idx);
        qsort(arr, idx+2, r);
    }
};

//Algorithm 7: BST (Binary Search Tree)
class BSTAlgorithm : public AlgorithmBase {
    MyVector<int> insertOrder;

public:
    explicit BSTAlgorithm(MyVector<int> order) : insertOrder(order) {}

    string name() const override { return "Binary Search Tree — Insert"; }
    string subtitle() const override { return "left < root < right  new Node() per insert"; }
    string timeComplexity() const override { return "O(log n) avg, O(n) worst"; }
    string spaceComplexity() const override { return "O(n)"; }
    string algoType() const override { return "Tree Structure"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "Node* insert(Node* root, int val) {"},
            {2, "  if (!root) return new Node(val);"},
            {3, "  if (val < root->val)"},
            {4, "    root->left = insert(root->left, val);"},
            {5, "  else"},
            {6, "    root->right = insert(root->right, val);"},
            {7, "  return root;"},
            {8, "}"}
        };
    }

    void generateSteps() override {
        clearSteps();
        MyVector<int> tree;   // heap-array BST representation

        auto snap = [&](int insertedVal, const string& exp, int line) {
            auto* s = new TreeState(exp, line, false);
            s->values = tree;
            s->insertedVal = insertedVal;
            steps.push_back(s);
        };

        snap(-1, "Build BST. Inserting: [" +
             joinVec(insertOrder) + "]. BST property: left < root < right.", 1);

        for(int i = 0; i < static_cast<int>(insertOrder.size()); i++) {
            int v = insertOrder[i];
            bstInsert(tree, v);

            string direction =
                (i == 0) ? "Tree was empty. Becomes root. new Node(" + to_string(v) + ")." :
                (v < tree[0]) ? "v < root → go LEFT." : "v > root → go RIGHT.";

            snap(v, "Insert " + to_string(v) + ". " + direction, i == 0 ? 2 : (v < tree[0] ? 4 : 6));
        }
    }

private:
    string joinVec(const MyVector<int>& v) const {
        string s;
        for(int i = 0; i < static_cast<int>(v.size()); i++) {
            s += to_string(v[i]);
            if(i < static_cast<int>(v.size()) - 1) s += ",";
        }
        return s;
    }

    void bstInsert(MyVector<int>& tree, int val) {
        if(tree.empty()) { tree.push_back(val); return; }
        int i = 0;

        while(i < static_cast<int>(tree.size())) {
            if(tree[i] == -1) { tree[i] = val; return; }
            if(val < tree[i]) {
                int left = 2*i + 1;
                while(static_cast<int>(tree.size()) <= left) tree.push_back(-1);
                if(tree[left] == -1) { tree[left] = val; return; }
                i = left;
            }
            else {
                int right = 2*i + 2;
                while(static_cast<int>(tree.size()) <= right) tree.push_back(-1);
                if(tree[right] == -1) { tree[right] = val; return; }
                i = right;
            }
        }
    }
};

//Algorithm 8: Min-Heap
class HeapAlgorithm : public AlgorithmBase {
    MyVector<int> insertOrder;

public:
    explicit HeapAlgorithm(MyVector<int> order) : insertOrder(order) {}

    string name() const override { return "Min-Heap — Insert & Heapify"; }
    string subtitle() const override { return "parent <= children  complete binary tree"; }
    string timeComplexity() const override { return "O(log n) per insert"; }
    string spaceComplexity() const override { return "O(n)"; }
    string algoType() const override { return "Priority Queue / Heap"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "void heapifyUp(vector<int>& h, int i) {"},
            {2, "  while (i > 0) {"},
            {3, "    int p = (i-1)/2;"},
            {4, "    if (h[p] > h[i]) {"},
            {5, "      swap(h[p], h[i]);"},
            {6, "      i = p;"},
            {7, "    } else break;"},
            {8, "  }"},
            {9, "}"}
        };
    }

    void generateSteps() override {
        clearSteps();
        MyVector<int> heap;

        auto snap = [&](int insertedVal, const string& exp, int line) {
            auto* s = new TreeState(exp, line, true);
            s->values = heap;
            s->insertedVal = insertedVal;
            steps.push_back(s);
        };

        snap(-1, "Min-Heap: parent is always <= children. " "We heapify-up after every insert.", 1);

        for(int i = 0; i < static_cast<int>(insertOrder.size()); i++) {
            int v = insertOrder[i];
            heap.push_back(v);

            int j = static_cast<int>(heap.size()) - 1;
            int swapCount = 0;
            while(j > 0) {
                int p = (j - 1) / 2;
                if(heap[p] > heap[j]) {
                    int tmp = heap[p]; heap[p] = heap[j]; heap[j] = tmp;
                    j = p;
                    swapCount++;
                } else break;
            }
            // Run heapify-up — same logic as MyMinHeap's insert().

            string exp = "Insert " + to_string(v) + ". ";
            if(swapCount == 0)
                exp += "Already in correct position. No swaps.";
            else
                exp += to_string(swapCount) + " swap(s) to restore heap property. " 
                "Parent index = (i-1)/2.";

            snap(v, exp, swapCount > 0 ? 5 : 7);
        }
    }
};

//Algorithm 9: Memory X-Ray
class MemoryXRayAlgorithm : public AlgorithmBase {
public:
    string name() const override { return "Memory X-Ray — Stack vs Heap"; }
    string subtitle() const override { return "int x=5  new int  foo()  delete"; }
    string timeComplexity() const override { return "O(1) per operation"; }
    string spaceComplexity() const override { return "Stack: auto  Heap: manual"; }
    string algoType() const override { return "C++ Memory Model"; }

    MyVector<CodeLine> codeLines() const override {
        return {
            {1, "int main() {"},
            {2, "  int x = 5;  // stack"},
            {3, "  int* p = new int(42);  // heap"},
            {4, "  foo(x);  // call stack"},
            {5, "  delete p;  // dealloc"},
            {6, "  return 0;"},
            {7, "}"}
        };
    }

    void generateSteps() override {
        clearSteps();

        // Step 1: int x = 5
        {
            auto* s = new MemoryState(
                "int x = 5;  Allocated on the STACK inside main()'s frame. "
                "Stack memory is fast and automatically freed when the function returns.", 2);
            s->stackCells.push_back(MemCell("0x7ffe10","main::x","5",MemCellType::STACK_FRAME));
            steps.push_back(s);
        }

        // Step 2: int* p = new int(42)
        {
            auto* s = new MemoryState(
                "int* p = new int(42);  The POINTER p lives on the stack. "
                "The VALUE 42 is on the HEAP at address 0x1f40. "
                "The OS allocates heap memory at runtime with 'new'.", 3);
            s->stackCells.push_back(
                MemCell("0x7ffe10","main::x","5", MemCellType::STACK_FRAME));
            s->stackCells.push_back(
                MemCell("0x7ffe08","main::p","0x1f40", MemCellType::STACK_FRAME));
            s->heapCells.push_back(
                MemCell("0x1f40","*p (int)","42", MemCellType::HEAP_ALLOC));
            steps.push_back(s);
        }

        // Step 3: foo(x)
        {
            auto* s = new MemoryState(
                "foo(x);  A new stack frame is PUSHED for foo(). "
                "The value of x is COPIED by value — foo gets its own copy. "
                "When foo() returns, its frame is automatically popped.", 4);
            s->stackCells.push_back(
                MemCell("0x7ffe10","main::x","5", MemCellType::STACK_FRAME));
            s->stackCells.push_back(
                MemCell("0x7ffe08","main::p","0x1f40", MemCellType::STACK_FRAME));
            s->stackCells.push_back(
                MemCell("0x7ffd90","foo::x","5 (copy)", MemCellType::STACK_FRAME));
            s->heapCells.push_back(
                MemCell("0x1f40","*p (int)","42", MemCellType::HEAP_ALLOC));
            steps.push_back(s);
        }

        // Step 4: delete p
        {
            auto* s = new MemoryState(
                "delete p;  Heap at 0x1f40 is FREED. "
                "p is now a DANGLING POINTER — it still holds the old address "
                "but the memory no longer belongs to us. "
                "Dereferencing it is undefined behavior. "
                "Without this delete: MEMORY LEAK!", 5);
            s->stackCells.push_back(
                MemCell("0x7ffe10","main::x","5", MemCellType::STACK_FRAME));
            s->stackCells.push_back(
                MemCell("0x7ffe08","main::p","0x1f40 ?!", MemCellType::STACK_FRAME));
            s->heapCells.push_back(
                MemCell("0x1f40","*p FREED","42", MemCellType::HEAP_FREED));
            steps.push_back(s);
        }
        //

        // Step 5: return 0
        {
            auto* s = new MemoryState(
                "return 0;  main()'s stack frame is popped. "
                "All local variables gone. No leaks detected. "
                "Golden rule: every 'new' must have exactly one matching 'delete'. "
                "In modern C++ use RAII / smart pointers to automate this.", 6);
            s->heapCells.push_back(
                MemCell("0x1f40","freed","---",MemCellType::HEAP_FREED));
            steps.push_back(s);
        }
    }
};
// This is the code for the Memory X-Ray algorithm which visualizes the stack and heap memory in C++. 
//It shows how variables are allocated on the stack, how dynamic memory is allocated on the heap, 
// and the effects of function calls and memory deallocation.
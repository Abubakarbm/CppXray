<div align="center">

```
 ██████╗██████╗ ██████╗ ██╗  ██╗██████╗  █████╗ ██╗   ██╗
██╔════╝██╔══██╗██╔══██╗╚██╗██╔╝██╔══██╗██╔══██╗╚██╗ ██╔╝
██║     ██████╔╝██████╔╝ ╚███╔╝ ██████╔╝███████║ ╚████╔╝
██║     ██╔═══╝ ██╔═══╝  ██╔██╗ ██╔══██╗██╔══██║  ╚██╔╝
╚██████╗██║     ██║     ██╔╝ ██╗██║  ██║██║  ██║   ██║
 ╚═════╝╚═╝     ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝
```

# C++.xray

**An interactive terminal visualiser for algorithms, data structures, and C++ memory.**

It lets you see what your code is doing, one step at a time.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)
![Dependencies](https://img.shields.io/badge/dependencies-none-brightgreen.svg)
![Award](https://img.shields.io/badge/%F0%9F%8F%86-Best%20Project%20in%20the%20Lab-gold.svg)

</div>

---


## Table of Contents

- [What is C++.xray?](#what-is-cxray)
- [Why it's useful](#why-its-useful)
- [Features at a glance](#features-at-a-glance)
- [Requirements](#requirements)
- [Building and running](#building-and-running)
- [How to use it](#how-to-use-it)
  - [The main menu](#the-main-menu)
  - [Stepping through an algorithm](#stepping-through-an-algorithm)
  - [Reading the screen](#reading-the-screen)
  - [Colour legend](#colour-legend)
  - [Custom Graph Editor](#custom-graph-editor)
  - [Quiz Mode](#quiz-mode)
  - [Complexity Lab](#complexity-lab)
  - [Saving your session](#saving-your-session)
  - [Themes and settings](#themes-and-settings)
- [A guided first session](#a-guided-first-session)
- [What each visualization teaches](#what-each-visualization-teaches)
- [Files the program creates](#files-the-program-creates)
- [Project structure](#project-structure)
- [How it works under the hood](#how-it-works-under-the-hood)
- [Troubleshooting](#troubleshooting)
- [Ideas for the future](#ideas-for-the-future)
- [Acknowledgements](#acknowledgements)

---

## What is C++.xray?

C++.xray is a terminal application that animates classic computer science algorithms **step by step**, right in your terminal. Each step shows you four things together:

1. **A picture** of the current state: a graph, a set of bars, a tree, or a memory layout.
2. **The code**, with the line that is running right now highlighted.
3. **A plain-English explanation** of what just happened and why.
4. **Live counters**, such as distances, comparisons, swaps, and how close the run is to the theoretical Big-O bound.

You can move forward and backward through the steps, press play and watch it run, bookmark interesting moments, and quiz yourself when you're done.

It's called *x-ray* because the point is to see inside things that are normally invisible, like the priority queue in Dijkstra, the call stack in DFS, or the heap in C++.

---

## Why it's useful

**If you're a student:**
- Algorithms are hard to learn from static diagrams in slides. Here you can pause at the exact moment something confusing happens, step back, and watch it again.
- The highlighted code line links the picture to the actual code, so you're not guessing which part of the algorithm is running.
- The quizzes check whether you actually understood it or just watched it.
- The Memory X-Ray explains stack versus heap, `new`/`delete`, dangling pointers, and memory leaks. Those are the C++ topics that trip up almost everyone.

**If you're a teacher or TA:**
- It runs in a plain terminal, so there's nothing to install except a compiler, and it's easy to project in class.
- The graph editor lets you build the exact example you want to talk about and run Dijkstra on it.
- The Complexity Lab gives students real comparison counts to talk about, so Big-O becomes something they can measure.

**If you're reading the code:**
- It's a compact, readable example of how to build a real-world interactive terminal UI in modern C++ without any third-party libraries.
- The custom containers show deep copies, move constructors, operator overloading, and RAII working in real code.

---

## Features at a glance

| Category | Feature | Complexity shown |
|---|---|---|
| **Graph algorithms** | Dijkstra's Shortest Path | O((V+E) log V) |
| | BFS (Breadth-First Search) | O(V+E) |
| | DFS (Depth-First Search) | O(V+E) |
| **Sorting** | Bubble Sort | O(n²) |
| | Merge Sort | O(n log n) |
| | Quick Sort | O(n log n) avg, O(n²) worst |
| **Data structures** | Binary Search Tree (insert) | O(log n) avg |
| | Min-Heap (insert and heapify-up) | O(log n) per insert |
| **C++ memory** | Memory X-Ray: stack vs heap, `new`/`delete` | O(1) per operation |
| **Extras** | Custom Graph Editor, Quiz Mode, Complexity Lab, bookmarks, auto-play, 4 themes, saved settings | |

---

## Requirements

- **A C++17 compiler.** g++ 7+ or clang++ 5+ on Linux/macOS, or MinGW-w64 g++ on Windows.
- **`make`**, optional but recommended.
- **A terminal that supports ANSI colours and UTF-8.** Most modern terminals do: GNOME Terminal, Konsole, iTerm2, macOS Terminal, Windows Terminal, and the VS Code integrated terminal.
- **A terminal window at least 74 columns wide and about 45 rows tall**, so the full visualization fits without wrapping.

There are no external libraries to install. Everything is standard C++ plus the operating system's terminal APIs.

---

## Building and running

### Linux / macOS

```bash
git clone <your-repo-url> cppxray_source
cd cppxray_source

make          # builds the executable ./cppxray_source
./cppxray_source
```

Or build and run in one step:

```bash
make run
```

To clean up build files:

```bash
make clean
```

### Without make

If you don't have `make`, you can compile it directly:

```bash
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/main.cpp src/Renderer.cpp -o cppxray_source
./cppxray_source
```

### Windows

Use MinGW-w64 (for example through MSYS2), then:

```bash
g++ -std=c++17 -O2 -Iinclude src/main.cpp src/Renderer.cpp -o cppxray.exe
cppxray.exe
```

The program switches the Windows console to UTF-8 and turns on ANSI colour support by itself. For the best result, run it in **Windows Terminal** rather than the old `cmd.exe` window.

---

## How to use it

Everything is driven by **single key presses, with no Enter needed**. Upper or lower case both work.

### The main menu

When you start the program you'll see the CPPXRAY banner and a menu split into four sections:

```
╔══[ GRAPH ALGORITHMS ]══════════════════════
║  [1]  Dijkstra's Shortest Path  O((V+E) log V)  ‹graph›
║  [2]  BFS — Breadth-First Search  O(V+E)  ‹graph›
║  [3]  DFS — Depth-First Search  O(V+E)  ‹graph›

╔══[ SORTING ALGORITHMS ]════════════════════
║  [4]  Bubble Sort  O(n²) stable  ‹sort›
║  [5]  Merge Sort  O(n log n) stable  ‹sort›
║  [6]  Quick Sort  O(n log n) avg  ‹sort›

╔══[ DATA STRUCTURES ]═══════════════════════
║  [7]  Binary Search Tree  O(log n) avg  ‹tree›
║  [8]  Min-Heap  O(log n) insert  ‹heap›
║  [9]  Memory X-Ray  Stack vs Heap  new/delete  ‹mem›

╔══[ SPECIAL FEATURES ]══════════════════════
║  [G]  Custom Graph Editor
║  [Q]  Quiz Mode
║  [C]  Complexity Lab
║  [S]  Save / Load Session
║  [T]  Theme
║  [F]  Settings
║  [H]  Help
║  [X]  Exit
```

Press a number from **1 to 9** to open a visualization, or a letter to open one of the special features.

### Stepping through an algorithm

Once a visualization is open, these are your controls:

| Key | What it does |
|---|---|
| `N` or `→` | Go forward one step |
| `P` or `←` | Go back one step |
| `A` | **Auto-play**, which runs through the steps by itself |
| `SPACE` *(during auto-play)* | Pause; press any key to resume |
| `Q` / `ESC` *(during auto-play)* | Stop auto-play |
| `+` | Speed up (80 ms less delay per step) |
| `-` | Slow down (80 ms more delay per step) |
| `R` | Go back to step 1 |
| `B` then `0`–`9` | Bookmark the current step in slot 0–9 |
| `J` then `0`–`9` | Jump to the step saved in that bookmark slot |
| `Q` | Start a quiz on this topic |
| `G` | Open the Custom Graph Editor (only for graph algorithms) |
| `S` | Save your current position to `session.dat` |
| `M` or `ESC` | Go back to the main menu |
| `X` | Quit the program |

You can change speed with `+` and `-` **while auto-play is running**, so you can slow down the interesting part and speed through the boring part.

> **Tip:** Bookmarks are really useful when you're studying. For example, in Quick Sort, press `B` then `1` right before a pivot is placed. Look around, then press `J` then `1` to come back to that moment.

### Reading the screen

Every visualization uses the same layout, from top to bottom:

```
╔═══════════════════════════════════════════════════════════╗
║ C++.xray │ Dijkstra's Shortest Path  source: 0  nodes: 6   ← what you're looking at
╠───────────────────────────────────────────────────────────╣
  T: O((V+E) log V)   S: O(V)   Greedy / Priority Queue   Step 5/22
  [████████████░░░░░░░░░░░░░░░░░░░░] 22%  speed:400ms  marks: 1 3
╠───────────────────────────────────────────────────────────╣
                                                            ← the visualization
         (graph / bars / tree / memory layout)
╠───────────────────────────────────────────────────────────╣
  Code:
        5  while (!pq.empty()) {
    ▶   6    auto [d,u] = pq.top(); pq.pop();                ← line running now
        7    for (auto [v,w] : g[u]) {
╠───────────────────────────────────────────────────────────╣
  ▶ Pop node 2 (dist=2). Now relax its neighbors.           ← what just happened
  ● Bookmark 1 set @ step 5                                 ← short status message
╠───────────────────────────────────────────────────────────╣
  Nav:  [N/→]fwd  [P/←]back  [R]reset  [A]auto  [+][-]speed
  More: [B]mark  [J]jump  [Q]quiz  [S]save  [G]editor  [M/ESC]menu  [X]exit
╚═══════════════════════════════════════════════════════════╝
```

- **T / S badges** show the time and space complexity.
- **The progress bar** shows how far through the algorithm you are, along with the current speed and which bookmark slots you've used.
- **The code panel** highlights the line matching the current step with a `▶`.
- **The explanation line** says what happened in plain words, for example "Relax edge (2->4, w=8). dist[4]=10  Updated!"

### Colour legend

**Graphs (Dijkstra, BFS, DFS):**

| Colour | Meaning |
|---|---|
| 🟩 Green | Source (the starting node) |
| 🟨 Amber | Current node being processed |
| 🟪 Purple | Waiting in the queue |
| 🟦 Teal | Visited / finished |
| ⬜ Grey | Not reached yet |

Below the graph, a **distance table** shows the best-known distance to every node, using `∞` for nodes that haven't been reached yet.

**Sorting (Bubble, Merge, Quick):**

| Colour | Meaning |
|---|---|
| 🟪 Purple | Unsorted element |
| 🟥 Coral | Currently being compared (also marked with ▲) |
| 🟨 Amber | The pivot (Quick Sort) |
| 🟩 Green | In its final sorted position |

Under the bars you'll see `cmp=`, `swaps=`, and `theory≤`, plus a bar showing how close the comparison count is to the theoretical maximum.

**Trees (BST, Min-Heap):**

| Colour | Meaning |
|---|---|
| 🟩 Green | The value that was just inserted |
| 🟦 Blue | The root of the min-heap (the minimum) |
| 🟪 Purple | Other nodes |

Below each tree you'll also see its **array representation**, so you can see how a tree maps onto an array (`left = 2i+1`, `right = 2i+2`).

**Memory X-Ray:**

| Colour | Meaning |
|---|---|
| 🟦 Teal column | The **stack**, which is managed automatically |
| 🟨 Amber column | The **heap**, which is managed with `new`/`delete` |
| 🟥 Coral | Freed memory, or a dangling pointer |

You can open the **Help screen** (`H` on the main menu) at any time to see the controls and colours again.

### Custom Graph Editor

This is my favourite feature. You can build **your own weighted graph** and watch Dijkstra solve it.

Open it with `G` from the main menu, or with `G` while you're viewing any graph algorithm.

1. Type the **number of nodes** (2–12) and press Enter.
2. Type the **source node** (the node Dijkstra starts from) and press Enter.
3. You're now in the editor. Use these keys:

| Key | Action |
|---|---|
| `A` | **Add** an edge. Type `u v weight`, e.g. `0 1 5` |
| `R` | **Remove** an edge by its index number |
| `C` | **Clear** all edges |
| `P` | **Preview** what the graph looks like |
| `S` | **Save** the graph to a file |
| `L` | **Load** a graph from a file |
| `Enter` | **Run Dijkstra** on your graph |
| `ESC` / `X` | Cancel and go back |

The editor checks your input and tells you exactly what's wrong. It rejects edges that point to nodes that don't exist, self-loops (`3 3 5`), and weights outside the range 1–99, and a clear message explains why each time.

#### Graph file format

Saved graphs are plain text, so you can also write them by hand in any editor:

```
# lines starting with # are comments
5 0        ← first line: number-of-nodes  source-node
0 1 4      ← every other line: from  to  weight
0 2 1
2 1 2
1 3 1
2 3 5
3 4 3
```

Save that as `mygraph.txt`, open the editor, press `L`, type `mygraph.txt`, and press Enter to run it. Edges are **undirected**, so `0 1 4` connects both ways.

### Quiz Mode

Each quiz has 3–4 multiple-choice questions, and each answer comes with a short explanation, so you still learn something when you get one wrong.

You can start one in two ways:
- From the main menu, press **`Q`** and pick a topic: **Dijkstra, Bubble Sort, Quick Sort, BST, or Memory X-Ray**.
- While viewing an algorithm, press **`Q`** to get a quiz on that topic straight away.

Answer with **`A`, `B`, `C`, or `D`**. At the end you get a score, a star rating (1–5 ★), and a message ranging from "Review this topic." up to "Perfect — flawless!"

Some of the questions go beyond basic definitions, for example:
- *Why does Dijkstra fail on negative edges?*
- *When does Quick Sort hit its worst case?*
- *What exactly is a dangling pointer?*
- *What is RAII?*

### Complexity Lab

This runs **Bubble Sort, Merge Sort, and Quick Sort on the same input** and compares them side by side.

1. Press `C` on the main menu.
2. Enter an array size between 2 and 20.
3. You'll see, for each algorithm:
   - how many **comparisons** and **swaps** it made
   - the **theoretical bound** for that input size
   - a bar showing how much of that bound it actually used
   - a **✓ WINNER** badge for the algorithm with the fewest comparisons

Every run is also added to `complexity.log` with a timestamp, so you can compare results across different sizes afterwards. Try n = 5 and then n = 20 and watch Bubble Sort fall behind.

### Saving your session

While viewing an algorithm, press `S`. This writes the algorithm name, the step you were on, the total number of steps, and a timestamp to `session.dat` in the current folder.

Press `S` on the main menu to see what you saved last time, e.g. *"Last session: Quick Sort (step 14)"*. You can then open that algorithm and pick up where you left off.

### Themes and settings

Press **`T`** to choose between four colour themes. The change applies immediately:

| Theme | Good for |
|---|---|
| **Default (dark)** | Most dark terminals |
| **High Contrast** | Projectors and bright rooms, since everything is bold and sharp |
| **Colourblind-safe** | Avoids relying on red/green differences |
| **Light Terminal** | Terminals with a white background |

Press **`F`** to set the **default auto-play speed** (80–2000 ms per step).

Both settings are **saved automatically** and loaded the next time you start the program:
- Linux/macOS: `~/.cppxray.conf`
- Windows: `%APPDATA%\cppxray.conf`

---

## A guided first session

If you're new, try this ten-minute tour:

1. **Start with Bubble Sort** (`4`). Press `N` a few times and watch two coral bars get compared and swapped. Look at the highlighted code line changing between line 5 (compare) and line 6 (swap).
2. **Press `A`** to auto-play. Press `+` a few times to speed up. Notice how the green "sorted" section grows from the right.
3. **Press `M`**, then open **Quick Sort** (`6`). Watch the amber pivot and how everything smaller moves to its left.
4. **Open Dijkstra** (`1`). Step through slowly and watch the distance table. Each time you see "Updated!", a shorter path was just found.
5. **Press `G`** and build a small graph of your own. Try a case where the direct edge is *longer* than a path through another node, then watch Dijkstra find the shorter route.
6. **Open Memory X-Ray** (`9`) and step through all five steps. Pay attention to step 4: after `delete p`, the pointer still holds the old address. That's a dangling pointer.
7. **Press `Q`** and take the Memory quiz. See if you get all 5 stars.
8. Finally, open the **Complexity Lab** (`C`) with n = 20 and compare the three sorting algorithms.

---

## What each visualization teaches

| # | Visualization | What to watch for |
|---|---|---|
| 1 | **Dijkstra** | The greedy choice: the node with the smallest known distance is always processed next. Watch edge relaxation update the distance table. Runs on a 6-node weighted graph by default, or on your own graph through the editor. |
| 2 | **BFS** | Level-by-level exploration. The explanation line shows the current **queue contents** and each node's **level**, so you can see why BFS finds the fewest-hops path. |
| 3 | **DFS** | Going deep first. The explanation shows the **call stack** at each step (e.g. `dfs(1) → dfs(3)`), which makes the recursion and backtracking easy to follow. |
| 4 | **Bubble Sort** | How the largest element "bubbles" to the end on every pass, with live comparison and swap counts compared against n(n−1)/2. |
| 5 | **Merge Sort** | Divide and conquer: the array is split into highlighted halves, then merged back in sorted order. |
| 6 | **Quick Sort** | Partitioning around the pivot, and each pivot landing in its final position before recursing. |
| 7 | **BST** | Each insert walks left or right based on comparisons. The array view below the tree shows how BSTs can end up with gaps. |
| 8 | **Min-Heap** | Heapify-up: each new value is swapped upward until its parent is smaller. The explanation tells you how many swaps were needed. |
| 9 | **Memory X-Ray** | A 7-line C++ program shown as stack and heap: a local variable, a `new` allocation, a function call that pushes a new stack frame, `delete` leaving a dangling pointer, and finally the frame being popped. |

---

## Files the program creates

| File | Where | What's in it |
|---|---|---|
| `.cppxray.conf` / `cppxray.conf` | Home folder / `%APPDATA%` | Your theme and default speed |
| `session.dat` | Current folder | Last saved algorithm and step |
| `complexity.log` | Current folder | A timestamped line for every Complexity Lab run |
| *any name you choose* | Current folder | Graphs you saved from the Graph Editor |

All of them are plain text, so you can open and read them in any text editor.

---

## Project structure

```
cppxray_source/
├── Makefile                 Build script (make / make run / make clean)
├── include/
│   ├── MyVector.h           Dynamic array: growth, shrinking, deep copy, move, bounds checks
│   ├── MyLinkedList.h       Doubly linked list, searches from whichever end is closer
│   ├── Containers.h         MyStack, MyQueue and MyMinHeap built on the two above
│   ├── Errors.h             Custom exception hierarchy (LibraryError and 8 subclasses)
│   ├── AlgorithmState.h     One "frame" of animation: GraphState, SortState, TreeState, MemoryState
│   ├── Algorithms.h         AlgorithmBase and all 9 algorithms, each generating its own steps
│   ├── Visualizer.h         The UI controller: frame drawing, auto-play, bookmarks, quiz, editor, lab
│   ├── FileAndQuiz.h        FileManager, GraphBuilder and QuizEngine
│   ├── Terminal.h           ANSI colour codes and text padding helpers
│   ├── Theme.h              The four colour themes, swappable at runtime
│   ├── Settings.h           Saved user settings (key=value config file)
│   └── RawTerm.h            Cross-platform raw keyboard input (termios / conio)
└── src/
    ├── main.cpp             Banner, main menu, help, theme and settings screens, key handling
    └── Renderer.cpp         Drawing code for graphs, bars, trees and memory
```

---

## How it works under the hood

This part is for anyone who wants to read the code or learn from how it's built.

### 1. Algorithms record everything first, then play it back

Every algorithm inherits from the abstract class `AlgorithmBase` and implements `generateSteps()`. When you open an algorithm, it **runs completely once** and stores a snapshot (`AlgorithmState*`) of every interesting moment. Stepping forward and backward is then just moving an index through that list.

That's why going **backwards** works perfectly. Nothing has to be "undone", because every earlier state is already saved.

### 2. Polymorphic rendering

`AlgorithmState` has a pure virtual `render()` method. `GraphState`, `SortState`, `TreeState`, and `MemoryState` each draw themselves differently. The `Visualizer` doesn't need to know what kind of algorithm it's showing. It just calls `steps[i]->render()`.

Adding a new visualization means writing one new state class and one new algorithm class. Nothing else has to change.

### 3. Custom data structures instead of the STL

The algorithms run on containers I wrote myself:

- **`MyVector<T>`** doubles its capacity when full and halves it when less than 25% is used. It has a deep-copy constructor and copy assignment, a `noexcept` move constructor, bounds-checked `operator[]`, and `operator+` for joining two vectors.
- **`MyLinkedList<T>`** is doubly linked, and index lookups start from whichever end is closer.
- **`MyQueue<T>`** is backed by the linked list, so enqueue and dequeue are O(1). **BFS** uses it.
- **`MyMinHeap<T>`** is backed by `MyVector`, using the `(i-1)/2`, `2i+1`, `2i+2` index formulas. **Dijkstra** uses it as its priority queue.
- **`MyStack<T>`** is backed by `MyVector`.

### 4. A custom exception hierarchy

Every error type inherits from `LibraryError`, which inherits from `std::exception`: `OutOfRangeError`, `EmptyContainerError`, `AllocationError`, `FileNotFoundError`, `CorruptFileError`, `InvalidGraphError`, `NoPathError`, and `EmptyQuizError`. The main loop catches `LibraryError` and shows a friendly message instead of crashing. Specific parts of the program, like the graph editor, catch errors themselves so they can show them inline.

### 5. Flicker-free drawing

Clearing the screen and redrawing it bit by bit makes terminals flicker. Instead, `Visualizer::draw()` temporarily redirects `cout` into an `ostringstream`, builds the **entire frame** in memory, and then writes it to the terminal all at once.

### 6. Drawing graphs in a character grid

`GraphState::render()` places the nodes evenly around an ellipse, stretched to make up for terminal characters being taller than they are wide. It then draws each edge with **Bresenham's line algorithm**, choosing `-`, `|`, `/` or `\` based on the slope, and puts the edge weight near the middle of the line. Node labels are drawn last so edges never cover them.

### 7. Raw input on every platform

`RawTerm` turns off line buffering and echo so single key presses work without Enter. It uses `termios` on Linux/macOS and `conio` on Windows, and it reads arrow keys by parsing their escape sequences. It follows RAII: the constructor turns raw mode on and the destructor restores your terminal.

A `CookedScope` helper temporarily switches back to normal line input whenever the program needs you to type something, like an edge or a filename. Signal handlers for `SIGINT`, `SIGTERM`, and `SIGSEGV` make sure your terminal still gets restored if something goes wrong.

### 8. Interruptible auto-play

Auto-play doesn't just sleep for the whole delay. It sleeps in **20 ms slices** and checks for a key press between them, which is why pause, stop, and speed changes respond immediately even at slow speeds.

---

## Troubleshooting

**The boxes and symbols show up as `?` or garbage characters.**
Your terminal isn't using UTF-8. On Linux, check that `echo $LANG` ends with `UTF-8`. On Windows, use Windows Terminal.

**There are no colours, or I see things like `\033[95m`.**
Your terminal doesn't support ANSI escape codes. Try a different terminal, such as Windows Terminal, the VS Code terminal, or any modern Linux/macOS terminal.

**The layout looks broken or wraps onto new lines.**
Make your terminal window wider (at least 74 columns) and taller.

**The colours are hard to see on my light background.**
Press `T` and choose **Light Terminal**, or **High Contrast** if you're projecting.

**My terminal stopped showing what I type after the program exited.**
This shouldn't happen, because the program restores the terminal on exit and on Ctrl-C. If it does happen after a hard kill, type `reset` and press Enter.

**Loading a graph file says it's corrupted.**
Make sure the first line is exactly two numbers (`nodes source`) and every other line is exactly three (`u v weight`). Lines starting with `#` are ignored.

---

## Awarded Best Project in the Lab

C++.xray was named **the best and most excellent project in our lab**. The award was for how complex and detailed it is, and for how much work went into it.

I didn't want to hand in just another console program that prints a sorted array. I wanted something I would have liked to have when I was learning this material: a tool that **shows** what happens inside Dijkstra's priority queue, why Quick Sort's pivot matters, and where `int* p = new int(42)` actually lives in memory.

Here's what makes the project bigger than a typical lab submission:

- **About 4,500 lines of hand-written C++17**, spread across 14 source and header files.
- **No STL containers for the core logic.** I built my own `MyVector`, a doubly linked `MyLinkedList`, `MyStack`, `MyQueue`, and `MyMinHeap` from scratch, and the algorithms run on top of them. Dijkstra really does use my heap, and BFS really does use my queue.
- **Nine different visualizations.** Each one has its own rendering logic: graph layouts drawn with Bresenham's line algorithm, bar charts, binary trees, and a side-by-side view of stack and heap memory.
- **Raw terminal input on both POSIX and Windows**, using `termios` on Linux/macOS and `conio` on Windows, with RAII guards that restore your terminal even if the program crashes or you press Ctrl-C.
- **Flicker-free rendering.** Each frame is built in memory first and written to the screen in one go.
- **A proper object-oriented design**, with abstract base classes, polymorphic rendering, a custom exception hierarchy, deep-copy and move semantics, and operator overloading.
- **Extra features on top**: a quiz engine, a graph editor, a complexity lab, bookmarks, auto-play, four colour themes (one of them colourblind-safe), and settings that are saved between runs.

---

## Ideas for the future

Things I'd like to add next:

- Showing the actual shortest **path** to a chosen node at the end of Dijkstra (the `PATH` node state is already defined for this)
- More algorithms: Insertion Sort, Heap Sort, Prim's and Kruskal's MST, A*
- Letting users enter their own arrays for the sorting visualizations
- BST delete and search, and Min-Heap extract-min with heapify-down
- Fully resuming a saved session, not just showing where you left off
- Quiz questions for BFS, DFS, Merge Sort and Min-Heap

---

## Acknowledgements

Thanks to my professor, lab instructors and classmates for their feedback, and for recognising this project as **the best in the lab**. That meant a lot after all the late nights spent getting a graph to draw properly with nothing but characters. 

If this project helps you understand an algorithm a little better, that's all I was hoping for. Feel free to open an issue or reach out with suggestions.

<div align="center">

**Built from scratch in C++17 by Abubakar.**

*"Every `new` must have exactly one matching `delete`."*

</div>

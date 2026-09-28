#include "Terminal.h"
#include "Theme.h"
#include "Settings.h"
#include "RawTerm.h"
#include "Visualizer.h"
#include "Algorithms.h"
#include "FileAndQuiz.h"
#include "Errors.h"
#include <iostream>
#include <string>
#include <limits>
#include <csignal>
#include <cstdlib>
#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
#endif

using namespace std;

static const int W = 74;

static RawTerm* g_rawTerm = nullptr;
// global pointer so the signal handler can restore the terminal on crash/SIGINT.

static void onFatal(int sig) {
    if(g_rawTerm) g_rawTerm->disable();
    cout << Colour::RESET << "\n[interrupted]\n";
    cout.flush();
    std::exit(sig == SIGINT ? 130 : 1);
}
// best-effort terminal restore if we get interrupted while in raw mode.

static const int ART_LMARGIN = 2;
static const int CELL_C = 8;
static const int CELL_P = 8;
static const int CELL_X = 8;
static const int CELL_R = 8;
static const int CELL_A = 8;
static const int CELL_Y = 9;
static const int ART_CELLS = CELL_C + CELL_P + CELL_P + CELL_X + CELL_R + CELL_A + CELL_Y;

void drawBanner() {
    Term::hardClear();

    cout << Colour::PURPLE << "╔";
    cout << Colour::MUTED;
    for(int i = 0; i < W-2; i++) cout << "═";
    cout << Colour::PURPLE << "╗" << Colour::RESET << "\n";

    const char* C[6] = {
        " ██████╗",
        "██╔════╝",
        "██║     ",
        "██║     ",
        "╚██████╗",
        " ╚═════╝"
    };
    const char* P[6] = {
        "██████╗ ",
        "██╔══██╗",
        "██████╔╝",
        "██╔═══╝ ",
        "██║     ",
        "╚═╝     "
    };
    const char* X[6] = {
        "██╗  ██╗",
        "╚██╗██╔╝",
        " ╚███╔╝ ",
        " ██╔██╗ ",
        "██╔╝╚██╗",
        "╚═╝  ╚═╝"
    };
    const char* R[6] = {
        "██████╗ ",
        "██╔══██╗",
        "██████╔╝",
        "██╔══██╗",
        "██║  ██║",
        "╚═╝  ╚═╝"
    };
    const char* A[6] = {
        " █████╗ ",
        "██╔══██╗",
        "███████║",
        "██╔══██║",
        "██║  ██║",
        "╚═╝  ╚═╝"
    };
    const char* Y[6] = {
        "██╗   ██╗",
        "╚██╗ ██╔╝",
        " ╚████╔╝ ",
        "  ╚██╔╝  ",
        "   ██║   ",
        "   ╚═╝   "
    };
    // each letter is its own 6-row array so we can colour CPP vs XRAY independently.

    const int rightPad = (W - 2) - ART_LMARGIN - ART_CELLS;

    for(int row = 0; row < 6; row++) {
        cout << Colour::PURPLE << "║" << Colour::RESET;
        for(int i = 0; i < ART_LMARGIN; i++) cout << ' ';
        cout << Colour::PURPLE << Colour::BOLD << C[row] << P[row] << P[row] << Colour::RESET;
        cout << Colour::TEAL << Colour::BOLD << X[row] << R[row] << A[row] << Y[row] << Colour::RESET;
        for(int i = 0; i < rightPad; i++) cout << ' ';
        cout << Colour::PURPLE << "║" << Colour::RESET << "\n";
    }

    cout << Colour::PURPLE << "╚";
    cout << Colour::MUTED;
    for(int i = 0; i < W-2; i++) cout << "═";
    cout << Colour::PURPLE << "╝" << Colour::RESET << "\n";
}
// draws the CPPXRAY ASCII banner at the top of the main menu.

void menuSection(const string& title, const string& col) {
    cout << "\n  " << col << "╔══[ " << Colour::BOLD << title
         << Colour::RESET << col << " ]";
    int dashCount = W - 12 - (int)title.size();
    for(int i = 0; i < dashCount && i < 40; i++) cout << "═";
    cout << "╗" << Colour::RESET << "\n";
}
// section header line used between groups of menu items.

void menuItem(const string& key, const string& label,
              const string& detail = "", const string& tag = "") {
    cout << "  " << Colour::MUTED << "║" << Colour::RESET;
    cout << "  " << Colour::TEAL << Colour::BOLD << "[" << key << "]"
         << Colour::RESET << "  " << Colour::WHITE << label << Colour::RESET;
    if(!detail.empty()) cout << "  " << Colour::MUTED << detail;
    if(!tag.empty()) cout << "  " << Colour::AMBER << "‹" << tag << "›";
    cout << Colour::RESET << "\n";
}
// one row of the main menu showing key, label, complexity hint, and category tag.

void drawMainMenu(const Settings& s) {
    drawBanner();

    menuSection("GRAPH ALGORITHMS", Colour::BRIGHT_BLUE);
    menuItem("1", "Dijkstra's Shortest Path", "O((V+E) log V)", "graph");
    menuItem("2", "BFS — Breadth-First Search", "O(V+E)", "graph");
    menuItem("3", "DFS — Depth-First Search", "O(V+E)", "graph");

    menuSection("SORTING ALGORITHMS", Colour::BRIGHT_GREEN);
    menuItem("4", "Bubble Sort", "O(n²) stable", "sort");
    menuItem("5", "Merge Sort", "O(n log n) stable", "sort");
    menuItem("6", "Quick Sort", "O(n log n) avg", "sort");

    menuSection("DATA STRUCTURES", Colour::CORAL);
    menuItem("7", "Binary Search Tree", "O(log n) avg", "tree");
    menuItem("8", "Min-Heap", "O(log n) insert", "heap");
    menuItem("9", "Memory X-Ray", "Stack vs Heap  new/delete", "mem");

    menuSection("SPECIAL FEATURES", Colour::AMBER);
    menuItem("G", "Custom Graph Editor", "Build your own graph & run Dijkstra");
    menuItem("Q", "Quiz Mode", "Test your understanding");
    menuItem("C", "Complexity Lab", "Compare sorts with live counters");
    menuItem("S", "Save / Load Session", "Last step saved to file");
    menuItem("T", "Theme", "Current: " + Theme::name(s.theme));
    menuItem("F", "Settings", "Default speed: " + to_string(s.defaultSpeed) + "ms");
    menuItem("H", "Help", "Controls & colour legend");

    cout << "\n  " << Colour::MUTED;
    for(int i = 0; i < W-4; i++) cout << "─";
    cout << Colour::RESET << "\n";
    menuItem("X", "Exit", "");

    cout << "\n  " << Colour::MUTED
         << "Tip: single key — no Enter needed.  Press a key: "
         << Colour::RESET;
    cout.flush();
}
// draws the full main menu with banner, section headers, items, and exit row.

void drawHelp() {
    Term::hardClear();
    cout << Colour::PURPLE << "╔";
    cout << Colour::MUTED;
    for(int i = 0; i < W-2; i++) cout << "═";
    cout << Colour::PURPLE << "╗" << Colour::RESET << "\n";
    cout << Colour::PURPLE << "║" << Colour::RESET;
    cout << Colour::PURPLE << Colour::BOLD << "  Help & Controls";
    for(int i = 17; i < W-2; i++) cout << ' ';
    cout << Colour::PURPLE << "║" << Colour::RESET << "\n";
    cout << Colour::PURPLE << "╠";
    cout << Colour::MUTED;
    for(int i = 0; i < W-2; i++) cout << "─";
    cout << Colour::PURPLE << "╣" << Colour::RESET << "\n\n";

    auto row = [](const string& k, const string& d) {
        cout << "  " << Colour::TEAL << Colour::BOLD << k << Colour::RESET
             << "  " << Colour::MUTED << d << Colour::RESET << "\n";
    };

    cout << Colour::WHITE << Colour::BOLD << "  Navigation (single key, no Enter):\n\n" << Colour::RESET;
    row("N  or  →        ", "Step forward one step");
    row("P  or  ←        ", "Step backward one step");
    row("A               ", "Auto-play (SPACE pauses, Q/ESC stops, +/- adjusts speed)");
    row("+               ", "Speed up");
    row("-               ", "Slow down");
    row("R               ", "Reset to step 1");
    row("B then 0-9      ", "Bookmark current step in slot 0-9");
    row("J then 0-9      ", "Jump to bookmark slot 0-9");
    row("Q               ", "Start quiz for this algorithm");
    row("G               ", "Open custom graph editor (graph algos only)");
    row("S               ", "Save current session");
    row("M  or  ESC      ", "Return to main menu");
    row("X               ", "Exit program");

    cout << "\n" << Colour::WHITE << Colour::BOLD << "  Colour legend:\n\n" << Colour::RESET;
    row(Colour::BRIGHT_GREEN + "■" + Colour::RESET + "  Source   ", "Starting node");
    row(Colour::AMBER + "■" + Colour::RESET + "  Current  ", "Currently being processed");
    row(Colour::PURPLE + "■" + Colour::RESET + "  In Queue ", "Waiting in queue");
    row(Colour::TEAL + "■" + Colour::RESET + "  Visited  ", "Fully processed");
    row(Colour::CORAL + "■" + Colour::RESET + "  Highlight", "Bars being compared");

    cout << "\n  " << Colour::MUTED << "Press any key to return..." << Colour::RESET;
    cout.flush();
    RawTerm::readKey();
}
// help screen listing all keybindings and the meaning of each colour in the visualizer.

void runThemePicker(Settings& s) {
    while(true) {
        Term::hardClear();
        cout << "\n  " << Colour::PURPLE << Colour::BOLD << "Theme" << Colour::RESET
             << "      " << Colour::MUTED << "current: "
             << Colour::WHITE << Theme::name(s.theme) << Colour::RESET << "\n\n";

        Theme::Mode opts[] = {
            Theme::Mode::DEFAULT,
            Theme::Mode::HIGH_CONTRAST,
            Theme::Mode::COLORBLIND,
            Theme::Mode::LIGHT
        };

        for(int i = 0; i < 4; i++) {
            bool active = (s.theme == opts[i]);
            cout << "  " << Colour::TEAL << Colour::BOLD << "[" << (i+1) << "]" << Colour::RESET
                 << "  " << (active ? Colour::BRIGHT_GREEN : Colour::WHITE)
                 << Theme::name(opts[i]);
            if(active) cout << Colour::MUTED << "  ◄ active";
            cout << Colour::RESET << "\n";
        }
        cout << "\n  " << Colour::MUTED << "[1-4] choose  [ESC/Q] back: " << Colour::RESET;
        cout.flush();

        int k = RawTerm::readKey();
        if(k == Key::ESC || k == 'q' || k == 'Q') return;
        if(k >= '1' && k <= '4') {
            s.theme = opts[k - '1'];
            Theme::apply(s.theme);
            s.save();
        }
    }
}
// shows the four palettes and lets the user switch live. saves the choice to disk.

void runSettingsMenu(Settings& s, RawTerm& rawTerm) {
    Term::hardClear();
    cout << "\n  " << Colour::PURPLE << Colour::BOLD << "Settings" << Colour::RESET << "\n\n";
    cout << "  " << Colour::MUTED << "Default auto-play speed: "
         << Colour::WHITE << s.defaultSpeed << "ms" << Colour::RESET << "\n";

    {
        CookedScope cs(rawTerm);
        cout << "  " << Colour::TEAL << "New speed (80-2000ms, or Enter to keep): "
             << Colour::RESET;
        cout.flush();
        string line; getline(cin, line);
        if(!line.empty()) {
            try {
                int v = stoi(line);
                if(v >= 80 && v <= 2000) {
                    s.defaultSpeed = v;
                    s.save();
                    cout << "  " << Colour::BRIGHT_GREEN << "Saved." << Colour::RESET << "\n";
                } else {
                    cout << "  " << Colour::CORAL << "Out of range." << Colour::RESET << "\n";
                }
            } catch(...) {
                cout << "  " << Colour::CORAL << "Not a number." << Colour::RESET << "\n";
            }
        }
    }
    cout << "  " << Colour::MUTED << "Press any key..." << Colour::RESET;
    cout.flush();
    RawTerm::readKey();
}
// lets the user adjust the default auto-play speed and persist it.

void runAlgorithm(Visualizer& viz, AlgorithmBase* algo, RawTerm&, Settings& settings) {
    viz.setAlgorithm(algo);
    viz.setSpeed(settings.defaultSpeed);

    while(true) {
        viz.draw();
        cout.flush();

        int key = RawTerm::readKey();
        int up = (key >= 'a' && key <= 'z') ? (key - 32) : key;
        char ch = (up < 128 && up > 0) ? (char)up : 0;

        if(ch == 'N' || ch == '>' || key == Key::RIGHT) {
            if(!viz.stepForward()) viz.notify("Already at last step", Colour::MUTED);
        }
        else if(ch == 'P' || ch == '<' || key == Key::LEFT) {
            if(!viz.stepBack()) viz.notify("Already at first step", Colour::MUTED);
        }
        else if(ch == 'R') {
            viz.reset();
            viz.notify("Reset to step 1", Colour::TEAL);
        }
        else if(ch == 'A') {
            viz.autoPlay();
        }
        else if(ch == '+') {
            viz.setSpeed(viz.getSpeed() - 80);
            viz.notify("Speed: " + to_string(viz.getSpeed()) + "ms", Colour::TEAL);
        }
        else if(ch == '-') {
            viz.setSpeed(viz.getSpeed() + 80);
            viz.notify("Speed: " + to_string(viz.getSpeed()) + "ms", Colour::MUTED);
        }
        else if(ch == 'B') {
            viz.notify("Bookmark slot (0-9)?", Colour::AMBER, 99);
            viz.draw();
            int k2 = RawTerm::readKey();
            if(k2 >= '0' && k2 <= '9') viz.setBookmark(k2 - '0');
            else viz.notify("Bookmark cancelled", Colour::MUTED);
        }
        else if(ch == 'J') {
            viz.notify("Jump to slot (0-9)?", Colour::AMBER, 99);
            viz.draw();
            int k2 = RawTerm::readKey();
            if(k2 >= '0' && k2 <= '9') viz.jumpBookmark(k2 - '0');
            else viz.notify("Jump cancelled", Colour::MUTED);
        }
        else if(ch == 'Q') {
            string topic = "dijkstra";
            string nm = algo->name();
            if      (nm.find("Bubble") != string::npos) topic = "bubble";
            else if (nm.find("Quick")  != string::npos) topic = "quick";
            else if (nm.find("Merge")  != string::npos) topic = "bubble";
            else if (nm.find("BST")    != string::npos
                  || nm.find("Binary") != string::npos) topic = "bst";
            else if (nm.find("Memory") != string::npos) topic = "memory";
            else if (nm.find("BFS")    != string::npos
                  || nm.find("DFS")    != string::npos) topic = "dijkstra";
            viz.startQuiz(topic);
        }
        else if(ch == 'G') {
            viz.runGraphEditor([&](AlgorithmBase* newAlgo) {
                viz.setAlgorithm(newAlgo);
                viz.setSpeed(settings.defaultSpeed);
            });
        }
        else if(ch == 'S') {
            try {
                FileManager::saveSession("session.dat", algo->name(),
                                         viz.getStep(), (int)algo->steps.size());
                viz.notify("Session saved to session.dat", Colour::BRIGHT_GREEN);
            } catch(const LibraryError& e) {
                viz.notify(string("Save failed: ") + e.what(), Colour::CORAL);
            }
        }
        else if(ch == 'M' || key == Key::ESC) {
            break;
        }
        else if(ch == 'X') {
            Term::hardClear();
            cout << Colour::TEAL << "\n  Thanks for using C++.xray!\n\n" << Colour::RESET;
            std::exit(0);
        }
    }
}
// the raw-mode key loop that drives an algorithm visualization. dispatches keys to the Visualizer.

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    Settings settings;
    settings.load();
    Theme::apply(settings.theme);

    RawTerm rawTerm;
    g_rawTerm = &rawTerm;

    std::signal(SIGINT,  onFatal);
    std::signal(SIGTERM, onFatal);
#ifdef SIGSEGV
    std::signal(SIGSEGV, onFatal);
#endif
    // hooks so a crash or Ctrl-C still restores the terminal mode.

    Visualizer viz(rawTerm);

    while(true) {
        drawMainMenu(settings);

        int key = RawTerm::readKey();
        int up = (key >= 'a' && key <= 'z') ? (key - 32) : key;
        char ch = (up < 128 && up > 0) ? (char)up : 0;

        try {
            switch(ch) {
            case '1': runAlgorithm(viz, DijkstraAlgorithm::defaultGraph(), rawTerm, settings); break;
            case '2': runAlgorithm(viz, new BFSAlgorithm(), rawTerm, settings); break;
            case '3': runAlgorithm(viz, new DFSAlgorithm(), rawTerm, settings); break;
            case '4': runAlgorithm(viz, new BubbleSortAlgorithm({64,34,25,12,22,11,90}), rawTerm, settings); break;
            case '5': runAlgorithm(viz, new MergeSortAlgorithm({38,27,43,3,9,82,10}), rawTerm, settings); break;
            case '6': runAlgorithm(viz, new QuickSortAlgorithm({10,80,30,90,40,50,70}), rawTerm, settings); break;
            case '7': runAlgorithm(viz, new BSTAlgorithm({50,30,70,20,40,60,80}), rawTerm, settings); break;
            case '8': runAlgorithm(viz, new HeapAlgorithm({15,10,8,12,3,6,7}), rawTerm, settings); break;
            case '9': runAlgorithm(viz, new MemoryXRayAlgorithm(), rawTerm, settings); break;

            case 'G': {
                AlgorithmBase* customAlgo = nullptr;
                viz.runGraphEditor([&](AlgorithmBase* a) { customAlgo = a; });
                if(customAlgo) runAlgorithm(viz, customAlgo, rawTerm, settings);
                break;
            }

            case 'Q': {
                Term::hardClear();
                cout << Colour::AMBER << Colour::BOLD << "\n  Quiz Mode\n\n" << Colour::RESET;
                cout << "  [1] Dijkstra  [2] Bubble Sort  [3] Quick Sort\n";
                cout << "  [4] BST        [5] Memory X-Ray\n\n";
                cout << "  Choose topic (1-5, ESC to cancel): ";
                cout.flush();

                int tk = RawTerm::readKey();
                if(tk == Key::ESC) break;

                string topics[] = {"dijkstra","bubble","quick","bst","memory"};
                int idx = tk - '1';
                if(idx >= 0 && idx < 5) {
                    AlgorithmBase* dummy = nullptr;
                    switch(idx) {
                    case 0: dummy = DijkstraAlgorithm::defaultGraph(); break;
                    case 1: dummy = new BubbleSortAlgorithm({5,3,1,4,2}); break;
                    case 2: dummy = new QuickSortAlgorithm({5,3,1,4,2}); break;
                    case 3: dummy = new BSTAlgorithm({50,30,70}); break;
                    case 4: dummy = new MemoryXRayAlgorithm(); break;
                    }
                    if(dummy) {
                        viz.setAlgorithm(dummy);
                        viz.startQuiz(topics[idx]);
                    }
                } else {
                    cout << "\n  " << Colour::CORAL << "Invalid choice." << Colour::RESET << "\n";
                    cout << "  Press any key..."; cout.flush();
                    RawTerm::readKey();
                }
                break;
            }

            case 'C':
                viz.runComplexityLab();
                break;

            case 'S': {
                Term::hardClear();
                if(FileManager::exists("session.dat")) {
                    string algoName;
                    int step = 0;
                    try {
                        FileManager::loadSession("session.dat", algoName, step);
                        cout << Colour::TEAL << "\n  Last session: " << algoName
                             << " (step " << step << ")\n" << Colour::RESET;
                    } catch(const LibraryError& e) {
                        cout << Colour::CORAL << "\n  Load failed: " << e.what()
                             << "\n" << Colour::RESET;
                    }
                } else {
                    cout << Colour::MUTED << "\n  No session file found.\n" << Colour::RESET;
                }
                cout << "  Press any key..."; cout.flush();
                RawTerm::readKey();
                break;
            }

            case 'T':
                runThemePicker(settings);
                break;

            case 'F':
                runSettingsMenu(settings, rawTerm);
                break;

            case 'H':
                drawHelp();
                break;

            case 'X':
                Term::hardClear();
                cout << Colour::TEAL << "\n  Thanks for using C++.xray!\n\n" << Colour::RESET;
                return 0;

            default:
                break;
            }
        }
        catch(const LibraryError& e) {
            cout << Colour::CORAL << "\n  Error: " << e.what() << Colour::RESET << "\n";
            cout << "  Press any key..."; cout.flush();
            RawTerm::readKey();
        }
        catch(const exception& e) {
            cout << Colour::CORAL << "\n  Unexpected: " << e.what() << Colour::RESET << "\n";
            cout << "  Press any key..."; cout.flush();
            RawTerm::readKey();
        }
    }
}
// entry point. loads settings, applies theme, enables raw mode, then runs the main menu loop.

#pragma once
#include "AlgorithmState.h"
#include "Algorithms.h"
#include "Terminal.h"
#include "FileAndQuiz.h"
#include "RawTerm.h"
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <limits>
#include <sstream>

using namespace std;

class Visualizer {
private:
    AlgorithmBase* algo;
    int stepIdx;
    int playSpeed;
    QuizEngine quiz;
    RawTerm& rawTerm;

    int bookmarks[10];
    // 10 bookmark slots, indexed 0-9. reset to -1 when the algorithm changes.

    mutable string notice;
    mutable string noticeColour;
    mutable int noticeTtl = 0;
    // transient status line shown above the controls for a few frames then fades.

    static const int SW = 74;

public:
    explicit Visualizer(RawTerm& rt) : algo(nullptr), stepIdx(0), playSpeed(400), rawTerm(rt) {
        for(int i = 0; i < 10; i++) bookmarks[i] = -1;
    }
    // constructor takes the shared RawTerm reference so we can swap to cooked mode inside graph editor / quiz.

    ~Visualizer() { delete algo; }
    // owns the algorithm pointer and cleans it up on destruction.

    void setAlgorithm(AlgorithmBase* a) {
        delete algo;
        algo = a;
        stepIdx = 0;
        for(int i = 0; i < 10; i++) bookmarks[i] = -1;
        notice.clear();
        noticeTtl = 0;
        algo->generateSteps();
    }
    // swaps in a new algorithm. clears bookmarks and notice since theyre tied to the old one.

    AlgorithmBase* currentAlgo() const { return algo; }

    void draw() {
        if(!algo || algo->steps.empty()) return;

        ostringstream buf;
        streambuf* old = cout.rdbuf(buf.rdbuf());

        cout << "\033[2J\033[H";
        drawHeader();
        drawProgressBar();
        drawVisualization();
        drawCodePanel();
        drawExplanation();
        drawNotice();
        drawControls();

        cout.rdbuf(old);
        cout << buf.str();
        cout.flush();

        if(noticeTtl > 0) noticeTtl--;
    }
    // redirects cout into an ostringstream, draws everything, then writes the whole frame in one syscall.
    // this kills the flicker because the terminal cant render partial frames.

    void notify(const string& msg, const string& col = Colour::TEAL, int ttl = 4) const {
        notice = msg;
        noticeColour = col;
        noticeTtl = ttl;
    }
    // posts a transient message that draws above the controls for ttl frames.

private:

    void drawHeader() const {
        cout << Colour::PURPLE << "╔";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "═";
        cout << Colour::PURPLE << "╗" << Colour::RESET << "\n";

        cout << Colour::PURPLE << "║" << Colour::RESET;
        cout << " " << Colour::PURPLE << Colour::BOLD << "C++" << Colour::TEAL << ".xray "
             << Colour::RESET << Colour::MUTED << "│ "
             << Colour::WHITE << Colour::BOLD << algo->name()
             << Colour::RESET << Colour::MUTED << "  " << algo->subtitle();
        cout << "\n";

        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
    }
    // top border + algorithm name banner.

    void drawProgressBar() const {
        cout << " "
             << Colour::BG_DARK << Colour::AMBER << " T: " << algo->timeComplexity() << " " << Colour::RESET
             << " "
             << Colour::BG_DARK << Colour::PURPLE << " S: " << algo->spaceComplexity() << " " << Colour::RESET
             << " "
             << Colour::BG_DARK << Colour::TEAL << " " << algo->algoType() << " " << Colour::RESET
             << Colour::MUTED
             << "   Step " << Colour::WHITE << (stepIdx+1) << "/" << algo->steps.size()
             << Colour::RESET << "\n";

        const int BW = 44;
        int total = (int)algo->steps.size();
        int filled = (total > 1) ? stepIdx * BW / (total-1) : BW;
        int pct = (int)((double)(stepIdx+1) / total * 100);

        cout << " " << Colour::MUTED << "[";
        for(int i = 0; i < filled; i++)  cout << Colour::TEAL << "█";
        for(int i = filled; i < BW; i++) cout << Colour::MUTED << "░";
        cout << Colour::MUTED << "] " << Colour::AMBER << pct << "%" << Colour::RESET
             << Colour::MUTED << "  speed:" << Colour::RESET
             << Colour::WHITE << playSpeed << "ms" << Colour::RESET;

        int set = 0;
        for(int i = 0; i < 10; i++) if(bookmarks[i] >= 0) set++;
        if(set > 0) {
            cout << "  " << Colour::MUTED << "marks:" << Colour::RESET;
            for(int i = 0; i < 10; i++) {
                if(bookmarks[i] >= 0)
                    cout << " " << Colour::AMBER << i << Colour::RESET;
            }
        }
        cout << "\n";

        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
    }
    // complexity badges + step progress + speed + bookmark dots.

    void drawVisualization() const {
        algo->steps[stepIdx]->render();
        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
    }
    // delegates to the current AlgorithmState's render() (polymorphism).

    void drawCodePanel() const {
        auto lines = algo->codeLines();
        auto* step = algo->steps[stepIdx];
        int activeLn = step->activeLine;

        cout << " " << Colour::MUTED << "Code:" << Colour::RESET << "\n";

        for(int i = 0; i < (int)lines.size(); i++) {
            bool active = (lines[i].ln == activeLn);
            if(active) {
                cout << Colour::BG_DARK << Colour::AMBER << Colour::BOLD
                     << "  ▶ " << Term::padLeft(to_string(lines[i].ln), 2)
                     << "  " << lines[i].code;
                int pad = SW - 10 - (int)lines[i].code.size();
                for(int p = 0; p < pad && p < 40; p++) cout << " ";
                cout << Colour::RESET << "\n";
            } else {
                cout << Colour::MUTED << "    " << Term::padLeft(to_string(lines[i].ln), 2)
                     << "  " << Colour::RESET << Colour::DIM << lines[i].code
                     << Colour::RESET << "\n";
            }
        }

        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
    }
    // shows the algorithm's pseudocode with the active line highlighted.

    void drawExplanation() const {
        auto* step = algo->steps[stepIdx];
        cout << "\n " << Colour::BRIGHT_CYAN << "▶ " << Colour::RESET
             << Colour::WHITE << step->explanation << Colour::RESET << "\n\n";
    }
    // one-line description of what the current step is doing.

    void drawNotice() const {
        if(noticeTtl > 0 && !notice.empty()) {
            cout << " " << noticeColour << "● " << notice << Colour::RESET << "\n";
        } else {
            cout << "\n";
        }
    }
    // the transient status line. emits a blank line if no notice so the frame height stays stable.

    void drawControls() const {
        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";

        cout << " "
             << Colour::MUTED << "Nav:  "
             << Colour::TEAL << "[N/→]" << Colour::MUTED << "fwd  "
             << Colour::TEAL << "[P/←]" << Colour::MUTED << "back  "
             << Colour::TEAL << "[R]" << Colour::MUTED << "reset  "
             << Colour::AMBER << "[A]" << Colour::MUTED << "auto  "
             << Colour::WHITE << "[+][-]" << Colour::MUTED << "speed"
             << Colour::RESET << "\n";

        cout << " " << Colour::MUTED << "More: "
             << Colour::PURPLE << "[B]" << Colour::MUTED << "mark  "
             << Colour::PURPLE << "[J]" << Colour::MUTED << "jump  "
             << Colour::PURPLE << "[Q]" << Colour::MUTED << "quiz  "
             << Colour::TEAL << "[S]" << Colour::MUTED << "save  ";

        if(algo) {
            string nm = algo->name();
            if(nm.find("Dijkstra") != string::npos
            || nm.find("BFS") != string::npos
            || nm.find("DFS") != string::npos)
                cout << Colour::GREEN << "[G]" << Colour::MUTED << "editor  ";
        }

        cout << Colour::CORAL << "[M/ESC]" << Colour::MUTED << "menu  "
             << Colour::RED << "[X]" << Colour::MUTED << "exit"
             << Colour::RESET << "\n";

        cout << Colour::PURPLE << "╚";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "═";
        cout << Colour::PURPLE << "╝" << Colour::RESET << "\n";
    }
    // bottom controls footer. [G] is only shown for graph algorithms so the hint is context-aware.

public:

    bool stepForward() {
        if(!algo || stepIdx >= (int)algo->steps.size()-1) return false;
        stepIdx++;
        return true;
    }
    // advances one step; returns false if we are already at the last one.

    bool stepBack() {
        if(stepIdx <= 0) return false;
        stepIdx--;
        return true;
    }
    // steps backward one step; returns false at the start.

    void reset() { stepIdx = 0; }
    bool isAtEnd() const { return algo && stepIdx >= (int)algo->steps.size()-1; }
    void setSpeed(int ms) { playSpeed = ms < 80 ? 80 : (ms > 2000 ? 2000 : ms); }
    int getSpeed() const { return playSpeed; }
    int getStep() const { return stepIdx; }

    bool setBookmark(int slot) {
        if(slot < 0 || slot > 9) return false;
        bookmarks[slot] = stepIdx;
        notify("Bookmark " + to_string(slot) + " set @ step " + to_string(stepIdx+1), Colour::AMBER);
        return true;
    }
    // saves the current step index into the chosen slot.

    bool jumpBookmark(int slot) {
        if(slot < 0 || slot > 9 || bookmarks[slot] < 0) {
            notify("No bookmark in slot " + to_string(slot), Colour::CORAL);
            return false;
        }
        stepIdx = bookmarks[slot];
        if(algo && stepIdx >= (int)algo->steps.size())
            stepIdx = (int)algo->steps.size() - 1;
        notify("Jumped to bookmark " + to_string(slot) + " (step " + to_string(stepIdx+1) + ")", Colour::TEAL);
        return true;
    }
    // jumps the current step index back to a saved bookmark.

    void autoPlay() {
        while(!isAtEnd()) {
            draw();
            cout << " " << Colour::AMBER << "▶ AUTO  "
                 << Colour::MUTED << "[SPACE]pause  [Q/ESC]stop  [+/-]speed"
                 << Colour::RESET << "\n";
            cout.flush();

            bool stop = false, paused = false;
            int slept = 0;
            while(slept < playSpeed) {
                this_thread::sleep_for(chrono::milliseconds(20));
                slept += 20;
                if(!RawTerm::keyAvailable()) continue;

                int k = RawTerm::readKey();
                if(k == 'q' || k == 'Q' || k == Key::ESC) { stop = true; break; }
                if(k == ' ') { paused = true; break; }
                if(k == '+') setSpeed(playSpeed - 80);
                if(k == '-') setSpeed(playSpeed + 80);
            }
            if(stop) {
                notify("Auto-play stopped", Colour::CORAL);
                break;
            }
            if(paused) {
                notify("PAUSED — press any key to resume", Colour::AMBER, 99);
                draw();
                RawTerm::readKey();
                notify("Resumed", Colour::TEAL);
                continue;
            }
            stepForward();
        }
        draw();
    }
    // interruptible auto-play. sleeps in 20ms chunks and polls stdin between them so we can pause/stop/adjust on the fly.

    void startQuiz(const string& topic) {
        try {
            quiz.loadTopic(topic);
            runQuiz();
        } catch(const EmptyQuizError& e) {
            cout << Colour::CORAL << "\n  No quiz available: " << e.what() << Colour::RESET << "\n";
            cout << "  Press any key...";
            cout.flush();
            RawTerm::readKey();
        }
    }
    // loads questions for the topic and runs the quiz, or shows an error if none exist.

private:

    void runQuiz() {
        quiz.reset();
        while(quiz.hasNext()) {
            Term::clearScreen();
            drawQuizHeader();

            const QuizQuestion* q = quiz.currentQuestion();
            if(!q) break;

            int pct = quiz.getTotal() > 0 ? quiz.getCurrent()*100/quiz.getTotal() : 0;
            int filled = pct * 40 / 100;
            cout << " " << Colour::MUTED << "[";
            for(int i = 0; i < filled; i++)  cout << Colour::PURPLE << "█";
            for(int i = filled; i < 40; i++) cout << Colour::MUTED << "░";
            cout << Colour::MUTED << "] " << Colour::AMBER << pct << "%\n\n";

            cout << " " << Colour::AMBER << Colour::BOLD
                 << "Q" << (quiz.getCurrent()+1) << "/" << quiz.getTotal()
                 << Colour::RESET << "  "
                 << Colour::WHITE << q->question << Colour::RESET << "\n\n";

            for(int i = 0; i < 4; i++) {
                cout << "  " << Colour::TEAL << Colour::BOLD << (char)('A'+i) << ". "
                     << Colour::RESET << q->options[i] << "\n";
            }

            cout << "\n " << Colour::MUTED << "Press A/B/C/D: " << Colour::RESET;
            cout.flush();

            int k = RawTerm::readKey();
            if(k >= 'a' && k <= 'z') k -= 32;
            int choice = k - 'A';

            if(choice < 0 || choice > 3) {
                cout << Colour::CORAL << "\n  Invalid. Skipping.\n" << Colour::RESET;
                quiz.answer(0);
                this_thread::sleep_for(chrono::milliseconds(800));
                continue;
            }

            bool correct = quiz.answer(choice);
            if(correct) {
                cout << "\n " << Colour::BRIGHT_GREEN << Colour::BOLD
                     << "✓  Correct!" << Colour::RESET << "\n";
            } else {
                cout << "\n " << Colour::CORAL << Colour::BOLD
                     << "✗  Wrong.  Correct: " << (char)('A'+q->correctIndex)
                     << Colour::RESET << "\n";
            }
            cout << " " << Colour::MUTED << q->explanation << Colour::RESET << "\n";
            cout << "\n " << Colour::MUTED << "Press any key..." << Colour::RESET;
            cout.flush();
            RawTerm::readKey();
        }

        Term::clearScreen();
        drawQuizHeader();

        int score = quiz.getScore();
        int total = quiz.getTotal();
        int pct = total > 0 ? score*100/total : 0;
        int stars = (pct >= 96) ? 5 : (pct >= 80) ? 4 : (pct >= 60) ? 3 : (pct >= 40) ? 2 : 1;
        string grade =
            (pct == 100) ? "Perfect — flawless!" :
            (pct >= 80)  ? "Great job!"          :
            (pct >= 60)  ? "Good effort!"        :
            (pct >= 40)  ? "Keep studying!"      :
                           "Review this topic.";

        cout << "\n\n  ";
        for(int i = 1; i <= 5; i++) {
            if(i <= stars) cout << Colour::AMBER << Colour::BOLD << "★ " << Colour::RESET;
            else           cout << Colour::MUTED << "☆ " << Colour::RESET;
        }
        cout << "\n\n";

        cout << "  " << Colour::WHITE << Colour::BOLD << score << " / " << total << " correct"
             << Colour::RESET << "   " << Colour::TEAL << grade << Colour::RESET << "\n\n";

        int filled = pct * 40 / 100;
        cout << "  [";
        for(int i = 0; i < filled;  i++) cout << Colour::TEAL << "█";
        for(int i = filled; i < 40; i++) cout << Colour::MUTED << "░";
        cout << Colour::RESET << "] " << Colour::AMBER << pct << "%" << Colour::RESET << "\n\n";

        cout << " " << Colour::MUTED << "Press any key to return..." << Colour::RESET;
        cout.flush();
        RawTerm::readKey();
    }
    // runs questions one at a time then shows a final score with a star rating.

    void drawQuizHeader() const {
        cout << Colour::PURPLE << "╔";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "═";
        cout << Colour::PURPLE << "╗" << Colour::RESET << "\n";

        cout << Colour::PURPLE << "║" << Colour::RESET;
        cout << " " << Colour::PURPLE << Colour::BOLD << "C++.xray "
             << Colour::RESET << Colour::MUTED << "│ "
             << Colour::AMBER << Colour::BOLD << "Quiz Mode"
             << Colour::MUTED << " — " << quiz.getTopic()
             << "  score: " << quiz.getScore() << "/" << quiz.getCurrent()
             << Colour::RESET << "\n";

        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
    }
    // header for the quiz screen, showing the running score.

    static int safeReadInt(const string& prompt) {
        while(true) {
            cout << prompt;
            cout.flush();
            int n;
            if(cin >> n) return n;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << Colour::CORAL << "  Not a number — enter an integer.\n" << Colour::RESET;
        }
    }
    // safe int read that clears the cin fail state on bad input and re-prompts.

public:

    void runGraphEditor(function<void(AlgorithmBase*)> onRun) {
        GraphBuilder builder;

        {
            CookedScope cs(rawTerm);

            Term::hardClear();
            drawEditorHeader();
            cout << "\n";

            int n;
            while(true) {
                n = safeReadInt(" " + Colour::TEAL + "Number of nodes (2-12): " + Colour::RESET);
                if(n >= 2 && n <= 12) break;
                cout << Colour::CORAL << "  Must be between 2 and 12.\n" << Colour::RESET;
            }
            try { builder.setNodes(n); }
            catch(const LibraryError& e) {
                cout << Colour::CORAL << "  " << e.what() << Colour::RESET << "\n";
                cout << "  Press Enter..."; cin.ignore(numeric_limits<streamsize>::max(),'\n'); cin.get();
                return;
            }

            int src;
            while(true) {
                src = safeReadInt(" " + Colour::TEAL + "Source node (0-" + to_string(n-1) + "): " + Colour::RESET);
                if(src >= 0 && src < n) break;
                cout << Colour::CORAL << "  Must be 0 to " << n-1 << ".\n" << Colour::RESET;
            }
            builder.setSource(src);

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        string editorMsg;
        string editorMsgCol = Colour::TEAL;

        while(true) {
            drawEditorScreen(builder, editorMsg, editorMsgCol);
            editorMsg.clear();

            int k = RawTerm::readKey();
            if(k >= 'a' && k <= 'z') k -= 32;

            if(k == Key::ESC || k == 'X') {
                editorMsg = "Cancelled — returning to main menu.";
                drawEditorScreen(builder, editorMsg, Colour::CORAL);
                this_thread::sleep_for(chrono::milliseconds(600));
                return;
            }
            if(k == Key::ENTER) {
                if(builder.edges.size() == 0) {
                    editorMsg = "Add at least one edge before running.";
                    editorMsgCol = Colour::CORAL;
                    continue;
                }
                break;
            }
            if(k == 'A') {
                CookedScope cs(rawTerm);
                cout << "\n " << Colour::TEAL << "New edge (format: u v weight, e.g. 0 1 5): "
                     << Colour::RESET;
                cout.flush();
                string line; getline(cin, line);
                istringstream iss(line);
                int u, v, w;
                if(!(iss >> u >> v >> w)) {
                    editorMsg = "Need three integers: u v weight";
                    editorMsgCol = Colour::CORAL;
                } else {
                    try {
                        builder.addEdge(u, v, w);
                        editorMsg = "Added edge " + to_string(u) + " ↔ " + to_string(v) +
                                    " (w=" + to_string(w) + ")";
                        editorMsgCol = Colour::BRIGHT_GREEN;
                    } catch(const LibraryError& e) {
                        editorMsg = e.what();
                        editorMsgCol = Colour::CORAL;
                    }
                }
                continue;
            }
            if(k == 'R') {
                if(builder.edges.size() == 0) {
                    editorMsg = "No edges to remove.";
                    editorMsgCol = Colour::CORAL;
                    continue;
                }
                CookedScope cs(rawTerm);
                int idx = safeReadInt("\n " + Colour::TEAL + "Edge index to remove: " + Colour::RESET);
                try {
                    builder.removeEdge(idx);
                    editorMsg = "Removed edge [" + to_string(idx) + "]";
                    editorMsgCol = Colour::AMBER;
                } catch(const LibraryError& e) {
                    editorMsg = e.what();
                    editorMsgCol = Colour::CORAL;
                }
                continue;
            }
            if(k == 'C') {
                builder.clear();
                editorMsg = "All edges cleared.";
                editorMsgCol = Colour::AMBER;
                continue;
            }
            if(k == 'S') {
                CookedScope cs(rawTerm);
                cout << "\n " << Colour::TEAL << "Save as (filename): " << Colour::RESET;
                cout.flush();
                string fn; getline(cin, fn);
                if(fn.empty()) {
                    editorMsg = "Save cancelled.";
                    editorMsgCol = Colour::MUTED;
                } else {
                    try {
                        builder.saveToFile(fn);
                        editorMsg = "Saved to " + fn;
                        editorMsgCol = Colour::BRIGHT_GREEN;
                    } catch(const LibraryError& e) {
                        editorMsg = e.what();
                        editorMsgCol = Colour::CORAL;
                    }
                }
                continue;
            }
            if(k == 'L') {
                CookedScope cs(rawTerm);
                cout << "\n " << Colour::TEAL << "Load from (filename): " << Colour::RESET;
                cout.flush();
                string fn; getline(cin, fn);
                if(fn.empty()) {
                    editorMsg = "Load cancelled.";
                    editorMsgCol = Colour::MUTED;
                } else {
                    try {
                        builder.loadFromFile(fn);
                        editorMsg = "Loaded " + to_string((int)builder.edges.size()) +
                                    " edges from " + fn;
                        editorMsgCol = Colour::BRIGHT_GREEN;
                    } catch(const LibraryError& e) {
                        editorMsg = e.what();
                        editorMsgCol = Colour::CORAL;
                    }
                }
                continue;
            }
            if(k == 'P') {
                if(builder.edges.size() == 0) {
                    editorMsg = "Nothing to preview yet.";
                    editorMsgCol = Colour::CORAL;
                    continue;
                }
                previewGraph(builder);
                continue;
            }
        }

        try {
            auto* d = new DijkstraAlgorithm(builder.nodeCount, builder.source);
            for(int i = 0; i < (int)builder.edges.size(); i++)
                d->addEdge(builder.edges[i].u, builder.edges[i].v, builder.edges[i].w);
            onRun(d);
        } catch(const LibraryError& e) {
            cout << Colour::CORAL << "\n  " << e.what() << Colour::RESET << "\n";
            cout << "  Press any key..."; cout.flush();
            RawTerm::readKey();
        }
    }
    // full menu loop for building a custom graph. uses cooked mode for line input, raw mode for menu keys.

private:

    void drawEditorHeader() const {
        cout << Colour::PURPLE << "╔";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "═";
        cout << Colour::PURPLE << "╗" << Colour::RESET << "\n";
        cout << Colour::PURPLE << "║" << Colour::RESET;
        cout << " " << Colour::PURPLE << Colour::BOLD << "C++.xray "
             << Colour::RESET << Colour::MUTED << "│ "
             << Colour::GREEN << Colour::BOLD << "Custom Graph Editor"
             << Colour::RESET << "\n";
        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
    }
    // small header banner for the graph editor screens.

    void drawEditorScreen(const GraphBuilder& b, const string& msg, const string& msgCol) const {
        ostringstream buf;
        streambuf* old = cout.rdbuf(buf.rdbuf());

        cout << "\033[2J\033[H";
        drawEditorHeader();

        cout << " " << Colour::MUTED << "Nodes: " << Colour::WHITE << b.nodeCount
             << Colour::MUTED << "    Source: " << Colour::BRIGHT_GREEN << b.source
             << Colour::MUTED << "    Edges: " << Colour::AMBER << b.edges.size()
             << Colour::RESET << "\n\n";

        if(b.edges.size() == 0) {
            cout << "   " << Colour::MUTED << "(no edges yet — press [A] to add)\n" << Colour::RESET;
        } else {
            for(int i = 0; i < (int)b.edges.size(); i++) {
                const auto& e = b.edges[i];
                cout << "   " << Colour::MUTED << "[" << Term::padLeft(to_string(i),2) << "]  "
                     << Colour::WHITE << e.u << " ↔ " << e.v
                     << Colour::MUTED << "   w="
                     << Colour::AMBER << e.w << Colour::RESET << "\n";
            }
        }
        cout << "\n";

        if(!msg.empty()) {
            cout << " " << msgCol << "● " << msg << Colour::RESET << "\n";
        } else {
            cout << "\n";
        }
        cout << "\n";

        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n";
        cout << " " << Colour::MUTED << "Edit: "
             << Colour::BRIGHT_GREEN << "[A]" << Colour::MUTED << "add  "
             << Colour::AMBER << "[R]" << Colour::MUTED << "remove  "
             << Colour::CORAL << "[C]" << Colour::MUTED << "clear  "
             << Colour::TEAL << "[P]" << Colour::MUTED << "preview"
             << Colour::RESET << "\n";
        cout << " " << Colour::MUTED << "File: "
             << Colour::TEAL << "[S]" << Colour::MUTED << "save  "
             << Colour::TEAL << "[L]" << Colour::MUTED << "load   "
             << Colour::MUTED << "│   "
             << Colour::BRIGHT_GREEN << "[Enter]" << Colour::MUTED << "run  "
             << Colour::CORAL << "[ESC/X]" << Colour::MUTED << "cancel"
             << Colour::RESET << "\n";
        cout << Colour::PURPLE << "╚";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "═";
        cout << Colour::PURPLE << "╝" << Colour::RESET << "\n";

        cout.rdbuf(old);
        cout << buf.str();
        cout.flush();
    }
    // frame-buffered draw of the editor screen: state line, edge list, status, controls.

    void previewGraph(const GraphBuilder& b) const {
        Term::hardClear();
        try {
            DijkstraAlgorithm dpv(b.nodeCount, b.source);
            for(int i = 0; i < (int)b.edges.size(); i++)
                dpv.addEdge(b.edges[i].u, b.edges[i].v, b.edges[i].w);
            dpv.generateSteps();

            cout << Colour::PURPLE << Colour::BOLD
                 << "  Graph Preview  " << Colour::RESET
                 << Colour::MUTED << "(nodes=" << b.nodeCount
                 << "  source=" << b.source
                 << "  edges=" << b.edges.size() << ")\n\n" << Colour::RESET;

            if(!dpv.steps.empty())
                dpv.steps[0]->render();

            cout << "\n  " << Colour::MUTED << "Press any key to return to editor..."
                 << Colour::RESET;
            cout.flush();
            RawTerm::readKey();
        } catch(const LibraryError& e) {
            cout << Colour::CORAL << "  " << e.what() << Colour::RESET << "\n";
            cout << "  Press any key..."; cout.flush();
            RawTerm::readKey();
        }
    }
    // builds a one-step Dijkstra just to render the current graph state for preview.

public:

    void runComplexityLab() {
        Term::hardClear();
        cout << Colour::PURPLE << "╔";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "═";
        cout << Colour::PURPLE << "╗" << Colour::RESET << "\n";
        cout << Colour::PURPLE << "║" << Colour::RESET;
        cout << " " << Colour::PURPLE << Colour::BOLD << "C++.xray "
             << Colour::RESET << Colour::MUTED << "│ "
             << Colour::AMBER << Colour::BOLD << "Complexity Lab"
             << Colour::RESET << "\n";
        cout << Colour::PURPLE << "╠";
        cout << Colour::MUTED;
        for(int i = 0; i < SW-2; i++) cout << "─";
        cout << Colour::PURPLE << "╣" << Colour::RESET << "\n\n";

        int n;
        {
            CookedScope cs(rawTerm);
            while(true) {
                n = safeReadInt(" " + Colour::TEAL + "Array size (2-20): " + Colour::RESET);
                if(n >= 2 && n <= 20) break;
                cout << Colour::CORAL << "  Must be 2 to 20.\n" << Colour::RESET;
            }
        }

        int srcArr[] = {64,34,25,12,22,11,90,45,67,33,88,15,72,50,3,99,41,28,77,56};
        MyVector<int> arr;
        for(int i = 0; i < n; i++) arr.push_back(srcArr[i % 20]);

        cout << "\n " << Colour::TEAL << "Running all three sorts on n=" << n << "...\n"
             << Colour::RESET;

        BubbleSortAlgorithm bub(arr); bub.generateSteps();
        MergeSortAlgorithm  mrg(arr); mrg.generateSteps();
        QuickSortAlgorithm  qck(arr); qck.generateSteps();

        auto getCx = [](AlgorithmBase& a) -> ComplexityData {
            if(a.steps.empty()) return {};
            SortState* ss = dynamic_cast<SortState*>(a.steps[(int)a.steps.size()-1]);
            return ss ? ss->cx : ComplexityData{};
        };

        ComplexityData bc = getCx(bub);
        ComplexityData mc = getCx(mrg);
        ComplexityData qc = getCx(qck);

        int winnerIdx = 0;
        int minCmp = bc.comparisons;
        if(mc.comparisons < minCmp) { minCmp = mc.comparisons; winnerIdx = 1; }
        if(qc.comparisons < minCmp) { minCmp = qc.comparisons; winnerIdx = 2; }
        // winner = whichever sort did the fewest comparisons.

        cout << "\n";
        cout << Colour::MUTED;
        for(int i = 0; i < SW; i++) cout << "─";
        cout << Colour::RESET << "\n";

        auto printRow = [&](int idx, const string& name, const string& col,
                            ComplexityData& cx, const string& big_o) {
            int pct = cx.theoretical > 0 ? cx.comparisons*100/cx.theoretical : 0;
            int bar = pct * 20 / 100; if(bar > 20) bar = 20;
            string crown = (idx == winnerIdx)
                ? (Colour::BRIGHT_GREEN + " ✓ WINNER" + Colour::RESET)
                : "";
            cout << "  " << col << Colour::BOLD << Term::padRight(name,13) << Colour::RESET
                 << "  " << Colour::MUTED << "n=" << Colour::WHITE << cx.n
                 << Colour::MUTED << "  cmp=" << Colour::AMBER << cx.comparisons
                 << Colour::MUTED << "  swaps=" << Colour::CORAL << cx.swaps
                 << Colour::MUTED << "  ≤" << Colour::PURPLE << cx.theoretical << Colour::RESET
                 << crown << "\n";
            cout << "  " << Colour::MUTED << Term::padRight(big_o,13)
                 << "  [";
            for(int i = 0; i < bar; i++)  cout << col << "█";
            for(int i = bar; i < 20; i++) cout << Colour::MUTED << "░";
            cout << Colour::MUTED << "] " << Colour::TEAL << pct << "%" << Colour::RESET << "\n\n";
        };

        printRow(0, "Bubble Sort", Colour::CORAL,  bc, "O(n²)");
        printRow(1, "Merge Sort",  Colour::TEAL,   mc, "O(n log n)");
        printRow(2, "Quick Sort",  Colour::PURPLE, qc, "O(n log n)");

        FileManager::appendLog("complexity.log","bubble",bc.comparisons,bc.swaps,bc.theoretical,n);
        FileManager::appendLog("complexity.log","merge", mc.comparisons,mc.swaps,mc.theoretical,n);
        FileManager::appendLog("complexity.log","quick", qc.comparisons,qc.swaps,qc.theoretical,n);

        cout << " " << Colour::MUTED << "Results appended to complexity.log\n" << Colour::RESET;
        cout << "\n " << Colour::MUTED << "Press any key..." << Colour::RESET;
        cout.flush();
        RawTerm::readKey();
    }
    // runs all three sorts on the same input and shows comparison counts side-by-side with a winner badge.
};
// the main UI controller. owns the current algorithm, tracks the step index, and renders each frame.

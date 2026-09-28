#include "AlgorithmState.h"
#include "Terminal.h"
#include <iostream>
#include <string>
#include <climits>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

// ────────────────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────────────────

static string nodeColour(NodeState s) {
    switch(s) {
        case NodeState::SOURCE:    return Colour::BRIGHT_GREEN;
        case NodeState::CURRENT:   return Colour::AMBER;
        case NodeState::VISITED:   return Colour::TEAL;
        case NodeState::IN_QUEUE:  return Colour::PURPLE;
        case NodeState::PATH:      return Colour::BRIGHT_YELLOW;
        default:                   return Colour::MUTED;
    }
}

// ────────────────────────────────────────────────────────────────────────────
// GraphState::render()  —  2-D graph with Bresenham edges
// ────────────────────────────────────────────────────────────────────────────

void GraphState::render() const {
    int n = static_cast<int>(nodes.size());
    if(n == 0) return;

    // ── legend ──────────────────────────────────────────────────────────────
    struct LI { NodeState s; const char* lbl; };
    LI leg[] = {
        {NodeState::SOURCE,    "Source "},
        {NodeState::CURRENT,   "Active "},
        {NodeState::IN_QUEUE,  "Queued "},
        {NodeState::VISITED,   "Visited"},
        {NodeState::UNVISITED, "Idle   "}
    };
    cout << "  ";
    for(auto& L : leg)
        cout << nodeColour(L.s) << "●" << Colour::MUTED << " " << L.lbl << Colour::RESET << "  ";
    cout << "\n\n";

    // ── 2-D character grid ──────────────────────────────────────────────────
    const int GR = 13;
    const int GC = 70;

    char  gc[13][71];   // characters
    int   gn[13][71];   // -3=empty -2=weight -1=edge >=0=node index
    bool  occ[13][71];  // cells occupied by a node (don't draw edges here)

    for(int r = 0; r < GR; r++) {
        for(int c = 0; c < GC; c++) {
            gc[r][c] = ' ';
            gn[r][c] = -3;
            occ[r][c] = false;
        }
        gc[r][GC] = '\0';
    }

    // ── circular node positions (aspect-ratio corrected) ───────────────────
    const double PI = 3.14159265358979;
    int px[12], py[12];

    double CX = GC * 0.50;
    double CY = GR * 0.50;
    double rx, ry;
    if(n <= 2)      { rx = GC*0.28; ry = GR*0.28; }
    else if(n <= 4) { rx = GC*0.33; ry = GR*0.33; }
    else if(n <= 6) { rx = GC*0.38; ry = GR*0.38; }
    else            { rx = GC*0.43; ry = GR*0.42; }

    for(int i = 0; i < n && i < 12; i++) {
        double angle = (n == 1) ? -PI/2 : (2.0*PI*i/n - PI/2.0);
        px[i] = (int)(CX + rx * cos(angle));
        py[i] = (int)(CY + ry * sin(angle));
        // clamp — leave room for 4-char node label
        if(px[i] < 2)      px[i] = 2;
        if(px[i] > GC-5)   px[i] = GC-5;
        if(py[i] < 0)      py[i] = 0;
        if(py[i] > GR-1)   py[i] = GR-1;
    }

    // mark node cells occupied (+1 padding on each side)
    for(int i = 0; i < n && i < 12; i++) {
        int lw = (i < 10) ? 3 : 4;      // "[N]" vs "[NN]"
        int sc = px[i] - lw/2 - 1;
        int ec = px[i] + lw/2 + 1;
        for(int c = sc; c <= ec; c++)
            if(c >= 0 && c < GC && py[i] >= 0 && py[i] < GR)
                occ[py[i]][c] = true;
    }

    // ── draw edges (Bresenham) ───────────────────────────────────────────────
    for(int ei = 0; ei < static_cast<int>(edges.size()); ei++) {
        const auto& e = edges[ei];
        if(e.u < 0 || e.u >= n || e.v < 0 || e.v >= n) continue;

        int x1 = px[e.u], y1 = py[e.u];
        int x2 = px[e.v], y2 = py[e.v];
        int adx = abs(x2-x1), ady = abs(y2-y1);
        int sx  = (x1 < x2) ? 1 : -1;
        int sy  = (y1 < y2) ? 1 : -1;

        // pick edge character from overall slope
        char ec;
        if(ady == 0)      ec = '-';
        else if(adx == 0) ec = '|';
        else              ec = (sx == sy) ? '\\' : '/';

        // Bresenham walk — skip endpoints (node cells) and occupied cells
        int x = x1, y = y1;
        int err = adx - ady;
        while(x != x2 || y != y2) {
            int e2 = 2 * err;
            if(e2 > -ady) { err -= ady; x += sx; }
            if(e2 <  adx) { err += adx; y += sy; }
            if(x == x2 && y == y2) break;
            if(y >= 0 && y < GR && x >= 0 && x < GC && !occ[y][x]) {
                gc[y][x] = ec;
                gn[y][x] = -1;
            }
        }

        // weight label at midpoint
        if(e.w > 0) {
            int mx = (x1+x2)/2, my = (y1+y2)/2;
            string ws = to_string(e.w);
            int sc2 = mx - (int)ws.size()/2;
            // prefer the row above or below the midpoint if on an edge char
            int wy = my;
            if(my > 0 && !occ[my-1][mx] && gc[my-1][mx] == ' ') wy = my-1;
            for(int k = 0; k < (int)ws.size(); k++) {
                int c = sc2 + k;
                if(c >= 0 && c < GC && wy >= 0 && wy < GR && !occ[wy][c]) {
                    gc[wy][c] = ws[(size_t)k];
                    gn[wy][c] = -2;
                }
            }
        }
    }

    // ── draw nodes on top ────────────────────────────────────────────────────
    for(int i = 0; i < n && i < 12; i++) {
        string label = "[" + to_string(i) + "]";
        int lw  = (int)label.size();
        int sc2 = px[i] - lw/2;
        for(int k = 0; k < lw; k++) {
            int c = sc2 + k;
            if(c >= 0 && c < GC && py[i] >= 0 && py[i] < GR) {
                gc[py[i]][c] = label[(size_t)k];
                gn[py[i]][c] = i;
            }
        }
    }

    // ── print grid (skip fully empty rows) ───────────────────────────────────
    for(int r = 0; r < GR; r++) {
        bool any = false;
        for(int c = 0; c < GC; c++) if(gc[r][c] != ' ') { any = true; break; }
        if(!any) continue;

        cout << "  ";
        for(int c = 0; c < GC; c++) {
            int ci = gn[r][c];
            if(ci >= 0 && ci < n)
                cout << nodeColour(nodes[ci].state) << Colour::BOLD;
            else if(ci == -1)
                cout << Colour::MUTED;
            else if(ci == -2)
                cout << Colour::AMBER << Colour::BOLD;
            cout << gc[r][c];
            if(ci >= -2)
                cout << Colour::RESET;
        }
        cout << "\n";
    }
    cout << "\n";

    // ── distance table ───────────────────────────────────────────────────────
    cout << "  " << Colour::MUTED << "Dist: " << Colour::RESET;
    for(int i = 0; i < n; i++) {
        const auto& nd = nodes[i];
        string col = nodeColour(nd.state);
        string ds  = (nd.dist == INT_MAX) ? "∞" : to_string(nd.dist);
        cout << col << "[" << nd.id << "]=" << ds << Colour::RESET;
        if(i < n-1) cout << Colour::MUTED << " · " << Colour::RESET;
    }
    cout << "\n";

    // ── edge list (compact) ───────────────────────────────────────────────────
    cout << "  " << Colour::MUTED << "Edges: " << Colour::RESET;
    int lineLen = 0;
    for(int i = 0; i < static_cast<int>(edges.size()); i++) {
        const auto& e = edges[i];
        string s = "(" + to_string(e.u) + "↔" + to_string(e.v);
        if(e.w > 0) s += " w=" + to_string(e.w);
        s += ")";
        if(lineLen + (int)s.size() > 60) { cout << "\n         "; lineLen = 0; }
        cout << Colour::MUTED << s << Colour::RESET;
        if(i < (int)edges.size()-1) { cout << " "; lineLen += (int)s.size()+1; }
    }
    cout << "\n";
}

// ────────────────────────────────────────────────────────────────────────────
// SortState::render()  —  bars with separators and comparison arrows
// ────────────────────────────────────────────────────────────────────────────

void SortState::render() const {
    const int BAR_H = 12;
    int n = static_cast<int>(bars.size());
    if(n == 0) return;

    // Each bar slot: 3 filled chars + 1 separator = 4 wide (same as before)
    int maxVal = 1;
    for(int i = 0; i < n; i++)
        if(bars[i].value > maxVal) maxVal = bars[i].value;

    // ── bars (top-down) ───────────────────────────────────────────────────────
    for(int row = BAR_H; row >= 1; row--) {
        cout << "  ";
        for(int i = 0; i < n; i++) {
            int barH = (int)round((double)bars[i].value / maxVal * BAR_H);

            string bodyCol, sepCol;
            if(bars[i].isPivot)        { bodyCol = Colour::AMBER;        sepCol = Colour::MUTED; }
            else if(bars[i].sorted)    { bodyCol = Colour::BRIGHT_GREEN; sepCol = Colour::MUTED; }
            else if(bars[i].highlighted){ bodyCol = Colour::CORAL;        sepCol = Colour::MUTED; }
            else                        { bodyCol = Colour::PURPLE;       sepCol = Colour::MUTED; }

            if(row <= barH) {
                // highlight top edge of bar
                if(row == barH)
                    cout << bodyCol << Colour::BOLD << "▀▀▀" << Colour::RESET;
                else
                    cout << bodyCol << "███" << Colour::RESET;
            } else {
                cout << "   ";
            }
            // separator column
            if(row <= barH)
                cout << sepCol << "▏" << Colour::RESET;
            else
                cout << sepCol << "▏" << Colour::RESET;
        }
        cout << "\n";
    }

    // ── value labels ─────────────────────────────────────────────────────────
    cout << "  ";
    for(int i = 0; i < n; i++) {
        string col;
        if(bars[i].isPivot)         col = Colour::AMBER;
        else if(bars[i].sorted)     col = Colour::BRIGHT_GREEN;
        else if(bars[i].highlighted)col = Colour::CORAL;
        else                         col = Colour::MUTED;
        cout << col << Term::padRight(to_string(bars[i].value), 3) << Colour::RESET
             << Colour::MUTED << " " << Colour::RESET;
    }
    cout << "\n";

    // ── comparison pointer arrows ─────────────────────────────────────────────
    bool hasArrows = false;
    for(int i = 0; i < n; i++)
        if(bars[i].highlighted || bars[i].isPivot) { hasArrows = true; break; }

    if(hasArrows) {
        cout << "  ";
        for(int i = 0; i < n; i++) {
            if(bars[i].highlighted)
                cout << Colour::CORAL << Colour::BOLD << " ▲ " << Colour::RESET
                     << Colour::MUTED << " " << Colour::RESET;
            else if(bars[i].isPivot)
                cout << Colour::AMBER << Colour::BOLD << " ▲ " << Colour::RESET
                     << Colour::MUTED << " " << Colour::RESET;
            else
                cout << "    ";
        }
        cout << "\n";
    }

    // ── complexity progress bar ───────────────────────────────────────────────
    cout << "\n";
    if(cx.n > 0) {
        int pct    = (cx.theoretical > 0) ? min(100,(int)(cx.comparisons*100.0/cx.theoretical)) : 0;
        int filled = pct * 24 / 100;

        cout << "  " << Colour::MUTED << "cmp=" << Colour::AMBER  << cx.comparisons
             << Colour::MUTED << "  swaps=" << Colour::CORAL  << cx.swaps
             << Colour::MUTED << "  theory≤" << Colour::PURPLE << cx.theoretical
             << Colour::MUTED << "  [";

        for(int i = 0; i < filled;  i++) cout << Colour::TEAL    << "█";
        for(int i = filled; i < 24; i++) cout << Colour::MUTED   << "░";
        cout << Colour::MUTED << "] " << Colour::AMBER << pct << "%" << Colour::RESET << "\n";
    }
}

void TreeState::render() const {
    int n = static_cast<int>(values.size());
    if(n == 0) {
        cout << "  " << Colour::MUTED << "(empty tree)\n" << Colour::RESET;
        return;
    }

    int levels = 1;
    for(int i = 0; i < n; i++) {
        if(values[i] == -1) continue;
        int lv = 0, t = i + 1;
        while(t > 1) { t /= 2; lv++; }
        if(lv + 1 > levels) levels = lv + 1;
    }
    // figures out actual depth, ignoring -1 placeholders.

    const int LEAF_W = 6;
    int bottomCount = 1 << (levels - 1);
    int gridW = bottomCount * LEAF_W;
    if(gridW > 72) gridW = 72;
    // bottom-up sizing so the root doesnt get spread across the whole screen.

    auto nodeLv = [](int idx) {
        int lv = 0, t = idx + 1;
        while(t > 1) { t /= 2; lv++; }
        return lv;
    };
    auto nodeX = [&](int idx) {
        int lv = nodeLv(idx);
        int slot = idx - ((1 << lv) - 1);
        int slotW = gridW / (1 << lv);
        return slot * slotW + slotW / 2;
    };
    auto colourFor = [&](int idx) -> string {
        if(idx == 0 && isMinHeap) return Colour::BLUE;
        if(values[idx] == insertedVal) return Colour::BRIGHT_GREEN;
        return Colour::PURPLE;
    };
    // helpers: level of a node, x position of a node, and the colour to draw it with.

    for(int lv = 0; lv < levels; lv++) {
        {
            vector<char> chars((size_t)gridW, ' ');
            vector<int> owner((size_t)gridW, -1);

            int startIdx = (1 << lv) - 1;
            int endIdx = min((1 << (lv + 1)) - 1, n);
            for(int idx = startIdx; idx < endIdx; idx++) {
                if(values[idx] == -1) continue;
                string label = "(" + to_string(values[idx]) + ")";
                int cx = nodeX(idx);
                int start = cx - (int)label.size() / 2;
                if(start < 0) start = 0;
                if(start + (int)label.size() > gridW) start = gridW - (int)label.size();
                for(int i = 0; i < (int)label.size(); i++) {
                    chars[(size_t)(start + i)] = label[(size_t)i];
                    owner[(size_t)(start + i)] = idx;
                }
            }

            cout << "  ";
            int i = 0;
            while(i < gridW) {
                if(owner[(size_t)i] == -1) {
                    cout << chars[(size_t)i];
                    i++;
                } else {
                    int o = owner[(size_t)i];
                    cout << colourFor(o) << Colour::BOLD;
                    while(i < gridW && owner[(size_t)i] == o) {
                        cout << chars[(size_t)i];
                        i++;
                    }
                    cout << Colour::RESET;
                }
            }
            cout << "\n";
        }
        // node row: lay out labels with their owners, then emit each contiguous run with one colour.

        if(lv < levels - 1) {
            vector<string> cells((size_t)gridW, " ");
            int startIdx = (1 << lv) - 1;
            int endIdx = min((1 << (lv + 1)) - 1, n);

            for(int idx = startIdx; idx < endIdx; idx++) {
                if(values[idx] == -1) continue;
                int px = nodeX(idx);

                int lc = 2 * idx + 1;
                if(lc < n && values[lc] != -1) {
                    int cx = nodeX(lc);
                    int x1 = cx + 1, x2 = px - 1;
                    for(int x = x1; x <= x2; x++)
                        if(x >= 0 && x < gridW) cells[(size_t)x] = "─";
                    if(x1 - 1 >= 0 && x1 - 1 < gridW) cells[(size_t)(x1 - 1)] = "╱";
                }

                int rc = 2 * idx + 2;
                if(rc < n && values[rc] != -1) {
                    int cx = nodeX(rc);
                    int x1 = px + 1, x2 = cx - 1;
                    for(int x = x1; x <= x2; x++)
                        if(x >= 0 && x < gridW) cells[(size_t)x] = "─";
                    if(x2 + 1 >= 0 && x2 + 1 < gridW) cells[(size_t)(x2 + 1)] = "╲";
                }
            }

            cout << "  " << Colour::MUTED;
            for(int x = 0; x < gridW; x++) cout << cells[(size_t)x];
            cout << Colour::RESET << "\n";
        }
        // connector row: horizontal ─ bars with ╱ and ╲ diagonals at the ends. uses string cells since these are multi-byte.
    }

    // array view
    cout << "\n  " << Colour::MUTED << "Array: [ " << Colour::RESET;
    for(int i = 0; i < n; i++) {
        if(values[i] == -1) {
            cout << Colour::MUTED << "─" << Colour::RESET;
        } else {
            string col;
            if(i == 0 && isMinHeap)             col = Colour::BLUE;
            else if(values[i] == insertedVal)    col = Colour::BRIGHT_GREEN;
            else                                  col = Colour::PURPLE;
            cout << col << values[i] << Colour::RESET;
        }
        if(i < n-1) cout << Colour::MUTED << ", " << Colour::RESET;
    }
    cout << Colour::MUTED << " ]" << Colour::RESET;

    if(isMinHeap && n > 0 && values[0] != -1)
        cout << "  " << Colour::BRIGHT_GREEN << "min=" << values[0] << Colour::RESET;
    cout << "\n";
}

// ────────────────────────────────────────────────────────────────────────────
// MemoryState::render()
// ────────────────────────────────────────────────────────────────────────────

void MemoryState::render() const {
    const int COL_W = 34;

    // column headers
    cout << "  ";
    cout << Colour::TEAL  << Colour::BOLD << Term::centre("STACK  (auto)", COL_W) << Colour::RESET;
    cout << "  ";
    cout << Colour::AMBER << Colour::BOLD << Term::centre("HEAP  (new/delete)", COL_W) << Colour::RESET;
    cout << "\n  ";

    // header dividers
    cout << Colour::TEAL;
    for(int i = 0; i < COL_W; i++) cout << "─";
    cout << Colour::RESET << "  ";
    cout << Colour::AMBER;
    for(int i = 0; i < COL_W; i++) cout << "─";
    cout << Colour::RESET << "\n";

    int maxRows = (int)max(stackCells.size(), heapCells.size());
    if(maxRows == 0) maxRows = 1;

    for(int i = 0; i < maxRows; i++) {
        cout << "  ";
        // stack column (reversed: latest frame first)
        if(i < (int)stackCells.size()) {
            const auto& c = stackCells[(int)stackCells.size()-1-i];
            cout << Colour::TEAL << "│ " << Colour::RESET;
            cout << Colour::MUTED << Term::padRight(c.address,10) << Colour::RESET << " ";
            cout << Colour::BRIGHT_GREEN << Term::padRight(c.label,12) << Colour::RESET << " ";
            cout << Colour::WHITE << Term::padRight(c.value,7) << Colour::RESET;
            cout << Colour::TEAL << " │" << Colour::RESET;
        } else {
            cout << Colour::MUTED << "│" << string(COL_W-2,' ') << "│" << Colour::RESET;
        }

        cout << "  ";
        // heap column
        if(i < (int)heapCells.size()) {
            const auto& c = heapCells[i];
            bool freed = (c.type == MemCellType::HEAP_FREED);
            string col = freed ? Colour::CORAL : Colour::AMBER;
            cout << col << "│ " << Colour::RESET;
            cout << Colour::MUTED << Term::padRight(c.address,10) << Colour::RESET << " ";
            cout << col << Term::padRight(c.label,12) << Colour::RESET << " ";
            cout << (freed ? Colour::CORAL : Colour::WHITE)
                 << Term::padRight(freed ? "FREED" : c.value, 7) << Colour::RESET;
            cout << col << " │" << Colour::RESET;
        } else {
            cout << Colour::MUTED << "│" << string(COL_W-2,' ') << "│" << Colour::RESET;
        }

        // pointer arrow: if stack cell holds a heap address, show →
        if(i < (int)stackCells.size() && i < (int)heapCells.size()) {
            const auto& sc = stackCells[(int)stackCells.size()-1-i];
            if(sc.value.find("0x") != string::npos
               && sc.value.size() > 2
               && heapCells[0].address.find("0x") != string::npos) {
                // crude check: stack value matches a heap address
                for(int h = 0; h < (int)heapCells.size(); h++) {
                    if(sc.value.find(heapCells[h].address) != string::npos
                       || heapCells[h].address.find(sc.value.substr(0,4)) != string::npos) {
                        // draw nothing extra here — layout is already clear
                        break;
                    }
                }
            }
        }
        cout << "\n";
    }

    // footer dividers
    cout << "  ";
    cout << Colour::TEAL;
    for(int i = 0; i < COL_W; i++) cout << "─";
    cout << Colour::RESET << "  ";
    cout << Colour::AMBER;
    for(int i = 0; i < COL_W; i++) cout << "─";
    cout << Colour::RESET << "\n\n";

    cout << "  " << Colour::MUTED
         << "Stack grows DOWN  │  Heap grows UP  │  "
         << Colour::CORAL << "Red = freed / dangling" << Colour::RESET << "\n";
}

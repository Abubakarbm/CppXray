#pragma once
#include <string>
#include <iostream>
using namespace std;

namespace Colour {
    inline string RESET = "\033[0m";
    inline string BOLD = "\033[1m";
    inline string DIM = "\033[2m";
    inline string ITALIC = "\033[3m";
    inline string UNDERLINE = "\033[4m";

    inline string RED = "\033[31m";
    inline string GREEN = "\033[32m";
    inline string YELLOW = "\033[33m";
    inline string BLUE = "\033[34m";
    inline string MAGENTA = "\033[35m";
    inline string CYAN = "\033[36m";
    inline string WHITE = "\033[37m";

    inline string BRIGHT_GREEN = "\033[92m";
    inline string BRIGHT_YELLOW = "\033[93m";
    inline string BRIGHT_CYAN = "\033[96m";
    inline string BRIGHT_BLUE = "\033[94m";
    inline string BRIGHT_WHITE = "\033[97m";

    inline string PURPLE = "\033[95m";
    inline string TEAL = "\033[96m";
    inline string AMBER = "\033[93m";
    inline string CORAL = "\033[91m";
    inline string MUTED = "\033[90m";

    inline string BG_GRAY = "\033[100m";
    inline string BG_DARK = "\033[48;5;235m";
    inline string BG_NAVY = "\033[48;5;17m";
    inline string BG_PURPLE_D = "\033[48;5;54m";
    inline string BG_TEAL_D = "\033[48;5;23m";
}
// ANSI colour codes. inline so the Theme system can rewrite them at runtime.

namespace Term {
    inline void clearScreen() { cout << "\033[H\033[J"; }
    // moves cursor home and clears down. used by frame-buffered draws so the screen doesnt flash.

    inline void hardClear() { cout << "\033[2J\033[H"; }
    // full screen wipe. for one-shot screens like the banner and submenus.

    inline void hideCursor() { cout << "\033[?25l"; }
    inline void showCursor() { cout << "\033[?25h"; }
    // hide/show the terminal cursor.

    inline string padRight(const string& t, int w) {
        if((int)t.size() >= w) return t.substr(0, (size_t)w);
        return t + string((size_t)w - t.size(), ' ');
    }
    // pads t on the right with spaces to width w, or truncates if too long.

    inline string padLeft(const string& t, int w) {
        if((int)t.size() >= w) return t.substr(0, (size_t)w);
        return string((size_t)w - t.size(), ' ') + t;
    }
    // pads t on the left with spaces to width w.

    inline string centre(const string& t, int w) {
        int pad = (w - (int)t.size()) / 2;
        if(pad < 0) pad = 0;
        return string((size_t)pad, ' ') + t + string((size_t)w - pad - t.size(), ' ');
    }
    // centres t within a field of width w.
}
// this header defines ANSI colour constants and small helpers for terminal output.

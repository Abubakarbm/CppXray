#pragma once
#include <iostream>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include <conio.h>
#else
  #include <termios.h>
  #include <unistd.h>
  #include <sys/select.h>
#endif

namespace Key {
    constexpr int NONE = 0;
    constexpr int ENTER = 13;
    constexpr int ESC = 27;
    constexpr int BACKSPACE = 127;
    constexpr int UP = 1001;
    constexpr int DOWN = 1002;
    constexpr int RIGHT = 1003;
    constexpr int LEFT = 1004;
}
// special key codes are above the ASCII range so regular chars pass through unchanged.

class RawTerm {
public:
    RawTerm() { saveOriginal(); enable(); }
    ~RawTerm() { disable(); }
    // constructor turns raw mode on. destructor restores the original mode.

    void enable() {
        if(enabled_) return;
#ifdef _WIN32
        DWORD m = origInMode_;
        m &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
        SetConsoleMode(hIn_, m);
#else
        termios t = oldt_;
        t.c_lflag &= ~(ICANON | ECHO);
        t.c_cc[VMIN] = 1;
        t.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif
        enabled_ = true;
    }
    // turns off line-buffering and echo so we can read keypress-by-keypress.

    void disable() {
        if(!enabled_) return;
#ifdef _WIN32
        SetConsoleMode(hIn_, origInMode_);
#else
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt_);
        std::cin.clear();
#endif
        enabled_ = false;
    }
    // restores the terminal back to the saved original mode.

    bool isEnabled() const { return enabled_; }

    static int readKey() {
#ifdef _WIN32
        int c = _getch();
        if(c == 0 || c == 0xE0) {
            int c2 = _getch();
            switch(c2) {
                case 72: return Key::UP;
                case 80: return Key::DOWN;
                case 75: return Key::LEFT;
                case 77: return Key::RIGHT;
            }
            return Key::NONE;
        }
        if(c == '\r' || c == '\n') return Key::ENTER;
        return c;
#else
        char c;
        if(::read(STDIN_FILENO, &c, 1) != 1) return Key::NONE;
        if(c != 27) {
            if(c == '\r' || c == '\n') return Key::ENTER;
            return (unsigned char)c;
        }
        if(!waitForByte(50)) return Key::ESC;
        char seq[2];
        if(::read(STDIN_FILENO, &seq[0], 1) != 1) return Key::ESC;
        if(!waitForByte(50)) return Key::ESC;
        if(::read(STDIN_FILENO, &seq[1], 1) != 1) return Key::ESC;
        if(seq[0] == '[') {
            switch(seq[1]) {
                case 'A': return Key::UP;
                case 'B': return Key::DOWN;
                case 'C': return Key::RIGHT;
                case 'D': return Key::LEFT;
            }
        }
        return Key::ESC;
#endif
    }
    // blocks until a key is pressed and returns its code. handles arrow-key escape sequences.

    static bool keyAvailable() {
#ifdef _WIN32
        return _kbhit() != 0;
#else
        fd_set fds; FD_ZERO(&fds); FD_SET(STDIN_FILENO, &fds);
        timeval tv{0, 0};
        return ::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
#endif
    }
    // non-blocking check: is there a key waiting in the input buffer right now?

    static void flushInput() {
        while(keyAvailable()) readKey();
    }
    // drains any leftover keys so the next prompt starts clean.

private:
    bool enabled_ = false;
#ifdef _WIN32
    HANDLE hIn_ = nullptr;
    DWORD origInMode_ = 0;
    void saveOriginal() {
        hIn_ = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hIn_, &origInMode_);
    }
#else
    termios oldt_{};
    void saveOriginal() { tcgetattr(STDIN_FILENO, &oldt_); }

    static bool waitForByte(int ms) {
        fd_set fds; FD_ZERO(&fds); FD_SET(STDIN_FILENO, &fds);
        timeval tv{0, ms * 1000};
        return ::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
    }
#endif
};
// cross-platform RAII guard for raw terminal mode. POSIX uses termios, Windows uses conio.

class CookedScope {
public:
    explicit CookedScope(RawTerm& r) : rt_(r), wasOn_(rt_.isEnabled()) {
        if(wasOn_) rt_.disable();
    }
    ~CookedScope() { if(wasOn_) rt_.enable(); }
private:
    RawTerm& rt_;
    bool wasOn_;
};
// scoped helper to drop to cooked mode for cin >> int / getline, then restores raw mode.

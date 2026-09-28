#pragma once
#include "Theme.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#ifdef _WIN32
  #include <windows.h>
  #include <shlobj.h>
#endif

using namespace std;

class Settings {
public:
    Theme::Mode theme = Theme::Mode::DEFAULT;
    int defaultSpeed = 400;
    bool replayEnabled = false;
    string lastGraphFile = "";

    static string configPath() {
#ifdef _WIN32
        char buf[MAX_PATH];
        if(SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, buf)))
            return string(buf) + "\\cppxray.conf";
        return "cppxray.conf";
#else
        const char* h = getenv("HOME");
        if(h && *h) return string(h) + "/.cppxray.conf";
        return ".cppxray.conf";
#endif
    }
    // resolves the config file path: ~/.cppxray.conf on linux/mac, %APPDATA%\cppxray.conf on windows.

    void load() {
        ifstream f(configPath());
        if(!f.is_open()) return;

        string line;
        while(getline(f, line)) {
            if(line.empty() || line[0] == '#') continue;
            auto pos = line.find('=');
            if(pos == string::npos) continue;
            string k = trim(line.substr(0, pos));
            string v = trim(line.substr(pos + 1));

            if(k == "theme")          theme = Theme::parse(v);
            else if(k == "speed")     defaultSpeed = clampSpeed(parseInt(v, 400));
            else if(k == "replay")    replayEnabled = (v == "1" || v == "true");
            else if(k == "lastGraph") lastGraphFile = v;
        }
    }
    // reads key=value pairs from the config file. silent if the file doesnt exist (first run).

    void save() const {
        ofstream f(configPath());
        if(!f.is_open()) return;
        f << "# C++.xray user settings\n";
        f << "theme=" << Theme::serialize(theme) << "\n";
        f << "speed=" << defaultSpeed << "\n";
        f << "replay=" << (replayEnabled ? "1" : "0") << "\n";
        f << "lastGraph=" << lastGraphFile << "\n";
    }
    // writes current settings out so they persist across runs.

private:
    static string trim(const string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        if(a == string::npos) return "";
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    }
    // strips leading and trailing whitespace from a string.

    static int parseInt(const string& s, int def) {
        try { return stoi(s); } catch(...) { return def; }
    }
    // safe int parse that returns a default if the string isnt a number.

    static int clampSpeed(int s) {
        if(s < 80)   return 80;
        if(s > 2000) return 2000;
        return s;
    }
    // keeps the speed within the supported range.
};
// persistent user settings stored as key=value text. survives across runs.

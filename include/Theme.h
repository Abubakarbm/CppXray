#pragma once
#include "Terminal.h"
#include <string>
using namespace std;

namespace Theme {

    enum class Mode {
        DEFAULT,
        HIGH_CONTRAST,
        COLORBLIND,
        LIGHT
    };
    // the four palettes we ship with.

    inline Mode currentMode = Mode::DEFAULT;

    inline string name(Mode m) {
        switch(m) {
            case Mode::DEFAULT:       return "Default (dark)";
            case Mode::HIGH_CONTRAST: return "High Contrast";
            case Mode::COLORBLIND:    return "Colourblind-safe";
            case Mode::LIGHT:         return "Light Terminal";
        }
        return "?";
    }
    // human-readable label for the menu.

    inline void apply(Mode m) {
        currentMode = m;
        switch(m) {
        case Mode::DEFAULT:
            Colour::RESET = "\033[0m";
            Colour::BOLD = "\033[1m";
            Colour::DIM = "\033[2m";
            Colour::RED = "\033[31m";
            Colour::GREEN = "\033[32m";
            Colour::YELLOW = "\033[33m";
            Colour::BLUE = "\033[34m";
            Colour::MAGENTA = "\033[35m";
            Colour::CYAN = "\033[36m";
            Colour::WHITE = "\033[37m";
            Colour::BRIGHT_GREEN = "\033[92m";
            Colour::BRIGHT_YELLOW = "\033[93m";
            Colour::BRIGHT_CYAN = "\033[96m";
            Colour::BRIGHT_BLUE = "\033[94m";
            Colour::BRIGHT_WHITE = "\033[97m";
            Colour::PURPLE = "\033[95m";
            Colour::TEAL = "\033[96m";
            Colour::AMBER = "\033[93m";
            Colour::CORAL = "\033[91m";
            Colour::MUTED = "\033[90m";
            Colour::BG_DARK = "\033[48;5;235m";
            break;

        case Mode::HIGH_CONTRAST:
            Colour::RESET = "\033[0m";
            Colour::BOLD = "\033[1m";
            Colour::DIM = "\033[0m";
            Colour::WHITE = "\033[97m";
            Colour::PURPLE = "\033[1;95m";
            Colour::TEAL = "\033[1;96m";
            Colour::AMBER = "\033[1;93m";
            Colour::CORAL = "\033[1;91m";
            Colour::MUTED = "\033[37m";
            Colour::BRIGHT_GREEN = "\033[1;92m";
            Colour::BRIGHT_CYAN = "\033[1;96m";
            Colour::BRIGHT_BLUE = "\033[1;94m";
            Colour::BRIGHT_YELLOW = "\033[1;93m";
            Colour::BRIGHT_WHITE = "\033[1;97m";
            Colour::RED = "\033[1;91m";
            Colour::GREEN = "\033[1;92m";
            Colour::YELLOW = "\033[1;93m";
            Colour::BLUE = "\033[1;94m";
            Colour::MAGENTA = "\033[1;95m";
            Colour::CYAN = "\033[1;96m";
            Colour::BG_DARK = "\033[48;5;232m";
            break;

        case Mode::COLORBLIND:
            Colour::RESET = "\033[0m";
            Colour::BOLD = "\033[1m";
            Colour::DIM = "\033[2m";
            Colour::WHITE = "\033[97m";
            Colour::PURPLE = "\033[95m";
            Colour::TEAL = "\033[94m";
            Colour::AMBER = "\033[93m";
            Colour::CORAL = "\033[95m";
            Colour::MUTED = "\033[90m";
            Colour::BRIGHT_GREEN = "\033[96m";
            Colour::BRIGHT_CYAN = "\033[96m";
            Colour::BRIGHT_BLUE = "\033[94m";
            Colour::BRIGHT_YELLOW = "\033[93m";
            Colour::BRIGHT_WHITE = "\033[97m";
            Colour::RED = "\033[95m";
            Colour::GREEN = "\033[96m";
            Colour::YELLOW = "\033[93m";
            Colour::BLUE = "\033[94m";
            Colour::MAGENTA = "\033[95m";
            Colour::CYAN = "\033[96m";
            Colour::BG_DARK = "\033[48;5;235m";
            break;

        case Mode::LIGHT:
            Colour::RESET = "\033[0m";
            Colour::BOLD = "\033[1m";
            Colour::DIM = "\033[2m";
            Colour::WHITE = "\033[30m";
            Colour::PURPLE = "\033[35m";
            Colour::TEAL = "\033[36m";
            Colour::AMBER = "\033[33m";
            Colour::CORAL = "\033[31m";
            Colour::MUTED = "\033[37m";
            Colour::BRIGHT_GREEN = "\033[32m";
            Colour::BRIGHT_CYAN = "\033[36m";
            Colour::BRIGHT_BLUE = "\033[34m";
            Colour::BRIGHT_YELLOW = "\033[33m";
            Colour::BRIGHT_WHITE = "\033[30m";
            Colour::RED = "\033[31m";
            Colour::GREEN = "\033[32m";
            Colour::YELLOW = "\033[33m";
            Colour::BLUE = "\033[34m";
            Colour::MAGENTA = "\033[35m";
            Colour::CYAN = "\033[36m";
            Colour::BG_DARK = "\033[47m";
            break;
        }
    }
    // overwrites the Colour:: globals in Terminal.h with the chosen palette's ANSI codes.

    inline Mode parse(const string& s) {
        if(s == "high_contrast") return Mode::HIGH_CONTRAST;
        if(s == "colorblind")    return Mode::COLORBLIND;
        if(s == "light")         return Mode::LIGHT;
        return Mode::DEFAULT;
    }
    // turns a key from the config file back into a Mode enum.

    inline string serialize(Mode m) {
        switch(m) {
            case Mode::HIGH_CONTRAST: return "high_contrast";
            case Mode::COLORBLIND:    return "colorblind";
            case Mode::LIGHT:         return "light";
            default:                  return "default";
        }
    }
    // turns a Mode into a string we can write into the config file.
}
// this file is a swappable colour-theme system for the visualizer. each Mode picks a different ANSI palette.

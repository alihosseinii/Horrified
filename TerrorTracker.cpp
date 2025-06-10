#include "TerrorTracker.hpp"

TerrorTracker::TerrorTracker() : level(0) {}

void TerrorTracker::reset() {
    level = 0;
}

void TerrorTracker::increase() {
    if (level < 5)
        level++;
}

void TerrorTracker::increase(int n) {
    level += n;
    if (level > 5)
       level = 5;
}

int TerrorTracker::getLevel() const {
    return level;
}

bool TerrorTracker::isMax() const {
    return level >= 5;
}

string TerrorTracker::getStatus() const {
    string bar = "Terror Level: ";
    bar += "[" + to_string(level) + "]";
    return bar;
}

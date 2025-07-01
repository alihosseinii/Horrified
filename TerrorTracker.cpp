#include "terrorteracker.hpp"

using namespace std;

TerrorTracker::TerrorTracker() : level(0) {}

void TerrorTracker::increase() {
    if (level < 5) level++;
}

int TerrorTracker::getLevel() const {
    return level;
}

string TerrorTracker::getStatus() const {
    string bar = "Terror Level: ";
    bar += "[" + to_string(level) + "/5]";
    return bar;
}

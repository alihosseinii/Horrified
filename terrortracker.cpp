#include "terrorteracker.hpp"

using namespace std;

TerrorTracker::TerrorTracker() : level(0) {}

void TerrorTracker::increase() {
    if (level < 5) level++;
}

int TerrorTracker::getLevel() const {
    return level;
}

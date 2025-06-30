#include "terrorteracker.hpp"

using namespace std;

TerrorTracker::TerrorTracker() : level(0) {}

void TerrorTracker::increase() {
    if (level < 5) level++;
}

int TerrorTracker::getLevel() const {
    return level;
}

bool TerrorTracker::lost() {
    if (level >= 5) {
        cout << "\n==========================================" << endl;
        cout << "           GAME OVER - MONSTERS WIN!" << endl;
        cout << "==========================================" << endl;
        cout << "The terror level has reached " << level << "!" << endl;
        cout << "The monsters have spread too much fear and chaos." << endl;
        cout << "The heroes have failed to protect the village." << endl;
        cout << "==========================================" << endl;
        return true;
    }
    return false;
}

string TerrorTracker::getStatus() const {
    string bar = "Terror Level: ";
    bar += "[" + to_string(level) + "/5]";
    return bar;
}

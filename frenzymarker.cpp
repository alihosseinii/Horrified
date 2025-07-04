#include "frenzymarker.hpp"
#include <algorithm>

using namespace std;

FrenzyMarker::FrenzyMarker(Monster* dracula, Monster* invisibleMan) {
    monsterOrder.push_back(dracula);
    monsterOrder.push_back(invisibleMan);
    currentFrenzied = dracula;
}

Monster* FrenzyMarker::getCurrentFrenzied() const {
    return currentFrenzied;
}

void FrenzyMarker::advance(Monster* dracula, Monster* invisibleMan) {
    if (dracula->getCurrentLocation() == nullptr) {
        monsterOrder.at(0) = nullptr;
        currentFrenzied = invisibleMan;
        return;
    }
    if (invisibleMan->getCurrentLocation() == nullptr) {
        monsterOrder.at(1) = nullptr;
        currentFrenzied = dracula;
        return;
    }

    if (monsterOrder.empty() || currentFrenzied == nullptr) return;

    auto it = find(monsterOrder.begin(), monsterOrder.end(), currentFrenzied);
    if (it != monsterOrder.end()) {
        ++it;
        if (it == monsterOrder.end()) {
            currentFrenzied = monsterOrder.at(0);
        } else {
            currentFrenzied = *it;
        }
    }
}

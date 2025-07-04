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
    monsterOrder.clear();
    if (dracula && dracula->getCurrentLocation()) {
        monsterOrder.push_back(dracula);
    }
    if (invisibleMan && invisibleMan->getCurrentLocation()) {
        monsterOrder.push_back(invisibleMan);
    }

    if (monsterOrder.empty()) {
        currentFrenzied = nullptr;
        return;
    }

    auto it = find(monsterOrder.begin(), monsterOrder.end(), currentFrenzied);
    if (it == monsterOrder.end() || currentFrenzied == nullptr) {
        currentFrenzied = monsterOrder.front();
        return;
    }

    ++it;
    if (it == monsterOrder.end()) {
        currentFrenzied = monsterOrder.front();
    } else {
        currentFrenzied = *it;
    }
}

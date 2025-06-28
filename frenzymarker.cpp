#include "frenzymarker.hpp"
#include <algorithm>

class Monster;
FrenzyMarker::FrenzyMarker(Monster* dracula, Monster* invisibleMan) {
    monsterOrder.push_back(dracula);
    monsterOrder.push_back(invisibleMan);
    currentFrenzied = dracula;
}

Monster* FrenzyMarker::getCurrentFrenzied() const {
    return currentFrenzied;
}
void FrenzyMarker::advance(Monster* dracula, Monster* invisibleMan) {
    if (dracula->getCurrentLocation() == nullptr){
        monsterOrder.at(1) = nullptr;
        currentFrenzied = dracula;
        return;
    }

    if (monsterOrder.empty() || currentFrenzied == nullptr) return;

    auto it = std::find(monsterOrder.begin(), monsterOrder.end(), currentFrenzied);
    if (it != monsterOrder.end()) {
        ++it;
        if (it == monsterOrder.end()) {
            currentFrenzied = monsterOrder[0]; 
        } else {
            currentFrenzied = *it;
        }
    }
}


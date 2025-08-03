#include "frenzymarker.hpp"
#include <algorithm>

using namespace std;

FrenzyMarker::FrenzyMarker(Monster* dracula, Monster* invisibleMan) {
    if (dracula && dracula->getCurrentLocation()) {
        monsterOrder.push_back(dracula);
    }
    if (invisibleMan && invisibleMan->getCurrentLocation()) {
        monsterOrder.push_back(invisibleMan);
    }
    
    if (!monsterOrder.empty()) {
        currentFrenzied = monsterOrder.front();
    } else {
        currentFrenzied = nullptr;
    }
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

int FrenzyMarker::getFrenzyLevel() const {
    if (currentFrenzied == nullptr) {
        return 0;
    }
    
    auto it = find(monsterOrder.begin(), monsterOrder.end(), currentFrenzied);
    if (it == monsterOrder.end()) {
        return 0;
    }
    
    return distance(monsterOrder.begin(), it) + 1;
}

void FrenzyMarker::setFrenzyLevel(int level) {
    if (level < 0 || level > static_cast<int>(monsterOrder.size())) {
        return;
    }
    
    if (level == 0) {
        currentFrenzied = nullptr;
    } else {
        currentFrenzied = monsterOrder[level - 1];
    }
}

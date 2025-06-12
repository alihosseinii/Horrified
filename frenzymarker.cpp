#include "frenzymarker.hpp"
#include <algorithm>

FrenzyMarker::FrenzyMarker(const std::vector<Monster*>& allMonsters)
    : monsterOrder(allMonsters) {
    currentFrenzied = !monsterOrder.empty() ? monsterOrder[0] : nullptr;
}

Monster* FrenzyMarker::getCurrentFrenzied() const {
    return currentFrenzied;
}

void FrenzyMarker::advance() {
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

void FrenzyMarker::reset() {
    currentFrenzied = !monsterOrder.empty() ? monsterOrder[0] : nullptr;
}

#include "Hero.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

Hero::Hero(const std::string& playerName, const std::string& heroName, int maxActions, std::shared_ptr<Town> startingTown) {
    setPlayerName(playerName);
    setHeroName(heroName);
    setMaxActions(maxActions);
    setRemainingActions(maxActions);
    setCurrentTown(startingTown);
    currentTown->addCharacter(heroName);
}

void Hero::setHeroName(string heroName) {
    if (heroName == "Archeologist" || heroName == "Mayor") {
        this->heroName = heroName;
    }
    else {
        throw invalid_argument("Invalid hero name.");
    }
}

void Hero::setPlayerName(string playerName) {
    this->playerName = playerName;
}

void Hero::setMaxActions(int maxActions) {
    if (maxActions < 0) {
        throw out_of_range("Invalid max actions: can't be negative.");
    }
    this->maxActions = maxActions;
}

int Hero::getMaxActions() const {
    return maxActions;
}

int Hero::getRemainingActions() const {
    return remainingActions;
}

void Hero::setRemainingActions(int remainingActions) {
    this->remainingActions = remainingActions;
}

void Hero::resetActions() {
    remainingActions = maxActions;
}

const string& Hero::getHeroName() const {
    return heroName;
}

const string& Hero::getPlayerName() const {
    return playerName;
}

shared_ptr<Town> Hero::getCurrentTown() const {
    return currentTown;
}

void Hero::setCurrentTown(shared_ptr<Town> currentTown) {
    this->currentTown = currentTown;
}

void Hero::move(shared_ptr<Town> newTown) {
    if (!newTown) {
        throw invalid_argument("There is no location called " + newTown->getName());
    }
    
    const auto& neighbors = currentTown->getNeighbors();

    bool isNeighbor = false;
    for (const auto& neighbor : neighbors) {
        if (neighbor && neighbor->getName() == newTown->getName()) {
            isNeighbor = true;
            break;
        }
    }
    if (!isNeighbor) {
        throw invalid_argument(playerName + " (" + heroName + ") can't move to " + newTown->getName() + " - not a neighbor.");
    }

    currentTown->removeCharacter(heroName);
    newTown->addCharacter(heroName);
    setCurrentTown(newTown);

    cout << heroName << " (" << playerName << ") moved to " << currentTown->getName() << ".\n";
    setRemainingActions(getRemainingActions() - 1);
}
  

// void Hero::pickUp() {
//     if (remainingActions <= 0) {
//         std::cout << "No remaining actions.\n";
//         return;
//     }

//     auto& items = currentLocation->getItems();
//     for (const auto& item : items) {
//         inventory.push_back(item);
//         std::cout << name << " picked up item from " << item.getLocationName() << ".\n";
//     }
//     items.clear();
//     remainingActions--;
// }

// void Hero::advance() {
//     cout << name << " attempts to advance mission.\n";
//     remainingActions--;
// }

// void Hero::defeat() {
//     std::cout << name << " attempts to defeat monster.\n";
//     remainingActions--;
// }


// void Hero::addItem(const Item& item) {
//     inventory.push_back(item);
// }

// const std::vector<Item>& Hero::getItems() const {
//     return inventory;
// }

// void Hero::addPerkCard(const PerkCard& card) {
//     perkCards.push_back(card);
// }

// const std::vector<PerkCard>& Hero::getPerkCards() const {
//     return perkCards;
// }



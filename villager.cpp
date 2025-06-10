#include "villager.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

Villager::Villager(const std::string& villagerName, std::shared_ptr<Town> startingTown) {
    setVillagerName(villagerName);
    setCurrentTown(startingTown);
}

string Villager::getVillagerName() const {
    return  villagerName;
}

void Villager::setVillagerName(string villagerName) {
    this->villagerName = villagerName;
}

shared_ptr<Town> Villager::getCurrentTown() const {
    return currentTown;
}

void Villager::setCurrentTown(shared_ptr<Town> currentTown) {
    this->currentTown = currentTown;
}

void Villager::move(shared_ptr<Town> newTown) {
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
        throw invalid_argument(villagerName + " can't move to " + newTown->getName() + " - not a neighbor.");
    }

    currentTown->removeCharacter(villagerName);
    newTown->addCharacter(villagerName);
    setCurrentTown(newTown);

    cout << villagerName << " moved to " << newTown->getName() << endl;
}
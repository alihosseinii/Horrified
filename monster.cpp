#include "monster.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

using namespace std;

Monster::Monster(const string& monsterName, shared_ptr<Location> startingLocation) {
    this->monsterName = monsterName;
    setCurrentLocation(startingLocation);
    try {
        currentLocation->addCharacter(monsterName);
    } catch (const exception& e) {
        throw runtime_error("Failed to add monster to location: " + string(e.what()));
    }
}

string Monster::getMonsterName() const {
    return monsterName;
}

void Monster::setMonsterName(string monsterName) {
    this->monsterName = monsterName;
}

shared_ptr<Location> Monster::getCurrentLocation() const {
    if (!currentLocation) {
        throw runtime_error("Current location is not set");
    }
    return currentLocation;
}

void Monster::setCurrentLocation(shared_ptr<Location> currentLocation) {
    this->currentLocation = currentLocation;
}

void Monster::moveToNearestCharacter(const string& targetCharacter, int stepNumber) {
    auto currentLocationCharacterExistence = currentLocation->getCharacters();
    if (!currentLocationCharacterExistence.empty()) {
        for (const auto& c : currentLocationCharacterExistence) {
            if (c == "Invisible man" || c == "Dracula") continue;
            return;
        }
    }

    queue<shared_ptr<Location>> q;
    unordered_set<string> visited;
    unordered_map<string, shared_ptr<Location>> parent;

    q.push(currentLocation);
    visited.insert(currentLocation->getName());

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        for (const auto& neighbor : current->getNeighbors()) {
            if (!neighbor) continue;

            if (visited.find(neighbor->getName()) == visited.end()) {
                visited.insert(neighbor->getName());
                parent[neighbor->getName()] = current;
                q.push(neighbor);

                auto characters = neighbor->getCharacters();
                for (const auto& character : characters) {
                    if (character != monsterName) {
                        vector<shared_ptr<Location>> path;
                        auto step = neighbor;
                        while (step && step != currentLocation) {
                            path.push_back(step);
                            step = parent[step->getName()];
                        }
                        path.push_back(currentLocation);

                        shared_ptr<Location> newLocation;
                        if (path.size() == 1) {
                            newLocation = path.back();
                        } else {
                            newLocation = path[stepNumber];
                        }

                        currentLocation->removeCharacter(monsterName);
                        newLocation->addCharacter(monsterName);
                        setCurrentLocation(newLocation);
                        cout << monsterName << " moved to " << newLocation->getName() << ".\n";
                        return;
                    }
                }
            }
        }
    }
}





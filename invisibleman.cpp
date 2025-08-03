#include "invisibleman.hpp"
#include "hero.hpp"
#include "terrorteracker.hpp"
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <unordered_map>

using namespace std;

InvisibleMan::InvisibleMan(shared_ptr<Location> startingLocation) : Monster("Invisible man", startingLocation) {
    powerName = "Stalk Unseen";
}

void InvisibleMan::power(Hero* hero, TerrorTracker& terrorTracker) {
    auto currentLocationCharacterExistence = currentLocation->getCharacters();
    if (!currentLocationCharacterExistence.empty()) {
        for (const auto& c : currentLocationCharacterExistence) {
            if (c == "Archeologist" || c == "Mayor" || c == "Scientist" || c == "Courier" || c == "Dracula" || c == "Invisible man") continue;
            currentLocation->removeCharacter(c);
            cout << c << " was killed by Invisible man.\n";
            terrorTracker.increase();
            cout << "Terror level increased to " << terrorTracker.getLevel() << " due to villager death.\n";
            return;
        }
    }
    return;
}

void InvisibleMan::moveTowardsVillager(int steps) {
    auto currentLocationCharacterExistence = currentLocation->getCharacters();
    for (const auto& c : currentLocationCharacterExistence) {
        if (c != "Invisible man" && c != "Dracula" && c != "Archeologist" && c != "Mayor" && c != "Scientist" && c != "Courier") {
            return;
        }
    }

    queue<pair<shared_ptr<Location>, int>> q;
    unordered_set<string> visited;
    unordered_map<string, shared_ptr<Location>> parent;

    q.push({currentLocation, 0});
    visited.insert(currentLocation->getName());

    shared_ptr<Location> villagerLocation = nullptr;

    while (!q.empty()) {
        auto [current, distance] = q.front();
        q.pop();

        auto characters = current->getCharacters();
        for (const auto& character : characters) {
            if (character != "Invisible man" && character != "Dracula" && character != "Archeologist" && character != "Mayor" && character != "Scientist" && character != "Courier") {
                villagerLocation = current;
                break;
            }
        }
        if (villagerLocation) break;

        for (const auto& neighbor : current->getNeighbors()) {
            if (!neighbor || visited.count(neighbor->getName())) continue;

            visited.insert(neighbor->getName());
            parent[neighbor->getName()] = current;
            q.push({neighbor, distance + 1});
        }
    }

    if (!villagerLocation) return;

    vector<shared_ptr<Location>> path;
    auto stepLoc = villagerLocation;
    while (stepLoc && stepLoc != currentLocation) {
        path.push_back(stepLoc);
        stepLoc = parent[stepLoc->getName()];
    }
    reverse(path.begin(), path.end());

    int moveCount = min(steps, static_cast<int>(path.size()));
    shared_ptr<Location> lastLocation = currentLocation;

    for (int i = 0; i < moveCount; ++i) {
        currentLocation->removeCharacter(monsterName);
        path[i]->addCharacter(monsterName);
        setCurrentLocation(path[i]);
        lastLocation = path[i];

        auto newChars = path[i]->getCharacters();
        for (const auto& c : newChars) {
            if (c != "Invisible man" && c != "Dracula" && c != "Archeologist" && c != "Mayor" && c != "Scientist" && c != "Courier") {
                cout << monsterName << " moved to " << lastLocation->getName() << ".\n";
                return;
            }
        }
    }

    if (lastLocation != currentLocation) {
        cout << monsterName << " moved to " << lastLocation->getName() << ".\n";
    }
}
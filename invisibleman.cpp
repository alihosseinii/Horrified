#include "invisibleman.hpp"
#include "hero.hpp"
#include "terrorteracker.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

InvisibleMan::InvisibleMan(shared_ptr<Location> startingLocation) : Monster("Invisible man", startingLocation) {
    powerName = "Stalk Unseen";
}

void InvisibleMan::power(Hero* hero, TerrorTracker& terrorTracker) {
    auto currentLocationCharacterExistence = currentLocation->getCharacters();
    if (!currentLocationCharacterExistence.empty()) {
        for (const auto& c : currentLocationCharacterExistence) {
            if (c == "Invisible man" || c == "Dracula" || c == "Archeologist" || c == "Mayor") continue;
            currentLocation->removeCharacter(c);
            cout << c << " was killed by Invisible man.\n";
            terrorTracker.increase();
            cout << "Terror level increased to " << terrorTracker.getLevel() << " due to villager death.\n";
            return;
        }
    }

    queue<pair<shared_ptr<Location>, int>> q;
    unordered_set<string> visited;
    unordered_map<string, shared_ptr<Location>> parent;

    q.push({currentLocation, 0});
    visited.insert(currentLocation->getName());

    while (!q.empty()) {
        auto [current, distance] = q.front();
        q.pop();

        if (distance > 2) continue;

        auto characters = current->getCharacters();
        for (const auto& character : characters) {
            if (character != "Invisible man" && character != "Dracula" && character != "Archeologist" && character != "Mayor") {
                if (distance == 2) {
                    vector<shared_ptr<Location>> path;
                    auto step = current;
                    while (step && step != currentLocation) {
                        path.push_back(step);
                        step = parent[step->getName()];
                    }
                    reverse(path.begin(), path.end());

                    if (!path.empty()) {
                        auto newLocation = path.front();
                        currentLocation->removeCharacter(monsterName);
                        newLocation->addCharacter(monsterName);
                        setCurrentLocation(newLocation);
                        cout << monsterName << " moved to " << newLocation->getName() << ".\n";
                    }
                    return;
                }
            }
        }

        for (const auto& neighbor : current->getNeighbors()) {
            if (!neighbor) continue;
            if (visited.find(neighbor->getName()) != visited.end()) continue;

            visited.insert(neighbor->getName());
            parent[neighbor->getName()] = current;
            q.push({neighbor, distance + 1});
        }
    }
}
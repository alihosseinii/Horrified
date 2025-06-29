#include "monster.hpp"
#include "terrorteracker.hpp"
#include "map.hpp"
#include "archeologist.hpp"
#include "mayor.hpp"
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

bool Monster::attack(Hero* archeologist, Hero* mayor, TerrorTracker& terrorTracker, Map& map) {
    auto currentLocationCharacters = currentLocation->getCharacters();
    Hero* targetHero = nullptr;
    string targetVillager = "";
    
    for (const auto& character : currentLocationCharacters) {
        if (character == "Archeologist") {
            targetHero = archeologist;
            break;
        } else if (character == "Mayor") {
            targetHero = mayor;
            break;
        }
    }
    
    if (!targetHero) {
        for (const auto& character : currentLocationCharacters) {
            if (character != "Invisible man" && character != "Dracula" && 
                character != "Archeologist" && character != "Mayor") {
                targetVillager = character;
                break;
            }
        }
    }
    
    if (!targetHero && targetVillager.empty()) {
        return false;
    }
    
    cout << monsterName << " attacks ";
    
    if (targetHero) {
        cout << targetHero->getPlayerName() << " (" << targetHero->getHeroName() << ")!\n";
        
        string answer;
        while (true) {
            cout << "Do you want to use an item to defend yourself? (Yes/No): ";
            getline(cin, answer);
            if (answer == "Yes" || answer == "yes" || answer == "No" || answer == "no") break;
            cout << "Invalid input. Please enter Yes or No.\n";
        }
        
        if (answer == "Yes" || answer == "yes") {
            auto items = targetHero->getItems();
            if (!items.empty()) {
                cout << "Available items:\n";
                for (size_t i = 0; i < items.size(); ++i) {
                    cout << i + 1 << ". " << items[i].getItemName() << " (" 
                         << Item::colorToString(items[i].getColor()) << ", Power: " 
                         << items[i].getPower() << ")\n";
                }
                
                cout << "Enter item number to use (0 to cancel): ";
                int choice;
                cin >> choice;
                cin.ignore();
                
                if (choice > 0 && choice <= static_cast<int>(items.size())) {
                    cout << targetHero->getPlayerName() << " used " << items[choice-1].getItemName() << " to defend!\n";
                    
                    targetHero->removeItem(choice - 1);
                    
                    return true;
                }
            } else {
                cout << "You have no items to use!\n";
            }
        }
        
        try {
            auto hospital = map.getLocation("Hospital");
            currentLocation->removeCharacter(targetHero->getHeroName());
            hospital->addCharacter(targetHero->getHeroName());
            targetHero->setCurrentLocation(hospital);
            cout << targetHero->getPlayerName() << " (" << targetHero->getHeroName() << ") was wounded and moved to Hospital.\n";

            terrorTracker.increase();
            cout << "Terror level increased to " << terrorTracker.getLevel() << " due to the attack!\n";
            return true;
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
        
    } else if (!targetVillager.empty()) {
        cout << targetVillager << "!\n";
        currentLocation->removeCharacter(targetVillager);
        cout << targetVillager << " was killed by " << monsterName << "!\n";

        terrorTracker.increase();
        cout << "Terror level increased to " << terrorTracker.getLevel() << " due to the attack!\n";
        
        return true;
    }
}
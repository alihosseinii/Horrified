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
    for (const auto& c : currentLocation->getCharacters()) {
        if (c != "Invisible man" && c != "Dracula") {
            return;
        }
    }

    using Path = vector<shared_ptr<Location>>;
    queue<Path> q;
    unordered_set<string> visited;
    q.push({currentLocation});
    visited.insert(currentLocation->getName());

    Path shortestPathToTarget;
    bool found = false;

    while (!q.empty() && !found) {
        Path path = q.front();
        q.pop();
        auto loc = path.back();
        for (const auto& neighbor : loc->getNeighbors()) {
            if (!neighbor) continue;
            if (visited.count(neighbor->getName())) continue;
            visited.insert(neighbor->getName());
            Path newPath = path;
            newPath.push_back(neighbor);
            for (const auto& c : neighbor->getCharacters()) {
                if (c != "Invisible man" && c != "Dracula") {
                    shortestPathToTarget = newPath;
                    found = true;
                    break;
                }
            }
            if (found) break;
            q.push(newPath);
        }
    }

    if (!found || shortestPathToTarget.size() < 2) {
        cout << monsterName << " didn't move: no valid targets nearby.\n";
        return;
    }

    int stepsToMove = min(stepNumber, static_cast<int>(shortestPathToTarget.size()) - 1);
    shared_ptr<Location> newLocation = shortestPathToTarget[stepsToMove];

    for (int i = 1; i <= stepsToMove; ++i) {
        auto loc = shortestPathToTarget[i];
        bool hasTarget = false;
        for (const auto& c : loc->getCharacters()) {
            if (c != "Invisible man" && c != "Dracula") {
                newLocation = loc;
                stepsToMove = i;
                hasTarget = true;
                break;
            }
        }
        if (hasTarget) {
            break;
        }
    }

    currentLocation->removeCharacter(monsterName);
    newLocation->addCharacter(monsterName);
    setCurrentLocation(newLocation);
    cout << monsterName << " moved to " << newLocation->getName() << ".\n";
}

bool Monster::attack(Hero* archeologist, Hero* mayor, Courier* courier, Scientist* scientist, TerrorTracker& terrorTracker, Map& map) {
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
        } else if (character == "Courier") {
            targetHero = courier;
            break;
        } else if (character == "Scientist") {
            targetHero = scientist;
            break;
        }
    }
    
    if (!targetHero) {
        for (const auto& character : currentLocationCharacters) {
            if (character != "Invisible man" && character != "Dracula" && 
                character != "Archeologist" && character != "Mayor" &&
                character != "Courier" && character != "Scientist") {
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
        auto items = targetHero->getItems();
        if (!items.empty()) {
            string answer;
            while (true) {
                cout << "Do you want to use an item to defend yourself? (Yes/No): ";
                getline(cin, answer);
                answer = toSentenceCase(answer);
                if (answer == "Yes" || answer == "No") break;
                cout << "Invalid input. Please enter Yes or No.\n";
            }
            if (answer == "Yes") {
                cout << "Available items:\n";
                for (size_t i = 0; i < items.size(); ++i) {
                    cout << i + 1 << ". " << items[i].getItemName() << " (" 
                         << Item::colorToString(items[i].getColor()) << ", Power: " 
                         << items[i].getPower() << ")\n";
                }
                int choice;
                while (true) {
                    cout << "Enter item number to use (0 to cancel): ";
                    cin >> choice;
                    if (cin.fail()) {
                        cout << "Invalid input. Please enter a number.\n";
                        cin.clear(); 
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                        continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                if (choice > 0 && choice <= static_cast<int>(items.size())) {
                    if (targetHero->getHeroName() == "Scientist") {
                        targetHero->ability(choice - 1);
                    }
                    cout << targetHero->getPlayerName() << " (" << targetHero->getHeroName() << ") used " << items[choice-1].getItemName() << " to defend!\n";
                    targetHero->removeItem(choice - 1);
                    return false;
                }
            }
        } else {
            cout << "You have no items to use!\n";
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
    return false;
}

void Monster::moveTwoSteps() {
    auto loc = getCurrentLocation();
    for (int step = 0; step < 2; ++step) {
        const auto& neighbors = loc->getNeighbors();
        if (neighbors.empty()) break;
        auto nextLoc = neighbors[0];
        loc->removeCharacter(getMonsterName());
        nextLoc->addCharacter(getMonsterName());
        setCurrentLocation(nextLoc);
        loc = nextLoc;
    }
}
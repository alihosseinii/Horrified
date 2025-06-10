#include "monster.hpp"
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <ctime>
#include <algorithm>

using namespace std;

Monster::Monster(const std::string& monsterName, std::shared_ptr<Town> startingTown) {
    setMonsterName(monsterName);
    setCurrentTown(startingTown);
    currentTown->addCharacter(monsterName);
}

string Monster::getMonsterName() const {
    return monsterName;
}

void Monster::setMonsterName(string monsterName) {
    if (monsterName == "Dracula" || monsterName == "Invisible man") {
        this->monsterName = monsterName;
    }
    else {
        throw invalid_argument("Invalid monster name.");
    }
}

shared_ptr<Town> Monster::getCurrentTown() const {
    return currentTown;
}

void Monster::setCurrentTown(shared_ptr<Town> currentTown) {
    this->currentTown = currentTown;
}

void Monster::moveToNearestCharacter(const string& targetCharacter, int stepNumber) {
    auto currentTownCharacterExistence = currentTown->getCharacters();
    if (!currentTownCharacterExistence.empty()) {
        for (const auto& c : currentTownCharacterExistence) {
            if (c == "Invisible man" || c == "Dracula") continue;
            return;
        }

    }

    queue<shared_ptr<Town>> q;
    unordered_map<string, shared_ptr<Town>> parent;
    unordered_set<string> visited;
    vector<shared_ptr<Town>> targets;

    q.push(currentTown);
    visited.insert(currentTown->getName());

    bool found = false;

    while (!q.empty() && !found) {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; ++i) {
            auto town = q.front();
            q.pop();

            for (const auto& character : town->getCharacters()) {
                if (character == "Invisible man") continue;
            
                if ((targetCharacter == "*" || character == targetCharacter) &&
                    town != currentTown) {
                    targets.push_back(town);
                    found = true;
                    break;
                }
            }

            for (const auto& neighbor : town->getNeighbors()) {
                if (!neighbor) continue;
                if (visited.count(neighbor->getName())) continue;

                visited.insert(neighbor->getName());
                parent[neighbor->getName()] = town;
                q.push(neighbor);
            }
        }

        if (found) break;
    }

    if (targets.empty()) {
        throw invalid_argument("There are no locations with characters.");
    }

    srand(static_cast<unsigned int>(time(nullptr)));
    auto chosenTarget = targets[rand() % targets.size()];

    vector<shared_ptr<Town>> path;
    auto step = chosenTarget;
    while (step && step != currentTown) {
        path.push_back(step);
        step = parent[step->getName()];
    }
    path.push_back(currentTown);
    reverse(path.begin(), path.end());

    shared_ptr<Town> newTown;
    if (stepNumber >= path.size()) {
        newTown = path.back();
    } else {
        newTown = path[stepNumber];
    }

    currentTown->removeCharacter(getMonsterName());
    newTown->addCharacter(getMonsterName());
    setCurrentTown(newTown);
}





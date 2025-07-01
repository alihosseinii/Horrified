#include "taskboard.hpp"
#include <iostream>

using namespace std;

TaskBoard::TaskBoard() {
    draculaCoffins["Cave"] = {};
    draculaCoffins["Dungeon"] = {};
    draculaCoffins["Crypt"] = {};
    draculaCoffins["Graveyard"] = {};

    invisibleManCluesDelivered["Inn"] = false;
    invisibleManCluesDelivered["Mansion"] = false;
    invisibleManCluesDelivered["Barn"] = false;
    invisibleManCluesDelivered["Laboratory"] = false;
    invisibleManCluesDelivered["Institute"] = false;
}

void TaskBoard::addStrengthToCoffin(const std::string& location, int strength) {
    auto it = draculaCoffins.find(location);
    if (it != draculaCoffins.end() && !it->second.completed) {
        it->second.currentStrength += strength;
        if (it->second.currentStrength >= 6) {
            it->second.completed = true;
        }
    }
}

bool TaskBoard::isCoffinLocation(const std::string& location) const {
    return draculaCoffins.count(location) > 0;
}

bool TaskBoard::isCoffinDestroyed(const string& location) const {
    auto it = draculaCoffins.find(location);
    if (it == draculaCoffins.end()) return true;
    return it->second.completed;
}

bool TaskBoard::allCoffinsDestroyed() const {
    for (const auto& [_, status] : draculaCoffins) {
        if (!status.completed) return false;
    }
    return true;
}

void TaskBoard::addStrengthToDracula(int strength) {
    if (!draculaDefeat.completed) {
        draculaDefeat.currentStrength += strength;
        if (draculaDefeat.currentStrength >= 6) {
            draculaDefeat.completed = true;
        }
    }
}

int TaskBoard::getDraculaDefeatStrength() const {
    return draculaDefeat.currentStrength;
}

bool TaskBoard::isDraculaDefeated() const {
    return draculaDefeat.completed;
}

std::string TaskBoard::getDraculaTaskStatus() const {
    std::string status = "Dracula Coffins:\n";
    for (const auto& [loc, task_status] : draculaCoffins) {
        status += "- " + loc + ": " + std::to_string(task_status.currentStrength) + "/6 Strength " 
                  + (task_status.completed ? "(Destroyed)" : "(Active)") + "\n";
    }
    status += "Defeat Dracula: " + std::to_string(draculaDefeat.currentStrength) + "/6 Strength "
              + (draculaDefeat.completed ? "(Defeated)" : "(Vulnerable)") + "\n";
    return status;
}

bool TaskBoard::isClueDelivered(const std::string& location) const {
    auto it = invisibleManCluesDelivered.find(location);
    return it != invisibleManCluesDelivered.end() && it->second;
}

void TaskBoard::deliverClue(const std::string& location) {
    if (invisibleManCluesDelivered.count(location)) {
        invisibleManCluesDelivered[location] = true;
    }
}

bool TaskBoard::allCluesDelivered() const {
    for (const auto& [_, delivered] : invisibleManCluesDelivered) {
        if (!delivered) return false;
    }
    return true;
}

void TaskBoard::addStrengthToInvisibleMan(int strength) {
    if (!invisibleManDefeated) {
        invisibleManDefeat.currentStrength += strength;
        if (invisibleManDefeat.currentStrength >= 9) {
            invisibleManDefeat.completed = true;
            invisibleManDefeated = true;
        }
    }
}

int TaskBoard::getInvisibleManDefeatStrength() const {
    return invisibleManDefeat.currentStrength;
}

void TaskBoard::defeatInvisibleMan() {
    invisibleManDefeated = true;
    invisibleManDefeat.completed = true;
}

bool TaskBoard::isInvisibleManDefeated() const {
    return invisibleManDefeated;
}

std::string TaskBoard::getInvisibleManClueStatus() const {
    std::string status = "Invisible Man Clues Delivered:\n";
    for (const auto& [loc, delivered] : invisibleManCluesDelivered) {
        status += "- " + loc + ": " + (delivered ? "Delivered" : "Missing") + "\n";
    }
    status += "Defeat Invisible Man: " + std::to_string(invisibleManDefeat.currentStrength) + "/9 Red Strength "
              + (invisibleManDefeated ? "(Defeated)" : "(Vulnerable)") + "\n";
    return status;
}

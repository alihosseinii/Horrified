#include "TaskBoard.hpp"
#include <iostream>

TaskBoard::TaskBoard() {
    draculaCoffins["Cave"] = false;
    draculaCoffins["Dungeon"] = false;
    draculaCoffins["Crypt"] = false;
    draculaCoffins["Graveyard"] = false;

    invisibleManEvidence["Inn"] = false;
    invisibleManEvidence["Mansion"] = false;
    invisibleManEvidence["Barn"] = false;
    invisibleManEvidence["Laboratory"] = false;
    invisibleManEvidence["Institute"] = false;
}

void TaskBoard::destroyCoffinAt(const string& location) {
    draculaCoffins[location] = true;
}

bool TaskBoard::isCoffinDestroyed(const string& location) const {
    auto it = draculaCoffins.find(location);
    return it != draculaCoffins.end() && it->second;
}

bool TaskBoard::allCoffinsDestroyed() const {
    for (const auto& [_, destroyed] : draculaCoffins) {
        if (!destroyed) return false;
    }
    return true;
}

void TaskBoard::placeEvidenceFrom(const string& location) {
    invisibleManEvidence[location] = true;
}

bool TaskBoard::isEvidencePlaced(const string& location) const {
    auto it = invisibleManEvidence.find(location);
    return it != invisibleManEvidence.end() && it->second;
}

bool TaskBoard::allEvidencePlaced() const {
    for (const auto& [_, placed] : invisibleManEvidence) {
        if (!placed) return false;
    }
    return true;
}

std::string TaskBoard::getDraculaTaskStatus() const {
    std::string status = "Dracula Coffins:\n";
    for (const auto& [loc, destroyed] : draculaCoffins) {
        status += "- " + loc + ": " + (destroyed ? "Destroyed" : "Remaining") + "\n";
    }
    return status;
}

string TaskBoard::getInvisibleManTaskStatus() const {
    string status = "Invisible Man Evidences:\n";
    for (const auto& [loc, placed] : invisibleManEvidence) {
        status += "- " + loc + ": " + (placed ? "Placed" : "Missing") + "\n";
    }
    return status;
}

#include "location.hpp"
#include "item.hpp"
#include <stdexcept>
#include <algorithm>

using namespace std;

Location::Location(const string& locationName) : name(locationName) {}

string Location::getName() const {
    return name;
}

const vector<shared_ptr<Location>>& Location::getNeighbors() const {
    return neighbors;
}

const vector<string>& Location::getCharacters() const {
    return characters;
}

const std::vector<Item>& Location::getItems() const {
    return items;
}

void Location::addNeighbor(shared_ptr<Location> neighbor) {
    if (neighbor.get() == this) {
        throw invalid_argument("Location cannot be its own neighbor");
    }
    if (find(neighbors.begin(), neighbors.end(), neighbor) != neighbors.end()) {
        throw invalid_argument("Location is already a neighbor");
    }
    if (neighbor) {
        neighbors.push_back(neighbor);
    }
    else {
        throw invalid_argument("Invalid neighbor.");
    }
}

void Location::addCharacter(const string& character) {
    if (find(characters.begin(), characters.end(), character) != characters.end()) {
        throw invalid_argument("Character is already present in this location.");
    }
    characters.push_back(character);
}

void Location::removeCharacter(const string& character) {
    auto it = find(characters.begin(), characters.end(), character);
    if (it == characters.end()) {
        throw invalid_argument("Character not found in this location.");
    }
    characters.erase(it);
}

void Location::addItem(const Item& item) {
    items.push_back(item);
}

void Location::removeItem(const Item& item) {
    auto it = std::find_if(items.begin(), items.end(), [&](const Item& i) {
        return i.getItemName() == item.getItemName() &&
               i.getColor() == item.getColor() &&
               i.getPower() == item.getPower() 
               && i.getLocation()->getName() == item.getLocation()->getName();
    });

    if (it == items.end()) {
        throw std::invalid_argument("Item not found in this location to remove.");
    }

    items.erase(it);
}

void Location::clearItems() {
    items.clear();
}

void Location::clearCharacters() {
    characters.clear();
}
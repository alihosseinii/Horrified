#include "town.hpp"
#include <stdexcept>
#include <algorithm>

using namespace std;

Town::Town(const string& townName) : name(townName) {}

const string& Town::getName() const{
    return name;
}

const vector<shared_ptr<Town>>& Town::getNeighbors() const{
    return neighbors;
}

const vector<string>& Town::getCharacters() const{
    return characters;
}

void Town::addNeighbor(shared_ptr<Town> neighbor) {
    if (find(neighbors.begin(), neighbors.end(), neighbor) != neighbors.end()) {
            throw invalid_argument("Invalid neighbor: it has already been added");
    }
    if (neighbor) {
        neighbors.push_back(neighbor);
    }
    else{
        throw invalid_argument("Invalid neighbor.");
    }
}

void Town::addCharacter(const string& character) {
    if (find(characters.begin(), characters.end(), character) != characters.end()) {
            throw invalid_argument("Character is already present in this location.");
    }
    characters.push_back(character);
}

void Town::removeCharacter(const string& character){
    auto it = find(characters.begin(), characters.end(), character);
    if (it == characters.end()) {
            throw invalid_argument("Character not found to remove.");
    }
    characters.erase(it);
}
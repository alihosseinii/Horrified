#include "mayor.hpp"
#include "map.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

Mayor::Mayor(const string& playerName, shared_ptr<Town> startingTown) : Hero(playerName, "Mayor", 5, startingTown) {}

void Mayor::specialAction() {
    throw invalid_argument("Mayor has no special action.");
}

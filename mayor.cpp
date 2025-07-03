#include "mayor.hpp"
#include "map.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

Mayor::Mayor(const string& playerName, shared_ptr<Location> startingLocation) : Hero(playerName, "Mayor", 5, startingLocation) {}

void Mayor::specialAction() {
    cout << "Mayor has no special action.";
}

#include "archeologist.hpp"

using namespace std;

Archeologist::Archeologist(const string& playerName, shared_ptr<Town> startingTown) : Hero(playerName, "Archeologist", 4, startingTown) {}

void Archeologist::specialAction() {
    const auto& neighbors = currentTown->getNeighbors();

    bool pickedAnything = false;

    // for (const auto& neighbor : neighbors) {
    //     auto& items = neighbor->getItems();

    //     if (!items.empty()) {
    //         for (const auto& item : items) {
    //             inventory.push_back(item);
    //             cout << playerName << " (Archeologist) picked up item from " << neighbor->getName() << ".\n";
    //         }
    //         items.clear();
    //         pickedAnything = true;
    //     }
    // }

    if (!pickedAnything) {
        cout << playerName << " (Archeologist) found no items in neighboring towns.\n";
    }

    remainingActions--;
}

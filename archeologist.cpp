#include "archeologist.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

Archeologist::Archeologist(const string& playerName, shared_ptr<Location> startingLocation) : Hero(playerName, "Archeologist", 4, startingLocation) {}

void Archeologist::specialAction() {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    const auto& neighbors = currentLocation->getNeighbors();
    if (neighbors.empty()) {
        throw invalid_argument("There are no neighboring locations.");
    }

    vector<shared_ptr<Location>> neighborsWithItems;
    for (const auto& neighbor : neighbors) {
        if (!neighbor->getItems().empty()) {
            neighborsWithItems.push_back(neighbor);
        }
    }

    if (neighborsWithItems.empty()) {
        throw invalid_argument(playerName + " (Archeologist) found no items in neighboring locations.");
    }

    cout << "Neighboring locations with items:\n";
    for (size_t i = 0; i < neighborsWithItems.size(); ++i) {
        cout << i + 1 << ". " << neighborsWithItems[i]->getName() << "\n";
    }

    cout << "Choose a location to pick items from (1-" << neighborsWithItems.size() << "): ";
    int locationChoice;
    cin >> locationChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (locationChoice < 1 || locationChoice > static_cast<int>(neighborsWithItems.size())) {
        cout << "Invalid location choice.\n";
        return;
    }

    auto chosenLocation = neighborsWithItems[locationChoice - 1];
    const auto& itemsAtLocation = chosenLocation->getItems();

    if (itemsAtLocation.empty()) {
        cout << "No items found at " << chosenLocation->getName() << ".\n";
        return;
    }

    bool itemWasPickedUp = false;
    int exitChoice = static_cast<int>(itemsAtLocation.size()) + 1;

    while (true) {
        cout << "\nItems in " << chosenLocation->getName() << ":\n";
        for (size_t i = 0; i < itemsAtLocation.size(); ++i) {
            const auto& it = itemsAtLocation[i];
            cout << i + 1 << ". " << it.getItemName() << " (" 
                 << Item::colorToString(it.getColor()) << ", Power: " 
                 << it.getPower() << ")\n";
        }
        cout << exitChoice << ". Exit\n";

        cout << "Enter the number of the item to pick up (" << exitChoice << " to finish): ";
        int itemChoice;
        cin >> itemChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (itemChoice == exitChoice) break;

        if (itemChoice < 1 || itemChoice > static_cast<int>(itemsAtLocation.size())) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        const Item& selectedItem = itemsAtLocation[itemChoice - 1];
        items.push_back(selectedItem);
        chosenLocation->removeItem(selectedItem);
        cout << playerName << " (" << heroName << ") picked up " 
             << selectedItem.getItemName() << " from " << chosenLocation->getName() << ".\n";

        itemWasPickedUp = true;

        if (chosenLocation->getItems().empty()) {
            cout << "No more items in " << chosenLocation->getName() << ".\n";
            break;
        }
    }

    if (itemWasPickedUp) {
        remainingActions--;
    }
}
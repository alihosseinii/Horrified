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
    bool foundItems = false;
    bool itemWasPickedUp = false;
    for (const auto& neighbor : neighbors) {
        const auto item = neighbor->getItems();
        if (!item.empty()) {
            foundItems = true;
            cout << "Items in " << neighbor->getName() << ":\n";
            for (size_t i = 0; i < static_cast<int>(item.size()); ++i) {
                const auto& it = item[i];
                cout << i + 1 << ". " << it.getItemName() << " (" 
                     << Item::colorToString(it.getColor()) << ", Power: " 
                     << it.getPower() << ")\n";
            }

            int choice;
            int exitChoice = static_cast<int>(item.size()) + 1;
            while (true) {
                cout << "Enter the number of the item to pick up from " << neighbor->getName() << " (" << exitChoice << " to exit): ";

                cin >> choice;
                if (choice > exitChoice || choice <= 0) {
                    cout << "Invalid answer. Please try again." << endl;
                    continue;
                }
                else if (choice == exitChoice) {
                    break;
                }
                else {
                    itemWasPickedUp = true;
                }

                const Item& selectedItem = item.at(choice - 1);
                items.emplace_back(selectedItem);
                neighbor->removeItem(selectedItem);
                    cout << playerName << " (" << heroName << ") picked up " 
                         << selectedItem.getItemName() << " from " << neighbor->getName() <<".\n";
            }
        }
    }

    if (!foundItems) {
        throw invalid_argument(playerName + " (Archeologist) found no items in neighboring locations.");
    }

    if (itemWasPickedUp) {
        remainingActions--;
    }
}
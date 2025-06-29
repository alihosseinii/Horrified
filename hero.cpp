#include "hero.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

string toSentenceCase(string name) {
    if (name.empty()) { 
        return name;
    }

    if (name[0] >= 'a' && name[0] <= 'z') { 
        name[0] = name[0] + ('A' - 'a');  
    }

    for (int i = 1; i < name.size(); i++) {
        if (name[i] == ' ') {
            name[i + 1] = name[i + 1] + ('A' - 'a');
            i++;
            continue;
        }
        if (name[i] == '.') {
            name[i + 1] = name[i + 1] + ('A' - 'a');
            i++;
            continue; 
        }
        if (name[i] >= 'A' && name[i] <= 'Z') {
            name[i] = name[i] + ('a' - 'A');
        }
    }
    return name;
}

Hero::Hero(const string& playerName, const string& heroName, int maxActions, shared_ptr<Location> startingLocation) {
    setPlayerName(playerName);
    setHeroName(heroName);
    setMaxActions(maxActions);
    setRemainingActions(maxActions);
    setCurrentLocation(startingLocation);
    currentLocation->addCharacter(heroName);
}

void Hero::setHeroName(string heroName) {
    this->heroName = heroName;
}

void Hero::setPlayerName(string playerName) {
    this->playerName = playerName;
}

void Hero::setMaxActions(int maxActions) {
    if (maxActions < 0) {
        throw out_of_range("Invalid max actions: can't be negative.");
    }
    this->maxActions = maxActions;
}

int Hero::getMaxActions() const {
    return maxActions;
}

int Hero::getRemainingActions() const {
    return remainingActions;
}

void Hero::setRemainingActions(int remainingActions) {
    this->remainingActions = remainingActions;
}

void Hero::resetActions() {
    remainingActions = maxActions;
}

const string& Hero::getHeroName() const {
    return heroName;
}

const string& Hero::getPlayerName() const {
    return playerName;
}

shared_ptr<Location> Hero::getCurrentLocation() const {
    return currentLocation;
}

void Hero::setCurrentLocation(shared_ptr<Location> currentLocation) {
    this->currentLocation = currentLocation;
}

vector<Item> Hero::getItems() const {
    return items;
}

void Hero::move(shared_ptr<Location> newLocation, VillagerManager& villagerManager, PerkDeck* perkDeck) {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    if (currentLocation == newLocation) {
        throw invalid_argument(playerName + " (" + heroName + ") is already in " + currentLocation->getName());
    }
    if (!newLocation) {
        throw invalid_argument("There is no location called " + newLocation->getName());
    }

    const auto& neighbors = currentLocation->getNeighbors();

    bool isNeighbor = false;
    for (const auto& neighbor : neighbors) {
        if (neighbor && neighbor->getName() == newLocation->getName()) {
            isNeighbor = true;
            break;
        }
    }
    if (!isNeighbor) {
        throw invalid_argument(playerName + " (" + heroName + ") can't move to " + newLocation->getName() + " - not a neighbor.");
    }

    auto characters = currentLocation->getCharacters();
    string answer;
    bool characterExists = false;
    if (!characters.empty()) {
        for (const auto& character : characters) {
            if (character == "Archeologist" || character == "Mayor" || character == "Dracula" || character == "Invisible man") {
                continue;
            }
            characterExists = true;
        }
    }

    if (characterExists) {
        cin.ignore();
        while (true) {
            cout << "Do you want to move villager(s) with yourself(Yes or No)? ";
            getline(cin, answer);
            answer = toSentenceCase(answer);
            if (answer == "No") break;
            if (answer == "Yes") {
                for (const auto& character : characters) {
                    if (character == "Archeologist" || character == "Mayor" || character == "Dracula" || character == "Invisible man") {
                        continue;
                    }
                    auto villager = villagerManager.getVillager(character);
                    villager->move(newLocation, this, perkDeck);
                }
                break;
            }
            cout << "Invalid answer. Please try again" << endl;
        }
    }

    currentLocation->removeCharacter(heroName);
    newLocation->addCharacter(heroName);
    setCurrentLocation(newLocation);

    cout << heroName << " (" << playerName << ") moved to " << currentLocation->getName() << ".\n";
    remainingActions--;
}

void Hero::guide(VillagerManager& villagerManager, Map& map, PerkDeck* perkDeck) {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    cout << "Which villager do you want to move? ";
    string chosenVillager;
    getline(cin, chosenVillager);
    chosenVillager = toSentenceCase(chosenVillager);

    bool villagerExistInCurrentLocation = false;
    bool villagerExistInNeighborLocation = false;

    const auto& neighbors = currentLocation->getNeighbors();
    auto characters = currentLocation->getCharacters();

    for (const auto& character : characters) {
        if (chosenVillager == character) {
            villagerExistInCurrentLocation = true;
            break;
        }
    }
    for (const auto& neighbor : neighbors) {
        auto characters = neighbor->getCharacters();
        for (const auto& character : characters) {
            if (chosenVillager == character) {
                villagerExistInNeighborLocation = true;
                break;
            }
        }
    }
    
    if (!villagerExistInCurrentLocation && !villagerExistInNeighborLocation) {
        throw invalid_argument("There is no " + chosenVillager + " in your nearby to move");
    }

    if (villagerExistInCurrentLocation) {
        cout << "Where do you wnat to take " << chosenVillager << "? ";
        string chosenLocation;
        getline(cin, chosenLocation);
        chosenLocation = toSentenceCase(chosenLocation);
        auto villager = villagerManager.getVillager(chosenVillager);
        auto location = map.getLocation(chosenLocation);
        villager->move(location, this, perkDeck);
    }

    if (villagerExistInNeighborLocation) {
        auto villager = villagerManager.getVillager(chosenVillager);
        villager->move(currentLocation, this, perkDeck);
    }

    remainingActions--;
}

void Hero::pickUp() {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    const auto locationItems = currentLocation->getItems();
    if (locationItems.empty()) {
        throw invalid_argument("No items to pick up in " + currentLocation->getName() + ".\n");
    }

    cout << "Items in " << currentLocation->getName() << ":\n";
    for (size_t i = 0; i < static_cast<int>(locationItems.size()); ++i) {
        const auto& item = locationItems[i];
        cout << i + 1 << ". " << item.getItemName() << " (" 
             << Item::colorToString(item.getColor()) << ", Power: " 
             << item.getPower() << ")\n";
    }

    int choice;
    bool itemWasPickedUp = false;
    int exitChoice = static_cast<int>(locationItems.size()) + 1;
    while (true) {
        cout << "Enter the number of the item to pick up (" << exitChoice << " to exit): ";
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
        const Item& selectedItem = locationItems.at(choice - 1);
        items.emplace_back(selectedItem);
        currentLocation->removeItem(selectedItem);
        cout << playerName << " (" << heroName << ") picked up " 
             << selectedItem.getItemName() << ".\n";
    }

    if (itemWasPickedUp) {
        remainingActions--;
    }
}

// void Hero::advance() {
//     cout << name << " attempts to advance mission.\n";
//     remainingActions--;
// }

// void Hero::defeat() {
//     std::cout << name << " attempts to defeat monster.\n";
//     remainingActions--;
// }

void Hero::addPerkCard(const PerkCard& card) {
    perkCards.push_back(card);
}

const std::vector<PerkCard>& Hero::getPerkCards() const {
    return perkCards;
}

void Hero::displayPerkCards() const {
    if (perkCards.empty()) {
        cout << playerName << " (" << heroName << ") has no perk cards.\n";
        return;
    }
    
    cout << playerName << " (" << heroName << ") has " << perkCards.size() << " perk card(s):\n";
    for (size_t i = 0; i < perkCards.size(); ++i) {
        cout << i + 1 << ". " << PerkCard::perkTypeToString(perkCards[i].getType()) 
             << ": " << perkCards[i].getDescription() << "\n";
    }
}

void Hero::removeItem(size_t index) {
    if (index >= items.size()) {
        throw out_of_range("Item index out of range");
    }
    items.erase(items.begin() + index);
}



#include "hero.hpp"
#include "villagermanager.hpp"
#include "perkdeck.hpp"
#include "invisibleman.hpp"
#include "dracula.hpp"
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
    skipNextMonsterPhase = false;
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
                    try {
                        auto villager = villagerManager.getVillager(character);
                        villager->move(newLocation, this, perkDeck);
                    } catch (const exception& e) {
                        cout << e.what() << endl;
                    }
                }
                break;
            }
            cout << "Invalid answer. Please try again" << endl;
        }
    }

    try {
        setCurrentLocation(newLocation);
        currentLocation->removeCharacter(heroName);
        newLocation->addCharacter(heroName);
    
        cout << heroName << " (" << playerName << ") moved to " << currentLocation->getName() << ".\n";
    } catch (const exception& e) {
        cout << e.what() << endl;
    }

    remainingActions--;
}

void Hero::guide(VillagerManager& villagerManager, Map& map, PerkDeck* perkDeck) {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    std::vector<std::shared_ptr<Villager>> guidableVillagers;
    std::vector<std::vector<std::shared_ptr<Location>>> guidableMoves;
    auto heroLoc = currentLocation;
    const auto& heroNeighbors = heroLoc->getNeighbors();

    for (const auto& locPair : map.locations) {
        auto loc = locPair.second;
        const auto& characters = loc->getCharacters();
        for (const auto& character : characters) {
            if (character == "Dracula" || character == "Invisible man" || character == "Archeologist" || character == "Mayor") continue;

            std::vector<std::shared_ptr<Location>> possibleMoves;

            if (loc->getName() == heroLoc->getName()) {
                for (const auto& neighbor : heroNeighbors) {
                    possibleMoves.push_back(neighbor);
                }
            } else {
                const auto& vNeighbors = loc->getNeighbors();
                for (const auto& vNeighbor : vNeighbors) {
                    if (vNeighbor->getName() == heroLoc->getName()) {
                        possibleMoves.push_back(heroLoc);
                        break;
                    }
                }
            }

            if (!possibleMoves.empty()) {
                try {
                    auto villager = villagerManager.getVillager(character);
                    guidableVillagers.push_back(villager);
                    guidableMoves.push_back(possibleMoves);
                } catch (const exception& e) {
                    cout << e.what() << endl;
                }
            }
        }
    }

    if (guidableVillagers.empty()) {
        cout << "There are no villagers on the map that you can guide right now.\n";
        return;
    }

    cout << "Villagers you can guide:\n";
    for (size_t i = 0; i < guidableVillagers.size(); ++i) {
        cout << i + 1 << ". " << guidableVillagers[i]->getVillagerName()
             << " (at " << guidableVillagers[i]->getCurrentLocation()->getName() << ")\n";
    }

    cout << "Choose a villager to guide (1-" << guidableVillagers.size() << "): ";
    int villagerIndex;
    cin >> villagerIndex;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (villagerIndex < 1 || villagerIndex > static_cast<int>(guidableVillagers.size())) {
        cout << "Invalid choice.\n";
        return;
    }

    auto chosenVillager = guidableVillagers[villagerIndex - 1];
    auto& possibleLocations = guidableMoves[villagerIndex - 1];
    std::shared_ptr<Location> chosenLocation;

    if (possibleLocations.size() == 1) {
        chosenLocation = possibleLocations[0];
        cout << chosenVillager->getVillagerName() << " will move to " << chosenLocation->getName() << ".\n";
    } else {
        cout << "Where do you want to take " << chosenVillager->getVillagerName() << "?\n";
        for (size_t i = 0; i < possibleLocations.size(); ++i) {
            cout << i + 1 << ". " << possibleLocations[i]->getName() << "\n";
        }
        cout << "Choose location (1-" << possibleLocations.size() << "): ";
        int locationIndex;
        cin >> locationIndex;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (locationIndex < 1 || locationIndex > static_cast<int>(possibleLocations.size())) {
            cout << "Invalid choice.\n";
            return;
        }

        chosenLocation = possibleLocations[locationIndex - 1];
    }

    try {
        chosenVillager->move(chosenLocation, this, perkDeck);
    } catch (const exception& e) {
        cout << e.what() << endl;
        return;
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
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice > exitChoice || choice <= 0) {
            cout << "Invalid answer. Please try again." << endl;
            continue;
        }
        else if (choice == exitChoice) {
            break;
        }
        const auto& selectedItem = locationItems[choice - 1];
        items.push_back(selectedItem);
        currentLocation->removeItem(selectedItem);
        cout << playerName << " (" << heroName << ") picked up " << selectedItem.getItemName() << ".\n";
        itemWasPickedUp = true;
    }

    if (itemWasPickedUp) {
        remainingActions--;
    }
}

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

bool Hero::usePerkCard(size_t index, Map& map, VillagerManager& villagerManager, PerkDeck* perkDeck, InvisibleMan* invisibleMan, ItemBag* itemBag, Hero* otherHero, Dracula* dracula) {
    if (index >= perkCards.size()) {
        cout << "Invalid perk card index.\n";
        return false;
    }

    PerkCard card = perkCards[index];
    PerkType type = card.getType();
    
    cout << playerName << " (" << heroName << ") uses " << PerkCard::perkTypeToString(type) << "!\n";
    
    switch (type) {
        case PerkType::VisitFromTheDetective: {
            cout << "Choose a location to place the Invisible Man: ";
            string locationName;
            getline(cin, locationName);
            locationName = toSentenceCase(locationName);
            
            try {
                auto targetLocation = map.getLocation(locationName);
                
                if (invisibleMan != nullptr) {
                    auto currentLocation = invisibleMan->getCurrentLocation();
                    currentLocation->removeCharacter("Invisible man");
                    invisibleMan->setCurrentLocation(targetLocation);
                    targetLocation->addCharacter("Invisible man");
                    cout << "Invisible man moved to " << targetLocation->getName() << ".\n";
                } else {
                    cout << "Invisible Man is dead.\n";
                    break;
                }
            } catch (const exception& e) {
                cout << "Invalid location: " << e.what() << "\n";
                break;
            }
            break;
        }
        
        case PerkType::BreakOfDawn: {
            setSkipNextMonsterPhase(true);

            if (itemBag != nullptr) {
                try {
                    itemBag->drawRandomItem(map);
                    itemBag->drawRandomItem(map);
                    cout << "Two items were added!\n";
                } catch (const exception& e) {
                    cout << e.what() << endl;
                }
            }
            break;
        }
        
        case PerkType::Overstock: {
            if (itemBag != nullptr) {
                try {
                    itemBag->drawRandomItem(map);
                    itemBag->drawRandomItem(map);
                    cout << "Two items were added!\n";
                } catch (const exception& e) {
                    cout << e.what() << endl;
                }
            }
            break;
        }
        
        case PerkType::LateIntoTheNight: {
            cout << "You gain 2 additional actions!\n";
            remainingActions += 2;
            break;
        }
        
        case PerkType::Repel: {
            cout << "Each monster moves 2 locations.\n";
            
            if (dracula != nullptr && dracula->getCurrentLocation() != nullptr) {
                dracula->moveToNearestCharacter("*", 2);
            }
            
            if (invisibleMan != nullptr && invisibleMan->getCurrentLocation() != nullptr) {
                invisibleMan->moveToNearestCharacter("*", 2);
            }
            break;
        }
        
        case PerkType::Hurry: {
            cout << "Each hero moves up to 2 locations.\n";
            this->moveTwoSteps();
            if (otherHero) otherHero->moveTwoSteps();
            break;
        }
    }
    
    removePerkCard(index);
    return true;
}

void Hero::removePerkCard(size_t index) {
    if (index >= perkCards.size()) {
        throw out_of_range("Perk card index out of range");
    }
    perkCards.erase(perkCards.begin() + index);
}

void Hero::removeItem(size_t index) {
    if (index >= items.size()) {
        throw out_of_range("Item index out of range");
    }
    items.erase(items.begin() + index);
}

bool Hero::shouldSkipNextMonsterPhase() const {
    return skipNextMonsterPhase;
}

void Hero::setSkipNextMonsterPhase(bool skip) {
    skipNextMonsterPhase = skip;
}

void Hero::advance(Dracula& dracula, TaskBoard& taskBoard) {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    if (currentLocation->getName() == "Precinct") {
        vector<string> clueLocations = {"Inn", "Barn", "Institute", "Laboratory", "Mansion"};
        vector<pair<size_t, Item>> eligibleClues;
        for (size_t i = 0; i < items.size(); ++i) {
            string itemLoc = items[i].getLocation() ? items[i].getLocation()->getName() : "";
            for (const auto& clueLoc : clueLocations) {
                if (itemLoc == clueLoc && !taskBoard.isClueDelivered(clueLoc)) {
                    eligibleClues.push_back({i, items[i]});
                }
            }
        }
        if (eligibleClues.empty()) {
            throw invalid_argument("You have no eligible clue items to deliver at the Precinct.");
        }
        cout << "Choose an item to use against Invisible man:\n";
        for (size_t i = 0; i < eligibleClues.size(); ++i) {
            cout << i + 1 << ". " << eligibleClues[i].second.getItemName() << " (from " << eligibleClues[i].second.getLocation()->getName() << ")\n";
        }
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice > 0 && choice <= static_cast<int>(eligibleClues.size())) {
            const auto& selected = eligibleClues[choice - 1];
            taskBoard.deliverClue(selected.second.getLocation()->getName());
            cout << playerName << "(" << heroName << ") used " << selected.second.getItemName() << " from " << selected.second.getLocation()->getName() << " on Invisible Man.\n";
            removeItem(selected.first);
            remainingActions--;
            return;
        } else {
            cout << "Invalid choice.\n";
            return;
        }
    }

    if (!taskBoard.isCoffinLocation(currentLocation->getName())) {
        throw invalid_argument("There is no coffin at this location.");
    }
    if (taskBoard.isCoffinDestroyed(currentLocation->getName())) {
        throw invalid_argument("The coffin at this location has already been destroyed.");
    }
    vector<pair<size_t, Item>> redItems;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].getColor() == ItemColor::Red) {
            redItems.push_back({i, items[i]});
        }
    }
    if (redItems.empty()) {
        throw invalid_argument("You have no red items to use.");
    }
    cout << "Choose a red item to use:\n";
    for (size_t i = 0; i < redItems.size(); ++i) {
        cout << i + 1 << ". " << redItems[i].second.getItemName() << " (Power: " << redItems[i].second.getPower() << ")\n";
    }
    int choice;
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice > 0 && choice <= static_cast<int>(redItems.size())) {
        const auto& selectedItem = redItems[choice - 1];
        taskBoard.addStrengthToCoffin(currentLocation->getName(), selectedItem.second.getPower());
        cout << playerName << "(" << heroName << ") used " << selectedItem.second.getItemName() << " on the coffin at " << currentLocation->getName() << ".\n";
        removeItem(selectedItem.first);
        remainingActions--;
        return;
    } else {
        cout << "Invalid choice.\n";
        return;
    }

    throw invalid_argument("You cannot use advance in " + currentLocation->getName() + ".");
}

void Hero::defeat(Dracula& dracula, TaskBoard& taskBoard) {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    bool atInvisibleMan = false;
    for (const auto& c : currentLocation->getCharacters()) {
        if (c == "Invisible man") atInvisibleMan = true;
    }

    if (!atInvisibleMan && currentLocation != dracula.getCurrentLocation()) {
        throw invalid_argument("Defeat action cannot be used when there is no monster in your location.");
    }

    if (atInvisibleMan) {
        if (!taskBoard.allCluesDelivered()) {
            throw invalid_argument("Not all items have been delivered. You cannot defeat the Invisible man yet.");
        }
        vector<pair<size_t, Item>> redItems;
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].getColor() == ItemColor::Red) {
                redItems.push_back({i, items[i]});
            }
        }
        if (redItems.empty()) {
            throw invalid_argument("You have no red items to use against the Invisible man.");
        }
        cout << "Choose a red item to use against the Invisible man (" << taskBoard.getInvisibleManDefeatStrength() << "/9 so far):\n";
        for (size_t i = 0; i < redItems.size(); ++i) {
            cout << i + 1 << ". " << redItems[i].second.getItemName() << " (Power: " << redItems[i].second.getPower() << ")\n";
        }
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice > 0 && choice <= static_cast<int>(redItems.size())) {
            const auto& selectedItem = redItems[choice - 1];
            taskBoard.addStrengthToInvisibleMan(selectedItem.second.getPower());
            cout << playerName << "(" << heroName << ") used " << selectedItem.second.getItemName() << " against the Invisible man.\n";
            removeItem(selectedItem.first);
            remainingActions--;
            if (taskBoard.getInvisibleManDefeatStrength() >= 9) {
                taskBoard.defeatInvisibleMan();
                cout << playerName << "(" << heroName << ") has defeated the Invisible man!\n";
            }
            return;
        } else {
            cout << "Invalid choice.\n";
            return;
        }
    }

    if (!taskBoard.allCoffinsDestroyed()) {
        throw invalid_argument("Not all coffins have been destroyed. You cannot defeat Dracula yet.");
    }
    if (currentLocation != dracula.getCurrentLocation()) {
        throw invalid_argument("You are not at the same location as Dracula.");
    }
    vector<pair<size_t, Item>> yellowItems;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].getColor() == ItemColor::Yellow) {
            yellowItems.push_back({i, items[i]});
        }
    }
    if (yellowItems.empty()) {
        throw invalid_argument("You have no yellow items to use against Dracula.");
    }
    cout << "Choose a yellow item to use against Dracula (" << taskBoard.getDraculaDefeatStrength() << "/6 so far):\n";
    for (size_t i = 0; i < yellowItems.size(); ++i) {
        cout << i + 1 << ". " << yellowItems[i].second.getItemName() << " (Power: " << yellowItems[i].second.getPower() << ")\n";
    }
    int choice;
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore();
    if (choice > 0 && choice <= static_cast<int>(yellowItems.size())) {
        const auto& selectedItem = yellowItems[choice - 1];
        taskBoard.addStrengthToDracula(selectedItem.second.getPower());
        cout << heroName << " used " << selectedItem.second.getItemName() << " against Dracula.\n";
        removeItem(selectedItem.first);
        remainingActions--;
        if (taskBoard.getDraculaDefeatStrength() >= 6) {
            cout << playerName << "(" << heroName << ") has defeated the Dracula!\n";
        }
    } else {
        cout << "Invalid choice.\n";
    }
}

void Hero::moveTwoSteps() {
    auto loc = getCurrentLocation();
    for (int step = 0; step < 2; ++step) {
        const auto& neighbors = loc->getNeighbors();
        if (neighbors.empty()) break;
        auto nextLoc = neighbors[0];
        loc->removeCharacter(getHeroName());
        nextLoc->addCharacter(getHeroName());
        setCurrentLocation(nextLoc);
        loc = nextLoc;
    }
}
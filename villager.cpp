#include "villager.hpp"
#include "hero.hpp"
#include "perkcard.hpp"
#include "perkdeck.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

using namespace std;

Villager::Villager(const string& villagerName, shared_ptr<Location> startingLocation) {
    this->villagerName = villagerName;
    setCurrentLocation(startingLocation);
}

string Villager::getVillagerName() const {
    return villagerName;
}

void Villager::setVillagerName(string villagerName) {
    this->villagerName = villagerName;
}

shared_ptr<Location> Villager::getCurrentLocation() const {
    return currentLocation;
}

void Villager::setCurrentLocation(shared_ptr<Location> currentLocation) {
    this->currentLocation = currentLocation;
}

void Villager::checkSafePlace(PerkDeck* perkDeck, Hero* hero1, Hero* hero2) {
    int reachedSafePlace = 0;
    if (villagerName == "Dr.Cranley" && currentLocation->getName() == "Precinct") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr);  
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }
    else if (villagerName == "Dr.Reed" && currentLocation->getName() == "Camp") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr);  
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }
    else if (villagerName == "Prof.Pearson" && currentLocation->getName() == "Museum") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr);  
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }
    else if (villagerName == "Maleva" && currentLocation->getName() == "Shop") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr);  
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }
    else if (villagerName == "Fritz" && currentLocation->getName() == "Institute") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr); 
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }
    else if (villagerName == "Wilbur And Chick" && currentLocation->getName() == "Dungeon") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr);
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }
    else if (villagerName == "Maria" && currentLocation->getName() == "Camp") {
        currentLocation->removeCharacter(villagerName);
        setCurrentLocation(nullptr);
        cout << villagerName << " has reached their safe place and left the game!\n";
        reachedSafePlace++;
    }

    for (size_t i = 0; i < reachedSafePlace; ++i) {
        if (perkDeck != nullptr && (hero1 != nullptr || hero2 != nullptr)) {
            try {
                PerkCard perk = perkDeck->drawRandomCard();
                Hero* randomHero = (hero1 != nullptr && hero2 != nullptr) ? 
                    (rand() % 2 == 0 ? hero1 : hero2) : 
                    (hero1 != nullptr ? hero1 : hero2);
                
                randomHero->addPerkCard(perk);
                cout << randomHero->getPlayerName() << " (" << randomHero->getHeroName() 
                     << ") received perk card: " << PerkCard::perkTypeToString(perk.getType()) 
                     << " for " << villagerName << " reaching their safe place!\n";
            } catch (const exception& e) {
                cout << e.what() << endl;
            }
        }
    }
}

void Villager::move(shared_ptr<Location> newLocation, Hero* guidingHero, PerkDeck* perkDeck) {
    if (currentLocation == newLocation) {
        throw invalid_argument(villagerName + " is already in " + currentLocation->getName());
    }
    if (!newLocation) {
        throw invalid_argument("There is no location called " + newLocation->getName());
    }

    const auto& neighbors = currentLocation->getNeighbors();
    bool canMove = false;

    for (const auto& neighbor : neighbors) {
        if (neighbor && neighbor->getName() == newLocation->getName()) {
            canMove = true;
            break;
        }
    }

    if (!canMove) {
        throw invalid_argument(villagerName + " can't move to " + newLocation->getName() + " - not a neighbor.");
    }

    try {
        currentLocation->removeCharacter(villagerName);
        newLocation->addCharacter(villagerName);
        setCurrentLocation(newLocation);
        cout << villagerName << " moved to " << newLocation->getName() << ".\n";

        bool reachedSafePlace = false;
        if (villagerName == "Dr.Cranley" && newLocation->getName() == "Precinct") {
            reachedSafePlace = true;
        }
        else if (villagerName == "Dr.Reed" && newLocation->getName() == "Camp") {
            reachedSafePlace = true;
        }
        else if (villagerName == "Prof.Pearson" && newLocation->getName() == "Museum") {
            reachedSafePlace = true;
        }
        else if (villagerName == "Maleva" && newLocation->getName() == "Shop") {
            reachedSafePlace = true;
        }
        else if (villagerName == "Fritz" && newLocation->getName() == "Institute") {
            reachedSafePlace = true;
        }
        else if (villagerName == "Wilbur And Chick" && newLocation->getName() == "Dungeon") {
            reachedSafePlace = true;
        }
        else if (villagerName == "Maria" && newLocation->getName() == "Camp") {
            reachedSafePlace = true;
        }

        if (reachedSafePlace) {
            newLocation->removeCharacter(villagerName);
            setCurrentLocation(nullptr);
            cout << villagerName << " has reached their safe place and left the game!\n";
            
            if (guidingHero != nullptr && perkDeck != nullptr) {
                try {
                    PerkCard perk = perkDeck->drawRandomCard();
                    guidingHero->addPerkCard(perk);
                    cout << guidingHero->getPlayerName() << " (" << guidingHero->getHeroName() 
                         << ") received perk card: " << PerkCard::perkTypeToString(perk.getType()) 
                         << " for helping " << villagerName << " reach their safe place!\n";
                } catch (const exception& e) {
                    cout << e.what() << endl;
                }
            }
        }
    } catch (const exception& e) {
        throw runtime_error("Failed to move villager: " + string(e.what()));
    }
}

void Villager::moveByMonster(shared_ptr<Location> newLocation, PerkDeck* perkDeck, Hero* hero1, Hero* hero2) {
    if (currentLocation == newLocation) {
        throw invalid_argument(villagerName + " is already in " + currentLocation->getName());
    }
    if (!newLocation) {
        throw invalid_argument("There is no location called " + newLocation->getName());
    }

    try {
        currentLocation->removeCharacter(villagerName);
        newLocation->addCharacter(villagerName);
        setCurrentLocation(newLocation);
        cout << villagerName << " moved to " << newLocation->getName() << ".\n";

        checkSafePlace(perkDeck, hero1, hero2);
    } catch (const exception& e) {
        throw runtime_error("Failed to move villager: " + string(e.what()));
    }
}
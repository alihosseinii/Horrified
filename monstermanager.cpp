#include "monstermanager.hpp"
#include "item.hpp"
#include "dice.hpp"
#include "frenzymarker.hpp"
#include "terrorteracker.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

MonsterManager::MonsterManager() {
    rng.seed(chrono::steady_clock::now().time_since_epoch().count());
    hasCurrentCard = false;
    initializeDefaultCards();
    shuffle();
}

void MonsterManager::initializeDefaultCards() {
    cards.emplace_back(MonsterCard("Form Of The Bat", 2, "Move Dracula to hero location.", {
        {MonsterType::InvisibleMan, 1, 2}
    }));
    cards.emplace_back(MonsterCard("Form Of The Bat", 2, "Move Dracula to hero location.", {
        {MonsterType::InvisibleMan, 1, 2}
    }));
    cards.emplace_back(MonsterCard("Form Of The Bat", 2, "Move Dracula to hero location.", {
        {MonsterType::InvisibleMan, 1, 2}
    }));

    cards.emplace_back(MonsterCard("Sunrise", 0, "Place Dracula in Crypt.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));
    cards.emplace_back(MonsterCard("Sunrise", 0, "Place Dracula in Crypt.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));
    cards.emplace_back(MonsterCard("Sunrise", 0, "Place Dracula in Crypt.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard("Thief", 2, "Move Invisible man to location with most items and remove all items there.", {
        {MonsterType::InvisibleMan, 1, 3}, {MonsterType::Dracula, 1, 3}
    }));
    cards.emplace_back(MonsterCard("Thief", 2, "Move Invisible man to location with most items and remove all items there.", {
        {MonsterType::InvisibleMan, 1, 3}, {MonsterType::Dracula, 1, 3}
    }));
    cards.emplace_back(MonsterCard("Thief", 2, "Move Invisible man to location with most items and remove all items there.", {
        {MonsterType::InvisibleMan, 1, 3}, {MonsterType::Dracula, 1, 3}
    }));
    cards.emplace_back(MonsterCard("Thief", 2, "Move Invisible man to location with most items and remove all items there.", {
        {MonsterType::InvisibleMan, 1, 3}, {MonsterType::Dracula, 1, 3}
    }));
    cards.emplace_back(MonsterCard("Thief", 2, "Move Invisible man to location with most items and remove all items there.", {
        {MonsterType::InvisibleMan, 1, 3}, {MonsterType::Dracula, 1, 3}
    }));

    cards.emplace_back(MonsterCard("The Delivery", 3, "Place Wilbur And Chick in Docks.", {
        {MonsterType::FrenziedMonster, 1, 3}
    }));

    cards.emplace_back(MonsterCard("Fortune Teller", 3, "Place Maleva in Camp.", {
        {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard("Former Employer", 3, "Place Dr.Cranley in Laboratory.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard("Hurried Assistant", 3, "Place Fritz in Tower.", {
        {MonsterType::Dracula, 2, 3}
    }));

    cards.emplace_back(MonsterCard("The Innocent", 3, "Place Maria in Barn.", {
        {MonsterType::FrenziedMonster, 1, 3}, {MonsterType::Dracula, 1, 3}, {MonsterType::InvisibleMan, 1, 3}
    }));

    cards.emplace_back(MonsterCard("Egyptian Expert", 3, "Place Prof.Pearson in Cave.", {
        {MonsterType::Dracula, 2, 2}, {MonsterType::FrenziedMonster, 2, 2}
    }));

    cards.emplace_back(MonsterCard("The Ichthyologist", 3, "Place Dr.Reed in Institute.", {
        {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard("Hypnotic Gaze", 2, "The closest hero or villager to Dracula approaches him by 1 location.", {
        {MonsterType::InvisibleMan, 1, 2}
    }));
    cards.emplace_back(MonsterCard("Hypnotic Gaze", 2, "The closest hero or villager to Dracula approaches him by 1 location.", {
        {MonsterType::InvisibleMan, 1, 2}
    }));

    cards.emplace_back(MonsterCard("On The Move", 3, "Give the next monster the frenzy marker and move each villager one location to its safe place.", {
        {MonsterType::FrenziedMonster, 3, 2}
    }));
    cards.emplace_back(MonsterCard("On The Move", 3, "Give the next monster the frenzy marker and move each villager one location to its safe place.", {
        {MonsterType::FrenziedMonster, 3, 2}
    }));
}

void MonsterManager::shuffle() {
    std::shuffle(cards.begin(), cards.end(), rng);
}

MonsterCard MonsterManager::drawCard() {
    if (isEmpty()) {
        throw runtime_error("No monster cards left!");
    }
    shuffle();
    MonsterCard card = cards.back();
    cards.pop_back();
    
    currentCard = card;
    hasCurrentCard = true;
    
    return card;
}

bool MonsterManager::isEmpty() const {
    return cards.empty();
}

void MonsterManager::MonsterPhase(Map& map, ItemBag& itemBag, Dracula* dracula, InvisibleMan* invisibleMan, FrenzyMarker& frenzyMarker, Hero* currentHero, TerrorTracker& terrorTracker, Archeologist* archeologist, Mayor* mayor, Courier* courier, Scientist* scientist, VillagerManager& villagerManager, std::vector<std::string>& diceResults
    , PerkDeck* perkDeck, Hero* hero1, Hero* hero2
    #ifndef TERMINAL
        , GameScreen* gameScreen
    #endif
) {
    diceResults.clear();
    auto monsterCard = drawCard();

    for (size_t i = 0; i < monsterCard.getItemCount(); ++i) {
        itemBag.drawRandomItem(map);
    }

    cout << monsterCard.getName() << endl;

    {
        Monster* fr = frenzyMarker.getCurrentFrenzied();
        if (fr != nullptr) {
            cout << "Frenzy marker: " << fr->getMonsterName() << "!\n";
        } else {
            cout << "Frenzy marker: None\n";
        }
    }

    if (monsterCard.getName() == "Form Of The Bat") {
        if (dracula != nullptr && archeologist && mayor && courier && scientist) {
            auto currentHeroLocation = currentHero->getCurrentLocation();
            dracula->getCurrentLocation()->removeCharacter("Dracula");
            currentHeroLocation->addCharacter("Dracula");
            dracula->setCurrentLocation(currentHeroLocation);
            cout << "Dracula moved to " << currentHeroLocation->getName() << "!\n";
        } else {
            cout << "Dracula is defeated.\n";
        }
    }
    else if (monsterCard.getName() == "Sunrise") {
        if (dracula != nullptr) {
            auto cryptLocation = map.getLocation("Crypt");
            dracula->getCurrentLocation()->removeCharacter("Dracula");
            cryptLocation->addCharacter("Dracula");
            dracula->setCurrentLocation(cryptLocation);
            cout << "Dracula moved to Crypt!\n";
        } else {
            cout << "Dracula is defeated.\n";
        }
    }
    else if (monsterCard.getName() == "Thief") {
        if (invisibleMan != nullptr) {
            auto locationWithMostItems = map.getLocationWithMostItems();
            if (locationWithMostItems != nullptr) {
                invisibleMan->getCurrentLocation()->removeCharacter("Invisible man");
                locationWithMostItems->addCharacter("Invisible man");
                invisibleMan->setCurrentLocation(locationWithMostItems);
                locationWithMostItems->clearItems();
                cout << "Invisible man moved to " << locationWithMostItems->getName() << "!\n";
            } else {
                cout << "No items found anywhere. Invisible man stays in place.\n";
            }
        } else {
            cout << "Invisible man is defeated.\n";
        }
    }
    else if (monsterCard.getName() == "The Delivery") {
        auto docksLocation = map.getLocation("Docks");
        docksLocation->addCharacter("Wilbur And Chick");
        villagerManager.addVillager("Wilbur And Chick", docksLocation);
        cout << "Wilbur And Chick was placed in Docks!\n";
    }
    else if (monsterCard.getName() == "Fortune Teller") {
        auto campLocation = map.getLocation("Camp");
        campLocation->addCharacter("Maleva");
        villagerManager.addVillager("Maleva", campLocation);
        cout << "Maleva was placed in Camp!\n";
    }
    else if (monsterCard.getName() == "Former Employer") {
        auto laboratoryLocation = map.getLocation("Laboratory");
        laboratoryLocation->addCharacter("Dr.Cranley");
        villagerManager.addVillager("Dr.Cranley", laboratoryLocation);
        cout << "Dr.Cranley was placed in Laboratory!\n";
    }
    else if (monsterCard.getName() == "Hurried Assistant") {
        auto towerLocation = map.getLocation("Tower");
        towerLocation->addCharacter("Fritz");
        villagerManager.addVillager("Fritz", towerLocation);
        cout << "Fritz was placed in Tower!\n";
    }
    else if (monsterCard.getName() == "The Innocent") {
        auto barnLocation = map.getLocation("Barn");
        barnLocation->addCharacter("Maria");
        villagerManager.addVillager("Maria", barnLocation);
        cout << "Maria was placed in Barn!\n";
    }
    else if (monsterCard.getName() == "Egyptian Expert") {
        auto caveLocation = map.getLocation("Cave");
        caveLocation->addCharacter("Prof.Pearson");
        villagerManager.addVillager("Prof.Pearson", caveLocation);
        cout << "Prof.Pearson was placed in Cave!\n";
    }
    else if (monsterCard.getName() == "The Ichthyologist") {
        auto instituteLocation = map.getLocation("Institute");
        instituteLocation->addCharacter("Dr.Reed");
        villagerManager.addVillager("Dr.Reed", instituteLocation);
        cout << "Dr.Reed was placed in Institute!\n";
    }
    else if (monsterCard.getName() == "Hypnotic Gaze") {
        if (dracula != nullptr) {
            auto draculaLocation = dracula->getCurrentLocation();
            shared_ptr<Location> closestLocation = nullptr;
            int shortestDistance = 50;
            string closestCharacter = "";
            
            for (const auto& [name, location] : map.locations) {
                auto characters = location->getCharacters();
                for (const auto& character : characters) {
                    if (character == "Dracula" || character == "Invisible man") continue;
                    
                    int distance = map.calculateDistance(location, draculaLocation);
                    if (distance < shortestDistance) {
                        shortestDistance = distance;
                        closestLocation = location;
                        closestCharacter = character;
                    }
                }
            }
            
            if (closestLocation != nullptr && shortestDistance > 0) {
                auto closerLocation = map.findCloserLocation(closestLocation, draculaLocation);
                if (closerLocation != nullptr) {
                    try {
                        if (closestCharacter == "Archeologist" && archeologist) {
                            archeologist->getCurrentLocation()->removeCharacter("Archeologist");
                            closerLocation->addCharacter("Archeologist");
                            archeologist->setCurrentLocation(closerLocation);
                            cout << archeologist->getPlayerName() << " (Archeologist) moved to " << closerLocation->getName() << ".\n";
                        } else if (closestCharacter == "Mayor" && mayor) {
                            mayor->getCurrentLocation()->removeCharacter("Mayor");
                            closerLocation->addCharacter("Mayor");
                            mayor->setCurrentLocation(closerLocation);
                            cout << mayor->getPlayerName() << " (Mayor) moved to " << closerLocation->getName() << ".\n";
                        } else if (closestCharacter == "Courier" && courier) {
                            courier->getCurrentLocation()->removeCharacter("Courier");
                            closerLocation->addCharacter("Courier");
                            courier->setCurrentLocation(closerLocation);
                            cout << courier->getPlayerName() << " (Courier) moved to " << closerLocation->getName() << ".\n";
                        } else if (closestCharacter == "Scientist" && scientist) {
                            scientist->getCurrentLocation()->removeCharacter("Scientist");
                            closerLocation->addCharacter("Scientist");
                            scientist->setCurrentLocation(closerLocation);
                            cout << scientist->getPlayerName() << " (Scientist) moved to " << closerLocation->getName() << ".\n";
                        } else {
                            auto villager = villagerManager.getVillager(closestCharacter);
                            villager->moveByMonster(closerLocation, perkDeck, hero1, hero2);
                        }
                    } catch (const exception& e) {
                        cout << e.what() << endl;
                    }
                }
            }
        } else {
            cout << "Dracula is defeated.\n";
        }
    }
    else if (monsterCard.getName() == "On The Move") {
        frenzyMarker.advance(dracula, invisibleMan);
        {
            Monster* fr = frenzyMarker.getCurrentFrenzied();
            if (fr != nullptr) {
                cout << "Frenzy marker was given to " << fr->getMonsterName() << "!\n";
            } else {
                cout << "Frenzy marker: None\n";
            }
        }
        
        moveVillagersCloserToSafePlaces(map, villagerManager, perkDeck, hero1, hero2);
    }

    bool monsterPhaseEnding = false;
    int invisibleManPowerDice = 0;

    Strike strike;
    for (size_t i = 0; i < monsterCard.getStrikeList().size(); ++i) {
        if (monsterPhaseEnding) break;
        strike = monsterCard.getStrikeList().at(i);
        Monster* monster = nullptr;
        switch (strike.monster) {
            case MonsterType::Dracula:
                monster = dracula;
                break;
            case MonsterType::InvisibleMan:
                monster = invisibleMan;
                break;
            case MonsterType::FrenziedMonster:
                monster = frenzyMarker.getCurrentFrenzied();
                break;
        }

        if (monster != nullptr) {
            monster->moveToNearestCharacter("*", strike.moveCount);
        } else {
            continue;
        }

        Dice dice;
        vector<string> dices;
        for (size_t j = 0; j < strike.diceCount; ++j) {
            DiceFace diceFace = dice.roll();
            string face = dice.faceToString(diceFace);
            diceResults.push_back(face);
            dices.push_back(face);
        }

        while (!dices.empty()) {
            auto attack = std::find(dices.begin(), dices.end(), "*");
            if (attack != dices.end()) {
                if (monster != nullptr) {
                    if (monster->attack(archeologist, mayor, courier, scientist, terrorTracker, map, villagerManager, gameScreen)) {
                        monsterPhaseEnding = true;
                        break;
                    }   
                }
                dices.erase(attack);
                continue;
            }

            auto power = std::find(dices.begin(), dices.end(), "!");
            if (power != dices.end()) {
                if (monster != nullptr) {
                    if (strike.monster == MonsterType::InvisibleMan) {
                        int terrorBefore = terrorTracker.getLevel();
                        monster->power(currentHero, terrorTracker, villagerManager);
                        int terrorAfter = terrorTracker.getLevel();
                        
                        if (terrorAfter == terrorBefore) {
                            invisibleManPowerDice++;
                        } else {
                            if (terrorAfter > terrorBefore) {
                                monsterPhaseEnding = true;
                                break;
                            }
                        }
                    } else {
                        int terrorBefore = terrorTracker.getLevel();
                        monster->power(currentHero, terrorTracker, villagerManager);
                        int terrorAfter = terrorTracker.getLevel();
                        
                        if (terrorAfter > terrorBefore) {
                            monsterPhaseEnding = true;
                            break;
                        }
                    }
                }
                dices.erase(power);
                continue;
            }
            
            auto empty = std::find(dices.begin(), dices.end(), " ");
            if (empty != dices.end()) {
                dices.erase(empty);
            } else {
                break;
            }
        }
    }
    
    if (invisibleMan != nullptr) {
        if (invisibleManPowerDice > 0) {
            int totalSteps = invisibleManPowerDice * 2;
            invisibleMan->moveTowardsVillager(totalSteps);
        }
    } 
} 

void MonsterManager::moveVillagersCloserToSafePlaces(Map& map, VillagerManager& villagerManager, PerkDeck* perkDeck, Hero* hero1, Hero* hero2) {
    unordered_map<string, string> safePlaces = {
        {"Dr.Cranley", "Precinct"},
        {"Dr.Reed", "Camp"},
        {"Prof.Pearson", "Museum"},
        {"Maleva", "Shop"},
        {"Fritz", "Institute"},
        {"Wilbur And Chick", "Dungeon"},
        {"Maria", "Camp"}
    };

    unordered_set<string> movedVillagers;

    for (const auto& [locName, location] : map.locations) {
        auto chars = location->getCharacters();

        for (const auto& character : chars) {
            if (movedVillagers.find(character) != movedVillagers.end()) continue;

            auto it = safePlaces.find(character);
            if (it == safePlaces.end()) continue;

            const string& safePlace = it->second;

            try {
                auto villager = villagerManager.getVillager(character);
                auto currentLocation = villager->getCurrentLocation();
                if (!currentLocation) continue;

                auto safeLocation = map.getLocation(safePlace);
                if (!safeLocation) continue;

                if (currentLocation == safeLocation) continue;

                auto closerLocation = map.findCloserLocation(currentLocation, safeLocation);
                if (closerLocation != nullptr && closerLocation != currentLocation) {
                    villager->moveByMonster(closerLocation, perkDeck, hero1, hero2);
                    movedVillagers.insert(character);
                }
            } catch (const exception& e) {
                cerr << "Error moving villager " << character << ": " << e.what() << endl;
                continue;
            }
        }
    }
}

const vector<MonsterCard>& MonsterManager::getCards() const {
    return cards;
}

void MonsterManager::setCards(const vector<MonsterCard>& newCards) {
    cards = newCards;
}

std::string MonsterManager::getCurrentCardName() const {
    if (hasCurrentCard) {
        return currentCard.getName();
    }
    return "No card drawn";
}

MonsterCard MonsterManager::getCurrentCard() const {
    return currentCard;
}
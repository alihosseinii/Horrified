#include "game.hpp"
#include "archeologist.hpp"
#include "mayor.hpp"
#include "map.hpp"
#include "monster.hpp"
#include "dracula.hpp"
#include "invisibleman.hpp"
#include "villager.hpp"
#include "villagermanager.hpp"
#include "hero.hpp"
#include "item.hpp"
#include "monstermanager.hpp"
#include "frenzymarker.hpp"
#include "terrorteracker.hpp"
#include "perkdeck.hpp"
#include "taskboard.hpp"
#include "TUI.hpp"
#include "courier.hpp"
#include "scientist.hpp"
#include "gamestate.hpp"
#include "savemanager.hpp"
#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

Game::Game() : saveManager(std::make_unique<SaveManager>()) {}

void Game::play() {
    tui.clearScreen();
    TerrorTracker terrorTracker;
    string player1Name, player2Name, startingPlayerName, otherPlayerName, startingPlayerHero, otherPlayerHero;
    int player1GarlicTime, player2GarlicTime;

    tui.showWelcomeScreen();

    while (true) {
        cout << "Please enter your names:";

        cout << "\nPlayer 1: ";
        getline(cin, player1Name);
        if (player1Name == "") {
            cout << "Player 1 name cannot be empty. Please try again.\n\n";
            continue;
        }
        
        cout << "Player 2: ";
        getline(cin, player2Name);
        if (player2Name == "") {
            cout << "Player 2 name cannot be empty. Please try again.\n\n";
            continue;
        }

        if (player1Name == player2Name) {
        cout << "Player names must be different. Please try again.\n\n";
        continue;
        }

        break;
    }

    cout << "\nTo determine who starts, we need to know something important..." << endl;
    while (true) {
    cout << "When was the last time you ate garlic (in hours)?" << endl;

    cout << "Player 1 (" << player1Name << "): ";
    cin >> player1GarlicTime;
    if (cin.fail()) {
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        continue;
    }

    cout << "Player 2 (" << player2Name << "): ";
    cin >> player2GarlicTime;
    if (cin.fail()) {
        cout << "Invalid input. Please enter a number.\n";
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        continue;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    break;
    }

    if (player1GarlicTime > player2GarlicTime) {
        startingPlayerName = player2Name;
        otherPlayerName = player1Name;
    } else {
        startingPlayerName = player1Name;
        otherPlayerName = player2Name;
    }
    cout << "\nBased on garlic consumption, " << startingPlayerName << " will be the starting player!" << endl;

    cout << "\nNow, let's pick your heroes." << endl;

    vector<string> heros = {"Archeologist", "Mayor", "Courier", "Scientist"};
    while (true) {
        cout << startingPlayerName << ", which hero do you want to play (Archeologist, Mayor, Courier, or Scientist)? ";
        getline(cin, startingPlayerHero);
        startingPlayerHero = toSentenceCase(startingPlayerHero);

        if (std::find(heros.begin(), heros.end(), startingPlayerHero) == heros.end()) {
            tui.showMessage("Invalid hero name. Please choose one of: Archeologist, Mayor, Courier, or Scientist.");
            continue;
        }

        auto it = std::find(heros.begin(), heros.end(), startingPlayerHero);
        heros.erase(it);

        cout << endl;
        while (true) {
            cout << otherPlayerName << ", which hero do you want to play (" << heros.at(0) << ", " << heros.at(1) << ", or " << heros.at(2) << ")? ";
            getline(cin, otherPlayerHero);
            otherPlayerHero = toSentenceCase(otherPlayerHero);

            if (std::find(heros.begin(), heros.end(), otherPlayerHero) != heros.end()) {
                cout << endl << startingPlayerName << " will play as the " << startingPlayerHero << ".\n"; 
                cout << otherPlayerName << " will play as the " << otherPlayerHero << ".\n";
                break;
            }

            tui.showMessage("Invalid hero name. Please choose one of the remaining: " + heros.at(0) + ", " + heros.at(1) + ", or " + heros.at(2) + ".");
        }
        break;
    }

    Map gamemap;
    TaskBoard taskBoard;
    VillagerManager villagerManager;
    ItemBag itembag(gamemap);
    MonsterManager monsterManager;
    PerkDeck perkDeck;

    villagerManager.addVillager("Dr.Cranley", gamemap.getLocation("Laboratory")); 
    villagerManager.addVillager("Dr.Reed", gamemap.getLocation("Institute"));
    villagerManager.addVillager("Prof.Pearson", gamemap.getLocation("Cave"));
    villagerManager.addVillager("Maleva", gamemap.getLocation("Camp"));
    villagerManager.addVillager("Fritz", gamemap.getLocation("Tower"));
    villagerManager.addVillager("Wilbur And Chick", gamemap.getLocation("Docks"));
    villagerManager.addVillager("Maria", gamemap.getLocation("Barn"));   

    unique_ptr<Hero> mayor;
    unique_ptr<Hero> archeologist;
    unique_ptr<Hero> courier;
    unique_ptr<Hero> scientist;
    unique_ptr<Monster> dracula;
    unique_ptr<Monster> invisibleMan;
    Hero* currentHero = nullptr;
    Hero* otherHero = nullptr;
    try {
        auto theatre = gamemap.getLocation("Theatre");
        auto docks = gamemap.getLocation("Docks");
        auto shop = gamemap.getLocation("Shop");
        auto institute = gamemap.getLocation("Institute");

        if (startingPlayerHero == "Archeologist") {
            archeologist = make_unique<Archeologist>(startingPlayerName, docks);
            currentHero = archeologist.get();
        } else if (startingPlayerHero == "Mayor") {
            mayor = make_unique<Mayor>(startingPlayerName, theatre);
            currentHero = mayor.get();
        } else if (startingPlayerHero == "Courier") {
            courier = make_unique<Courier>(startingPlayerName, shop);
            currentHero = courier.get();
        } else if (startingPlayerHero == "Scientist") {
            scientist = make_unique<Scientist>(startingPlayerName, institute);
            currentHero = scientist.get();
        }

        if (otherPlayerHero == "Archeologist") {
            archeologist = make_unique<Archeologist>(otherPlayerName, docks);
            otherHero = archeologist.get();
        } else if (otherPlayerHero == "Mayor") {
            mayor = make_unique<Mayor>(otherPlayerName, theatre);
            otherHero = mayor.get();
        } else if (otherPlayerHero == "Courier") {
            courier = make_unique<Courier>(otherPlayerName, shop);
            otherHero = courier.get();
        } else if (otherPlayerHero == "Scientist") {
            scientist = make_unique<Scientist>(otherPlayerName, institute);
            otherHero = scientist.get();
        }

        auto invisibleManStartingPos = gamemap.getLocation("Inn"); 
        auto draculaStartingPos = gamemap.getLocation("Crypt"); 
        dracula = make_unique<Dracula>(draculaStartingPos);
        invisibleMan = make_unique<InvisibleMan>(invisibleManStartingPos);
    } catch (const exception &e) {
        tui.showMessage(e.what());
        return;
    }

    try {
        PerkCard perk = perkDeck.drawRandomCard();
        currentHero->addPerkCard(perk);
        perk = perkDeck.drawRandomCard();
        otherHero->addPerkCard(perk);
    } catch (const exception& e) {
        tui.showMessage(e.what());
    }

    FrenzyMarker frenzyMarker(static_cast<Dracula*>(dracula.get()), static_cast<InvisibleMan*>(invisibleMan.get()));

    cout << "\nGame setup complete! Let the horror begin!\n";
    cout << "Press Enter to continue..."; 
    cin.get();

    int turnCount = 1;
    bool gameRunning = true;
    Hero* temp = nullptr;

    while (gameRunning) {
        tui.clearScreen();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itembag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        if (terrorTracker.getLevel() >= 5) {
            cout << "The terror has reached its peak! The monsters have won. The heroes were unable to save the town.\n";
            gameRunning = false;
            break;
        }

        string choice;
        while (currentHero->getRemainingActions() > 0) {
            cout << "Press Enter to continue..."; 
            cin.get();

            tui.clearScreen();
            tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
            tui.showMapWithHeroInfo(currentHero, otherHero);
            tui.showActionMenuWithVillagers(gamemap);
            tui.showItemsOnBoard(itembag, gamemap);
            tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);
    
            cout << "\n======== HERO PHASE ========" << endl;
            cout << "Choose an action: ";
            getline(cin, choice);
            choice = toSentenceCase(choice);
            try {
                if (choice == "M" || choice == "Move") {
                    auto neighbors = currentHero->getCurrentLocation()->getNeighbors();
                     if (neighbors.empty()) {
                        cout << "No neighbor locations available.\n";
                        continue;
                    }
                    cout << "Available locations:\n";
                    for (size_t i = 0; i < neighbors.size(); ++i) {
                        cout << i + 1 << ". " << neighbors[i]->getName() << endl;
                    }
                    int locationChoice;  
                    while (true) {
                        cout << "Choose location (1-" << neighbors.size() << "): ";
                        cin >> locationChoice;
                        if (cin.fail()) {
                            cout << "Invalid input. Please enter a number.\n";
                            cin.clear(); 
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                            continue;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }
                    if (locationChoice > 0 && locationChoice <= static_cast<int>(neighbors.size())) {
                        currentHero->move(neighbors[locationChoice - 1], villagerManager, &perkDeck);
                    } else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    continue;
                } else if (choice == "G" || choice == "Guide") {
                    currentHero->guide(villagerManager, gamemap, &perkDeck);
                    continue;
                } else if (choice == "P" || choice == "Pick Up") {
                    currentHero->pickUp();
                    continue;
                } else if (choice == "A" || choice == "Advance") {
                    currentHero->advance(*static_cast<Dracula*>(dracula.get()), *static_cast<InvisibleMan*>(invisibleMan.get()), taskBoard);
                    continue;
                } else if (choice == "D" || choice == "Defeat") {
                    currentHero->defeat(*static_cast<Dracula*>(dracula.get()), taskBoard);
                    bool draculaDead = taskBoard.isDraculaDefeated();
                    bool invisibleManDead = taskBoard.isInvisibleManDefeated();
                    if (draculaDead) {
                        if (dracula->getCurrentLocation() != nullptr) {
                            dracula->getCurrentLocation()->removeCharacter("Dracula");
                            dracula->setCurrentLocation(nullptr);
                            dracula = nullptr;
                            frenzyMarker.advance(dracula.get(), invisibleMan.get());
                        }
                    }
                    if (invisibleManDead) {
                        if (invisibleMan->getCurrentLocation() != nullptr) {
                            invisibleMan->getCurrentLocation()->removeCharacter("Invisible man");
                            invisibleMan->setCurrentLocation(nullptr);
                            invisibleMan = nullptr;
                            frenzyMarker.advance(dracula.get(), invisibleMan.get());
                        }
                    }
                    if (draculaDead && invisibleManDead) {
                        cout << "Heroes win! Both Dracula and Invisible man are defeated!" << endl;
                        gameRunning = false;
                        break;
                    }
                    continue;
                } else if (choice == "S" || choice == "Special Action") {
                    currentHero->setOtherHero(otherHero);
                    currentHero->specialAction();
                    continue;
                } else if (choice == "U" || choice == "Use Perk") {
                    auto perkCards = currentHero->getPerkCards();
                    if (perkCards.empty()) {
                        cout << "You have no perk cards to use.\n";
                        continue;
                    }
                    cout << "Choose a perk card to use:\n";
                    for (size_t i = 0; i < perkCards.size(); ++i) {
                        cout << i + 1 << ". " << PerkCard::perkTypeToString(perkCards[i].getType()) 
                             << ": " << perkCards[i].getDescription() << "\n";
                    }
                    int exitChoice = static_cast<int>(perkCards.size()) + 1;
                    int perkChoice;
                    while (true) {
                        cout << "Choose perk card (" << exitChoice << " to exit): ";
                        cin >> perkChoice;
                        if (cin.fail()) {
                            cout << "Invalid input. Please enter a number.\n";
                            cin.clear(); 
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                            continue;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }
                    if (perkChoice > 0 && perkChoice <= static_cast<int>(perkCards.size())) {
                        currentHero->usePerkCard(perkChoice - 1, gamemap, villagerManager, &perkDeck, static_cast<InvisibleMan*>(invisibleMan.get()), &itembag, otherHero, static_cast<Dracula*>(dracula.get()));
                    } else if (perkChoice == exitChoice) {
                        continue;
                    } else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    continue;
                } else if (choice == "H" || choice == "Help") {
                    tui.showHelpMenu();
                    continue;
                } else if (choice == "E" || choice == "End Turn") {
                    break;
                } else if (choice == "Save") {
                    saveCurrentGame(player1Name, player2Name, startingPlayerName, otherPlayerName,
                                  startingPlayerHero, otherPlayerHero, player1GarlicTime, player2GarlicTime,
                                  turnCount, terrorTracker, gameRunning, currentHero, otherHero,
                                  dracula, invisibleMan, villagerManager, itembag, gamemap, taskBoard,
                                  monsterManager, perkDeck, frenzyMarker);
                    continue;
                } else if (choice == "Q" || choice == "Quit") {
                    gameRunning = false;
                    break;
                } else {
                    cout << "Invalid choice. Please try again.\n";
                    continue;
                }
            } catch (const exception& e) {
                tui.showMessage(e.what());
            }
        }

        if (!gameRunning) break;

        cout << "\nEnd of Hero Phase. Press Enter to continue..."; 
        cin.get();

        tui.clearScreen();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itembag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        cout << "\n======== MONSTER PHASE ========" << endl;
        vector<string> diceResults;
        if (currentHero->shouldSkipNextMonsterPhase()) {
            cout << "Monster phase skipped due to Break of Dawn perk card!\n";
            currentHero->setSkipNextMonsterPhase(false);
        } else {
            try {
                monsterManager.MonsterPhase(gamemap, itembag, static_cast<Dracula*>(dracula.get()), 
                                       static_cast<InvisibleMan*>(invisibleMan.get()), frenzyMarker, currentHero, terrorTracker,
                                       static_cast<Archeologist*>(archeologist.get()), static_cast<Mayor*>(mayor.get()),
                                       static_cast<Courier*>(courier.get()), static_cast<Scientist*>(scientist.get()), villagerManager, diceResults);
            } catch (const exception& e) {
                tui.showMessage(string("Error during monster phase: ") + e.what());
            }
            tui.showDiceRoll(diceResults);
        }
        cout << "\nEnd of Monster Phase. Press Enter to continue..."; 
        cin.get();

        tui.clearScreen();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itembag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        if (monsterManager.isEmpty() && 
            (!taskBoard.isDraculaDefeated() || !taskBoard.isInvisibleManDefeated())) {
            cout << "The monster card deck is empty, but not all monsters have been defeated!" << endl;
            cout << "The monsters have won. The town is lost." << endl;
            gameRunning = false;
            break;
        }

        currentHero->resetActions();
        otherHero->resetActions();

        temp = currentHero;
        currentHero = otherHero;
        otherHero = temp;

        turnCount++;
    }
    cout << "\n=========Game Over=========" << endl;
}

void Game::runMainMenu() {    
    tui.clearScreen();
    while (true) {
        showMainMenu();
        string choice;
        getline(cin, choice);
        choice = toSentenceCase(choice);
        
        if (choice == "N" || choice == "New Game") {
            startNewGame();
        } else if (choice == "L" || choice == "Load Game") {
            loadGame();
        } else if (choice == "D" || choice == "Delete Save") {
            deleteSave();
        } else if (choice == "Q" || choice == "Quit") {
            cout << "Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void Game::showMainMenu() {
    cout << "========= HORRIFIED =========" << endl;
    cout << "Welcome to the game!" << endl;
    cout << "\nMain Menu:" << endl;
    cout << "N - New Game" << endl;
    cout << "L - Load Game" << endl;
    cout << "D - Delete Save" << endl;
    cout << "Q - Quit" << endl;
    cout << "\nEnter your choice: ";
}

void Game::startNewGame() {
    play();
}

void Game::loadGame() {
    showSaveSlots();
    cout << "\nEnter save slot number (1-5), 'D' for detailed info, or 0 to cancel: ";
    string slotChoice;
    getline(cin, slotChoice);
    
    if (slotChoice == "0") {
        return;
    }
    
    if (slotChoice == "D" || slotChoice == "d") {
        cout << "Enter slot number to show detailed info (1-5): ";
        int detailSlot;
        cin >> detailSlot;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (detailSlot >= 1 && detailSlot <= 5) {
            showDetailedSaveInfo(detailSlot);
        } else {
            cout << "Invalid slot number." << endl;
        }
        return;
    }
    
    int slotNumber;
    try {
        slotNumber = stoi(slotChoice);
    } catch (const exception& e) {
        cout << "Invalid input. Please try again." << endl;
        return;
    }
    
    if (slotNumber < 1 || slotNumber > 5) {
        cout << "Invalid slot number. Please try again." << endl;
        return;
    }
    
    if (!saveManager->hasSave(slotNumber)) {
        cout << "No save found in slot " << slotNumber << "." << endl;
        return;
    }
    
    GameState gameState;
    if (saveManager->loadGame(gameState, slotNumber)) {
        cout << "Game loaded successfully!" << endl;
        restoreGameFromState(gameState);
    } else {
        cout << "Failed to load game from slot " << slotNumber << "." << endl;
    }
}

void Game::restoreGameFromState(const GameState& gameState) {
    // player information
    string p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHeroName;
    int p1Garlic, p2Garlic;
    gameState.restorePlayerInfo(p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHeroName, p1Garlic, p2Garlic);
    
    // game state
    int turnCount, terrorLevel;
    bool gameRunning;
    gameState.restoreGameState(turnCount, terrorLevel, gameRunning);
    int currentHeroIndex = gameState.getCurrentHeroIndex();
    
    Map gamemap;
    TaskBoard taskBoard;
    VillagerManager villagerManager;
    ItemBag itembag(gamemap);
    MonsterManager monsterManager;
    PerkDeck perkDeck;
    
    // villagers
    auto villagerStates = gameState.getVillagerStates();
    for (const auto& villagerState : villagerStates) {
        if (!villagerState.currentLocationName.empty() && villagerState.currentLocationName != "Defeated") {
            auto location = gamemap.getLocation(villagerState.currentLocationName);
            if (location) {
                villagerManager.addVillager(villagerState.villagerName, location);
            }
        }
    }
    
    unique_ptr<Hero> mayor;
    unique_ptr<Hero> archeologist;
    unique_ptr<Hero> courier;
    unique_ptr<Hero> scientist;
    Hero* currentHero = nullptr;
    Hero* otherHero = nullptr;
    
    try {
        auto theatre = gamemap.getLocation("Theatre");
        auto docks = gamemap.getLocation("Docks");
        auto shop = gamemap.getLocation("Shop");
        auto institute = gamemap.getLocation("Institute");
        
        if (startHero == "Archeologist") {
            archeologist = make_unique<Archeologist>(startPlayer, docks);
            currentHero = archeologist.get();
        } else if (startHero == "Mayor") {
            mayor = make_unique<Mayor>(startPlayer, theatre);
            currentHero = mayor.get();
        } else if (startHero == "Courier") {
            courier = make_unique<Courier>(startPlayer, shop);
            currentHero = courier.get();
        } else if (startHero == "Scientist") {
            scientist = make_unique<Scientist>(startPlayer, institute);
            currentHero = scientist.get();
        }
        
        if (otherHeroName == "Archeologist") {
            archeologist = make_unique<Archeologist>(otherPlayer, docks);
            otherHero = archeologist.get();
        } else if (otherHeroName == "Mayor") {
            mayor = make_unique<Mayor>(otherPlayer, theatre);
            otherHero = mayor.get();
        } else if (otherHeroName == "Courier") {
            courier = make_unique<Courier>(otherPlayer, shop);
            otherHero = courier.get();
        } else if (otherHeroName == "Scientist") {
            scientist = make_unique<Scientist>(otherPlayer, institute);
            otherHero = scientist.get();
        }
        
        if (!currentHero || !otherHero) {
            throw runtime_error("Failed to create heroes during restoration");
        }
        
        // hero states
        auto hero1State = gameState.getHeroState(true);
        auto hero2State = gameState.getHeroState(false);
        
        Hero* hero1 = nullptr;
        Hero* hero2 = nullptr;
        
        if (currentHero->getPlayerName() == hero1State.playerName && 
            currentHero->getHeroName() == hero1State.heroName) {
            hero1 = currentHero;
            hero2 = otherHero;
        } else if (otherHero->getPlayerName() == hero1State.playerName && 
                   otherHero->getHeroName() == hero1State.heroName) {
            hero1 = otherHero;
            hero2 = currentHero;
        } else {
            if (currentHeroIndex == 0) {
                hero1 = currentHero;
                hero2 = otherHero;
            } else {
                hero1 = otherHero;
                hero2 = currentHero;
            }
        }
        
        // hero 1 state
        if (hero1) {
            auto hero1Location = gamemap.getLocation(hero1State.currentLocationName);
            if (hero1Location) {
                hero1->setCurrentLocation(hero1Location);
            }
            hero1->setRemainingActions(hero1State.remainingActions);
            hero1->setSkipNextMonsterPhase(hero1State.skipNextMonsterPhase);
            
            // items
            for (const auto& item : hero1State.items) {
                hero1->addItem(item);
            }
            
            // perk cards
            for (const auto& perk : hero1State.perkCards) {
                hero1->addPerkCard(perk);
            }
        }
        
        // hero 2 state
        if (hero2) {
            auto hero2Location = gamemap.getLocation(hero2State.currentLocationName);
            if (hero2Location) {
                hero2->setCurrentLocation(hero2Location);
            }
            hero2->setRemainingActions(hero2State.remainingActions);
            hero2->setSkipNextMonsterPhase(hero2State.skipNextMonsterPhase);
            
            // items
            for (const auto& item : hero2State.items) {
                hero2->addItem(item);
            }
            
            // perk cards
            for (const auto& perk : hero2State.perkCards) {
                hero2->addPerkCard(perk);
            }
            
            if (currentHeroIndex == 0) {
                if (hero1 == currentHero) {
                } else {
                    Hero* temp = currentHero;
                    currentHero = otherHero;
                    otherHero = temp;
                }
            } else {
                if (hero2 == currentHero) {
                } else {
                    Hero* temp = currentHero;
                    currentHero = otherHero;
                    otherHero = temp;
                }
            }
        }
        
        auto invisibleManStartingPos = gamemap.getLocation("Inn"); 
        auto draculaStartingPos = gamemap.getLocation("Crypt"); 
        unique_ptr<Monster> dracula = nullptr;
        unique_ptr<Monster> invisibleMan = nullptr;
        auto draculaState = gameState.getMonsterState(true);
        auto invisibleManState = gameState.getMonsterState(false);
        if (draculaState.isAlive) {
            dracula = make_unique<Dracula>(draculaStartingPos);
        }
        if (invisibleManState.isAlive) {
            invisibleMan = make_unique<InvisibleMan>(invisibleManStartingPos);
        }
        
        // map state
        auto mapLocationStates = gameState.getMapLocationStates();
        for (const auto& locationState : mapLocationStates) {
            auto location = gamemap.getLocation(locationState.locationName);
            if (location) {
                location->clearItems();
                location->clearCharacters();
                
                for (const auto& character : locationState.characters) {
                    location->addCharacter(character);
                }
                
                for (const auto& itemState : locationState.items) {
                    Item item(itemState.itemName, itemState.color, itemState.power, location);
                    location->addItem(item);
                }
            }
        }
        
        // monster states
        if (dracula && draculaState.isAlive) {
            auto draculaLocation = gamemap.getLocation(draculaState.currentLocationName);
            if (draculaLocation) {
                dracula->setCurrentLocation(draculaLocation);
                try {
                    draculaLocation->addCharacter("Dracula");
                } catch (const exception& e) {
                }
            }
        } else if (dracula) {
            dracula->setCurrentLocation(nullptr);
            for (const auto& [locName, location] : gamemap.locations) {
                try {
                    location->removeCharacter("Dracula");
                } catch (const exception& e) {
                }
            }
            dracula = nullptr;
        }
        if (invisibleMan && invisibleManState.isAlive) {
            auto invisibleManLocation = gamemap.getLocation(invisibleManState.currentLocationName);
            if (invisibleManLocation) {
                invisibleMan->setCurrentLocation(invisibleManLocation);
                try {
                    invisibleManLocation->addCharacter("Invisible man");
                } catch (const exception& e) {
                }
            }
        } else if (invisibleMan) {
            invisibleMan->setCurrentLocation(nullptr);
            for (const auto& [locName, location] : gamemap.locations) {
                try {
                    location->removeCharacter("Invisible man");
                } catch (const exception& e) {
                }
            }
            invisibleMan = nullptr;
        }
        
        // task board state
        auto taskBoardState = gameState.getTaskBoardState();
        taskBoard.setDraculaCoffins(taskBoardState.draculaCoffins);
        taskBoard.setInvisibleManCluesDelivered(taskBoardState.invisibleManCluesDelivered);
        taskBoard.setDraculaDefeat(taskBoardState.draculaDefeat);
        taskBoard.setInvisibleManDefeat(taskBoardState.invisibleManDefeat);
        taskBoard.setInvisibleManDefeated(taskBoardState.invisibleManDefeated);
        
        // monster manager state
        auto monsterCards = gameState.getMonsterCards();
        monsterManager.setCards(monsterCards);
        
        // perk deck state
        auto perkDeckCards = gameState.getPerkDeckCards();
        perkDeck.setCards(perkDeckCards);
        
        // frenzy marker
        FrenzyMarker frenzyMarker(
            dracula ? static_cast<Dracula*>(dracula.get()) : nullptr,
            invisibleMan ? static_cast<InvisibleMan*>(invisibleMan.get()) : nullptr
        );
        frenzyMarker.setFrenzyLevel(gameState.getFrenzyLevel());
        
        TerrorTracker terrorTracker;
        terrorTracker.setLevel(terrorLevel);
                
        playRestoredGame(p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHeroName,
                        p1Garlic, p2Garlic, turnCount, terrorTracker, gameRunning,
                        currentHero, otherHero, dracula, invisibleMan, villagerManager,
                        itembag, gamemap, taskBoard, monsterManager, perkDeck, frenzyMarker, 
                        archeologist, mayor, courier, scientist);
        
    } catch (const exception &e) {
        cout << "Error restoring game: " << e.what() << endl;
    }
}

void Game::saveGame() {
    showSaveSlots();
    cout << "\nEnter save slot number (1-5) or 0 to cancel: ";
    int slotChoice;
    cin >> slotChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (slotChoice == 0) {
        return;
    }
    
    if (slotChoice < 1 || slotChoice > 5) {
        cout << "Invalid slot number. Please try again." << endl;
        return;
    }
    
    if (saveManager->hasSave(slotChoice)) {
        if (!confirmAction("Save slot " + to_string(slotChoice) + " already has a save. Overwrite it?")) {
            return;
        }
    }
    
    cout << "Enter save name: ";
    string saveName;
    getline(cin, saveName);
    
    if (saveName.empty()) {
        saveName = "Save " + to_string(slotChoice);
    }
    
    cout << "Save functionality requires a game to be in progress." << endl;
    cout << "Please use the 'Save' option during gameplay to save your current game." << endl;
}

void Game::playRestoredGame(const string& player1Name, const string& player2Name,
                           const string& startingPlayerName, const string& otherPlayerName,
                           const string& startingPlayerHero, const string& otherPlayerHero,
                           int player1GarlicTime, int player2GarlicTime, int turnCount,
                           TerrorTracker& terrorTracker, bool gameRunning, Hero* currentHero,
                           Hero* otherHero, unique_ptr<Monster>& dracula,
                           unique_ptr<Monster>& invisibleMan, VillagerManager& villagerManager,
                           ItemBag& itemBag, Map& gamemap, TaskBoard& taskBoard,
                           MonsterManager& monsterManager, PerkDeck& perkDeck,
                           FrenzyMarker& frenzyMarker, unique_ptr<Hero>& archeologist, 
                           unique_ptr<Hero>& mayor, unique_ptr<Hero>& courier, unique_ptr<Hero>& scientist) {

    cout << "\nLet the horror continue!\n";
    cout << "Press Enter to continue..."; 
    cin.get();

    Hero* temp = nullptr;

    while (gameRunning) {
        tui.clearScreen();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itemBag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        if (terrorTracker.getLevel() >= 5) {
            cout << "The terror has reached its peak! The monsters have won. The heroes were unable to save the town.\n";
            gameRunning = false;
            break;
        }

        string choice;
        while (currentHero->getRemainingActions() > 0) {
            cout << "Press Enter to continue..."; 
            cin.get();

            tui.clearScreen();
            tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
            tui.showMapWithHeroInfo(currentHero, otherHero);
            tui.showActionMenuWithVillagers(gamemap);
            tui.showItemsOnBoard(itemBag, gamemap);
            tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);
    
            cout << "\n======== HERO PHASE ========" << endl;
            cout << "Choose an action: ";
            getline(cin, choice);
            choice = toSentenceCase(choice);
            try {
                if (choice == "M" || choice == "Move") {
                    auto neighbors = currentHero->getCurrentLocation()->getNeighbors();
                     if (neighbors.empty()) {
                        cout << "No neighbor locations available.\n";
                        continue;
                    }
                    cout << "Available locations:\n";
                    for (size_t i = 0; i < neighbors.size(); ++i) {
                        cout << i + 1 << ". " << neighbors[i]->getName() << endl;
                    }
                    int locationChoice;  
                    while (true) {
                        cout << "Choose location (1-" << neighbors.size() << "): ";
                        cin >> locationChoice;
                        if (cin.fail()) {
                            cout << "Invalid input. Please enter a number.\n";
                            cin.clear(); 
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                            continue;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }
                    if (locationChoice > 0 && locationChoice <= static_cast<int>(neighbors.size())) {
                        currentHero->move(neighbors[locationChoice - 1], villagerManager, &perkDeck);
                    } else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    continue;
                } else if (choice == "G" || choice == "Guide") {
                    currentHero->guide(villagerManager, gamemap, &perkDeck);
                    continue;
                } else if (choice == "P" || choice == "Pick Up") {
                    currentHero->pickUp();
                    continue;
                } else if (choice == "A" || choice == "Advance") {
                    currentHero->advance(*static_cast<Dracula*>(dracula.get()), *static_cast<InvisibleMan*>(invisibleMan.get()), taskBoard);
                    continue;
                } else if (choice == "D" || choice == "Defeat") {
                    currentHero->defeat(*static_cast<Dracula*>(dracula.get()), taskBoard);
                    bool draculaDead = taskBoard.isDraculaDefeated();
                    bool invisibleManDead = taskBoard.isInvisibleManDefeated();
                    if (draculaDead) {
                        if (dracula->getCurrentLocation() != nullptr) {
                            dracula->getCurrentLocation()->removeCharacter("Dracula");
                            dracula->setCurrentLocation(nullptr);
                            dracula = nullptr;
                            frenzyMarker.advance(dracula.get(), invisibleMan.get());
                        }
                    }
                    if (invisibleManDead) {
                        if (invisibleMan->getCurrentLocation() != nullptr) {
                            invisibleMan->getCurrentLocation()->removeCharacter("Invisible man");
                            invisibleMan->setCurrentLocation(nullptr);
                            invisibleMan = nullptr;
                            frenzyMarker.advance(dracula.get(), invisibleMan.get());
                        }
                    }
                    if (draculaDead && invisibleManDead) {
                        cout << "Heroes win! Both Dracula and Invisible man are defeated!" << endl;
                        gameRunning = false;
                        break;
                    }
                    continue;
                } else if (choice == "S" || choice == "Special Action") {
                    currentHero->setOtherHero(otherHero);
                    currentHero->specialAction();
                    continue;
                } else if (choice == "U" || choice == "Use Perk") {
                    auto perkCards = currentHero->getPerkCards();
                    if (perkCards.empty()) {
                        cout << "You have no perk cards to use.\n";
                        continue;
                    }
                    cout << "Choose a perk card to use:\n";
                    for (size_t i = 0; i < perkCards.size(); ++i) {
                        cout << i + 1 << ". " << PerkCard::perkTypeToString(perkCards[i].getType()) 
                             << ": " << perkCards[i].getDescription() << "\n";
                    }
                    int exitChoice = static_cast<int>(perkCards.size()) + 1;
                    int perkChoice;
                    while (true) {
                        cout << "Choose perk card (" << exitChoice << " to exit): ";
                        cin >> perkChoice;
                        if (cin.fail()) {
                            cout << "Invalid input. Please enter a number.\n";
                            cin.clear(); 
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                            continue;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }
                    if (perkChoice > 0 && perkChoice <= static_cast<int>(perkCards.size())) {
                        currentHero->usePerkCard(perkChoice - 1, gamemap, villagerManager, &perkDeck, static_cast<InvisibleMan*>(invisibleMan.get()), &itemBag, otherHero, static_cast<Dracula*>(dracula.get()));
                    } else if (perkChoice == exitChoice) {
                        continue;
                    } else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    continue;
                } else if (choice == "H" || choice == "Help") {
                    tui.showHelpMenu();
                    continue;
                } else if (choice == "E" || choice == "End Turn") {
                    break;
                } else if (choice == "Save") {
                    saveCurrentGame(player1Name, player2Name, startingPlayerName, otherPlayerName,
                                  startingPlayerHero, otherPlayerHero, player1GarlicTime, player2GarlicTime,
                                  turnCount, terrorTracker, gameRunning, currentHero, otherHero,
                                  dracula, invisibleMan, villagerManager, itemBag, gamemap, taskBoard,
                                  monsterManager, perkDeck, frenzyMarker);
                    continue;
                } else if (choice == "Q" || choice == "Quit") {
                    gameRunning = false;
                    break;
                } else {
                    cout << "Invalid choice. Please try again.\n";
                    continue;
                }
            } catch (const exception& e) {
                tui.showMessage(e.what());
            }
        }

        if (!gameRunning) break;

        cout << "\nEnd of Hero Phase. Press Enter to continue..."; 
        cin.get();

        tui.clearScreen();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itemBag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        cout << "\n======== MONSTER PHASE ========" << endl;
        vector<string> diceResults;
        if (currentHero->shouldSkipNextMonsterPhase()) {
            cout << "Monster phase skipped due to Break of Dawn perk card!\n";
            currentHero->setSkipNextMonsterPhase(false);
        } else {
            try {
                monsterManager.MonsterPhase(gamemap, itemBag, static_cast<Dracula*>(dracula.get()), 
                                       static_cast<InvisibleMan*>(invisibleMan.get()), frenzyMarker, currentHero, terrorTracker,
                                       static_cast<Archeologist*>(archeologist.get()), static_cast<Mayor*>(mayor.get()),
                                       static_cast<Courier*>(courier.get()), static_cast<Scientist*>(scientist.get()), villagerManager, diceResults);
            } catch (const exception& e) {
                tui.showMessage(string("Error during monster phase: ") + e.what());
            }
            tui.showDiceRoll(diceResults);
        }
        cout << "\nEnd of Monster Phase. Press Enter to continue..."; 
        cin.get();

        tui.clearScreen();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itemBag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        if (monsterManager.isEmpty() && 
            (!taskBoard.isDraculaDefeated() || !taskBoard.isInvisibleManDefeated())) {
            cout << "The monster card deck is empty, but not all monsters have been defeated!" << endl;
            cout << "The monsters have won. The town is lost." << endl;
            gameRunning = false;
            break;
        }

        currentHero->resetActions();
        otherHero->resetActions();

        temp = currentHero;
        currentHero = otherHero;
        otherHero = temp;

        turnCount++;
    }
    cout << "\n=========Game Over=========" << endl;
}

void Game::saveCurrentGame(const string& player1Name, const string& player2Name,
                          const string& startingPlayerName, const string& otherPlayerName,
                          const string& startingPlayerHero, const string& otherPlayerHero,
                          int player1GarlicTime, int player2GarlicTime, int turnCount,
                          const TerrorTracker& terrorTracker, bool gameRunning, Hero* currentHero,
                          Hero* otherHero, const unique_ptr<Monster>& dracula,
                          const unique_ptr<Monster>& invisibleMan, const VillagerManager& villagerManager,
                          const ItemBag& itemBag, const Map& gamemap, const TaskBoard& taskBoard,
                          const MonsterManager& monsterManager, const PerkDeck& perkDeck,
                          const FrenzyMarker& frenzyMarker) {
    
    showSaveSlots();
    cout << "\nEnter save slot number (1-5) or 0 to cancel: ";
    int slotChoice;
    cin >> slotChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (slotChoice == 0) {
        return;
    }
    
    if (slotChoice < 1 || slotChoice > 5) {
        cout << "Invalid slot number. Please try again." << endl;
        return;
    }
    
    if (saveManager->hasSave(slotChoice)) {
        if (!confirmAction("Save slot " + to_string(slotChoice) + " already has a save. Overwrite it?")) {
            return;
        }
    }
    
    cout << "Enter save name: ";
    string saveName;
    getline(cin, saveName);
    
    if (saveName.empty()) {
        saveName = "Save " + to_string(slotChoice);
    }
    
    GameState gameState;
    
    // player information
    gameState.setPlayerInfo(player1Name, player2Name, startingPlayerName, otherPlayerName, 
                           startingPlayerHero, otherPlayerHero, player1GarlicTime, player2GarlicTime);
    
    // game state
    gameState.setGameState(turnCount, terrorTracker.getLevel(), gameRunning);
    
    Hero* startingPlayerHeroPtr = nullptr;
    Hero* otherPlayerHeroPtr = nullptr;
    
    if (currentHero->getPlayerName() == startingPlayerName) {
        startingPlayerHeroPtr = currentHero;
        otherPlayerHeroPtr = otherHero;
    } else {
        startingPlayerHeroPtr = otherHero;
        otherPlayerHeroPtr = currentHero;
    }
    
    // hero states
    gameState.setHeroState(startingPlayerHeroPtr, true);
    gameState.setHeroState(otherPlayerHeroPtr, false);
    
    if (currentHero == startingPlayerHeroPtr) {
        gameState.setCurrentHeroIndex(0);
    } else {
        gameState.setCurrentHeroIndex(1);
    }
    
    // monster states
    bool draculaAlive = dracula && dracula.get() != nullptr && dracula->getCurrentLocation() != nullptr;
    bool invisibleManAlive = invisibleMan && invisibleMan.get() != nullptr && invisibleMan->getCurrentLocation() != nullptr;
    gameState.setMonsterState(draculaAlive ? dracula.get() : nullptr, true);
    gameState.setMonsterState(invisibleManAlive ? invisibleMan.get() : nullptr, false);
    
    // other game components
    gameState.setVillagerStates(villagerManager);
    gameState.setMapState(gamemap);
    gameState.setTaskBoardState(taskBoard);
    gameState.setMonsterManagerState(monsterManager);
    gameState.setPerkDeckState(perkDeck);
    gameState.setFrenzyMarkerState(frenzyMarker);
    
    // game state
    if (saveManager->saveGame(gameState, slotChoice, saveName)) {
        cout << "Game saved successfully to slot " << slotChoice << "!" << endl;
    } else {
        cout << "Failed to save game to slot " << slotChoice << "." << endl;
    }
}

void Game::showSaveSlots() {
    tui.clearScreen();
    cout << "\n========= SAVE SLOTS =========" << endl;
    auto saveSlots = saveManager->getSaveSlots();
    
    for (const auto& slot : saveSlots) {
        cout << "Slot " << slot.slotNumber << ": ";
        if (slot.hasSave) {
            cout << slot.saveName << " (" << slot.saveDate << ")" << endl;
            cout << "  Players: " << slot.player1Name << " & " << slot.player2Name << endl;
            cout << "  Heroes: " << slot.startingPlayerHero << " & " << slot.otherPlayerHero << endl;
            cout << "  Turn: " << slot.turnCount << ", Terror: " << slot.terrorLevel << endl;
        } else {
            cout << "Empty" << endl;
        }
        cout << endl;
    }
}

void Game::showDetailedSaveInfo(int slotNumber) {
    tui.clearScreen();
    saveManager->showDetailedSaveInfo(slotNumber);
}

void Game::deleteSave() {
    showSaveSlots();
    cout << "\nEnter save slot number to delete (1-5) or 0 to cancel: ";
    int slotChoice;
    cin >> slotChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (slotChoice == 0) {
        return;
    }
    
    if (slotChoice < 1 || slotChoice > 5) {
        cout << "Invalid slot number. Please try again." << endl;
        return;
    }
    
    if (!saveManager->hasSave(slotChoice)) {
        cout << "No save found in slot " << slotChoice << "." << endl;
        return;
    }
    
    if (confirmAction("Are you sure you want to delete the save in slot " + to_string(slotChoice) + "?")) {
        if (saveManager->deleteSave(slotChoice)) {
            cout << "Save deleted successfully!" << endl;
        } else {
            cout << "Failed to delete save from slot " << slotChoice << "." << endl;
        }
    }
}

bool Game::confirmAction(const string& message) {
    cout << message << " (Yes/No): ";
    string answer;
    getline(cin, answer);
    answer = toSentenceCase(answer);
    return answer == "Y" || answer == "Yes";
}
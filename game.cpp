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
#include "TaskBoard.hpp"
#include "TUI.hpp"
#include <iostream>
#include <random>
#include <chrono>
#include <vector>

using namespace std;

void Game::play() {
    TUI tui;
    tui.clearScreen();
    TerrorTracker terrorTracker;
    string player1Name, player2Name, startingPlayerName, otherPlayerName, startingPlayerHero;
    int player1GarlicTime, player2GarlicTime;

    tui.showWelcomeScreen();

    cout << "Please enter your names:";
    cout << "\nPlayer 1: ";
    getline(cin, player1Name);
    cout << "Player 2: ";
    getline(cin, player2Name);

    cout << "\nTo determine who starts, we need to know something important..." << endl;
    cout << "When was the last time you ate garlic(in hours)?" << endl;
    cout << "Player 1 (" << player1Name << "): ";
    cin >> player1GarlicTime;
    cout << "Player 2 (" << player2Name << "): ";
    cin >> player2GarlicTime;
    cin.ignore();

    if (player1GarlicTime > player2GarlicTime) {
        startingPlayerName = player2Name;
        otherPlayerName = player1Name;
    } else {
        startingPlayerName = player1Name;
        otherPlayerName = player2Name;
    }
    cout << "\nBased on garlic consumption, " << startingPlayerName << " will be the starting player!" << endl;

    cout << "\nNow, let's pick your heroes." << endl;
    while (true) {
        cout << startingPlayerName << ", which hero do you want to play(Archeologist or Mayor)? ";
        cin >> startingPlayerHero;
        startingPlayerHero = toSentenceCase(startingPlayerHero);
        if (startingPlayerHero == "Archeologist" || startingPlayerHero == "Mayor") {
            cout << startingPlayerName << " has chosen to play as the " << startingPlayerHero << "." << endl;
            cout << otherPlayerName << " will play as the " << ((startingPlayerHero == "Archeologist") ? "Mayor" : "Archeologist") << ".\n"; 
            break;
        }
        tui.showMessage("That's not a valid hero. Please choose 'Archeologist' or 'Mayor'. Try again.");
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
    unique_ptr<Monster> dracula;
    unique_ptr<Monster> invisibleMan;
    Hero* currentHero = nullptr;
    try {
        auto theatre = gamemap.getLocation("Theatre");
        auto docks = gamemap.getLocation("Docks");
        if (startingPlayerHero == "Archeologist") {
            archeologist = make_unique<Archeologist>(startingPlayerName, docks);
            mayor = make_unique<Mayor>(otherPlayerName, theatre);
            currentHero = archeologist.get();
        } else {
            mayor = make_unique<Mayor>(startingPlayerName, theatre);
            archeologist = make_unique<Archeologist>(otherPlayerName, docks);
            currentHero = mayor.get();
        }
        string draculaStartingPositions[4] = {"Cave", "Dungeon", "Crypt", "Graveyard"};
        mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        uniform_int_distribution<int> choose(0, 3);
        int randomIndex = choose(rng);
        string draculaStartingPosition = draculaStartingPositions[randomIndex];
        string invisibleManStartingPositions[5] = {"Inn", "Barn", "Institute", "Laboratory", "Mansion"};
        choose = uniform_int_distribution<int>(0, 4);
        randomIndex = choose(rng);
        string invisibleManStartingPosition = invisibleManStartingPositions[randomIndex];
        auto invisibleManStartingPos = gamemap.getLocation(invisibleManStartingPosition); 
        auto draculaStartingPos = gamemap.getLocation(draculaStartingPosition); 
        dracula = make_unique<Dracula>(draculaStartingPos);
        invisibleMan = make_unique<InvisibleMan>(invisibleManStartingPos);
    } catch(const exception &e) {
        tui.showMessage(e.what());
        return;
    }

    try {
        PerkCard perk = perkDeck.drawRandomCard();
        archeologist->addPerkCard(perk);
        perk = perkDeck.drawRandomCard();
        mayor->addPerkCard(perk);
    } catch (const exception& e) {
        tui.showMessage(e.what());
    }

    FrenzyMarker frenzyMarker(static_cast<Dracula*>(dracula.get()), static_cast<InvisibleMan*>(invisibleMan.get()));

    cin.ignore();
    cout << "Game setup complete! Let the horror begin!\n";
    cout << "Press Enter to continue..."; 
    cin.get();

    int turnCount = 1;
    bool gameRunning = true;
    while (gameRunning) {
        tui.clearScreen();
        Hero* otherHero = (currentHero == mayor.get()) ? archeologist.get() : mayor.get();
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

        tui.clearScreen();
        string choice;
        while (currentHero->getRemainingActions() > 0) {
            cout << "Press Enter to continue..."; 
            cin.get();

            tui.clearScreen();
            otherHero = (currentHero == mayor.get()) ? archeologist.get() : mayor.get();
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
                    cout << "Choose location (1-" << neighbors.size() << "): ";
                    cin >> locationChoice;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
                    currentHero->advance(*static_cast<Dracula*>(dracula.get()), taskBoard);
                    continue;
                } else if (choice == "D" || choice == "Defeat") {
                    currentHero->defeat(*static_cast<Dracula*>(dracula.get()), taskBoard);
                    bool draculaDead = taskBoard.isDraculaDefeated();
                    bool invisibleManDead = taskBoard.isInvisibleManDefeated();
                    if (draculaDead) {
                        dracula->getCurrentLocation()->removeCharacter("Dracula");
                        dracula->setCurrentLocation(nullptr);
                        frenzyMarker.advance(dracula.get(), invisibleMan.get());
                    }
                    if (invisibleManDead) {
                        invisibleMan->getCurrentLocation()->removeCharacter("Invisible man");
                        invisibleMan->setCurrentLocation(nullptr);
                        frenzyMarker.advance(dracula.get(), invisibleMan.get());
                    }
                    if (draculaDead && invisibleManDead) {
                        cout << "Heroes win! Both Dracula and Invisible man are defeated!" << endl;
                        gameRunning = false;
                        break;
                    }
                    continue;
                } else if (choice == "S" || choice == "Special Action") {
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
                    int perkChoice;
                    cout << "Choose perk card (1-" << perkCards.size() << "): ";
                    cin >> perkChoice;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (perkChoice > 0 && perkChoice <= static_cast<int>(perkCards.size())) {
                        otherHero = (currentHero == mayor.get()) ? archeologist.get() : mayor.get();
                        currentHero->usePerkCard(perkChoice - 1, gamemap, villagerManager, &perkDeck, static_cast<InvisibleMan*>(invisibleMan.get()), &itembag, otherHero, static_cast<Dracula*>(dracula.get()));
                    } else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                    continue;
                } else if (choice == "H" || choice == "Help") {
                    tui.showHelpMenu();
                    continue;
                } else if (choice == "E" || choice == "End Turn") {
                    break;
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
        otherHero = (currentHero == mayor.get()) ? archeologist.get() : mayor.get();
        tui.showTerrorLevelAndTurn(terrorTracker.getLevel(), 5, turnCount);
        tui.showMapWithHeroInfo(currentHero, otherHero);
        tui.showActionMenuWithVillagers(gamemap);
        tui.showItemsOnBoard(itembag, gamemap);
        tui.showMonsterStatus({dracula.get(), invisibleMan.get()}, gamemap, taskBoard);

        cout << "\n======== MONSTER PHASE ========" << endl;
        vector<string> diceResults;
        if (currentHero->shouldSkipNextMonsterPhase()) {
            cout << "Monster phase skipped due to Break of Dawn perk!\n";
            currentHero->setSkipNextMonsterPhase(false);
        } else {
            try {
                monsterManager.MonsterPhase(gamemap, itembag, static_cast<Dracula*>(dracula.get()), 
                                       static_cast<InvisibleMan*>(invisibleMan.get()), frenzyMarker, currentHero, terrorTracker,
                                       static_cast<Archeologist*>(archeologist.get()), static_cast<Mayor*>(mayor.get()), villagerManager, diceResults);
            } catch (const exception& e) {
                tui.showMessage(string("Error during monster phase: ") + e.what());
            }
        }
        tui.showDiceRoll(diceResults);
        cout << "\nEnd of Monster Phase. Press Enter to continue..."; 
        cin.get();

        tui.clearScreen();
        otherHero = (currentHero == mayor.get()) ? archeologist.get() : mayor.get();
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

        currentHero = (currentHero == mayor.get()) ? archeologist.get() : mayor.get();
        mayor->resetActions();
        archeologist->resetActions();
        turnCount++;
    }
    cout << "\n=========Game Over=========" << endl;
}

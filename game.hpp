#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include "gamestate.hpp"
#include "savemanager.hpp"
#include "tui.hpp"

class Game {
private:
    std::unique_ptr<SaveManager> saveManager;
    TUI tui;
    
    void showMainMenu();
    void startNewGame();
    void loadGame();
    void saveGame();
    void showSaveSlots();
    void showDetailedSaveInfo(int slotNumber);
    void deleteSave();
    bool confirmAction(const std::string& message);
    
    void restoreGameFromState(const GameState& gameState);
    void playRestoredGame(const std::string& player1Name, const std::string& player2Name,
                         const std::string& startingPlayerName, const std::string& otherPlayerName,
                         const std::string& startingPlayerHero, const std::string& otherPlayerHero,
                         int player1GarlicTime, int player2GarlicTime, int turnCount,
                         TerrorTracker& terrorTracker, bool gameRunning, Hero* currentHero,
                         Hero* otherHero, std::unique_ptr<Monster>& dracula,
                         std::unique_ptr<Monster>& invisibleMan, VillagerManager& villagerManager,
                         ItemBag& itemBag, Map& gamemap, TaskBoard& taskBoard,
                         MonsterManager& monsterManager, PerkDeck& perkDeck,
                         FrenzyMarker& frenzyMarker, unique_ptr<Hero>& Archeologist, 
                         unique_ptr<Hero>& Mayor, unique_ptr<Hero>& Courier, unique_ptr<Hero>& Scientist);
    
    void saveCurrentGame(const std::string& player1Name, const std::string& player2Name,
                        const std::string& startingPlayerName, const std::string& otherPlayerName,
                        const std::string& startingPlayerHero, const std::string& otherPlayerHero,
                        int player1GarlicTime, int player2GarlicTime, int turnCount,
                        const TerrorTracker& terrorTracker, bool gameRunning, Hero* currentHero,
                        Hero* otherHero, const std::unique_ptr<Monster>& dracula,
                        const std::unique_ptr<Monster>& invisibleMan, const VillagerManager& villagerManager,
                        const ItemBag& itemBag, const Map& gamemap, const TaskBoard& taskBoard,
                        const MonsterManager& monsterManager, const PerkDeck& perkDeck,
                        const FrenzyMarker& frenzyMarker);

public:
    Game();
    void play();
    void runMainMenu();
};

#endif
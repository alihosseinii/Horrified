#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include "hero.hpp"
#include "monster.hpp"
#include "villager.hpp"
#include "item.hpp"
#include "taskboard.hpp"
#include "terrorteracker.hpp"
#include "frenzymarker.hpp"
#include "perkdeck.hpp"
#include "monstermanager.hpp"

class Map;
class VillagerManager;
class ItemBag;

struct HeroState {
    std::string playerName;
    std::string heroName;
    std::string currentLocationName;
    int maxActions;
    int remainingActions;
    bool skipNextMonsterPhase;
    std::vector<Item> items;
    std::vector<PerkCard> perkCards;
};

struct MonsterState {
    std::string monsterName;
    std::string currentLocationName;
    bool isAlive;
};

struct VillagerState {
    std::string villagerName;
    std::string currentLocationName;
};

struct ItemState {
    std::string itemName;
    ItemColor color;
    int power;
    std::string locationName;
};

struct MapLocationState {
    std::string locationName;
    std::vector<std::string> characters;
    std::vector<ItemState> items;
};

struct TaskBoardState {
    std::unordered_map<std::string, TaskStatus> draculaCoffins;
    std::unordered_map<std::string, bool> invisibleManCluesDelivered;
    TaskStatus draculaDefeat;
    TaskStatus invisibleManDefeat;
    bool invisibleManDefeated;
};

class GameState {
private:
    std::string player1Name;
    std::string player2Name;
    std::string startingPlayerName;
    std::string otherPlayerName;
    std::string startingPlayerHero;
    std::string otherPlayerHero;
    int player1GarlicTime;
    int player2GarlicTime;
    
    int turnCount;
    int terrorLevel;
    bool gameRunning;
    int currentHeroIndex;
    
    HeroState hero1State;
    HeroState hero2State;
    
    MonsterState draculaState;
    MonsterState invisibleManState;
    
    std::vector<VillagerState> villagerStates;
    
    std::vector<ItemState> itemStates;
    
    std::vector<MapLocationState> mapLocationStates;
    
    TaskBoardState taskBoardState;
    
    std::vector<MonsterCard> monsterCards;
    
    std::vector<PerkCard> perkDeckCards;
    
    int frenzyLevel;
    
    std::string saveName;
    std::string saveDate;
    int saveVersion;

public:
    GameState();
    
    void saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
    
    void setPlayerInfo(const std::string& p1Name, const std::string& p2Name, 
                      const std::string& startPlayer, const std::string& otherPlayer,
                      const std::string& startHero, const std::string& otherHero,
                      int p1Garlic, int p2Garlic);
    
    void setGameState(int turn, int terror, bool running);
    void setCurrentHeroIndex(int heroIndex);
    
    void setHeroState(const Hero* hero, bool isFirstHero);
    void setMonsterState(const Monster* monster, bool isDracula);
    void setVillagerStates(const VillagerManager& villagerManager);
    void setItemStates(const ItemBag& itemBag, const Map& map);
    void setMapState(const Map& map);
    void setTaskBoardState(const TaskBoard& taskBoard);
    void setMonsterManagerState(const MonsterManager& monsterManager);
    void setPerkDeckState(const PerkDeck& perkDeck);
    void setFrenzyMarkerState(const FrenzyMarker& frenzyMarker);
    
    void restorePlayerInfo(std::string& p1Name, std::string& p2Name, 
                          std::string& startPlayer, std::string& otherPlayer,
                          std::string& startHero, std::string& otherHero,
                          int& p1Garlic, int& p2Garlic) const;
    
    void restoreGameState(int& turn, int& terror, bool& running) const;
    int getCurrentHeroIndex() const;
    
    HeroState getHeroState(bool isFirstHero) const;
    MonsterState getMonsterState(bool isDracula) const;
    std::vector<VillagerState> getVillagerStates() const;
    std::vector<ItemState> getItemStates() const;
    std::vector<MapLocationState> getMapLocationStates() const;
    TaskBoardState getTaskBoardState() const;
    std::vector<MonsterCard> getMonsterCards() const;
    std::vector<PerkCard> getPerkDeckCards() const;
    int getFrenzyLevel() const;
    
    void setSaveMetadata(const std::string& name, const std::string& date, int version);
    std::string getSaveName() const;
    std::string getSaveDate() const;
    int getSaveVersion() const;
};

#endif 
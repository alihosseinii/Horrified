#include "gamestate.hpp"
#include "map.hpp"
#include "villagermanager.hpp"
#include "item.hpp"
#include "hero.hpp"
#include "monster.hpp"
#include "taskboard.hpp"
#include "monstermanager.hpp"
#include "perkdeck.hpp"
#include "frenzymarker.hpp"
#include "perkcard.hpp"
#include "monstercard.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

GameState::GameState() : saveVersion(1) {
    turnCount = 1;
    terrorLevel = 0;
    gameRunning = true;
    currentHeroIndex = 0;
    frenzyLevel = 0;
}

void GameState::saveToFile(const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Could not open file for writing: " + filename);
    }
    
    file.write(reinterpret_cast<const char*>(&saveVersion), sizeof(saveVersion));
    
    // player info
    size_t p1NameSize = player1Name.size();
    file.write(reinterpret_cast<const char*>(&p1NameSize), sizeof(p1NameSize));
    file.write(player1Name.c_str(), p1NameSize);
    
    size_t p2NameSize = player2Name.size();
    file.write(reinterpret_cast<const char*>(&p2NameSize), sizeof(p2NameSize));
    file.write(player2Name.c_str(), p2NameSize);
    
    size_t startPlayerSize = startingPlayerName.size();
    file.write(reinterpret_cast<const char*>(&startPlayerSize), sizeof(startPlayerSize));
    file.write(startingPlayerName.c_str(), startPlayerSize);
    
    size_t otherPlayerSize = otherPlayerName.size();
    file.write(reinterpret_cast<const char*>(&otherPlayerSize), sizeof(otherPlayerSize));
    file.write(otherPlayerName.c_str(), otherPlayerSize);
    
    size_t startHeroSize = startingPlayerHero.size();
    file.write(reinterpret_cast<const char*>(&startHeroSize), sizeof(startHeroSize));
    file.write(startingPlayerHero.c_str(), startHeroSize);
    
    size_t otherHeroSize = otherPlayerHero.size();
    file.write(reinterpret_cast<const char*>(&otherHeroSize), sizeof(otherHeroSize));
    file.write(otherPlayerHero.c_str(), otherHeroSize);
    
    file.write(reinterpret_cast<const char*>(&player1GarlicTime), sizeof(player1GarlicTime));
    file.write(reinterpret_cast<const char*>(&player2GarlicTime), sizeof(player2GarlicTime));
    
    // game state
    file.write(reinterpret_cast<const char*>(&turnCount), sizeof(turnCount));
    file.write(reinterpret_cast<const char*>(&terrorLevel), sizeof(terrorLevel));
    file.write(reinterpret_cast<const char*>(&gameRunning), sizeof(gameRunning));
    file.write(reinterpret_cast<const char*>(&currentHeroIndex), sizeof(currentHeroIndex));
    
    // hero 1
    size_t hero1PlayerSize = hero1State.playerName.size();
    file.write(reinterpret_cast<const char*>(&hero1PlayerSize), sizeof(hero1PlayerSize));
    file.write(hero1State.playerName.c_str(), hero1PlayerSize);
    
    size_t hero1NameSize = hero1State.heroName.size();
    file.write(reinterpret_cast<const char*>(&hero1NameSize), sizeof(hero1NameSize));
    file.write(hero1State.heroName.c_str(), hero1NameSize);
    
    size_t hero1LocSize = hero1State.currentLocationName.size();
    file.write(reinterpret_cast<const char*>(&hero1LocSize), sizeof(hero1LocSize));
    file.write(hero1State.currentLocationName.c_str(), hero1LocSize);
    
    file.write(reinterpret_cast<const char*>(&hero1State.maxActions), sizeof(hero1State.maxActions));
    file.write(reinterpret_cast<const char*>(&hero1State.remainingActions), sizeof(hero1State.remainingActions));
    file.write(reinterpret_cast<const char*>(&hero1State.skipNextMonsterPhase), sizeof(hero1State.skipNextMonsterPhase));
    
    // hero 1 items
    size_t hero1ItemsSize = hero1State.items.size();
    file.write(reinterpret_cast<const char*>(&hero1ItemsSize), sizeof(hero1ItemsSize));
    for (const auto& item : hero1State.items) {
        size_t itemNameSize = item.getItemName().size();
        file.write(reinterpret_cast<const char*>(&itemNameSize), sizeof(itemNameSize));
        file.write(item.getItemName().c_str(), itemNameSize);
        
        ItemColor color = item.getColor();
        file.write(reinterpret_cast<const char*>(&color), sizeof(color));
        
        int power = item.getPower();
        file.write(reinterpret_cast<const char*>(&power), sizeof(power));
    }
    
    // hero 1 perk cards
    size_t hero1PerksSize = hero1State.perkCards.size();
    file.write(reinterpret_cast<const char*>(&hero1PerksSize), sizeof(hero1PerksSize));
    for (const auto& perk : hero1State.perkCards) {
        PerkType type = perk.getType();
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    
    // hero 2
    size_t hero2PlayerSize = hero2State.playerName.size();
    file.write(reinterpret_cast<const char*>(&hero2PlayerSize), sizeof(hero2PlayerSize));
    file.write(hero2State.playerName.c_str(), hero2PlayerSize);
    
    size_t hero2NameSize = hero2State.heroName.size();
    file.write(reinterpret_cast<const char*>(&hero2NameSize), sizeof(hero2NameSize));
    file.write(hero2State.heroName.c_str(), hero2NameSize);
    
    size_t hero2LocSize = hero2State.currentLocationName.size();
    file.write(reinterpret_cast<const char*>(&hero2LocSize), sizeof(hero2LocSize));
    file.write(hero2State.currentLocationName.c_str(), hero2LocSize);
    
    file.write(reinterpret_cast<const char*>(&hero2State.maxActions), sizeof(hero2State.maxActions));
    file.write(reinterpret_cast<const char*>(&hero2State.remainingActions), sizeof(hero2State.remainingActions));
    file.write(reinterpret_cast<const char*>(&hero2State.skipNextMonsterPhase), sizeof(hero2State.skipNextMonsterPhase));
    
    // hero 2 items
    size_t hero2ItemsSize = hero2State.items.size();
    file.write(reinterpret_cast<const char*>(&hero2ItemsSize), sizeof(hero2ItemsSize));
    for (const auto& item : hero2State.items) {
        size_t itemNameSize = item.getItemName().size();
        file.write(reinterpret_cast<const char*>(&itemNameSize), sizeof(itemNameSize));
        file.write(item.getItemName().c_str(), itemNameSize);
        
        ItemColor color = item.getColor();
        file.write(reinterpret_cast<const char*>(&color), sizeof(color));
        
        int power = item.getPower();
        file.write(reinterpret_cast<const char*>(&power), sizeof(power));
    }
    
    // hero 2 perk cards
    size_t hero2PerksSize = hero2State.perkCards.size();
    file.write(reinterpret_cast<const char*>(&hero2PerksSize), sizeof(hero2PerksSize));
    for (const auto& perk : hero2State.perkCards) {
        PerkType type = perk.getType();
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    
    // monster states
    size_t draculaNameSize = draculaState.monsterName.size();
    file.write(reinterpret_cast<const char*>(&draculaNameSize), sizeof(draculaNameSize));
    file.write(draculaState.monsterName.c_str(), draculaNameSize);
    
    size_t draculaLocSize = draculaState.currentLocationName.size();
    file.write(reinterpret_cast<const char*>(&draculaLocSize), sizeof(draculaLocSize));
    file.write(draculaState.currentLocationName.c_str(), draculaLocSize);
    
    file.write(reinterpret_cast<const char*>(&draculaState.isAlive), sizeof(draculaState.isAlive));
    
    size_t invisibleNameSize = invisibleManState.monsterName.size();
    file.write(reinterpret_cast<const char*>(&invisibleNameSize), sizeof(invisibleNameSize));
    file.write(invisibleManState.monsterName.c_str(), invisibleNameSize);
    
    size_t invisibleLocSize = invisibleManState.currentLocationName.size();
    file.write(reinterpret_cast<const char*>(&invisibleLocSize), sizeof(invisibleLocSize));
    file.write(invisibleManState.currentLocationName.c_str(), invisibleLocSize);
    
    file.write(reinterpret_cast<const char*>(&invisibleManState.isAlive), sizeof(invisibleManState.isAlive));
    
    // villager states
    size_t villagerCount = villagerStates.size();
    file.write(reinterpret_cast<const char*>(&villagerCount), sizeof(villagerCount));
    for (const auto& villager : villagerStates) {
        size_t villagerNameSize = villager.villagerName.size();
        file.write(reinterpret_cast<const char*>(&villagerNameSize), sizeof(villagerNameSize));
        file.write(villager.villagerName.c_str(), villagerNameSize);
        
        size_t villagerLocSize = villager.currentLocationName.size();
        file.write(reinterpret_cast<const char*>(&villagerLocSize), sizeof(villagerLocSize));
        file.write(villager.currentLocationName.c_str(), villagerLocSize);
    }
    
    // item states
    size_t itemCount = itemStates.size();
    file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
    for (const auto& item : itemStates) {
        size_t itemNameSize = item.itemName.size();
        file.write(reinterpret_cast<const char*>(&itemNameSize), sizeof(itemNameSize));
        file.write(item.itemName.c_str(), itemNameSize);
        
        file.write(reinterpret_cast<const char*>(&item.color), sizeof(item.color));
        file.write(reinterpret_cast<const char*>(&item.power), sizeof(item.power));
        
        size_t itemLocSize = item.locationName.size();
        file.write(reinterpret_cast<const char*>(&itemLocSize), sizeof(itemLocSize));
        file.write(item.locationName.c_str(), itemLocSize);
    }
    
    // map location states
    size_t mapLocationCount = mapLocationStates.size();
    file.write(reinterpret_cast<const char*>(&mapLocationCount), sizeof(mapLocationCount));
    for (const auto& locationState : mapLocationStates) {
        size_t locNameSize = locationState.locationName.size();
        file.write(reinterpret_cast<const char*>(&locNameSize), sizeof(locNameSize));
        file.write(locationState.locationName.c_str(), locNameSize);
        
        size_t charCount = locationState.characters.size();
        file.write(reinterpret_cast<const char*>(&charCount), sizeof(charCount));
        for (const auto& character : locationState.characters) {
            size_t charNameSize = character.size();
            file.write(reinterpret_cast<const char*>(&charNameSize), sizeof(charNameSize));
            file.write(character.c_str(), charNameSize);
        }
        
        size_t itemCount = locationState.items.size();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        for (const auto& item : locationState.items) {
            size_t itemNameSize = item.itemName.size();
            file.write(reinterpret_cast<const char*>(&itemNameSize), sizeof(itemNameSize));
            file.write(item.itemName.c_str(), itemNameSize);
            
            file.write(reinterpret_cast<const char*>(&item.color), sizeof(item.color));
            file.write(reinterpret_cast<const char*>(&item.power), sizeof(item.power));
            
            size_t itemLocSize = item.locationName.size();
            file.write(reinterpret_cast<const char*>(&itemLocSize), sizeof(itemLocSize));
            file.write(item.locationName.c_str(), itemLocSize);
        }
    }
    
    // task board state
    size_t coffinCount = taskBoardState.draculaCoffins.size();
    file.write(reinterpret_cast<const char*>(&coffinCount), sizeof(coffinCount));
    for (const auto& [location, status] : taskBoardState.draculaCoffins) {
        size_t locSize = location.size();
        file.write(reinterpret_cast<const char*>(&locSize), sizeof(locSize));
        file.write(location.c_str(), locSize);
        
        file.write(reinterpret_cast<const char*>(&status.currentStrength), sizeof(status.currentStrength));
        file.write(reinterpret_cast<const char*>(&status.completed), sizeof(status.completed));
    }
    
    size_t clueCount = taskBoardState.invisibleManCluesDelivered.size();
    file.write(reinterpret_cast<const char*>(&clueCount), sizeof(clueCount));
    for (const auto& [location, delivered] : taskBoardState.invisibleManCluesDelivered) {
        size_t locSize = location.size();
        file.write(reinterpret_cast<const char*>(&locSize), sizeof(locSize));
        file.write(location.c_str(), locSize);
        
        file.write(reinterpret_cast<const char*>(&delivered), sizeof(delivered));
    }
    
    file.write(reinterpret_cast<const char*>(&taskBoardState.draculaDefeat.currentStrength), sizeof(taskBoardState.draculaDefeat.currentStrength));
    file.write(reinterpret_cast<const char*>(&taskBoardState.draculaDefeat.completed), sizeof(taskBoardState.draculaDefeat.completed));
    
    file.write(reinterpret_cast<const char*>(&taskBoardState.invisibleManDefeat.currentStrength), sizeof(taskBoardState.invisibleManDefeat.currentStrength));
    file.write(reinterpret_cast<const char*>(&taskBoardState.invisibleManDefeat.completed), sizeof(taskBoardState.invisibleManDefeat.completed));
    
    file.write(reinterpret_cast<const char*>(&taskBoardState.invisibleManDefeated), sizeof(taskBoardState.invisibleManDefeated));
    
    // monster cards
    size_t monsterCardCount = monsterCards.size();
    file.write(reinterpret_cast<const char*>(&monsterCardCount), sizeof(monsterCardCount));
    for (const auto& card : monsterCards) {
        size_t cardNameSize = card.getName().size();
        file.write(reinterpret_cast<const char*>(&cardNameSize), sizeof(cardNameSize));
        file.write(card.getName().c_str(), cardNameSize);
            
        size_t eventTextSize = card.getEventText().size();
        file.write(reinterpret_cast<const char*>(&eventTextSize), sizeof(eventTextSize));
        file.write(card.getEventText().c_str(), eventTextSize);
        
        int itemCount = card.getItemCount();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        
        const auto& strikes = card.getStrikeList();
        size_t strikeCount = strikes.size();
        file.write(reinterpret_cast<const char*>(&strikeCount), sizeof(strikeCount));
        
        for (const auto& strike : strikes) {
            MonsterType monsterType = strike.monster;
            file.write(reinterpret_cast<const char*>(&monsterType), sizeof(monsterType));
            
            int moveCount = strike.moveCount;
            file.write(reinterpret_cast<const char*>(&moveCount), sizeof(moveCount));
            
            int diceCount = strike.diceCount;
            file.write(reinterpret_cast<const char*>(&diceCount), sizeof(diceCount));
        }
     }
    
    // perk deck cards
    size_t perkCardCount = perkDeckCards.size();
    file.write(reinterpret_cast<const char*>(&perkCardCount), sizeof(perkCardCount));
    for (const auto& perk : perkDeckCards) {
        PerkType type = perk.getType();
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }
    
    // frenzy level
    file.write(reinterpret_cast<const char*>(&frenzyLevel), sizeof(frenzyLevel));
    
    size_t saveNameSize = saveName.size();
    file.write(reinterpret_cast<const char*>(&saveNameSize), sizeof(saveNameSize));
    file.write(saveName.c_str(), saveNameSize);
    
    size_t saveDateSize = saveDate.size();
    file.write(reinterpret_cast<const char*>(&saveDateSize), sizeof(saveDateSize));
    file.write(saveDate.c_str(), saveDateSize);
    
    file.close();
}

bool GameState::loadFromFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        file.read(reinterpret_cast<char*>(&saveVersion), sizeof(saveVersion));
        
        // player info
        size_t p1NameSize;
        file.read(reinterpret_cast<char*>(&p1NameSize), sizeof(p1NameSize));
        player1Name.resize(p1NameSize);
        file.read(&player1Name[0], p1NameSize);
        
        size_t p2NameSize;
        file.read(reinterpret_cast<char*>(&p2NameSize), sizeof(p2NameSize));
        player2Name.resize(p2NameSize);
        file.read(&player2Name[0], p2NameSize);
        
        size_t startPlayerSize;
        file.read(reinterpret_cast<char*>(&startPlayerSize), sizeof(startPlayerSize));
        startingPlayerName.resize(startPlayerSize);
        file.read(&startingPlayerName[0], startPlayerSize);
        
        size_t otherPlayerSize;
        file.read(reinterpret_cast<char*>(&otherPlayerSize), sizeof(otherPlayerSize));
        otherPlayerName.resize(otherPlayerSize);
        file.read(&otherPlayerName[0], otherPlayerSize);
        
        size_t startHeroSize;
        file.read(reinterpret_cast<char*>(&startHeroSize), sizeof(startHeroSize));
        startingPlayerHero.resize(startHeroSize);
        file.read(&startingPlayerHero[0], startHeroSize);
        
        size_t otherHeroSize;
        file.read(reinterpret_cast<char*>(&otherHeroSize), sizeof(otherHeroSize));
        otherPlayerHero.resize(otherHeroSize);
        file.read(&otherPlayerHero[0], otherHeroSize);
        
        file.read(reinterpret_cast<char*>(&player1GarlicTime), sizeof(player1GarlicTime));
        file.read(reinterpret_cast<char*>(&player2GarlicTime), sizeof(player2GarlicTime));
        
        // game state
        file.read(reinterpret_cast<char*>(&turnCount), sizeof(turnCount));
        file.read(reinterpret_cast<char*>(&terrorLevel), sizeof(terrorLevel));
        file.read(reinterpret_cast<char*>(&gameRunning), sizeof(gameRunning));
        file.read(reinterpret_cast<char*>(&currentHeroIndex), sizeof(currentHeroIndex));
        
        // Hero 1
        size_t hero1PlayerSize;
        file.read(reinterpret_cast<char*>(&hero1PlayerSize), sizeof(hero1PlayerSize));
        hero1State.playerName.resize(hero1PlayerSize);
        file.read(&hero1State.playerName[0], hero1PlayerSize);
        
        size_t hero1NameSize;
        file.read(reinterpret_cast<char*>(&hero1NameSize), sizeof(hero1NameSize));
        hero1State.heroName.resize(hero1NameSize);
        file.read(&hero1State.heroName[0], hero1NameSize);
        
        size_t hero1LocSize;
        file.read(reinterpret_cast<char*>(&hero1LocSize), sizeof(hero1LocSize));
        hero1State.currentLocationName.resize(hero1LocSize);
        file.read(&hero1State.currentLocationName[0], hero1LocSize);
        
        file.read(reinterpret_cast<char*>(&hero1State.maxActions), sizeof(hero1State.maxActions));
        file.read(reinterpret_cast<char*>(&hero1State.remainingActions), sizeof(hero1State.remainingActions));
        file.read(reinterpret_cast<char*>(&hero1State.skipNextMonsterPhase), sizeof(hero1State.skipNextMonsterPhase));
        
        // Hero 1 items
        size_t hero1ItemsSize;
        file.read(reinterpret_cast<char*>(&hero1ItemsSize), sizeof(hero1ItemsSize));
        hero1State.items.clear();
        for (size_t i = 0; i < hero1ItemsSize; ++i) {
            size_t itemNameSize;
            file.read(reinterpret_cast<char*>(&itemNameSize), sizeof(itemNameSize));
            string itemName;
            itemName.resize(itemNameSize);
            file.read(&itemName[0], itemNameSize);
            
            ItemColor color;
            file.read(reinterpret_cast<char*>(&color), sizeof(color));
            
            int power;
            file.read(reinterpret_cast<char*>(&power), sizeof(power));
            
            hero1State.items.emplace_back(itemName, color, power, nullptr);
        }
        
        // Hero 1 perk cards
        size_t hero1PerksSize;
        file.read(reinterpret_cast<char*>(&hero1PerksSize), sizeof(hero1PerksSize));
        hero1State.perkCards.clear();
        for (size_t i = 0; i < hero1PerksSize; ++i) {
            PerkType type;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            
            hero1State.perkCards.emplace_back(type);
        }
        
        // Hero 2
        size_t hero2PlayerSize;
        file.read(reinterpret_cast<char*>(&hero2PlayerSize), sizeof(hero2PlayerSize));
        hero2State.playerName.resize(hero2PlayerSize);
        file.read(&hero2State.playerName[0], hero2PlayerSize);
        
        size_t hero2NameSize;
        file.read(reinterpret_cast<char*>(&hero2NameSize), sizeof(hero2NameSize));
        hero2State.heroName.resize(hero2NameSize);
        file.read(&hero2State.heroName[0], hero2NameSize);
        
        size_t hero2LocSize;
        file.read(reinterpret_cast<char*>(&hero2LocSize), sizeof(hero2LocSize));
        hero2State.currentLocationName.resize(hero2LocSize);
        file.read(&hero2State.currentLocationName[0], hero2LocSize);
        
        file.read(reinterpret_cast<char*>(&hero2State.maxActions), sizeof(hero2State.maxActions));
        file.read(reinterpret_cast<char*>(&hero2State.remainingActions), sizeof(hero2State.remainingActions));
        file.read(reinterpret_cast<char*>(&hero2State.skipNextMonsterPhase), sizeof(hero2State.skipNextMonsterPhase));
        
        // Hero 2 items
        size_t hero2ItemsSize;
        file.read(reinterpret_cast<char*>(&hero2ItemsSize), sizeof(hero2ItemsSize));
        hero2State.items.clear();
        for (size_t i = 0; i < hero2ItemsSize; ++i) {
            size_t itemNameSize;
            file.read(reinterpret_cast<char*>(&itemNameSize), sizeof(itemNameSize));
            string itemName;
            itemName.resize(itemNameSize);
            file.read(&itemName[0], itemNameSize);
            
            ItemColor color;
            file.read(reinterpret_cast<char*>(&color), sizeof(color));
            
            int power;
            file.read(reinterpret_cast<char*>(&power), sizeof(power));
            
            hero2State.items.emplace_back(itemName, color, power, nullptr);
        }
        
        // Hero 2 perk cards
        size_t hero2PerksSize;
        file.read(reinterpret_cast<char*>(&hero2PerksSize), sizeof(hero2PerksSize));
        hero2State.perkCards.clear();
        for (size_t i = 0; i < hero2PerksSize; ++i) {
            PerkType type;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            
            hero2State.perkCards.emplace_back(type);
        }
        
        // monster states
        size_t draculaNameSize;
        file.read(reinterpret_cast<char*>(&draculaNameSize), sizeof(draculaNameSize));
        draculaState.monsterName.resize(draculaNameSize);
        file.read(&draculaState.monsterName[0], draculaNameSize);
        
        size_t draculaLocSize;
        file.read(reinterpret_cast<char*>(&draculaLocSize), sizeof(draculaLocSize));
        draculaState.currentLocationName.resize(draculaLocSize);
        file.read(&draculaState.currentLocationName[0], draculaLocSize);
        
        file.read(reinterpret_cast<char*>(&draculaState.isAlive), sizeof(draculaState.isAlive));
        
        size_t invisibleNameSize;
        file.read(reinterpret_cast<char*>(&invisibleNameSize), sizeof(invisibleNameSize));
        invisibleManState.monsterName.resize(invisibleNameSize);
        file.read(&invisibleManState.monsterName[0], invisibleNameSize);
        
        size_t invisibleLocSize;
        file.read(reinterpret_cast<char*>(&invisibleLocSize), sizeof(invisibleLocSize));
        invisibleManState.currentLocationName.resize(invisibleLocSize);
        file.read(&invisibleManState.currentLocationName[0], invisibleLocSize);
        
        file.read(reinterpret_cast<char*>(&invisibleManState.isAlive), sizeof(invisibleManState.isAlive));
        
        // villager states
        size_t villagerCount;
        file.read(reinterpret_cast<char*>(&villagerCount), sizeof(villagerCount));
        villagerStates.clear();
        for (size_t i = 0; i < villagerCount; ++i) {
            VillagerState villager;
            
            size_t villagerNameSize;
            file.read(reinterpret_cast<char*>(&villagerNameSize), sizeof(villagerNameSize));
            villager.villagerName.resize(villagerNameSize);
            file.read(&villager.villagerName[0], villagerNameSize);
            
            size_t villagerLocSize;
            file.read(reinterpret_cast<char*>(&villagerLocSize), sizeof(villagerLocSize));
            villager.currentLocationName.resize(villagerLocSize);
            file.read(&villager.currentLocationName[0], villagerLocSize);
            
            villagerStates.push_back(villager);
        }
        
        // item states
        size_t itemCount;
        file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
        itemStates.clear();
        for (size_t i = 0; i < itemCount; ++i) {
            ItemState item;
            
            size_t itemNameSize;
            file.read(reinterpret_cast<char*>(&itemNameSize), sizeof(itemNameSize));
            item.itemName.resize(itemNameSize);
            file.read(&item.itemName[0], itemNameSize);
            
            file.read(reinterpret_cast<char*>(&item.color), sizeof(item.color));
            file.read(reinterpret_cast<char*>(&item.power), sizeof(item.power));
            
            size_t itemLocSize;
            file.read(reinterpret_cast<char*>(&itemLocSize), sizeof(itemLocSize));
            item.locationName.resize(itemLocSize);
            file.read(&item.locationName[0], itemLocSize);
            
            itemStates.push_back(item);
        }
        
        // map location states
        size_t mapLocationCount;
        file.read(reinterpret_cast<char*>(&mapLocationCount), sizeof(mapLocationCount));
        mapLocationStates.clear();
        for (size_t i = 0; i < mapLocationCount; ++i) {
            MapLocationState locationState;
            
            size_t locNameSize;
            file.read(reinterpret_cast<char*>(&locNameSize), sizeof(locNameSize));
            locationState.locationName.resize(locNameSize);
            file.read(&locationState.locationName[0], locNameSize);
            
            size_t charCount;
            file.read(reinterpret_cast<char*>(&charCount), sizeof(charCount));
            locationState.characters.clear();
            for (size_t j = 0; j < charCount; ++j) {
                size_t charNameSize;
                file.read(reinterpret_cast<char*>(&charNameSize), sizeof(charNameSize));
                string character;
                character.resize(charNameSize);
                file.read(&character[0], charNameSize);
                locationState.characters.push_back(character);
            }
            
            size_t itemCount;
            file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
            locationState.items.clear();
            for (size_t j = 0; j < itemCount; ++j) {
                ItemState item;
                
                size_t itemNameSize;
                file.read(reinterpret_cast<char*>(&itemNameSize), sizeof(itemNameSize));
                item.itemName.resize(itemNameSize);
                file.read(&item.itemName[0], itemNameSize);
                
                file.read(reinterpret_cast<char*>(&item.color), sizeof(item.color));
                file.read(reinterpret_cast<char*>(&item.power), sizeof(item.power));
                
                size_t itemLocSize;
                file.read(reinterpret_cast<char*>(&itemLocSize), sizeof(itemLocSize));
                item.locationName.resize(itemLocSize);
                file.read(&item.locationName[0], itemLocSize);
                
                locationState.items.push_back(item);
            }
            
            mapLocationStates.push_back(locationState);
        }
        
        // task board state
        size_t coffinCount;
        file.read(reinterpret_cast<char*>(&coffinCount), sizeof(coffinCount));
        taskBoardState.draculaCoffins.clear();
        for (size_t i = 0; i < coffinCount; ++i) {
            size_t locSize;
            file.read(reinterpret_cast<char*>(&locSize), sizeof(locSize));
            string location;
            location.resize(locSize);
            file.read(&location[0], locSize);
            
            TaskStatus status;
            file.read(reinterpret_cast<char*>(&status.currentStrength), sizeof(status.currentStrength));
            file.read(reinterpret_cast<char*>(&status.completed), sizeof(status.completed));
            
            taskBoardState.draculaCoffins[location] = status;
        }
        
        size_t clueCount;
        file.read(reinterpret_cast<char*>(&clueCount), sizeof(clueCount));
        taskBoardState.invisibleManCluesDelivered.clear();
        for (size_t i = 0; i < clueCount; ++i) {
            size_t locSize;
            file.read(reinterpret_cast<char*>(&locSize), sizeof(locSize));
            string location;
            location.resize(locSize);
            file.read(&location[0], locSize);
            
            bool delivered;
            file.read(reinterpret_cast<char*>(&delivered), sizeof(delivered));
            
            taskBoardState.invisibleManCluesDelivered[location] = delivered;
        }
        
        file.read(reinterpret_cast<char*>(&taskBoardState.draculaDefeat.currentStrength), sizeof(taskBoardState.draculaDefeat.currentStrength));
        file.read(reinterpret_cast<char*>(&taskBoardState.draculaDefeat.completed), sizeof(taskBoardState.draculaDefeat.completed));
        
        file.read(reinterpret_cast<char*>(&taskBoardState.invisibleManDefeat.currentStrength), sizeof(taskBoardState.invisibleManDefeat.currentStrength));
        file.read(reinterpret_cast<char*>(&taskBoardState.invisibleManDefeat.completed), sizeof(taskBoardState.invisibleManDefeat.completed));
        
        file.read(reinterpret_cast<char*>(&taskBoardState.invisibleManDefeated), sizeof(taskBoardState.invisibleManDefeated));
        
        // monster cards
        size_t monsterCardCount;
        file.read(reinterpret_cast<char*>(&monsterCardCount), sizeof(monsterCardCount));
        monsterCards.clear();
        for (size_t i = 0; i < monsterCardCount; ++i) {
            size_t cardNameSize;
            file.read(reinterpret_cast<char*>(&cardNameSize), sizeof(cardNameSize));
            string cardName;
            cardName.resize(cardNameSize);
            file.read(&cardName[0], cardNameSize);
            
            size_t eventTextSize;
            file.read(reinterpret_cast<char*>(&eventTextSize), sizeof(eventTextSize));
            string eventText;
            eventText.resize(eventTextSize);
            file.read(&eventText[0], eventTextSize);
            
            int itemCount;
            file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
            
            size_t strikeCount;
            file.read(reinterpret_cast<char*>(&strikeCount), sizeof(strikeCount));
            vector<Strike> strikes;
            for (size_t j = 0; j < strikeCount; ++j) {
                MonsterType monsterType;
                file.read(reinterpret_cast<char*>(&monsterType), sizeof(monsterType));
                
                int moveCount;
                file.read(reinterpret_cast<char*>(&moveCount), sizeof(moveCount));
                
                int diceCount;
                file.read(reinterpret_cast<char*>(&diceCount), sizeof(diceCount));
                
                strikes.emplace_back(monsterType, moveCount, diceCount);
            }
            
            monsterCards.emplace_back(cardName, itemCount, eventText, strikes);
        }
        
        // perk deck cards
        size_t perkCardCount;
        file.read(reinterpret_cast<char*>(&perkCardCount), sizeof(perkCardCount));
        perkDeckCards.clear();
        for (size_t i = 0; i < perkCardCount; ++i) {
            PerkType type;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            
            perkDeckCards.emplace_back(type);
        }
        
        // frenzy level
        file.read(reinterpret_cast<char*>(&frenzyLevel), sizeof(frenzyLevel));
        
        size_t saveNameSize;
        file.read(reinterpret_cast<char*>(&saveNameSize), sizeof(saveNameSize));
        saveName.resize(saveNameSize);
        file.read(&saveName[0], saveNameSize);
        
        size_t saveDateSize;
        file.read(reinterpret_cast<char*>(&saveDateSize), sizeof(saveDateSize));
        saveDate.resize(saveDateSize);
        file.read(&saveDate[0], saveDateSize);
        
        file.close();
        return true;
    } catch (...) {
        file.close();
        return false;
    }
}

void GameState::setPlayerInfo(const string& p1Name, const string& p2Name, 
                             const string& startPlayer, const string& otherPlayer,
                             const string& startHero, const string& otherHero,
                             int p1Garlic, int p2Garlic) {
    player1Name = p1Name;
    player2Name = p2Name;
    startingPlayerName = startPlayer;
    otherPlayerName = otherPlayer;
    startingPlayerHero = startHero;
    otherPlayerHero = otherHero;
    player1GarlicTime = p1Garlic;
    player2GarlicTime = p2Garlic;
}

void GameState::setGameState(int turn, int terror, bool running) {
    turnCount = turn;
    terrorLevel = terror;
    gameRunning = running;
}

void GameState::setCurrentHeroIndex(int heroIndex) {
    currentHeroIndex = heroIndex;
}

void GameState::setHeroState(const Hero* hero, bool isFirstHero) {
    HeroState& state = isFirstHero ? hero1State : hero2State;
    state.playerName = hero->getPlayerName();
    state.heroName = hero->getHeroName();
    state.currentLocationName = hero->getCurrentLocation()->getName();
    state.maxActions = hero->getMaxActions();
    state.remainingActions = hero->getRemainingActions();
    state.skipNextMonsterPhase = hero->shouldSkipNextMonsterPhase();
    state.items = hero->getItems();
    state.perkCards = hero->getPerkCards();
}

void GameState::setMonsterState(const Monster* monster, bool isDracula) {
    MonsterState& state = isDracula ? draculaState : invisibleManState;
    
    if (monster == nullptr) {
        // Monster is dead/defeated
        state.monsterName = isDracula ? "Dracula" : "Invisible man";
        state.currentLocationName = "";
        state.isAlive = false;
    } else {
        // Monster is alive
        state.monsterName = monster->getMonsterName();
        state.currentLocationName = monster->getCurrentLocation() ? monster->getCurrentLocation()->getName() : "";
        state.isAlive = monster->getCurrentLocation() != nullptr;
    }
}

void GameState::setVillagerStates(const VillagerManager& villagerManager) {
    villagerStates.clear();
    const auto& villagers = villagerManager.getAllVillagers();
    for (const auto& [name, villager] : villagers) {
        VillagerState state;
        state.villagerName = name;
        state.currentLocationName = villager->getCurrentLocation()->getName();
        villagerStates.push_back(state);
    }
}

void GameState::setItemStates(const ItemBag& itemBag, const Map& map) {
    itemStates.clear();
    
    const auto& items = itemBag.getItems();
    for (const auto& item : items) {
        ItemState state;
        state.itemName = item.getItemName();
        state.color = item.getColor();
        state.power = item.getPower();
        state.locationName = item.getLocation() ? item.getLocation()->getName() : "";
        itemStates.push_back(state);
    }
}

void GameState::setMapState(const Map& map) {
    mapLocationStates.clear();
    
    for (const auto& [locationName, location] : map.locations) {
        MapLocationState locationState;
        locationState.locationName = locationName;
        
        locationState.characters = location->getCharacters();
        
        const auto& locationItems = location->getItems();
        for (const auto& item : locationItems) {
            ItemState itemState;
            itemState.itemName = item.getItemName();
            itemState.color = item.getColor();
            itemState.power = item.getPower();
            itemState.locationName = locationName;
            locationState.items.push_back(itemState);
        }
        
        mapLocationStates.push_back(locationState);
    }
}

void GameState::setTaskBoardState(const TaskBoard& taskBoard) {
    taskBoardState.draculaCoffins = taskBoard.getDraculaCoffins();
    taskBoardState.invisibleManCluesDelivered = taskBoard.getInvisibleManCluesDelivered();
    taskBoardState.draculaDefeat = taskBoard.getDraculaDefeat();
    taskBoardState.invisibleManDefeat = taskBoard.getInvisibleManDefeat();
    taskBoardState.invisibleManDefeated = taskBoard.getInvisibleManDefeated();
}

void GameState::setMonsterManagerState(const MonsterManager& monsterManager) {
    monsterCards = monsterManager.getCards();
}

void GameState::setPerkDeckState(const PerkDeck& perkDeck) {
    perkDeckCards = perkDeck.getCards();
}

void GameState::setFrenzyMarkerState(const FrenzyMarker& frenzyMarker) {
    frenzyLevel = frenzyMarker.getFrenzyLevel();
}

void GameState::restorePlayerInfo(string& p1Name, string& p2Name, 
                                 string& startPlayer, string& otherPlayer,
                                 string& startHero, string& otherHero,
                                 int& p1Garlic, int& p2Garlic) const {
    p1Name = player1Name;
    p2Name = player2Name;
    startPlayer = startingPlayerName;
    otherPlayer = otherPlayerName;
    startHero = startingPlayerHero;
    otherHero = otherPlayerHero;
    p1Garlic = player1GarlicTime;
    p2Garlic = player2GarlicTime;
}

void GameState::restoreGameState(int& turn, int& terror, bool& running) const {
    turn = turnCount;
    terror = terrorLevel;
    running = gameRunning;
}

int GameState::getCurrentHeroIndex() const {
    return currentHeroIndex;
}

HeroState GameState::getHeroState(bool isFirstHero) const {
    return isFirstHero ? hero1State : hero2State;
}

MonsterState GameState::getMonsterState(bool isDracula) const {
    return isDracula ? draculaState : invisibleManState;
}

vector<VillagerState> GameState::getVillagerStates() const {
    return villagerStates;
}

vector<ItemState> GameState::getItemStates() const {
    return itemStates;
}

vector<MapLocationState> GameState::getMapLocationStates() const {
    return mapLocationStates;
}

TaskBoardState GameState::getTaskBoardState() const {
    return taskBoardState;
}

vector<MonsterCard> GameState::getMonsterCards() const {
    return monsterCards;
}

vector<PerkCard> GameState::getPerkDeckCards() const {
    return perkDeckCards;
}

int GameState::getFrenzyLevel() const {
    return frenzyLevel;
}

void GameState::setSaveMetadata(const string& name, const string& date, int version) {
    saveName = name;
    saveDate = date;
    saveVersion = version;
}

string GameState::getSaveName() const {
    return saveName;
}

string GameState::getSaveDate() const {
    return saveDate;
}

int GameState::getSaveVersion() const {
    return saveVersion;
}
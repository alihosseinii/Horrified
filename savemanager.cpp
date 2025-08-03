#include "savemanager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "gamestate.hpp"

using namespace std;

SaveManager::SaveManager() {
    saveDirectory = "saves";
    
    if (!filesystem::exists(saveDirectory)) {
        filesystem::create_directory(saveDirectory);
    }
    
    initializeSaveSlots();
    refreshSaveSlots();
}

void SaveManager::initializeSaveSlots() {
    saveSlots.clear();
    for (int i = 1; i <= MAX_SAVES; ++i) {
        SaveSlot slot;
        slot.slotNumber = i;
        slot.hasSave = false;
        saveSlots.push_back(slot);
    }
}

bool SaveManager::saveGame(const GameState& gameState, int slotNumber, const string& saveName) {
    if (slotNumber < 1 || slotNumber > MAX_SAVES) {
        return false;
    }
    
    try {
        string filename = getSaveFileName(slotNumber);
        
        GameState stateToSave = gameState;
        stateToSave.setSaveMetadata(saveName, getCurrentDateTime(), 1);
        
        stateToSave.saveToFile(filename);
        
        refreshSaveSlots();
        
        return true;
    } catch (const exception& e) {
        cerr << "Error saving game: " << e.what() << endl;
        return false;
    }
}

bool SaveManager::loadGame(GameState& gameState, int slotNumber) {
    if (slotNumber < 1 || slotNumber > MAX_SAVES) {
        return false;
    }
    
    if (!hasSave(slotNumber)) {
        return false;
    }
    
    try {
        string filename = getSaveFileName(slotNumber);
        return gameState.loadFromFile(filename);
    } catch (const exception& e) {
        cerr << "Error loading game: " << e.what() << endl;
        return false;
    }
}

bool SaveManager::deleteSave(int slotNumber) {
    if (slotNumber < 1 || slotNumber > MAX_SAVES) {
        return false;
    }
    
    try {
        string filename = getSaveFileName(slotNumber);
        if (filesystem::exists(filename)) {
            filesystem::remove(filename);
            refreshSaveSlots();
            return true;
        }
        return false;
    } catch (const exception& e) {
        cerr << "Error deleting save: " << e.what() << endl;
        return false;
    }
}

vector<SaveSlot> SaveManager::getSaveSlots() const {
    return saveSlots;
}

SaveSlot SaveManager::getSaveSlot(int slotNumber) const {
    if (slotNumber >= 1 && slotNumber <= MAX_SAVES) {
        return saveSlots[slotNumber - 1];
    }
    
    SaveSlot emptySlot;
    emptySlot.slotNumber = slotNumber;
    emptySlot.hasSave = false;
    return emptySlot;
}

bool SaveManager::hasSave(int slotNumber) const {
    if (slotNumber < 1 || slotNumber > MAX_SAVES) {
        return false;
    }
    return saveSlots[slotNumber - 1].hasSave;
}

string SaveManager::getSaveFileName(int slotNumber) const {
    return saveDirectory + "/save_" + to_string(slotNumber) + ".bin";
}

string SaveManager::getCurrentDateTime() const {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    auto tm = *localtime(&time_t);
    
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void SaveManager::refreshSaveSlots() {
    for (int i = 1; i <= MAX_SAVES; ++i) {
        SaveSlot& slot = saveSlots[i - 1];
        slot.slotNumber = i;
        
        string filename = getSaveFileName(i);
        if (filesystem::exists(filename)) {
            if (readSaveMetadata(i, slot)) {
                slot.hasSave = true;
            } else {
                slot.hasSave = false;
            }
        } else {
            slot.hasSave = false;
        }
    }
}

void SaveManager::showDetailedSaveInfo(int slotNumber) const {
    if (!hasSave(slotNumber)) {
        cout << "No save found in slot " << slotNumber << "." << endl;
        return;
    }
    
    try {
        GameState gameState;
        string filename = getSaveFileName(slotNumber);
        if (gameState.loadFromFile(filename)) {
            cout << "\n=== Save Slot " << slotNumber << " Details ===" << endl;
            cout << "Save Name: " << gameState.getSaveName() << endl;
            cout << "Save Date: " << gameState.getSaveDate() << endl;
            
            // hero information
            auto hero1State = gameState.getHeroState(true);
            auto hero2State = gameState.getHeroState(false);
            cout << "\nHeroes:" << endl;
            cout << "1. " << hero1State.playerName << " (" << hero1State.heroName << ") at " << hero1State.currentLocationName << endl;
            cout << "2. " << hero2State.playerName << " (" << hero2State.heroName << ") at " << hero2State.currentLocationName << endl;
            
            // monster information
            auto draculaState = gameState.getMonsterState(true);
            auto invisibleManState = gameState.getMonsterState(false);
            cout << "\nMonsters:" << endl;
            cout << "Dracula: " << (draculaState.isAlive ? "Alive at " + draculaState.currentLocationName : "Defeated") << endl;
            cout << "Invisible Man: " << (invisibleManState.isAlive ? "Alive at " + invisibleManState.currentLocationName : "Defeated") << endl;
            
            // villager information
            auto mapLocationStates = gameState.getMapLocationStates();
            cout << "\nVillagers in Map:" << endl;
            for (const auto& locationState : mapLocationStates) {
                for (const auto& character : locationState.characters) {
                    if (character != "Dracula" && character != "Invisible man" && 
                        character != hero1State.heroName && character != hero2State.heroName &&
                        character != hero1State.playerName && character != hero2State.playerName) {
                        cout << "- " << character << " at " << locationState.locationName << endl;
                    }
                }
            }
            
            // item information
            mapLocationStates = gameState.getMapLocationStates();
            cout << "\nItems in Map:" << endl;
            for (const auto& locationState : mapLocationStates) {
                for (const auto& item : locationState.items) {
                    cout << "- " << item.itemName << " (" << static_cast<int>(item.color) << ") at " << locationState.locationName << endl;
                }
            }
            
            // frenzy level
            int frenzyLevel = gameState.getFrenzyLevel();
            cout << "\nFrenzy Level: " << frenzyLevel;
            if (frenzyLevel == 1) {
                cout << " (Dracula is frenzied)" << endl;
            } else if (frenzyLevel == 2) {
                cout << " (Invisible Man is frenzied)" << endl;
            } else {
                cout << " (No monster is frenzied)" << endl;
            }
            
            int turnCount, terrorLevel;
            bool gameRunning;
            gameState.restoreGameState(turnCount, terrorLevel, gameRunning);
            
            cout << "\nTurn Count: " << turnCount << endl;
            cout << "Terror Level: " << terrorLevel << endl;
            
        } else {
            cout << "Failed to load save details from slot " << slotNumber << "." << endl;
        }
    } catch (const exception& e) {
        cout << "Error reading save details: " << e.what() << endl;
    }
}

bool SaveManager::readSaveMetadata(int slotNumber, SaveSlot& slot) const {
    try {
        GameState gameState;
        string filename = getSaveFileName(slotNumber);
        if (!gameState.loadFromFile(filename)) {
            return false;
        }
        
        slot.saveName = gameState.getSaveName();
        slot.saveDate = gameState.getSaveDate();
        
        string p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHero;
        int p1Garlic, p2Garlic;
        gameState.restorePlayerInfo(p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHero, p1Garlic, p2Garlic);
        
        slot.player1Name = p1Name;
        slot.player2Name = p2Name;
        slot.startingPlayerHero = startHero;
        slot.otherPlayerHero = otherHero;
        
        int turnCount, terrorLevel;
        bool gameRunning;
        gameState.restoreGameState(turnCount, terrorLevel, gameRunning);
        
        slot.turnCount = turnCount;
        slot.terrorLevel = terrorLevel;
        
        return true;
    } catch (...) {
        return false;
    }
} 
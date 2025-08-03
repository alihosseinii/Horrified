#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "gamestate.hpp"

struct SaveSlot {
    int slotNumber;
    std::string saveName;
    std::string saveDate;
    std::string player1Name;
    std::string player2Name;
    std::string startingPlayerHero;
    std::string otherPlayerHero;
    int turnCount;
    int terrorLevel;
    bool hasSave;
};

class SaveManager {
private:
    static const int MAX_SAVES = 5;
    std::vector<SaveSlot> saveSlots;
    std::string saveDirectory;

public:
    SaveManager();
    
    bool saveGame(const GameState& gameState, int slotNumber, const std::string& saveName);
    bool loadGame(GameState& gameState, int slotNumber);
    bool deleteSave(int slotNumber);
    
    std::vector<SaveSlot> getSaveSlots() const;
    SaveSlot getSaveSlot(int slotNumber) const;
    bool hasSave(int slotNumber) const;
    
    std::string getSaveFileName(int slotNumber) const;
    std::string getCurrentDateTime() const;
    void refreshSaveSlots();
    void showDetailedSaveInfo(int slotNumber) const;
    
private:
    void initializeSaveSlots();
    bool readSaveMetadata(int slotNumber, SaveSlot& slot) const;
};

#endif 
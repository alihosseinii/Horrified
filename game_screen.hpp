#ifndef GAME_SCREEN_HPP
#define GAME_SCREEN_HPP

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "game.hpp"
#include "map.hpp"
#include "location.hpp"
#include "item.hpp"
#include "hero.hpp"
#include "monster.hpp"
#include "villager.hpp"
#include "terrorteracker.hpp"
#include "perkcard.hpp"
#include "gamestate.hpp"
#include "savemanager.hpp"
#include "villagermanager.hpp"

struct PlayerInfo {
    std::string name;
    std::string hero;
    int garlicTime = 0;

    PlayerInfo() = default;

    PlayerInfo(const std::string& playerName, const std::string& playerHero = "", const std::string& playerLastGarlicTime = "0")
        : name(playerName), hero(playerHero), garlicTime(playerLastGarlicTime.empty() ? 0 : std::stoi(playerLastGarlicTime)) {}
};

class MonsterManager;
class PerkDeck;
class TaskBoard;
class FrenzyMarker;
class VillagerManager;
class ItemBag;

struct MapLocation {
    std::string name;
    Vector2 position;
    float radius;
    bool isClickable;
    std::vector<std::string> characters;
    std::vector<Item> items;
    bool hasCoffin;
    bool hasEvidence;
    bool coffinDestroyed;
    std::string evidenceItem;
};

struct ActionButton {
    std::string name;
    Rectangle bounds;
    bool isEnabled;
    std::function<void()> action;
    Color color;
    Color hoverColor;
};

struct ItemTooltip {
    std::string locationName;
    std::vector<Item> items;
    Vector2 position;
    bool isVisible;
    float displayTime;
    std::string name;
    std::string description;
};

enum GamePhase {
    HERO_PHASE,
    MONSTER_PHASE
};

class GameScreen {
private:
    int screenWidth = 1400;
    int screenHeight = 900;
    
    // Game state
    std::vector<PlayerInfo> players;
    std::string startingPlayer;
    int currentTurn;
    TerrorTracker terrorTracker;
    bool gameRunning;
    GamePhase currentPhase;
    Hero* currentHero;
    Hero* otherHero;
    
    // Game components
    std::unique_ptr<Map> gameMap;
    std::unique_ptr<Monster> dracula;
    std::unique_ptr<Monster> invisibleMan;
    VillagerManager villagerManager;
    ItemBag* itemBag;
    MonsterManager monsterManager;
    PerkDeck perkDeck;
    TaskBoard taskBoard;
    std::unique_ptr<FrenzyMarker> frenzyMarker;
    std::unique_ptr<SaveManager> saveManager;
    
    // Graphics & UI Colors
    Font gameFont, titleFont, largeFont;
    Color backgroundColor = {20, 20, 20, 255};
    Color textColor = {255, 255, 255, 255};
    Color titleColor = {200, 50, 50, 255};
    Color buttonColor = {60, 60, 60, 255};
    Color buttonHoverColor = {80, 80, 80, 255};
    Color dangerColor = {200, 50, 50, 255};
    Color successColor = {50, 200, 50, 255};
    
    // Map and game elements
    Texture2D mapTexture;
    bool mapLoaded = false;
    std::unordered_map<std::string, MapLocation> mapLocations;
    
    // UI Layout
    Rectangle mapArea, turnPhasePanel, heroInfoPanel, actionsPanel, evidencePanel, monsterPhasePanel, terrorTrackerArea;
    
    // Action system
    std::vector<ActionButton> actionButtons;
    std::string selectedAction, selectedLocation;
    std::vector<std::string> availableMoveLocations;
    bool showEndTurnPrompt = false;
    float endTurnPromptTimer = 0.0f;
    
    // Terror tracker
    std::vector<Rectangle> terrorTrackers;
    int currentTerrorLevel;
    
    // Monster phase elements
    Texture2D monsterCardTexture, frenziedMonsterTexture;
    bool monsterCardLoaded, frenziedMonsterTextureLoaded;
    std::string currentMonsterCard, diceResultText, drawnMonsterCardName, currentFrenziedMonster = "";
    bool showMonsterPhaseUI = false;
    std::vector<std::string> diceResults;
    Rectangle monsterCardBox, diceResultsBox, frenzyMarkerBox;
    
    // Image Textures
    std::unordered_map<std::string, Texture2D> heroImages, monsterImages, villagerImages, itemImages, perkCardImages;
    bool heroImagesLoaded, monsterImagesLoaded, villagerImagesLoaded, itemImagesLoaded, perkCardImagesLoaded, coffinImagesLoaded;
    Texture2D coffinTexture, smashedCoffinTexture;
    std::vector<std::string> coffinLocations = {"Cave", "Dungeon", "Crypt", "Graveyard"};
    
    // Help Menu
    bool helpMenuActive = false;
    Rectangle helpMenuArea, helpCloseButton;
    Rectangle helpPrevButton, helpNextButton;
    int currentHelpPage = 1;
    std::vector<Texture2D> helpPageImages;
    bool helpImagesLoaded = false;
    
    // Hero info
    int remainingActions, maxActions;
    std::vector<std::string> perkCards;
    std::vector<Item> inventory;

    bool showHeroInventoryOverlay;  
    Hero* hoveredHero;              
    Rectangle heroOverlayRect{};
    Rectangle heroOverlayContentRect{};
    float heroOverlayScroll = 0.0f;
    float heroOverlayMaxScroll = 0.0f;
    
    ItemTooltip itemTooltip;

     bool locationPopupOpen = false;
     std::string locationPopupLocationName;

    bool showConfirmationPrompt = false;
    std::string confirmationPromptText;
    std::function<void()> onConfirmYes;
    std::function<void()> onConfirmNo;
    Rectangle confirmYesButton;
    Rectangle confirmNoButton;

    // These are still needed for the move action itself
    std::string pendingMoveLocation;
    std::vector<std::string> villagersToMove;

    // Guide action variables
    bool showGuideVillagers = false;
    bool showGuideLocations = false;
    std::vector<std::shared_ptr<Villager>> guidableVillagers;
    std::vector<std::vector<std::shared_ptr<Location>>> guidableMoves;
    std::shared_ptr<Villager> selectedVillager;
    std::vector<std::shared_ptr<Location>> availableGuideLocations;
    std::string guideMessage;
    float guideMessageTimer = 0.0f;

    // Pick up action variables
    bool showPickUpItems = false;
    std::vector<Item> availableItems;
    std::string pickUpLocation;
    std::string pickUpMessage;
    float pickUpMessageTimer = 0.0f;
    bool itemWasPickedUpThisTurn;

    // Hero defense system
    bool showHeroDefense = false;
    bool isHeroDefenseChoiceActive = false; 
    std::vector<Item> heroDefenseItems;
    std::vector<int> heroDefenseItemIndices;
    std::function<void(int)> onItemSelectedIndex;
    std::function<void()> onDefenseCanceled;
    Hero* defendingHero = nullptr;
    bool showHeroDefenseYesNoOverlay;
    std::function<void(int)> onItemSelectedCallback;
    std::function<void()> onDefenseCanceledCallback;
    Rectangle defenseYesButton; 
    Rectangle defenseNoButton; 
    
    // Game message system
    struct GameMessage {
        std::string text;
        float timer;
        bool requiresAction;
        std::function<void()> onConfirm;
        std::function<void()> onCancel;
    };
    std::vector<GameMessage> gameMessages;
    bool showGameMessage = false;
    GameMessage currentGameMessage;

    // Monster phase state tracking
    struct MonsterPhaseState {
        std::unordered_map<std::string, std::string> heroLocations; 
        std::unordered_map<std::string, std::string> villagerLocations; 
        std::vector<std::string> aliveVillagers;
    };
    MonsterPhaseState preMonsterPhaseState;

    bool isGameOver = false;
    std::string gameOverMessage;
    std::string gameOverReason;
    Rectangle gameOverCloseButton;
    bool returnToMenu = false;

    bool showArcheologistLocationChoice;
    std::vector<std::shared_ptr<Location>> archeologistTargetLocations;
    std::vector<Rectangle> archeologistLocationButtons;

    bool showArcheologistItemChoice;
    std::shared_ptr<Location> archeologistChosenLocation;
    std::vector<Rectangle> archeologistItemButtons;
    Rectangle archeologistDoneButton;
    bool archeologistPickedUpItem;

    // Perk selection overlay
    bool showPerkSelection = false;
    std::vector<Rectangle> perkSelectionButtons;
    Rectangle perkCancelButton;

    // Visit from Detective location selection overlay
    bool showVisitFromDetectiveSelection = false;
    Rectangle visitFromDetectiveCancelButton;
    std::vector<Rectangle> visitFromDetectiveLocationButtons;

    // Advance/Defeat item selection overlays
    bool showAdvanceItemSelection = false;
    bool showDefeatItemSelection = false;
    std::vector<Rectangle> itemSelectionButtons;
    Rectangle itemSelectionCancelButton;
    std::string advanceDefeatAction; 
    std::string advanceDefeatTarget; 

    // Evidence display
    std::unordered_map<std::string, std::string> evidenceItems;

    bool showSaveSlots = false;
    std::vector<Rectangle> saveSlotButtons;
    Rectangle saveBackToGameButton{};
    Rectangle saveGoToMenuButton{};

public:
    GameScreen(const std::vector<PlayerInfo>& playerInfo, const std::string& startPlayer, int windowWidth = 1400, int windowHeight = 900);
    ~GameScreen();
    
    void run();
    void draw();
    void handleInput();
    void updateGame();
    bool shouldReturnToMenu() const { return returnToMenu; }

    void startHeroDefense(Hero* hero,
                          std::function<void(int)> onItemSelected,
                          std::function<void()> onCanceled);
    
    void showHeroDefenseYesNoChoice(Hero* hero, 
                                   std::function<void(int)> onItemSelected, 
                                   std::function<void()> onCanceled);
                                   void handleHeroDefenseYesNoChoice(bool useItem);
                                   void drawHeroDefenseYesNoOverlay();
                                   void handleHeroDefenseYesNoClick(Vector2 mousePos);
                                   
    void addGameMessage(const std::string& message, float duration = 3.0f); 
    void restoreFromGameState(const GameState& gameState);
    
private:
    // Initialization
    void initializeGameState();
    void initializeMap();
    void initializeLocations();
    void initializeActions();
    void initializeTerrorTracker();
    void initializeHelpMenu();
    
    // Drawing functions
    void drawMap();
    void drawLocations();
    void drawCharacters();
    void drawItems();
    void drawCoffins();
    void drawTerrorTracker();
    void drawHeroInfoPanel();
    void drawActionsPanel();
    void drawEvidencePanel();
    void drawMonsterPhasePanel();
    void drawMonsterPhaseUI();
    void drawMonsterCardBox();
    void drawDiceResultsBox();
    void drawFrenzyMarkerBox();
    void drawItemTooltip();
    void drawPhaseIndicator();
    void drawHelpMenu();
    void loadHelpImages();
    void unloadHelpImages();
    void drawHeroInventoryOverlay(); 
    void drawLocationDetailPopup();  
    void drawGuideVillagersOverlay(); 
    void drawGuideLocationsOverlay(); 
    void drawGuideMessage(); 
    void drawPickUpItemsOverlay(); 
    void drawPickUpMessage(); 
    void drawGameMessage(); 
    void drawHeroDefenseOverlay(); 
    void drawPerkSelectionOverlay(); 
    void drawVisitFromDetectiveSelectionOverlay(); 
    void drawAdvanceDefeatItemSelectionOverlay(); 
    
    // Input handling
    void handleMapClick(Vector2 mousePos);
    void handleActionClick(Vector2 mousePos);
    void handleItemHover(Vector2 mousePos);
    void handleHelpMenuClick(Vector2 mousePos);
    void handleHeroInventoryHover(Vector2 mousePos); // ADDED
    void handleMapRightClick(Vector2 mousePos);      // ADDED
    void handleGuideVillagersClick(Vector2 mousePos); // ADDED
    void handleGuideLocationsClick(Vector2 mousePos); // ADDED
    void handlePickUpItemsClick(Vector2 mousePos); // ADDED
    void handleGameMessageClick(Vector2 mousePos); // ADDED
    void handleHeroDefenseClick(Vector2 mousePos); // ADDED
    void handlePerkSelectionClick(Vector2 mousePos); // ADDED
    void handleVisitFromDetectiveSelectionClick(Vector2 mousePos); // ADDED
    void handleAdvanceDefeatItemSelectionClick(Vector2 mousePos); // ADDED
    
    // Game actions
    void executeAction(const std::string& action, const std::string& location);
    void moveHero(const std::string& location);
    void completeHeroMove(bool withVillagers);
    void startGuideAction(); // ADDED
    void selectVillagerToGuide(int villagerIndex); // ADDED
    void selectLocationToGuide(int locationIndex); // ADDED
    void cancelGuideAction(); // ADDED
    void startPickUpAction(const std::string& location); // ADDED
    void pickUpItem(int itemIndex); // ADDED
    void cancelPickUpAction(); // ADDED
    void addGameMessageWithAction(const std::string& message, std::function<void()> onConfirm, std::function<void()> onCancel); // ADDED
    void processNextGameMessage(); // ADDED
    void addImportantGameMessages(int initialTerrorLevel, const std::string& initialFrenziedMonster); // ADDED
    void showHeroDefenseChoice(const std::vector<Item>& items, std::function<void(int)> onItemSelected, std::function<void()> onDefenseCanceled); // ADDED
    void cancelHeroDefense(); // ADDED
    void capturePreMonsterPhaseState(); // ADDED
    void compareAndAddStateChangeMessages(); // ADDED
    void pickUpItem(const std::string& location);
    void advanceMonster(const std::string& location);
    void defeatMonster(const std::string& location);
    void specialAction();
    void usePerkCard();
    void openPerkSelectionOverlay(); // ADDED
    void showHelpMenu();
    void endTurn();
    void saveGame();
    void quitGame();
    
    // Monster phase logic
    void startMonsterPhase();
    void executeMonsterTurn();
    void useItemForDefense(const Item& item);
    void useItemForDefenseByIndex(int itemIndex);
    void showMonsterPhaseResults();
    void advanceMonsterPhase();
    
    // Utility functions
    std::string getAssetPath(const std::string& relativePath);
    void updateItemTooltip(const std::string& locationName, const std::vector<Item>& items);
    void updateTerrorLevel(int level);
    
    // Image loading functions
    void loadHeroImages();
    void loadMonsterImages();
    void loadVillagerImages();
    void loadItemImages();
    void loadPerkCardImages();
    void loadCoffinImages();
    void unloadImages();
    
    // Perk card & Item utilities
    std::string getPerkCardImageName(PerkType type);
    std::string itemColorToString(ItemColor color); // ADDED
    
    // Coffin functions
    bool isCoffinLocation(const std::string& location) const;
    
    // Save/Load integration
    void saveCurrentGame();
    void loadGameFromSlot(int slotNumber);
    
    // Monster card & texture utilities
    std::string convertMonsterCardNameToImage(const std::string& cardName);
    void loadFrenziedMonsterTexture();
    void unloadFrenziedMonsterTexture();

    void showConfirmation(const std::string& question, std::function<void()> onYes, std::function<void()> onNo);
    void drawConfirmationPrompt();
    void handleConfirmationPromptClick(Vector2 mousePos);
    void hideConfirmation();

    void setGameOver(const std::string& message, const std::string& reason);
    void drawGameOverScreen();
    void handleGameOverClick(Vector2 mousePos);

    void startArcheologistSpecialAction();
    void drawArcheologistLocationChoiceOverlay();
    void handleArcheologistLocationChoiceClick(Vector2 mousePos);
    void drawArcheologistItemChoiceOverlay();
    void handleArcheologistItemChoiceClick(Vector2 mousePos);
    void endArcheologistSpecialAction();

    // In-game save UI helpers
    void openSaveSlots();
    void drawSaveSlotsOverlay();
    void handleSaveSlotsClick(Vector2 mousePos);
    GameState buildCurrentGameStateSnapshot();
};

#endif
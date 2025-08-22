#include "game_screen.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
#include <sstream>
#include "invisibleman.hpp"
#include "perkcard.hpp"
#include "item.hpp"

GameScreen::GameScreen(const std::vector<PlayerInfo>& playerInfo, const std::string& startPlayer, int windowWidth, int windowHeight) 
    : players(playerInfo), startingPlayer(startPlayer), currentTurn(1), gameRunning(true), 
      currentTerrorLevel(0), currentPhase(HERO_PHASE), currentHero(nullptr), otherHero(nullptr),
      remainingActions(5), maxActions(5), monsterCardLoaded(false), frenziedMonsterTextureLoaded(false) {
    
    // Use the provided window dimensions instead of creating a new window
    screenWidth = windowWidth;
    screenHeight = windowHeight;
    
    // Load fonts with responsive sizes
    int gameFontSize = screenHeight * 0.02f;  // 2% of screen height
    int titleFontSize = screenHeight * 0.04f;  // 4% of screen height
    int largeFontSize = screenHeight * 0.03f;  // 3% of screen height
    
    gameFont = LoadFontEx(getAssetPath("fonts/Rubik-Regular.ttf").c_str(), gameFontSize, 0, 0);
    titleFont = LoadFontEx(getAssetPath("fonts/Creepster-Regular.ttf").c_str(), titleFontSize, 0, 0);
    largeFont = LoadFontEx(getAssetPath("fonts/Rubik-Regular.ttf").c_str(), largeFontSize, 0, 0);
    
    if (gameFont.texture.id == 0) gameFont = GetFontDefault();
    if (titleFont.texture.id == 0) titleFont = GetFontDefault();
    if (largeFont.texture.id == 0) largeFont = GetFontDefault();
    
    // Load map texture
    std::string mapPath = getAssetPath("Horrified_Assets/map2.jpg");
    if (FileExists(mapPath.c_str())) {
        mapTexture = LoadTexture(mapPath.c_str());
        mapLoaded = true;
        std::cout << "Map loaded successfully from: " << mapPath << std::endl;
    }
    
    // Load hero and monster images
    loadHeroImages();
    loadMonsterImages();
    loadVillagerImages();
    loadItemImages();
    loadPerkCardImages();
    loadCoffinImages();
    
    // Initialize UI Layout - Fully responsive positioning based on screen size
    float margin = screenWidth * 0.015f;  // 1.5% margin (reduced)
    float mapWidth = screenWidth * 0.58f;  // 58% of screen width for map (reduced)
    float mapHeight = screenHeight * 0.85f;  // 85% of screen height for map
    float rightPanelWidth = screenWidth * 0.39f;  // 39% of screen width for right panel
    float rightPanelX = mapWidth + margin * 2;  // Right panel starts after map + margin
    
    // Ensure total width doesn't exceed screen width
    float totalWidth = mapWidth + margin * 2 + rightPanelWidth;
    if (totalWidth > screenWidth) {
        // Adjust if needed
        float excess = totalWidth - screenWidth;
        mapWidth -= excess * 0.6f;  // Reduce map width by 60% of excess
        rightPanelWidth -= excess * 0.4f;  // Reduce right panel by 40% of excess
        rightPanelX = mapWidth + margin * 2;
    }
    
    // Top section (Terror Tracker and Turn/Phase)
    float topHeight = screenHeight * 0.08f;  // 8% of screen height
    terrorTrackerArea = {margin, margin, mapWidth, topHeight};
    turnPhasePanel = {rightPanelX, margin, rightPanelWidth - 100, topHeight};
    
    // Main map area
    mapArea = {margin, margin + topHeight + margin, mapWidth, mapHeight - topHeight - margin};
    
    // Right panel sections - dynamically sized based on remaining space
    float rightPanelY = margin + topHeight + margin;
    float remainingHeight = screenHeight - rightPanelY - margin;
    
    // Calculate section heights based on phase
    float heroInfoHeight = remainingHeight * 0.30f;  // 30% of remaining height
    float actionsHeight = remainingHeight * 0.25f;   // 25% of remaining height
    float evidenceHeight = remainingHeight * 0.20f;  // 20% of remaining height
    float monsterPhaseHeight = remainingHeight * 0.23f; // 23% of remaining height
    
    heroInfoPanel = {rightPanelX, rightPanelY, rightPanelWidth - 100, heroInfoHeight};
    actionsPanel = {rightPanelX, rightPanelY + heroInfoHeight + margin, rightPanelWidth - 100, actionsHeight + 100};
    evidencePanel = {rightPanelX, rightPanelY + heroInfoHeight + margin + actionsHeight + margin * 4, rightPanelWidth - 100, evidenceHeight};
    monsterPhasePanel = {rightPanelX, rightPanelY + heroInfoHeight + margin + actionsHeight + margin * 5 + evidenceHeight, rightPanelWidth - 100, monsterPhaseHeight};
    
    // Initialize game components
    initializeGameState();
    initializeMap();
    initializeLocations();
    initializeActions();
    initializeTerrorTracker();
    initializeHelpMenu();
    loadHelpImages();
    
    // Initialize tooltip
    itemTooltip = {"", {}, {0, 0}, false, 0.0f, "", ""};

    itemWasPickedUpThisTurn = false;

    showArcheologistLocationChoice = false;
    showArcheologistItemChoice = false;
    archeologistPickedUpItem = false;
}

GameScreen::~GameScreen() {
    if (mapLoaded && mapTexture.id != 0) {
        UnloadTexture(mapTexture);
    }
    if (monsterCardLoaded && monsterCardTexture.id != 0) {
        UnloadTexture(monsterCardTexture);
    }
    if (frenziedMonsterTextureLoaded && frenziedMonsterTexture.id != 0) {
        UnloadTexture(frenziedMonsterTexture);
    }
    if (gameFont.texture.id != 0 && gameFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(gameFont);
    }
    if (titleFont.texture.id != 0 && titleFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(titleFont);
    }
    if (largeFont.texture.id != 0 && largeFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(largeFont);
    }
    
    // Unload hero and monster images
    unloadImages();
    unloadHelpImages();
    
}

void GameScreen::initializeGameState() {
    // Initialize save manager
    saveManager = std::make_unique<SaveManager>();
    
    // Initialize game map
    gameMap = std::make_unique<Map>();
    
    // Initialize game components
    itemBag = new ItemBag(*gameMap);
    monsterManager = MonsterManager();
    perkDeck = PerkDeck();
    taskBoard = TaskBoard();
    
    
    for (const auto& player : players) {
        if (player.name == startingPlayer) {
            if (player.hero == "Mayor") {
                currentHero = new Mayor(player.name, gameMap->getLocation("Theatre"));
            } else if (player.hero == "Archeologist") {
                currentHero = new Archeologist(player.name, gameMap->getLocation("Docks"));
            } else if (player.hero == "Courier") {
                currentHero = new Courier(player.name, gameMap->getLocation("Shop"));
            } else if (player.hero == "Scientist") {
                currentHero = new Scientist(player.name, gameMap->getLocation("Laboratory"));
            }
        } else {
            if (player.hero == "Mayor") {
                otherHero = new Mayor(player.name, gameMap->getLocation("Theatre"));
            } else if (player.hero == "Archeologist") {
                otherHero = new Archeologist(player.name, gameMap->getLocation("Docks"));
            } else if (player.hero == "Courier") {
                otherHero = new Courier(player.name, gameMap->getLocation("Shop"));
            } else if (player.hero == "Scientist") {
                otherHero = new Scientist(player.name, gameMap->getLocation("Laboratory"));
            }
        }
    }
    
    // Initialize monsters
    dracula = std::make_unique<Dracula>(gameMap->getLocation("Crypt"));
    invisibleMan = std::make_unique<InvisibleMan>(gameMap->getLocation("Inn"));
    
    // Initialize frenzy marker
    frenzyMarker = std::make_unique<FrenzyMarker>(static_cast<Dracula*>(dracula.get()), 
                                                 static_cast<InvisibleMan*>(invisibleMan.get()));
    
    // Initialize current frenzied monster
    if (frenzyMarker->getCurrentFrenzied()) {
        currentFrenziedMonster = frenzyMarker->getCurrentFrenzied()->getMonsterName();
    } else {
        currentFrenziedMonster = "";
    }
    
    // Give initial perk cards to heroes
    try {
        PerkCard perk = perkDeck.drawRandomCard();
        currentHero->addPerkCard(perk);
        perk = perkDeck.drawRandomCard();
        otherHero->addPerkCard(perk);
    } catch (const std::exception& e) {
        std::cout << "Error giving perk cards: " << e.what() << std::endl;
    }
    
    // Initialize action counts
    remainingActions = maxActions;
}

void GameScreen::initializeMap() {
    // Initialize map locations with relative positions
    float mapWidth = mapArea.width;
    float mapHeight = mapArea.height;
    
    std::vector<std::pair<std::string, Vector2>> locationPositions = {
        {"Cave", {mapWidth * 0.02f, mapHeight * -0.05f}}, 
        {"Camp", {mapWidth * 0.2f, mapHeight * -0.06f}}, 
        {"Precinct", {mapWidth * 0.35f, mapHeight * -0.08f}},
        {"Inn", {mapWidth * 0.53f, mapHeight * -0.12f}}, 
        {"Barn", {mapWidth * 0.71f, mapHeight * -0.085f}}, 
        {"Dungeon", {mapWidth * 0.88f, mapHeight * -0.09f}},
        {"Theatre", {mapWidth * 0.6f, mapHeight * 0.1f}}, 
        {"Tower", {mapWidth * 0.8f, mapHeight * 0.09f}}, 
        {"Docks", {mapWidth * 0.85f, mapHeight * 0.28f}},
        {"Mansion", {mapWidth * 0.24f, mapHeight * 0.27f}}, 
        {"Abbey", {mapWidth * 0.05f, mapHeight * 0.36f}}, 
        {"Shop", {mapWidth * 0.52f, mapHeight * 0.4f}},
        {"Crypt", {mapWidth * 0.03f, mapHeight * 0.56f}}, 
        {"Museum", {mapWidth * 0.19f, mapHeight * 0.52f}}, 
        {"Church", {mapWidth * 0.42f, mapHeight * 0.57f}},
        {"Laboratory", {mapWidth * 0.65f, mapHeight * 0.52f}}, 
        {"Hospital", {mapWidth * 0.33f, mapHeight * 0.71f}}, 
        {"Graveyard", {mapWidth * 0.55f, mapHeight * 0.69f}},
        {"Institute", {mapWidth * 0.77f, mapHeight * 0.69f}}
    };
    
    for (const auto& [name, pos] : locationPositions) {
        MapLocation location;
        location.name = name;
        location.position = {mapArea.x + pos.x, mapArea.y + pos.y};
        location.radius = screenWidth * 0.035f;  // 1.5% of screen width
        location.isClickable = true;
        location.hasCoffin = false;
        location.hasEvidence = false;
        location.coffinDestroyed = false;
        mapLocations[name] = location;
    }
    // Set coffin locations based on TaskBoard
    for (const auto& coffinLocation : coffinLocations) {
        if (mapLocations.find(coffinLocation) != mapLocations.end()) {
            mapLocations[coffinLocation].hasCoffin = true;
        }
    }
}

void GameScreen::initializeLocations() {
    try {
        // Clear all characters from all locations first
        for (auto& [name, location] : mapLocations) {
            location.characters.clear();
        }
        
        // Add characters to locations
        if (currentHero && currentHero->getCurrentLocation()) {
            try {
                std::string heroLocation = currentHero->getCurrentLocation()->getName();
                if (mapLocations.find(heroLocation) != mapLocations.end()) {
                    mapLocations[heroLocation].characters.push_back(currentHero->getHeroName());
                }
            } catch (const std::exception& e) {
                std::cout << "Error adding current hero to location: " << e.what() << std::endl;
            }
        }
        
        if (otherHero && otherHero->getCurrentLocation()) {
            try {
                std::string heroLocation = otherHero->getCurrentLocation()->getName();
                if (mapLocations.find(heroLocation) != mapLocations.end()) {
                    mapLocations[heroLocation].characters.push_back(otherHero->getHeroName());
                }
            } catch (const std::exception& e) {
                std::cout << "Error adding other hero to location: " << e.what() << std::endl;
            }
        }
        
        // Add monsters to locations
        if (dracula && dracula->getCurrentLocation()) {
            try {
                std::string monsterLocation = dracula->getCurrentLocation()->getName();
                if (mapLocations.find(monsterLocation) != mapLocations.end()) {
                    mapLocations[monsterLocation].characters.push_back(dracula->getMonsterName());
                }
            } catch (const std::exception& e) {
                std::cout << "Error adding Dracula to location: " << e.what() << std::endl;
            }
        }
        
        if (invisibleMan && invisibleMan->getCurrentLocation()) {
            try {
                std::string monsterLocation = invisibleMan->getCurrentLocation()->getName();
                if (mapLocations.find(monsterLocation) != mapLocations.end()) {
                    mapLocations[monsterLocation].characters.push_back(invisibleMan->getMonsterName());
                }
            } catch (const std::exception& e) {
                std::cout << "Error adding Invisible Man to location: " << e.what() << std::endl;
            }
        }
        
        // Add villagers to locations
        try {
            for (const auto& [villagerName, villager] : villagerManager.getAllVillagers()) {
                if (villager && villager->getCurrentLocation()) {
                    std::string villagerLocation = villager->getCurrentLocation()->getName();
                    if (mapLocations.find(villagerLocation) != mapLocations.end()) {
                        mapLocations[villagerLocation].characters.push_back(villagerName);
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error adding villagers to locations: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error in initializeLocations: " << e.what() << std::endl;
    }
}

void GameScreen::initializeActions() {
    try {
        // Initialize action buttons with proper colors and actions
        std::vector<std::pair<std::string, std::function<void()>>> actions = {
            {"Move", [this]() { 
                selectedAction = "Move"; 
                // Populate available move locations
                availableMoveLocations.clear();
                if (currentHero && currentHero->getCurrentLocation()) {
                    try {
                        const auto& neighbors = currentHero->getCurrentLocation()->getNeighbors();
                        for (const auto& neighbor : neighbors) {
                            if (neighbor) {
                                availableMoveLocations.push_back(neighbor->getName());
                            }
                        }
                    } catch (const std::exception& e) {
                        std::cout << "Error getting hero neighbors: " << e.what() << std::endl;
                    }
                }
            }},
            {"Guide", [this]() { startGuideAction(); }},
            {"Pick Up", [this]() { 
                if (currentHero && currentHero->getCurrentLocation()) {
                    try {
                        startPickUpAction(currentHero->getCurrentLocation()->getName());
                    } catch (const std::exception& e) {
                        std::cout << "Error starting pick up action: " << e.what() << std::endl;
                    }
                }
            }},
            {"Advance", [this]() { selectedAction = "Advance"; }},
            {"Defeat", [this]() { selectedAction = "Defeat"; }},
            {"Special Action", [this]() { specialAction(); }},
            {"Use Perk Card", [this]() { usePerkCard(); }},
            {"Help", [this]() { showHelpMenu(); }},
            {"End Turn", [this]() { endTurn(); }},
            {"Save", [this]() { saveGame(); }},
            {"Quit", [this]() { quitGame(); }}
        };
        
        // Arrange buttons in a 2-column grid to fit better
        int buttonsPerRow = 2;
        float buttonWidth = (actionsPanel.width * 0.8f) / buttonsPerRow;  // 80% of panel width, 2 columns
        float buttonHeight = screenHeight * 0.03f;  // 3% of screen height
        float buttonSpacingX = actionsPanel.width * 0.1f;  // 10% spacing between columns
        float buttonSpacingY = screenHeight * 0.01f;  // 1% spacing between rows
        float startX = actionsPanel.x + (actionsPanel.width - (buttonWidth * buttonsPerRow + buttonSpacingX)) / 2;
        float startY = actionsPanel.y + screenHeight * 0.06f;
        
        for (size_t i = 0; i < actions.size(); i++) {
            int row = i / buttonsPerRow;
            int col = i % buttonsPerRow;
            
            ActionButton button;
            button.name = actions[i].first;
            button.bounds = Rectangle{
                startX + col * (buttonWidth + buttonSpacingX),
                startY + row * (buttonHeight + buttonSpacingY),
                buttonWidth,
                buttonHeight
            };
            button.isEnabled = true;
            button.action = actions[i].second;
            button.color = buttonColor;
            button.hoverColor = buttonHoverColor;
            
            actionButtons.push_back(button);
        }
    } catch (const std::exception& e) {
        std::cout << "Error in initializeActions: " << e.what() << std::endl;
    }
}

void GameScreen::initializeTerrorTracker() {
    // Initialize 5 upside-down triangles for terror levels
    terrorTrackers.clear();
    float trackerWidth = terrorTrackerArea.width * 0.15f;  // 15% of tracker area width
    float trackerHeight = terrorTrackerArea.height * 0.6f;  // 60% of tracker area height
    float trackerSpacing = terrorTrackerArea.width * 0.05f;  // 5% spacing
    float startX = terrorTrackerArea.x + (terrorTrackerArea.width - (trackerWidth * 5 + trackerSpacing * 4)) / 2;
    float trackerY = terrorTrackerArea.y + (terrorTrackerArea.height - trackerHeight) / 2;
    
    for (int i = 0; i < 5; i++) {
        Rectangle tracker = {
            startX + i * (trackerWidth + trackerSpacing),
            trackerY,
            trackerWidth,
            trackerHeight
        };
        terrorTrackers.push_back(tracker);
    }
    currentTerrorLevel = terrorTracker.getLevel();
}

void GameScreen::run() {
}

void GameScreen::draw() {
    // Clear the screen with the game's background color
    ClearBackground(backgroundColor);
    
    // Draw game elements in order
    drawMap();
    drawLocations();
    drawCoffins();  // Draw coffins first (background)
    drawCharacters(); // Draw characters on top
    drawItems();     // Draw items on top
    drawTerrorTracker();
    drawPhaseIndicator();
    
    // Show either hero phase UI or monster phase UI based on current phase
    if (currentPhase == MONSTER_PHASE && showMonsterPhaseUI) {
        // Show monster phase UI in place of hero info and actions
        drawMonsterPhaseUI();
    } else {
        // Show hero phase UI (hero info and actions)
        drawHeroInfoPanel();
        drawActionsPanel();
        drawEvidencePanel();
    }
    
    // Draw interactive elements
    drawItemTooltip();
    
    // Draw perk card/hero inventory overlay only during hero phase and only if no modal popup
    if (currentPhase == HERO_PHASE && !locationPopupOpen) {
        drawHeroInventoryOverlay();
    }

    // Perk selection overlay (modal)
    if (showPerkSelection) {
        drawPerkSelectionOverlay();
    }

    // Visit from Detective location selection overlay
    if (showVisitFromDetectiveSelection) {
        drawVisitFromDetectiveSelectionOverlay();
    }

    // Advance/Defeat item selection overlay
    if (showAdvanceItemSelection || showDefeatItemSelection) {
        drawAdvanceDefeatItemSelectionOverlay();
    }
    
    // Draw location detail popup (modal)
    if (locationPopupOpen) {
        drawLocationDetailPopup();
    }

    if (showEndTurnPrompt) {
        // Dark semi-transparent background
        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

        // Message box
        float boxWidth = screenWidth * 0.4f;
        float boxHeight = screenHeight * 0.2f;
        float boxX = (screenWidth - boxWidth) / 2;
        float boxY = (screenHeight - boxHeight) / 2;

        DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
        DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

        std::string message = "No actions left!\nClick 'End Turn' to continue.";
        Vector2 textSize = MeasureTextEx(titleFont, message.c_str(), titleFont.baseSize, 1);
        DrawTextEx(titleFont, message.c_str(),
                {boxX + (boxWidth - textSize.x) / 2,
                    boxY + (boxHeight - textSize.y) / 2},
                titleFont.baseSize, 1, WHITE);
    }

    if (showArcheologistLocationChoice) {
        drawArcheologistLocationChoiceOverlay();
    } else if (showArcheologistItemChoice) {
        drawArcheologistItemChoiceOverlay();
    }
    
    // Draw help menu if active
    drawHelpMenu();

    if (showHeroDefenseYesNoOverlay) {
        drawHeroDefenseYesNoOverlay();
    } else if (showHeroDefense) { // The item grid
        drawHeroDefenseOverlay();
    }

    // Draw the confirmation prompt if active
    if (showConfirmationPrompt) {
        drawConfirmationPrompt();
    }
    
    // Draw guide overlays
    if (showGuideVillagers) {
        drawGuideVillagersOverlay();
    }
    
    if (guideMessageTimer > 0.0f) {
        drawGuideMessage();
    }
    
    // Draw pick up overlays
    if (showPickUpItems) {
        drawPickUpItemsOverlay();
    }
    
    if (pickUpMessageTimer > 0.0f) {
        drawPickUpMessage();
    }
    
    if (showGameMessage) {
        drawGameMessage();
    }
    
    if (showHeroDefense) {
        drawHeroDefenseOverlay();
    }

    // Draw the Game Over screen on top of everything else if the game has ended
    if (isGameOver) {
        drawGameOverScreen();
    }

    // Draw Save Slots overlay last (modal)
    if (showSaveSlots) {
        drawSaveSlotsOverlay();
    }
}

void GameScreen::handleInput() {
    // If the game is over, only process input for the game over screen
    if (isGameOver) {
        handleGameOverClick(GetMousePosition());
        return; 
    }

    Vector2 mousePos = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (showHeroDefenseYesNoOverlay) {
            handleHeroDefenseYesNoClick(mousePos);
        } else if (showGameMessage) {
            handleGameMessageClick(mousePos);
        } else if (showHeroDefense) {
            handleHeroDefenseClick(mousePos);
        } else if (showPerkSelection) {
            handlePerkSelectionClick(mousePos);
        } else if (showVisitFromDetectiveSelection) {
            handleVisitFromDetectiveSelectionClick(mousePos);
        } else if (showAdvanceItemSelection || showDefeatItemSelection) {
            handleAdvanceDefeatItemSelectionClick(mousePos);
        } else if (showArcheologistLocationChoice) { 
            handleArcheologistLocationChoiceClick(mousePos);
        } else if (showArcheologistItemChoice) { 
            handleArcheologistItemChoiceClick(mousePos);
        } else if (showHeroDefense) {
            handleHeroDefenseClick(mousePos);
        } else if (showConfirmationPrompt) {
            handleConfirmationPromptClick(mousePos);
        } else if (showGuideVillagers) {
            handleGuideVillagersClick(mousePos);
        } else if (showPickUpItems) {
            handlePickUpItemsClick(mousePos);
        } else if (helpMenuActive) {
            handleHelpMenuClick(mousePos);
        } else if (showSaveSlots) {
            handleSaveSlotsClick(mousePos);
        } else {
            handleMapClick(mousePos);
            handleActionClick(mousePos);
        }
    }

    // Right-click toggles location detail popup
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        handleMapRightClick(mousePos);
    }
    
    // Handle item hover for tooltips
    handleItemHover(mousePos);
    
    if (currentPhase == HERO_PHASE && !locationPopupOpen) {
        handleHeroInventoryHover(mousePos);
    }

    // Scroll hero inventory overlay when hovered and open
    if (showHeroInventoryOverlay && hoveredHero) {
        // Only scroll if mouse is within overlay content rect
        Vector2 mp = mousePos;
        if (CheckCollisionPointRec(mp, heroOverlayRect)) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f) {
                // Scroll direction: positive wheel moves up (decrease scroll)
                heroOverlayScroll -= wheel * (screenHeight * 0.05f);
                if (heroOverlayScroll < 0.0f) heroOverlayScroll = 0.0f;
                if (heroOverlayScroll > heroOverlayMaxScroll) heroOverlayScroll = heroOverlayMaxScroll;
            }
        }
    }
    
    // Handle monster phase advancement (only if no defense dialog is active)
    if (currentPhase == MONSTER_PHASE && IsKeyPressed(KEY_SPACE) && !showHeroDefense) {
        advanceMonsterPhase();
    }
    
    // Handle keyboard input
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (showGameMessage) {
            if (currentGameMessage.requiresAction && currentGameMessage.onCancel) {
                currentGameMessage.onCancel();
            }
            processNextGameMessage();
        } else if (showHeroDefense) {
            // Treat ESC as cancel/No for defense selection when item grid is open
            if (onDefenseCanceled) {
                onDefenseCanceled();
            }
            cancelHeroDefense();
        } else if (showGuideVillagers || selectedVillager) {
            cancelGuideAction();
        } else if (showPickUpItems) {
            cancelPickUpAction();
        } else {
            quitGame();
        }
    }
    
    // Handle game message keyboard input
    if (showGameMessage && currentGameMessage.requiresAction) {
        if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) {
            if (currentGameMessage.onConfirm) {
                currentGameMessage.onConfirm();
            }
            processNextGameMessage();
        } else if (IsKeyPressed(KEY_N)) {
            if (currentGameMessage.onCancel) {
                currentGameMessage.onCancel();
            }
            processNextGameMessage();
        }
    }
    
    // Handle hero defense keyboard input
    if (showHeroDefense) {
        for (int i = 0; i < static_cast<int>(heroDefenseItems.size()); i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                if (onItemSelectedIndex && i < static_cast<int>(heroDefenseItemIndices.size())) {
                    onItemSelectedIndex(heroDefenseItemIndices[i]);
                }
                cancelHeroDefense();
                return;
            }
        }
    }
    
    // Handle guide overlays keyboard input
    if (showGuideVillagers) {
        for (int i = 0; i < static_cast<int>(guidableVillagers.size()); i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                selectVillagerToGuide(i);
                return;
            }
        }
    }
    
    // Handle pick up overlays keyboard input
    if (showPickUpItems) {
        for (int i = 0; i < static_cast<int>(availableItems.size()); i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                pickUpItem(i);
                return;
            }
        }
    }
    
    // Handle monster phase advancement
    if (currentPhase == MONSTER_PHASE && showMonsterPhaseUI && IsKeyPressed(KEY_SPACE)) {
        advanceMonsterPhase();
    }
}

void GameScreen::updateGame() {
    // If the game is over, stop updating the game state
    if (isGameOver) {
        return;
    }

    // Update tooltip display time
    if (itemTooltip.isVisible) {
        itemTooltip.displayTime += GetFrameTime();
        if (itemTooltip.displayTime > 3.0f) {
            itemTooltip.isVisible = false;
        }
    }

    if (showEndTurnPrompt) {
        endTurnPromptTimer -= GetFrameTime();
        if (endTurnPromptTimer <= 0.0f) {
            showEndTurnPrompt = false;
        }
    }
    
    if (guideMessageTimer > 0.0f) {
        guideMessageTimer -= GetFrameTime();
    }
    
    if (pickUpMessageTimer > 0.0f) {
        pickUpMessageTimer -= GetFrameTime();
    }
    
    // Update game message timer
    if (showGameMessage && !currentGameMessage.requiresAction && currentGameMessage.timer > 0.0f) {
        currentGameMessage.timer -= GetFrameTime();
        if (currentGameMessage.timer <= 0.0f) {
            processNextGameMessage();
        }
    }
}

void GameScreen::drawMap() {
    // Draw map background
    DrawRectangleRec(mapArea, {40, 40, 40, 255});
    DrawRectangleLinesEx(mapArea, 2, WHITE);
    
    if (mapLoaded) {
        // Draw map texture
        DrawTexturePro(mapTexture,
            Rectangle{0, 0, (float)mapTexture.width, (float)mapTexture.height},
            mapArea,
            Vector2{0, 0}, 0.0f, WHITE);
    } else {
        // Draw placeholder map
        DrawTextEx(gameFont, "GAME MAP", 
            Vector2{mapArea.x + mapArea.width/2 - 50, mapArea.y + mapArea.height/2 - 20}, 
            gameFont.baseSize * 2, 2, GRAY);
    }
}

void GameScreen::drawLocations() {
    // Draw clickable location circles (invisible but functional)
    for (const auto& [name, location] : mapLocations) {
        Vector2 pos = {mapArea.x + location.position.x, mapArea.y + location.position.y};
        
        // Check if location is hovered
        bool isHovered = CheckCollisionPointCircle(GetMousePosition(), pos, location.radius);
        bool isSelected = (selectedLocation == name);
        bool isAvailableMove = (selectedAction == "Move" && 
                               std::find(availableMoveLocations.begin(), availableMoveLocations.end(), name) != availableMoveLocations.end());
        bool isAvailableGuide = (selectedVillager && 
                                std::find_if(availableGuideLocations.begin(), availableGuideLocations.end(),
                                    [&name](const std::shared_ptr<Location>& loc) { return loc->getName() == name; }) != availableGuideLocations.end());
        
        if (isHovered || isSelected || isAvailableMove || isAvailableGuide) {
            Color circleColor;
            if (isSelected) {
                circleColor = RED;
            } else if (isAvailableMove) {
                circleColor = GREEN;  // Green for available move locations
            } else if (isAvailableGuide) {
                circleColor = GREEN;  // Green for available guide locations
            } else {
                circleColor = YELLOW;  // Yellow for hovered
            }
            DrawCircleLines(pos.x, pos.y, location.radius, circleColor);
            
            // Draw location name for available move/guide locations
            if (isAvailableMove || isAvailableGuide) {
                DrawTextEx(gameFont, name.c_str(), 
                    Vector2{pos.x - MeasureTextEx(gameFont, name.c_str(), gameFont.baseSize, 1).x/2, 
                           pos.y + location.radius + 5}, 
                    gameFont.baseSize * 0.8f, 1, GREEN);
            }
        }    
    }
}

void GameScreen::drawCharacters() {
    // Draw characters on the map
    for (const auto& [name, location] : mapLocations) {
        if (!location.characters.empty()) {
            Vector2 pos = {mapArea.x + location.position.x, mapArea.y + location.position.y};
            
            // Draw character indicators
            for (size_t i = 0; i < location.characters.size(); i++) {
                float offsetX = (i - location.characters.size()/2.0f) * screenWidth * 0.03f;  // Increased spacing
                float imageSize = screenWidth * 0.045f;  // Increased from 0.025f to 0.045f (45% larger)
                
                // Try to find and draw the character image
                bool imageDrawn = false;
                std::string charName = location.characters[i];
                
                // Check for hero images
                for (const auto& [heroName, texture] : heroImages) {
                    if (charName.find(heroName) != std::string::npos) {
                        Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
                        Rectangle destRect = {pos.x + offsetX - imageSize/2, pos.y - imageSize/2, imageSize, imageSize};
                        DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                        imageDrawn = true;
                        break;
                    }
                }
                
                // Check for monster images
                if (!imageDrawn) {
                    for (const auto& [monsterName, texture] : monsterImages) {
                        // Handle special case for "Invisible man" vs "InvisibleMan"
                        std::string searchName = charName;
                        if (charName == "Invisible man") {
                            searchName = "InvisibleMan";
                        }
                        if (searchName.find(monsterName) != std::string::npos) {
                            Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
                            Rectangle destRect = {pos.x + offsetX - imageSize/2, pos.y - imageSize/2, imageSize, imageSize};
                            DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                            imageDrawn = true;
                            break;
                        }
                    }
                }
                
                // Check for villager images
                if (!imageDrawn) {
                    for (const auto& [villagerName, texture] : villagerImages) {
                        if (charName.find(villagerName) != std::string::npos) {
                            Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
                            Rectangle destRect = {pos.x + offsetX - imageSize/2, pos.y - imageSize/2, imageSize, imageSize};
                            DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                            imageDrawn = true;
                            break;
                        }
                    }
                }
                
                // Fallback to colored circle if no image found
                if (!imageDrawn) {
                    Color charColor = BLUE;
                    if (charName.find("Dracula") != std::string::npos) {
                        charColor = RED;
                    } else if (charName.find("Invisible") != std::string::npos) {
                        charColor = PURPLE;
                    } else if (charName.find("Mayor") != std::string::npos || 
                               charName.find("Archeologist") != std::string::npos ||
                               charName.find("Courier") != std::string::npos ||
                               charName.find("Scientist") != std::string::npos) {
                        charColor = GREEN;
                    }
                    DrawCircle(pos.x + offsetX, pos.y, imageSize/2, charColor);
                }
                
                // Draw character name with better positioning
                DrawTextEx(gameFont, charName.c_str(), 
                    Vector2{pos.x + offsetX - MeasureTextEx(gameFont, charName.c_str(), gameFont.baseSize, 1).x/2, 
                           pos.y + imageSize/2 + 8},  // Increased spacing from 5 to 8
                    gameFont.baseSize * 0.9f, 1, textColor);  // Increased font size from 0.8f to 0.9f
            }
        }
    }
}

void GameScreen::drawItems() {
    // Draw items on the map using itemBag
    if (!gameMap) return;

    for (const auto& pair : gameMap->locations) {
        const std::string& locationName = pair.first;
        const auto& location = pair.second;
        auto mapLocIt = mapLocations.find(locationName);
        if (mapLocIt == mapLocations.end()) continue;

        Vector2 pos = {mapArea.x + mapLocIt->second.position.x, mapArea.y + mapLocIt->second.position.y};

        const auto& itemsAtLocation = location->getItems();
        for (const auto& item : itemsAtLocation) {
            const std::string itemName = item.getItemName();
            auto it = itemImages.find(itemName);
            if (it != itemImages.end()) {
                float imageSize = screenWidth * 0.03f;  // 3% of screen width
                Rectangle sourceRect = {0, 0, (float)it->second.width, (float)it->second.height};
                Rectangle destRect = {pos.x - imageSize/2, pos.y + 10 - imageSize/2, imageSize, imageSize};
                DrawTexturePro(it->second, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
            }

            DrawTextEx(gameFont, itemName.c_str(),
                Vector2{pos.x - MeasureTextEx(gameFont, itemName.c_str(), gameFont.baseSize * 0.7f, 1).x/2,
                        pos.y + 20},
                gameFont.baseSize * 0.7f, 1, textColor);
        }
    }
}

void GameScreen::drawCoffins() {
    if (!coffinImagesLoaded) return;
    
    for (const auto& [name, location] : mapLocations) {
        if (isCoffinLocation(name)) {
            Vector2 pos = {mapArea.x + location.position.x, mapArea.y + location.position.y};
            
            // Check if coffin is destroyed using TaskBoard
            bool isDestroyed = taskBoard.isCoffinDestroyed(name);
            
            // Choose appropriate texture
            Texture2D& texture = isDestroyed ? smashedCoffinTexture : coffinTexture;
            
            // Scale image to fit properly
            float imageSize = screenWidth * 0.04f;  // 4% of screen width
            
            Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
            Rectangle destRect = {pos.x - imageSize/2, pos.y - imageSize/2, imageSize, imageSize};
            
            DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
            
            // Draw coffin label
            std::string label = isDestroyed ? "Destroyed" : "Coffin";
            DrawTextEx(gameFont, label.c_str(), 
                Vector2{pos.x - MeasureTextEx(gameFont, label.c_str(), gameFont.baseSize, 1).x/2, 
                       pos.y + imageSize/2 + 5}, 
                gameFont.baseSize * 0.8f, 1, isDestroyed ? RED : WHITE);
        }
    }
}

void GameScreen::drawTerrorTracker() {
    // ESSENTIAL: Get the latest terror level from the game state each frame.
    currentTerrorLevel = terrorTracker.getLevel();

    // Draw terror tracker background
    DrawRectangleRec(terrorTrackerArea, {30, 30, 30, 255});
    DrawRectangleLinesEx(terrorTrackerArea, 2, WHITE);

    // Draw title
    const float padding = 10;
    const float titleX = terrorTrackerArea.x + padding;
    const float titleY = terrorTrackerArea.y + padding + 10;
    DrawTextEx(titleFont, "TERROR LEVEL:", 
        Vector2{titleX, titleY}, 
        titleFont.baseSize, 2, titleColor);

    // --- Rectangle Indicator Setup ---
    int numLevels = 5;
    float indicatorHeight = terrorTrackerArea.height * 0.6f; // 60% of the panel's height
    float indicatorWidth = 45; // A clear, fixed width for each indicator
    float spacing = 20;

    // Calculate the total width to center the indicators
    float totalIndicatorsWidth = (numLevels * indicatorWidth) + ((numLevels - 1) * spacing);
    
    // Position the indicators to be vertically centered and horizontally after the title
    float startX = titleX + MeasureTextEx(titleFont, "TERROR LEVEL:", titleFont.baseSize, 2).x + 30;
    float startY = terrorTrackerArea.y + (terrorTrackerArea.height - indicatorHeight) / 2;

    // Draw the 5 rectangle indicators
    for (int i = 0; i < numLevels; i++) {
        float x = startX + i * (indicatorWidth + spacing);

        // Define the rectangle for the current level
        Rectangle indicatorRect = {x, startY, indicatorWidth, indicatorHeight};

        // CORE LOGIC: Set color to RED if the level is active, otherwise GRAY.
        Color indicatorColor = (i < currentTerrorLevel) ? RED : GRAY;

        // Draw the filled rectangle and its border
        DrawRectangleRec(indicatorRect, indicatorColor);
        DrawRectangleLinesEx(indicatorRect, 2, WHITE);

        // Draw the level number centered inside the rectangle
        std::string levelText = std::to_string(i + 1);
        Vector2 textSize = MeasureTextEx(gameFont, levelText.c_str(), gameFont.baseSize, 1);
        Vector2 textPos = {
            indicatorRect.x + (indicatorRect.width - textSize.x) / 2,
            indicatorRect.y + (indicatorRect.height - textSize.y) / 2
        };
        DrawTextEx(gameFont, levelText.c_str(), textPos, gameFont.baseSize, 1, WHITE);
    }
}

void GameScreen::drawPhaseIndicator() {
    // Draw turn/phase panel background
    DrawRectangleRec(turnPhasePanel, {60, 60, 60, 255});
    DrawRectangleLinesEx(turnPhasePanel, 2, WHITE);
    
    // Calculate responsive padding
    float padding = screenWidth * 0.005f;  // 0.5% padding
    
    // Draw turn number on the left side of the panel
    std::string turnText = "TURN: " + std::to_string(currentTurn);
    float turnX = turnPhasePanel.x + padding;
    float turnY = turnPhasePanel.y + (turnPhasePanel.height - largeFont.baseSize) / 2;
    
    DrawTextEx(titleFont, turnText.c_str(), 
        Vector2{turnX, turnY}, 
        largeFont.baseSize, 1, textColor);
    
    // Draw phase indicator on the right side of the panel
    std::string phaseText;
    if (currentPhase == HERO_PHASE) {
        phaseText = "HERO PHASE";
    } else if (currentPhase == MONSTER_PHASE) {
        phaseText = "MONSTER PHASE";
    } else {
        phaseText = "UNKNOWN PHASE";
    }
    
    // Calculate text width to center it with proper margins
    int textWidth = MeasureTextEx(titleFont, phaseText.c_str(), largeFont.baseSize, 1).x;
    float phaseX = turnPhasePanel.x + turnPhasePanel.width - textWidth - padding;
    float phaseY = turnPhasePanel.y + (turnPhasePanel.height - largeFont.baseSize) / 2;
    
    DrawTextEx(titleFont, phaseText.c_str(), 
        Vector2{phaseX, phaseY}, 
        largeFont.baseSize, 1, titleColor);
}

void GameScreen::drawHeroInfoPanel() {
    // Draw hero info panel background
    DrawRectangleRec(heroInfoPanel, {60, 60, 60, 255});
    DrawRectangleLinesEx(heroInfoPanel, 2, WHITE);
    
    // Calculate responsive split for two parts
    float leftWidth = heroInfoPanel.width * 0.49f;  // 49% for left side
    float rightWidth = heroInfoPanel.width * 0.49f;  // 49% for right side
    float gap = heroInfoPanel.width * 0.02f;  // 2% gap in middle
    float padding = screenWidth * 0.005f;  // 0.5% padding
    
    Rectangle leftPanel = {heroInfoPanel.x + padding, heroInfoPanel.y + padding, leftWidth - 10, heroInfoPanel.height - padding * 2};
    Rectangle rightPanel = {heroInfoPanel.x + leftWidth + gap + padding - 10, heroInfoPanel.y + padding , rightWidth - 10, heroInfoPanel.height - padding * 2};
    
    // Draw left panel (current hero)
    DrawRectangleRec(leftPanel, {50, 50, 50, 255});
    DrawRectangleLinesEx(leftPanel, 1, WHITE);
    
    // Draw right panel (other hero)
    DrawRectangleRec(rightPanel, {50, 50, 50, 255});
    DrawRectangleLinesEx(rightPanel, 1, WHITE);
    
    // Calculate responsive text spacing
    float titleSpacing = screenHeight * 0.015f;  // 1.5% of screen height
    float lineSpacing = screenHeight * 0.025f;   // 1.2% of screen height
    
    // Draw current hero info (left side)
    float leftY = leftPanel.y + titleSpacing;
    DrawTextEx(gameFont, "CURRENT HERO", 
        Vector2{leftPanel.x + padding, leftY}, 
        gameFont.baseSize, 1, titleColor);
    
    if (currentHero) {
        leftY += lineSpacing * 2;
        DrawTextEx(gameFont, ("Hero: " + currentHero->getHeroName()).c_str(), 
            Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
        
        leftY += lineSpacing;
        DrawTextEx(gameFont, ("Player: " + currentHero->getPlayerName()).c_str(), 
            Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
        
        leftY += lineSpacing;
        DrawTextEx(gameFont, ("Location: " + currentHero->getCurrentLocation()->getName()).c_str(), 
            Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
        
        leftY += lineSpacing;
        DrawTextEx(gameFont, ("Actions: " + std::to_string(remainingActions) + "/" + std::to_string(maxActions)).c_str(), 
            Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
        
        leftY += lineSpacing;
        // Show actual perk card information
        const auto& currentHeroPerks = currentHero->getPerkCards();
        if (currentHeroPerks.empty()) {
            DrawTextEx(gameFont, "Perk Cards: None", 
                Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
        } else {
            DrawTextEx(gameFont, ("Perk Cards: " + std::to_string(currentHeroPerks.size()) + " (Hover to view)").c_str(), 
                Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
        }
        
        leftY += lineSpacing;
        // Show actual item information
        const auto& currentHeroItems = currentHero->getItems();
        DrawTextEx(gameFont, ("Items: " + std::to_string(currentHeroItems.size())).c_str(), 
            Vector2{leftPanel.x + padding, leftY}, gameFont.baseSize, 1, textColor);
    }
    
    // Draw other hero info (right side)
    float rightY = rightPanel.y + titleSpacing;
    DrawTextEx(gameFont, "OTHER HERO", 
        Vector2{rightPanel.x + padding, rightY}, 
        gameFont.baseSize, 1, titleColor);
    
    if (otherHero) {
        rightY += lineSpacing * 2;
        DrawTextEx(gameFont, ("Hero: " + otherHero->getHeroName()).c_str(), 
            Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
        
        rightY += lineSpacing;
        DrawTextEx(gameFont, ("Player: " + otherHero->getPlayerName()).c_str(), 
            Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
        
        rightY += lineSpacing;
        DrawTextEx(gameFont, ("Location: " + otherHero->getCurrentLocation()->getName()).c_str(), 
            Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
        
        rightY += lineSpacing;
        DrawTextEx(gameFont, ("Actions: " + std::to_string(otherHero->getRemainingActions()) + "/" + std::to_string(otherHero->getMaxActions())).c_str(), 
            Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
        
        rightY += lineSpacing;
        // Show actual perk card information for other hero
        const auto& otherHeroPerks = otherHero->getPerkCards();
        if (otherHeroPerks.empty()) {
            DrawTextEx(gameFont, "Perk Cards: None", 
                Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
        } else {
            DrawTextEx(gameFont, ("Perk Cards: " + std::to_string(otherHeroPerks.size()) + " (Hover to view)").c_str(), 
                Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
        }
        
        rightY += lineSpacing;
        // Show actual item information for other hero
        const auto& otherHeroItems = otherHero->getItems();
        DrawTextEx(gameFont, ("Items: " + std::to_string(otherHeroItems.size())).c_str(), 
            Vector2{rightPanel.x + padding, rightY}, gameFont.baseSize, 1, textColor);
    }
}

void GameScreen::drawActionsPanel() {
    DrawRectangleRec(actionsPanel, {60, 60, 60, 255});
    DrawRectangleLinesEx(actionsPanel, 2, WHITE);
    float padding = screenWidth * 0.005f;
    DrawTextEx(gameFont, "ACTIONS", Vector2{actionsPanel.x + padding, actionsPanel.y + padding}, gameFont.baseSize * 1.5, 2, titleColor);
    Vector2 mousePos = GetMousePosition();
    for (const auto& button : actionButtons) {
        bool isHovered = CheckCollisionPointRec(mousePos, button.bounds);
        bool isDisabled = (currentPhase == MONSTER_PHASE) || !button.isEnabled;
        Color buttonColor = isDisabled ? GRAY : (isHovered ? button.hoverColor : button.color);
        DrawRectangleRec(button.bounds, buttonColor);
        DrawRectangleLinesEx(button.bounds, 1, WHITE);
        int textWidth = MeasureTextEx(gameFont, button.name.c_str(), gameFont.baseSize, 1).x;
        DrawTextEx(gameFont, button.name.c_str(), Vector2{button.bounds.x + button.bounds.width/2 - textWidth/2, button.bounds.y + button.bounds.height/2 - gameFont.baseSize/2}, gameFont.baseSize, 1, isDisabled ? DARKGRAY : textColor);
    }
}

void GameScreen::drawEvidencePanel() {
    // Draw evidence panel background
    DrawRectangleRec(evidencePanel, {60, 60, 60, 255});
    DrawRectangleLinesEx(evidencePanel, 2, WHITE);
    
    // Calculate responsive padding
    float padding = screenWidth * 0.005f;  // 0.5% padding
    
    // Draw panel title
    DrawTextEx(gameFont, "INVISIBLE MAN EVIDENCES", 
        Vector2{evidencePanel.x + padding, evidencePanel.y + padding}, 
        gameFont.baseSize * 1.2, 2, titleColor);
    
    // Get evidence delivery status from TaskBoard
    const auto& evidenceLocations = taskBoard.getInvisibleManCluesDelivered();
    
    // Define the 5 evidence locations
    std::vector<std::string> locations = {"Inn", "Mansion", "Barn", "Laboratory", "Institute"};
    
    // Calculate section dimensions
    float sectionWidth = (evidencePanel.width - padding * 3) / 5 - 8;  // 5 sections with padding
    float sectionHeight = evidencePanel.height - gameFont.baseSize * 2 - padding * 3 + 15;  // Leave space for title
    float startX = evidencePanel.x + padding;
    float startY = evidencePanel.y + gameFont.baseSize * 1.5 + padding;
    
    // Draw each evidence section
    for (size_t i = 0; i < locations.size(); i++) {
        float sectionX = startX + i * (sectionWidth + padding);
        Rectangle sectionRect = {sectionX, startY, sectionWidth, sectionHeight};
        
        // Draw section background
        bool isDelivered = evidenceLocations.find(locations[i]) != evidenceLocations.end() && 
                          evidenceLocations.at(locations[i]);
        Color sectionColor = isDelivered ? successColor : (Color){40, 40, 40, 255};
        DrawRectangleRec(sectionRect, sectionColor);
        DrawRectangleLinesEx(sectionRect, 1, WHITE);
        
        // Draw location name
        int textWidth = MeasureTextEx(gameFont, locations[i].c_str(), gameFont.baseSize * 0.8f, 1).x;
        DrawTextEx(gameFont, locations[i].c_str(), 
            Vector2{sectionX + sectionWidth/2 - textWidth/2 - 5, startY + padding}, 
            gameFont.baseSize, 1, textColor);
        
        // Draw evidence status
        std::string status = isDelivered ? "Delivered" : "Empty";
        Color statusColor = isDelivered ? WHITE : GRAY;
        int statusWidth = MeasureTextEx(gameFont, status.c_str(), gameFont.baseSize * 0.7f, 1).x;
        DrawTextEx(gameFont, status.c_str(), 
            Vector2{sectionX + sectionWidth/2 - statusWidth/2, startY + sectionHeight - gameFont.baseSize * 0.7f - padding}, 
            gameFont.baseSize * 0.7f, 1, statusColor);
    }
}

void GameScreen::drawMonsterPhasePanel() {
    // Draw monster phase panel background
    DrawRectangleRec(monsterPhasePanel, {60, 60, 60, 255});
    DrawRectangleLinesEx(monsterPhasePanel, 2, WHITE);
    
    float padding = screenWidth * 0.005f;
    
    // Draw panel title
    DrawTextEx(gameFont, "MONSTER PHASE", 
        Vector2{monsterPhasePanel.x + padding, monsterPhasePanel.y + padding}, 
        gameFont.baseSize * 1.5, 2, titleColor);
    
    // Show monster phase UI if active
    if (showMonsterPhaseUI && currentPhase == MONSTER_PHASE) {
        // Simple monster phase display
        float startY = monsterPhasePanel.y + gameFont.baseSize * 2 + padding;
        
        // Show monster card info
        if (!currentMonsterCard.empty()) {
            DrawTextEx(gameFont, ("Monster Card: " + currentMonsterCard).c_str(), 
                Vector2{monsterPhasePanel.x + padding, startY}, 
                gameFont.baseSize, 1, textColor);
        }
        
        // Show dice results
        if (!diceResults.empty()) {
            std::string diceText = "Dice: ";
            for (const auto& result : diceResults) {
                diceText += result + " ";
            }
            DrawTextEx(gameFont, diceText.c_str(), 
                Vector2{monsterPhasePanel.x + padding, startY + gameFont.baseSize + padding}, 
                gameFont.baseSize, 1, textColor);
        }
        
        // Show frenzied monster
        if (!currentFrenziedMonster.empty()) {
            DrawTextEx(gameFont, ("Frenzied: " + currentFrenziedMonster).c_str(), 
                Vector2{monsterPhasePanel.x + padding, startY + (gameFont.baseSize + padding) * 2}, 
                gameFont.baseSize, 1, textColor);
        }
        
        // Show instruction
        std::string instruction = "Press SPACE to continue";
        int textWidth = MeasureTextEx(gameFont, instruction.c_str(), gameFont.baseSize, 1).x;
        DrawTextEx(gameFont, instruction.c_str(), 
            Vector2{monsterPhasePanel.x + (monsterPhasePanel.width - textWidth) / 2, 
                   monsterPhasePanel.y + monsterPhasePanel.height - gameFont.baseSize - padding}, 
            gameFont.baseSize, 1, GRAY);
    } else {
        // Draw default monster info
        std::string monsterName = "Current Monster: " + (dracula ? dracula->getMonsterName() : "None");
        DrawTextEx(gameFont, monsterName.c_str(), 
            Vector2{monsterPhasePanel.x + padding, monsterPhasePanel.y + 50}, 
            gameFont.baseSize, 1, textColor);
        
        std::string monsterLocation = "Location: " + (dracula ? dracula->getCurrentLocation()->getName() : "N/A");
        DrawTextEx(gameFont, monsterLocation.c_str(), 
            Vector2{monsterPhasePanel.x + padding, monsterPhasePanel.y + 80}, 
            gameFont.baseSize, 1, textColor);
        
        std::string monsterAction = "Current Action: " + std::string(dracula ? "Active" : "Idle");
        DrawTextEx(gameFont, monsterAction.c_str(), 
            Vector2{monsterPhasePanel.x + padding, monsterPhasePanel.y + 110}, 
            gameFont.baseSize, 1, textColor);
    }
}

void GameScreen::drawMonsterPhaseUI() {
    // Draw monster phase background covering the area where hero info and actions would be
    Rectangle monsterPhaseArea = {
        heroInfoPanel.x, 
        heroInfoPanel.y, 
        heroInfoPanel.width, 
        heroInfoPanel.height + actionsPanel.height + screenWidth * 0.1f // Include margin between panels
    };
    
    // Draw background
    DrawRectangleRec(monsterPhaseArea, {60, 60, 60, 255});
    DrawRectangleLinesEx(monsterPhaseArea, 2, WHITE);
    
    // Draw title
    float padding = screenWidth * 0.005f;
    DrawTextEx(gameFont, "MONSTER PHASE", 
        Vector2{monsterPhaseArea.x + padding, monsterPhaseArea.y + padding}, 
        gameFont.baseSize * 1.5, 2, titleColor);
    
    // Calculate box positions within the monster phase area
    float titleHeight = screenHeight * 0.025f;
    float availableHeight = monsterPhaseArea.height - titleHeight - padding * 4 - 40;
    float boxHeight = availableHeight / 3;
    float boxWidth = monsterPhaseArea.width - padding * 2;
    float startY = monsterPhaseArea.y + titleHeight + padding + 10;
    
    // Update box positions to use the monster phase area
    monsterCardBox = {monsterPhaseArea.x + padding, startY, boxWidth, boxHeight};
    diceResultsBox = {monsterPhaseArea.x + padding, startY + boxHeight + padding, boxWidth, boxHeight};
    frenzyMarkerBox = {monsterPhaseArea.x + padding, startY + (boxHeight + padding) * 2, boxWidth, boxHeight};
    
    // Draw the three boxes
    drawMonsterCardBox();
    drawDiceResultsBox();
    drawFrenzyMarkerBox();
    
    // Draw advancement instruction
    std::string instructionText = "Press SPACE to continue to next turn";
    int textWidth = MeasureTextEx(gameFont, instructionText.c_str(), gameFont.baseSize, 1).x;
    DrawTextEx(gameFont, instructionText.c_str(), 
        Vector2{monsterPhaseArea.x + (monsterPhaseArea.width - textWidth) / 2, 
               monsterPhaseArea.y + monsterPhaseArea.height - gameFont.baseSize - padding}, 
        gameFont.baseSize, 1, GRAY);
}

void GameScreen::drawMonsterCardBox() {
    // Draw box background
    DrawRectangleRec(monsterCardBox, {50, 50, 50, 255});
    DrawRectangleLinesEx(monsterCardBox, 1, WHITE);
    
    // Calculate responsive padding
    float padding = screenWidth * 0.005f;  // 0.5% padding
    
    // Draw title
    DrawTextEx(gameFont, "MONSTER CARD", 
        Vector2{monsterCardBox.x + padding, monsterCardBox.y + padding}, 
        gameFont.baseSize, 1, titleColor);
    
    // Calculate layout: image on left, text on right
    float titleHeight = gameFont.baseSize + padding * 2;
    float availableWidth = monsterCardBox.width - padding * 3; // Account for left, middle, and right padding
    float availableHeight = monsterCardBox.height - titleHeight - padding;
    float imageWidth = availableWidth * 0.4f;  // 40% for image
    float textWidth = availableWidth * 0.55f;  // 55% for text, 5% gap
    
    // Draw monster card image if available
    bool monsterCardsOver = monsterManager.isEmpty();
    std::string cardName = monsterManager.getCurrentCardName();
    std::string eventText = monsterManager.getCurrentCard().getEventText();
    std::string imageName = convertMonsterCardNameToImage(cardName);

    if (monsterCardsOver) {
        // Show message if deck is empty
        float textX = monsterCardBox.x + padding * 2 + imageWidth;
        float textY = monsterCardBox.y + titleHeight;
        DrawTextEx(gameFont, "Monster cards are over", Vector2{textX, textY}, gameFont.baseSize, 1, textColor);
    } else {
        // Always show the current monster card image if available
        if (!imageName.empty()) {
            std::string cardPath = getAssetPath("Horrified_Assets/Monster_Cards/" + imageName);
            if (FileExists(cardPath.c_str())) {
                Texture2D cardTexture = LoadTexture(cardPath.c_str());
                if (cardTexture.id != 0) {
                    float scale = std::min(imageWidth / cardTexture.width, availableHeight / cardTexture.height);
                    float scaledWidth = cardTexture.width * scale;
                    float scaledHeight = cardTexture.height * scale;
                    Rectangle sourceRect = {0, 0, (float)cardTexture.width, (float)cardTexture.height};
                    Rectangle destRect = {monsterCardBox.x + padding,
                                       monsterCardBox.y + titleHeight + (availableHeight - scaledHeight) / 2,
                                       scaledWidth, scaledHeight};
                    DrawTexturePro(cardTexture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                }
            }
        }
        // Draw card name above event text, next to image
        float nameX = monsterCardBox.x + padding * 2 + imageWidth;
        float nameY = monsterCardBox.y + titleHeight;
        DrawTextEx(gameFont, cardName.c_str(), Vector2{nameX, nameY}, gameFont.baseSize, 1, titleColor);
        // Draw event text below card name
        float textY = nameY + gameFont.baseSize + 5;
        float textX = nameX;
        float eventTextWidth = textWidth;
        // Word wrap event text
        std::vector<std::string> wrappedLines;
        std::string currentLine = "";
        std::string word = "";
        for (char c : eventText) {
            if (c == ' ' || c == '\n') {
                if (!word.empty()) {
                    std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                    float lineWidth = MeasureTextEx(gameFont, testLine.c_str(), gameFont.baseSize * 0.9f, 1).x;
                    if (lineWidth <= eventTextWidth) {
                        currentLine = testLine;
                    } else {
                        if (!currentLine.empty()) {
                            wrappedLines.push_back(currentLine);
                        }
                        currentLine = word;
                    }
                    word = "";
                }
                if (c == '\n') {
                    wrappedLines.push_back(currentLine);
                    currentLine = "";
                }
            } else {
                word += c;
            }
        }
        if (!word.empty()) {
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            float lineWidth = MeasureTextEx(gameFont, testLine.c_str(), gameFont.baseSize * 0.9f, 1).x;
            if (lineWidth <= eventTextWidth) {
                currentLine = testLine;
            } else {
                if (!currentLine.empty()) {
                    wrappedLines.push_back(currentLine);
                }
                wrappedLines.push_back(word);
                currentLine = "";
            }
        }
        if (!currentLine.empty()) {
            wrappedLines.push_back(currentLine);
        }
        float lineHeight = gameFont.baseSize * 0.9f + 3;
        for (size_t i = 0; i < wrappedLines.size() && textY + i * lineHeight < monsterCardBox.y + monsterCardBox.height - padding; i++) {
            DrawTextEx(gameFont, wrappedLines[i].c_str(), Vector2{textX, textY + i * lineHeight}, gameFont.baseSize * 0.9f, 1, textColor);
        }
    }
}

void GameScreen::drawDiceResultsBox() {
    // Draw box background
    DrawRectangleRec(diceResultsBox, {50, 50, 50, 255});
    DrawRectangleLinesEx(diceResultsBox, 1, WHITE);
    
    // Calculate responsive padding and spacing
    float padding = screenWidth * 0.005f;  // 0.5% padding
    float lineSpacing = screenHeight * 0.012f;  // 1.2% line spacing
    
    // Draw title
    DrawTextEx(gameFont, "DICE RESULTS", 
        Vector2{diceResultsBox.x + padding, diceResultsBox.y + padding}, 
        gameFont.baseSize, 1, titleColor);
    
    // Draw dice results to match terminal format: [!] [*] [ ]
    std::string resultsText = "";
    for (size_t i = 0; i < diceResults.size(); ++i) {
        resultsText += "[" + diceResults[i] + "]";
        if (i < diceResults.size() - 1) resultsText += " ";
    }
    
    DrawTextEx(gameFont, resultsText.c_str(), 
        Vector2{diceResultsBox.x + padding, diceResultsBox.y + gameFont.baseSize + padding + lineSpacing}, 
        gameFont.baseSize, 1, textColor);
    
    // Draw legend
    std::string legendText = "[!] = Power\n[*] = Strike\n[ ] = Empty";
    DrawTextEx(gameFont, legendText.c_str(), 
        Vector2{diceResultsBox.x + padding, diceResultsBox.y + gameFont.baseSize + padding + lineSpacing * 3}, 
        gameFont.baseSize, 1, GRAY);
}

void GameScreen::drawFrenzyMarkerBox() {
    // Draw box background
    DrawRectangleRec(frenzyMarkerBox, {50, 50, 50, 255});
    DrawRectangleLinesEx(frenzyMarkerBox, 1, WHITE);
    
    // Calculate responsive padding and spacing
    float padding = screenWidth * 0.005f;  // 0.5% padding
    float lineSpacing = screenHeight * 0.012f;  // 1.2% line spacing
    
    // Draw title
    DrawTextEx(gameFont, "FRENZIED MONSTER", 
        Vector2{frenzyMarkerBox.x + padding, frenzyMarkerBox.y + padding}, 
        gameFont.baseSize, 1, titleColor);
    
    // Draw frenzied monster name
    DrawTextEx(gameFont, currentFrenziedMonster.c_str(), 
        Vector2{frenzyMarkerBox.x + padding, frenzyMarkerBox.y + gameFont.baseSize + padding + lineSpacing}, 
        gameFont.baseSize, 1, textColor);
    
    // Load frenzied monster texture if needed
    if (!currentFrenziedMonster.empty() && !frenziedMonsterTextureLoaded) {
        loadFrenziedMonsterTexture();
    }
    
    // Draw monster image if texture is loaded
    if (frenziedMonsterTextureLoaded && frenziedMonsterTexture.id != 0) {
        // Scale image to fit in box with responsive margins
        float imageMargin = padding * 2;
        float textAreaHeight = gameFont.baseSize + padding + lineSpacing * 2;
        float scale = std::min((frenzyMarkerBox.width - imageMargin) / frenziedMonsterTexture.width, 
                             (frenzyMarkerBox.height - imageMargin - textAreaHeight) / frenziedMonsterTexture.height);
        float scaledWidth = frenziedMonsterTexture.width * scale;
        float scaledHeight = frenziedMonsterTexture.height * scale;
        
        Rectangle sourceRect = {0, 0, (float)frenziedMonsterTexture.width, (float)frenziedMonsterTexture.height};
        Rectangle destRect = {frenzyMarkerBox.x + (frenzyMarkerBox.width - scaledWidth) / 2,
                           frenzyMarkerBox.y + textAreaHeight,
                           scaledWidth, scaledHeight};
        
        DrawTexturePro(frenziedMonsterTexture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
        // Do NOT unload the texture here; keep it loaded until monster phase ends
    }
}

void GameScreen::drawItemTooltip() {
    if (itemTooltip.isVisible) {
        DrawRectangle(itemTooltip.position.x, itemTooltip.position.y, 200, 50, {0, 0, 0, 150});
        DrawTextEx(gameFont, itemTooltip.name.c_str(), 
            Vector2{itemTooltip.position.x + 10, itemTooltip.position.y + 10}, 
            gameFont.baseSize, 1, WHITE);
        DrawTextEx(gameFont, itemTooltip.description.c_str(), 
            Vector2{itemTooltip.position.x + 10, itemTooltip.position.y + 30}, 
            gameFont.baseSize, 1, GRAY);
    }
}

void GameScreen::handleMapClick(Vector2 mousePos) {
    if (!CheckCollisionPointRec(mousePos, mapArea)) return;
    
    // Check if any location was clicked
    for (const auto& [name, location] : mapLocations) {
        Vector2 locationPos = {mapArea.x + location.position.x, mapArea.y + location.position.y};
        if (CheckCollisionPointCircle(mousePos, locationPos, location.radius)) {
            selectedLocation = name;
            std::cout << "Selected location: " << name << std::endl;
            
            // Check if this is a guide location click
            if (selectedVillager && !availableGuideLocations.empty()) {
                auto it = std::find_if(availableGuideLocations.begin(), availableGuideLocations.end(),
                    [&name](const std::shared_ptr<Location>& loc) { return loc->getName() == name; });
                if (it != availableGuideLocations.end()) {
                    // Find the index of the clicked location
                    int locationIndex = std::distance(availableGuideLocations.begin(), it);
                    selectLocationToGuide(locationIndex);
                    return;
                } else {
                    // Clicked on a location that's not available for guide - cancel the action
                    cancelGuideAction();
                    return;
                }
            }
            
            // Execute action if one is selected
            if (!selectedAction.empty()) {
                executeAction(selectedAction, name);
            }
            break;
        }
    }
}

void GameScreen::handleActionClick(Vector2 mousePos) {
    if (currentPhase == MONSTER_PHASE) {
        return;
    }

    if (!CheckCollisionPointRec(mousePos, actionsPanel)) return;
    
    // Check if any action button was clicked
    for (auto& button : actionButtons) {
        if (CheckCollisionPointRec(mousePos, button.bounds) && button.isEnabled) {
            button.action();
            std::cout << "Selected action: " << button.name << std::endl;
            break;
        }
    }
}

void GameScreen::executeAction(const std::string& action, const std::string& location) {
    if (action == "Move") {
        moveHero(location);
    } else if (action == "Pick Up") {
        pickUpItem(location);
    } else if (action == "Advance") {
        advanceMonster(location);
    } else if (action == "Defeat") {
        defeatMonster(location);
    } else if (action == "Special Action") {
        specialAction();
    } else if (action == "End Turn") {
        endTurn();
    } else if (action == "Save") {
        saveGame();
    } else if (action == "Quit") {
        quitGame();
    }
    
    selectedAction.clear();
    availableMoveLocations.clear();
}

void GameScreen::moveHero(const std::string& location) {
    if (!currentHero || remainingActions <= 0) return;
    
    try {
        auto newLocation = gameMap->getLocation(location);
        if (!newLocation) return;
        if (currentHero->getCurrentLocation()->getName() == location) return;
        
        const auto& neighbors = currentHero->getCurrentLocation()->getNeighbors();
        bool isNeighbor = false;
        for (const auto& neighbor : neighbors) {
            if (neighbor && neighbor->getName() == location) {
                isNeighbor = true;
                break;
            }
        }
        if (!isNeighbor) return;

        // Store data needed for the action
        pendingMoveLocation = location;
        
        villagersToMove.clear();
        auto characters = currentHero->getCurrentLocation()->getCharacters();
        for (const auto& character : characters) {
            if (character != "Archeologist" && character != "Mayor" && character != "Scientist" && 
                character != "Courier" && character != "Dracula" && character != "Invisible man") {
                villagersToMove.push_back(character);
            }
        }
        
        if (!villagersToMove.empty()) {            
            // 1. Build the question string
            std::string question = "There are villagers here. Move them with you?";
            
            // 2. Define the actions for Yes and No using lambdas
            auto yesAction = [this]() { this->completeHeroMove(true); };
            auto noAction = [this]() { this->completeHeroMove(false); };

            // 3. Show the prompt
            showConfirmation(question, yesAction, noAction);

        } else {
            // No villagers, so complete the move immediately
            completeHeroMove(false);
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error during move: " << e.what() << std::endl;
    }
}

void GameScreen::completeHeroMove(bool withVillagers) {
    // 1. Get the target location using the stored "pending" variable.
    auto newLocation = gameMap->getLocation(pendingMoveLocation);
    if (!newLocation) return; // Safety check in case the location name is invalid.

    // 2. Check the player's decision about moving villagers.
    if (withVillagers && !villagersToMove.empty()) {
        std::cout << "Moving villagers with hero..." << std::endl;
        
        // Loop through the list of villagers that were at the hero's starting location.
        for (const auto& villagerName : villagersToMove) {
            try {
                auto villager = villagerManager.getVillager(villagerName);
                if (villager) {
                    // Capture perk count before move to detect perk grant
                    size_t prevPerkCount = currentHero->getPerkCards().size();
                    std::string villagerNameCopy = villager->getVillagerName();
                    // Move each villager to the same new location.
                    villager->move(newLocation, currentHero, &perkDeck);
                    // If villager left the board (reached safe place), show message
                    if (!villager->getCurrentLocation()) {
                        addGameMessage(villagerNameCopy + " has reached their safe place and left the game!");
                    }
                    // If a perk was granted to the guiding hero, show message
                    const auto& heroPerks = currentHero->getPerkCards();
                    if (heroPerks.size() > prevPerkCount) {
                        const auto& grantedPerk = heroPerks.back();
                        addGameMessage(currentHero->getPlayerName() + std::string(" (") + currentHero->getHeroName() + ") received perk card: " +
                                       PerkCard::perkTypeToString(grantedPerk.getType()) +
                                       " for helping " + villagerNameCopy + " reach their safe place!");
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Error moving villager " << villagerName << ": " << e.what() << std::endl;
            }
        }
    }
    
    // 3. Move the hero itself.
    currentHero->getCurrentLocation()->removeCharacter(currentHero->getHeroName());
    newLocation->addCharacter(currentHero->getHeroName());
    currentHero->setCurrentLocation(newLocation);
    
    // 4. Update the map display to show all characters in their new positions.
    initializeLocations();
    
    // 5. Consume one action point. This is only done *after* the move is complete.
    remainingActions--;
    if (remainingActions <= 0) {
        showEndTurnPrompt = true;
        endTurnPromptTimer = 3.0f;
    }
    
    std::cout << currentHero->getHeroName() << " (" << currentHero->getPlayerName() << ") moved to " << pendingMoveLocation << std::endl;

    // 6. Clean up the temporary variables used for this specific action.
    pendingMoveLocation.clear();
    villagersToMove.clear();
}

void GameScreen::startGuideAction() {
    if (!currentHero || remainingActions <= 0) return;
    
    try {
        // Clear previous state
        guidableVillagers.clear();
        guidableMoves.clear();
        selectedVillager = nullptr;
        availableGuideLocations.clear();
        
        auto heroLoc = currentHero->getCurrentLocation();
        const auto& heroNeighbors = heroLoc->getNeighbors();

        // Find all guidable villagers (same logic as in hero.cpp)
        for (const auto& locPair : gameMap->locations) {
            auto loc = locPair.second;
            const auto& characters = loc->getCharacters();
            for (const auto& character : characters) {
                if (character == "Archeologist" || character == "Mayor" || character == "Scientist" || character == "Courier" || character == "Dracula" || character == "Invisible man") continue;

                std::vector<std::shared_ptr<Location>> possibleMoves;

                if (loc->getName() == heroLoc->getName()) {
                    for (const auto& neighbor : heroNeighbors) {
                        possibleMoves.push_back(neighbor);
                    }
                } else {
                    const auto& vNeighbors = loc->getNeighbors();
                    for (const auto& vNeighbor : vNeighbors) {
                        if (vNeighbor->getName() == heroLoc->getName()) {
                            possibleMoves.push_back(heroLoc);
                            break;
                        }
                    }
                }

                if (!possibleMoves.empty()) {
                    try {
                        auto villager = villagerManager.getVillager(character);
                        guidableVillagers.push_back(villager);
                        guidableMoves.push_back(possibleMoves);
                    } catch (const std::exception& e) {
                        std::cout << e.what() << std::endl;
                    }
                }
            }
        }

        if (guidableVillagers.empty()) {
            guideMessage = "There are no villagers on the map that you can guide right now.";
            guideMessageTimer = 3.0f;
            return;
        }

        // Show villagers selection overlay
        showGuideVillagers = true;
        showGuideLocations = false;
        
    } catch (const std::exception& e) {
        std::cout << "Error starting guide action: " << e.what() << std::endl;
    }
}

void GameScreen::selectVillagerToGuide(int villagerIndex) {
    if (villagerIndex < 0 || villagerIndex >= static_cast<int>(guidableVillagers.size())) {
        return;
    }
    
    selectedVillager = guidableVillagers[villagerIndex];
    availableGuideLocations = guidableMoves[villagerIndex];
    
    showGuideVillagers = false;
    showGuideLocations = false; // Don't show overlay, show green circles on map instead
}

void GameScreen::selectLocationToGuide(int locationIndex) {
    if (!selectedVillager || locationIndex < 0 || locationIndex >= static_cast<int>(availableGuideLocations.size())) {
        return;
    }
    
    try {
        auto chosenLocation = availableGuideLocations[locationIndex];
        // Capture perk count before move to detect perk grant
        size_t prevPerkCount = currentHero->getPerkCards().size();
        std::string villagerNameCopy = selectedVillager->getVillagerName();
        selectedVillager->move(chosenLocation, currentHero, &perkDeck);
        // If villager left the board (reached safe place), show message
        if (!selectedVillager->getCurrentLocation()) {
            addGameMessage(villagerNameCopy + " has reached their safe place and left the game!");
        }
        // If a perk was granted to the guiding hero, show message
        const auto& heroPerks = currentHero->getPerkCards();
        if (heroPerks.size() > prevPerkCount) {
            const auto& grantedPerk = heroPerks.back();
            addGameMessage(currentHero->getPlayerName() + std::string(" (") + currentHero->getHeroName() + ") received perk card: " +
                           PerkCard::perkTypeToString(grantedPerk.getType()) +
                           " for helping " + villagerNameCopy + " reach their safe place!");
        }
        
        // Update map display
        initializeLocations();
        
        remainingActions--;
        if (remainingActions <= 0) {
            showEndTurnPrompt = true;
            endTurnPromptTimer = 3.0f;
        }
        
        // Clear guide state
        cancelGuideAction();
        
    } catch (const std::exception& e) {
        std::cout << "Error during guide: " << e.what() << std::endl;
        cancelGuideAction();
    }
}

void GameScreen::cancelGuideAction() {
    showGuideVillagers = false;
    showGuideLocations = false;
    guidableVillagers.clear();
    guidableMoves.clear();
    selectedVillager = nullptr;
    availableGuideLocations.clear();
    selectedAction = "";
}

void GameScreen::startPickUpAction(const std::string& location) {
    if (!currentHero || remainingActions <= 0) return;
    
    try {
        // Get items from the current location
        auto currentLoc = currentHero->getCurrentLocation();
        if (!currentLoc) return;

        itemWasPickedUpThisTurn = false;
        
        availableItems = currentLoc->getItems();
        pickUpLocation = location;
        
        if (availableItems.empty()) {
            pickUpMessage = "No items to pick up in " + location + ".";
            pickUpMessageTimer = 3.0f;
            return;
        }
        
        // Show pick up items overlay
        showPickUpItems = true;
        
    } catch (const std::exception& e) {
        std::cout << "Error starting pick up action: " << e.what() << std::endl;
    }
}

void GameScreen::pickUpItem(int itemIndex) {
    if (!currentHero || itemIndex < 0 || itemIndex >= static_cast<int>(availableItems.size())) {
        return;
    }
    
    try {
        // Get the item to pick up
        Item itemToPickUp = availableItems[itemIndex];
        
        // Add item to hero's inventory
        currentHero->addItem(itemToPickUp);
        
        // Remove item from location
        auto currentLoc = currentHero->getCurrentLocation();
        if (currentLoc) {
            currentLoc->removeItem(itemToPickUp);
        }
        
        // Remove item from available items list
        availableItems.erase(availableItems.begin() + itemIndex);
        
        // Update map display
        initializeLocations();

        itemWasPickedUpThisTurn = true;
        
        // If no more items, close the overlay and consume action
        if (availableItems.empty()) {
            cancelPickUpAction();
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error during pick up: " << e.what() << std::endl;
    }
}

void GameScreen::cancelPickUpAction() {
    // THE FIX: Check the flag. If true, consume one action.
    if (itemWasPickedUpThisTurn) {
        remainingActions--;
        if (remainingActions <= 0) {
            showEndTurnPrompt = true;
            endTurnPromptTimer = 3.0f;
        }
    }

    // Now, perform the normal cleanup.
    showPickUpItems = false;
    availableItems.clear();
    pickUpLocation.clear();
}

void GameScreen::pickUpItem(const std::string& location) {
    // This function is now deprecated - use startPickUpAction instead
    startPickUpAction(location);
}

void GameScreen::advanceMonster(const std::string& location) {
    if (!currentHero || remainingActions <= 0) return;
    
    try {
        // Check if we're at Precinct (evidence delivery) or coffin location
        if (currentHero->getCurrentLocation()->getName() == "Precinct") {
            // Evidence delivery for Invisible Man
            showAdvanceItemSelection = true;
            advanceDefeatAction = "advance";
            advanceDefeatTarget = "invisibleman";
        } else if (taskBoard.isCoffinLocation(currentHero->getCurrentLocation()->getName())) {
            // Coffin destruction for Dracula
            showAdvanceItemSelection = true;
            advanceDefeatAction = "advance";
            advanceDefeatTarget = "coffin";
        } else {
            addGameMessage("Cannot use advance action here.", 3.0f);
        }
    } catch (const std::exception& e) {
        addGameMessage(std::string("Error during advance: ") + e.what());
    }
}

void GameScreen::defeatMonster(const std::string& location) {
    if (!currentHero || remainingActions <= 0) return;
    
    try {
        // Check if we're at Invisible Man or Dracula location
        bool atInvisibleMan = false;
        for (const auto& c : currentHero->getCurrentLocation()->getCharacters()) {
            if (c == "Invisible man") atInvisibleMan = true;
        }
        
        if (atInvisibleMan) {
            // Defeat Invisible Man
            showDefeatItemSelection = true;
            advanceDefeatAction = "defeat";
            advanceDefeatTarget = "invisibleman";
        } else if (currentHero->getCurrentLocation() == dracula->getCurrentLocation()) {
            // Defeat Dracula
            showDefeatItemSelection = true;
            advanceDefeatAction = "defeat";
            advanceDefeatTarget = "dracula";
        } else {
            addGameMessage("No monster to defeat at this location.", 3.0f);
        }
    } catch (const std::exception& e) {
        addGameMessage(std::string("Error during defeat: ") + e.what());
    }
}

void GameScreen::specialAction() {
    if (!currentHero || remainingActions <= 0) {
        if (remainingActions <= 0) {
            addGameMessage("You have no actions left.", 3.0f);
        }
        return;
    }

    std::string heroName = currentHero->getHeroName();

    if (heroName == "Mayor" || heroName == "Scientist") {
        // --- MAYOR & SCIENTIST: Show message, no action cost ---
        addGameMessage(heroName + " has no special action.", 3.0f);
        // This does NOT consume an action.

    } else if (heroName == "Courier") {
        // --- COURIER: Perform special action, costs 1 action ---
        try {
            // Ensure the other hero is set before the action is called
            currentHero->setOtherHero(otherHero);
            currentHero->specialAction(); // This will throw if it fails

            remainingActions--;

            // The action cost is handled inside Courier::specialAction()
            if (remainingActions <= 0) {
                showEndTurnPrompt = true;
                endTurnPromptTimer = 3.0f;
            }
            initializeLocations(); // Update map display
        } catch (const std::exception& e) {
            addGameMessage(e.what(), 3.0f);
        }

    } else if (heroName == "Archeologist") {
        // --- ARCHEOLOGIST: Start the UI flow, costs 1 action if items are taken ---
        startArcheologistSpecialAction();
    }
}

void GameScreen::usePerkCard() {
    if (!currentHero || remainingActions <= 0) return;
    
    auto perkCards = currentHero->getPerkCards();
    if (perkCards.empty()) {
        addGameMessage("You have no perk cards to use.", 3.0f);
        return;
    }
    openPerkSelectionOverlay();
}

void GameScreen::openPerkSelectionOverlay() {
    showPerkSelection = true;
}

void GameScreen::endTurn() {
    std::cout << "Ending turn" << std::endl;
    
    // Switch to monster phase
    currentPhase = MONSTER_PHASE;
    showMonsterPhaseUI = true;
    
    // Reset for next turn
    selectedAction.clear();
    selectedLocation.clear();
    
    // Execute monster phase first to get real results
    executeMonsterTurn();
    
    // Now display the results
    showMonsterPhaseResults();
}

// Starts the process for the Archeologist's special action.
void GameScreen::startArcheologistSpecialAction() {
    archeologistTargetLocations.clear();
    const auto& neighbors = currentHero->getCurrentLocation()->getNeighbors();

    // Find all neighboring locations that have at least one item.
    for (const auto& neighbor : neighbors) {
        if (neighbor && !neighbor->getItems().empty()) {
            archeologistTargetLocations.push_back(neighbor);
        }
    }

    if (archeologistTargetLocations.empty()) {
        addGameMessage("No items found in any neighboring locations.", 3.0f);
        return; 
    }

    // If locations with items are found, show the selection screen.
    showArcheologistLocationChoice = true;
    archeologistPickedUpItem = false; // Reset for the new action.
}

// Ends and cleans up the Archeologist's special action state.
void GameScreen::endArcheologistSpecialAction() {
    if (archeologistPickedUpItem) {
        remainingActions--;
        if (remainingActions <= 0) {
            showEndTurnPrompt = true;
            endTurnPromptTimer = 3.0f;
        }
    }
    showArcheologistLocationChoice = false;
    showArcheologistItemChoice = false;
    archeologistTargetLocations.clear();
    archeologistLocationButtons.clear();
    archeologistItemButtons.clear();
    archeologistChosenLocation = nullptr;
}

// Draws the overlay for choosing a location.
void GameScreen::drawArcheologistLocationChoiceOverlay() {
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    float boxWidth = screenWidth * 0.5f;
    float boxHeight = screenHeight * 0.5f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    std::string title = "Choose a location to search:";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    archeologistLocationButtons.clear();
    float buttonHeight = 50;
    float spacing = 15;
    float startY = boxY + 80;

    for (size_t i = 0; i < archeologistTargetLocations.size(); ++i) {
        Rectangle btnRect = {boxX + 40, startY + i * (buttonHeight + spacing), boxWidth - 80, buttonHeight};
        archeologistLocationButtons.push_back(btnRect);
        
        bool isHovered = CheckCollisionPointRec(GetMousePosition(), btnRect);
        DrawRectangleRec(btnRect, isHovered ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(btnRect, 2, WHITE);

        const std::string& locName = archeologistTargetLocations[i]->getName();
        Vector2 textSize = MeasureTextEx(gameFont, locName.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, locName.c_str(), {btnRect.x + (btnRect.width - textSize.x) / 2, btnRect.y + (btnRect.height - textSize.y) / 2}, gameFont.baseSize, 1, WHITE);
    }
}

void GameScreen::drawArcheologistItemChoiceOverlay() {
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    float boxWidth = screenWidth * 0.6f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    std::string title = "Items at " + archeologistChosenLocation->getName();
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    archeologistItemButtons.clear();
    const auto& items = archeologistChosenLocation->getItems();
    float itemHeight = 40;
    float spacing = 10;
    float startY = boxY + 80;

    for (size_t i = 0; i < items.size(); ++i) {
        Rectangle itemRect = {boxX + 40, startY + i * (itemHeight + spacing), boxWidth - 80, itemHeight};
        archeologistItemButtons.push_back(itemRect);
        
        bool isHovered = CheckCollisionPointRec(GetMousePosition(), itemRect);
        DrawRectangleRec(itemRect, isHovered ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(itemRect, 2, WHITE);

        std::string itemText = items[i].getItemName() + " (Power: " + std::to_string(items[i].getPower()) + ")";
        DrawTextEx(gameFont, itemText.c_str(), {itemRect.x + 20, itemRect.y + 10}, gameFont.baseSize, 1, WHITE);
    }

    // "Done" button
    float doneWidth = 120, doneHeight = 50;
    archeologistDoneButton = {boxX + (boxWidth - doneWidth) / 2, boxY + boxHeight - doneHeight - 20, doneWidth, doneHeight};
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), archeologistDoneButton);
    DrawRectangleRec(archeologistDoneButton, isHovered ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(archeologistDoneButton, 2, WHITE);
    DrawTextEx(gameFont, "Done", {archeologistDoneButton.x + 35, archeologistDoneButton.y + 15}, gameFont.baseSize, 1, WHITE);
}

// Handles clicks for the location choice overlay.
void GameScreen::handleArcheologistLocationChoiceClick(Vector2 mousePos) {
    for (size_t i = 0; i < archeologistLocationButtons.size(); ++i) {
        if (CheckCollisionPointRec(mousePos, archeologistLocationButtons[i])) {
            archeologistChosenLocation = archeologistTargetLocations[i];
            showArcheologistLocationChoice = false;
            showArcheologistItemChoice = true;
            return;
        }
    }
}

// Handles clicks for the item choice overlay.
void GameScreen::handleArcheologistItemChoiceClick(Vector2 mousePos) {
    auto itemsCopy = archeologistChosenLocation->getItems();
    for (size_t i = 0; i < itemsCopy.size(); ++i) {
        if (CheckCollisionPointRec(mousePos, archeologistItemButtons[i])) {
            Item selectedItem = itemsCopy[i];
            currentHero->addItem(selectedItem);
            archeologistChosenLocation->removeItem(selectedItem);
            archeologistPickedUpItem = true; // Mark that an item was taken
            return;
        }
    }

    // Check "Done" button
    if (CheckCollisionPointRec(mousePos, archeologistDoneButton)) {
        endArcheologistSpecialAction();
    }
}

void GameScreen::startMonsterPhase() {
    std::cout << "Starting monster phase..." << std::endl;
}

void GameScreen::advanceMonsterPhase() {
    std::cout << "Advancing monster phase..." << std::endl;
    
    // Monster phase is already executed, just switch back to hero phase
    currentPhase = HERO_PHASE;
    showMonsterPhaseUI = false;
    currentTurn++;
    remainingActions = maxActions;
    
    // Switch heroes for next turn
    Hero* temp = currentHero;
    currentHero = otherHero;
    otherHero = temp;
    
    std::cout << "Monster phase complete. Starting turn " << currentTurn << std::endl;
}

void GameScreen::executeMonsterTurn() {
    std::cout << "Executing monster turn..." << std::endl;
    
    // Skip monster phase if hero has Break of Dawn perk
    if (currentHero->shouldSkipNextMonsterPhase()) {
        addGameMessage("Monster phase skipped due to Break of Dawn perk card!");
        currentHero->setSkipNextMonsterPhase(false);
        return;
    }
    
    // Store initial state for comparison
    int initialTerrorLevel = terrorTracker.getLevel();
    std::string initialFrenziedMonster = currentFrenziedMonster;
    
    // Capture pre-monster phase state
    capturePreMonsterPhaseState();
    
    // Execute monster phase using MonsterManager
    try {
        std::vector<std::string> diceResults;
        
        monsterManager.MonsterPhase(*gameMap, *itemBag, 
                                  static_cast<Dracula*>(dracula.get()), 
                                  static_cast<InvisibleMan*>(invisibleMan.get()), 
                                  *frenzyMarker, currentHero, terrorTracker,
                                  static_cast<Archeologist*>(dynamic_cast<Archeologist*>(currentHero)),
                                  static_cast<Mayor*>(dynamic_cast<Mayor*>(currentHero)),
                                  static_cast<Courier*>(dynamic_cast<Courier*>(currentHero)),
                                  static_cast<Scientist*>(dynamic_cast<Scientist*>(currentHero)),
                                  villagerManager, diceResults, &perkDeck, currentHero, otherHero
                            #ifndef TERMINAL
                                , this
                            #endif
    );
        
        // Store dice results for display
        this->diceResults = diceResults;
        
        // Update current frenzied monster after monster phase
        std::string newFrenziedMonster = "";
        if (frenzyMarker && frenzyMarker->getCurrentFrenzied()) {
            newFrenziedMonster = frenzyMarker->getCurrentFrenzied()->getMonsterName();
        }
        
        if (newFrenziedMonster != currentFrenziedMonster) {
            currentFrenziedMonster = newFrenziedMonster;
            if (!currentFrenziedMonster.empty()) {
                loadFrenziedMonsterTexture();
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error during monster phase: " << e.what() << std::endl;
        addGameMessage("Error during monster phase: " + std::string(e.what()));
    }
    
    initializeLocations();

    if (terrorTracker.getLevel() >= 5) {
        setGameOver("DEFEAT!", "The terror level has reached its peak!");
    } else if (monsterManager.isEmpty()) {
        setGameOver("DEFEAT!", "Time has run out! The monster deck is empty.");
    }
}

void GameScreen::drawPerkSelectionOverlay() {
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    float boxWidth = screenWidth * 0.75f;
    float boxHeight = screenHeight * 0.75f;
    float boxX = (screenWidth - boxWidth) / 2.0f;
    float boxY = (screenHeight - boxHeight) / 2.0f;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.08f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    std::string title = "Choose a Perk Card";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 18}, titleFont.baseSize, 1, WHITE);

    // Grid of cards
    const auto& perks = currentHero->getPerkCards();
    const int columns = 3;
    const float padding = 20.0f;
    const float cardW = (boxWidth - padding * (columns + 1)) / columns;
    const float cardH = cardW * 1.4f;
    float gridTop = boxY + 70.0f;

    perkSelectionButtons.clear();

    for (size_t i = 0; i < perks.size(); ++i) {
        int row = static_cast<int>(i) / columns;
        int col = static_cast<int>(i) % columns;
        float x = boxX + padding + col * (cardW + padding);
        float y = gridTop + row * (cardH + padding);
        Rectangle r = {x, y, cardW, cardH};
        perkSelectionButtons.push_back(r);

        bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.04f, 6, hovered ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(r, 2, WHITE);

        // Image
        auto imgName = getPerkCardImageName(perks[i].getType());
        auto it = perkCardImages.find(imgName);
        if (it != perkCardImages.end()) {
            Texture2D tex = it->second;
            float imgW = r.width - 20.0f;
            float imgH = imgW * (static_cast<float>(tex.height) / static_cast<float>(tex.width));
            if (imgH > r.height * 0.55f) {
                imgH = r.height * 0.55f;
                imgW = imgH * (static_cast<float>(tex.width) / static_cast<float>(tex.height));
            }
            float imgX = r.x + (r.width - imgW) / 2.0f;
            float imgY = r.y + 10.0f;
            DrawTexturePro(tex, {0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)}, {imgX, imgY, imgW, imgH}, {0, 0}, 0.0f, WHITE);
        }

        // Name
        std::string name = PerkCard::perkTypeToString(perks[i].getType());
        Vector2 nameSize = MeasureTextEx(gameFont, name.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, name.c_str(), {r.x + (r.width - nameSize.x) / 2.0f, r.y + r.height * 0.6f}, gameFont.baseSize, 1, WHITE);

        // Description
        std::string desc = perks[i].getDescription();
        // simple wrap: draw as 2 lines max
        float textY = r.y + r.height * 0.6f + nameSize.y + 6.0f;
        DrawTextEx(gameFont, desc.c_str(), {r.x + 10.0f, textY}, gameFont.baseSize, 1, {230,230,230,255});
    }

    // Cancel button
    float btnW = 140.0f, btnH = 44.0f;
    perkCancelButton = {boxX + boxWidth - btnW - 20.0f, boxY + boxHeight - btnH - 20.0f, btnW, btnH};
    bool hovered = CheckCollisionPointRec(GetMousePosition(), perkCancelButton);
    DrawRectangleRec(perkCancelButton, hovered ? dangerColor : buttonColor);
    DrawRectangleLinesEx(perkCancelButton, 2, WHITE);
    std::string cancel = "Cancel";
    Vector2 csz = MeasureTextEx(gameFont, cancel.c_str(), gameFont.baseSize, 1);
    DrawTextEx(gameFont, cancel.c_str(), {perkCancelButton.x + (perkCancelButton.width - csz.x) / 2.0f, perkCancelButton.y + (perkCancelButton.height - csz.y) / 2.0f}, gameFont.baseSize, 1, WHITE);
}

void GameScreen::handlePerkSelectionClick(Vector2 mousePos) {
    // Cancel
    if (CheckCollisionPointRec(mousePos, perkCancelButton)) {
        showPerkSelection = false;
        return;
    }

    const auto& perks = currentHero->getPerkCards();
    for (size_t i = 0; i < perkSelectionButtons.size() && i < perks.size(); ++i) {
        if (CheckCollisionPointRec(mousePos, perkSelectionButtons[i])) {
            // Special handling for Visit from the Detective
            if (perks[i].getType() == PerkType::VisitFromTheDetective) {
                showPerkSelection = false;
                showVisitFromDetectiveSelection = true;
                return;
            }

            try {
                currentHero->usePerkCard(i, *gameMap, villagerManager, &perkDeck,
                                         static_cast<InvisibleMan*>(invisibleMan.get()),
                                         itemBag, otherHero,
                                         static_cast<Dracula*>(dracula.get()));

                remainingActions = currentHero->getRemainingActions();
                maxActions = currentHero->getMaxActions();

                addGameMessage("Used perk card: " + PerkCard::perkTypeToString(perks[i].getType()));
                initializeLocations();
            } catch (const std::exception& e) {
                addGameMessage(std::string("Error using perk card: ") + e.what());
            }
            showPerkSelection = false;
            break;
        }
    }
}



void GameScreen::showMonsterPhaseResults() {
    // Show monster phase results after execution
    try {        
        // Show important game messages based on what happened
        addImportantGameMessages(terrorTracker.getLevel(), currentFrenziedMonster);
        
        // Compare state changes and add additional messages
        compareAndAddStateChangeMessages();
        
    } catch (const std::exception& e) {
        std::cout << "Error showing monster phase results: " << e.what() << std::endl;
        addGameMessage("Error: Could not show monster phase results");
    }
}

void GameScreen::saveGame() {
    std::cout << "Saving game..." << std::endl;
    openSaveSlots();
}

void GameScreen::saveCurrentGame() {
    try {
        GameState snapshot = buildCurrentGameStateSnapshot();
    } catch (const std::exception& e) {
        std::cout << "Error building save snapshot: " << e.what() << std::endl;
    }
}

void GameScreen::loadGameFromSlot(int slotNumber) {
    // This would integrate with the Game class load logic
    std::cout << "Loading game from slot " << slotNumber << std::endl;
    // Implementation would restore game state from save file
}

void GameScreen::restoreFromGameState(const GameState& gameState) {
    try {
        // Restore players and heroes
        std::string p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHeroName;
        int p1Garlic, p2Garlic;
        gameState.restorePlayerInfo(p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHeroName, p1Garlic, p2Garlic);

        startingPlayer = startPlayer;
        players.clear();
        players.push_back(PlayerInfo(p1Name, (startPlayer == p1Name ? startHero : otherHeroName), std::to_string(p1Garlic)));
        players.push_back(PlayerInfo(p2Name, (startPlayer == p2Name ? startHero : otherHeroName), std::to_string(p2Garlic)));

        int turnCount, terrorLevel;
        bool running;
        gameState.restoreGameState(turnCount, terrorLevel, running);
        currentTurn = turnCount;
        terrorTracker.setLevel(terrorLevel);
        gameRunning = running;

        // Rebuild map-dependent visuals
        initializeLocations();

        // Restore heroes' detailed state
        auto h1 = gameState.getHeroState(true);
        auto h2 = gameState.getHeroState(false);
        // Map hero objects by name for assignment
        if (currentHero) {
            auto loc = gameMap->getLocation(h1.currentLocationName);
            if (loc) { currentHero->setCurrentLocation(loc); }
            currentHero->setRemainingActions(h1.remainingActions);
            currentHero->setSkipNextMonsterPhase(h1.skipNextMonsterPhase);
            // Clear and re-add items/perks is not available; append is acceptable in current codebase
        }
        if (otherHero) {
            auto loc2 = gameMap->getLocation(h2.currentLocationName);
            if (loc2) { otherHero->setCurrentLocation(loc2); }
            otherHero->setRemainingActions(h2.remainingActions);
            otherHero->setSkipNextMonsterPhase(h2.skipNextMonsterPhase);
        }

        // Restore monsters
        auto dr = gameState.getMonsterState(true);
        auto im = gameState.getMonsterState(false);
        if (dracula) {
            auto l = gameMap->getLocation(dr.currentLocationName);
            if (l) dracula->setCurrentLocation(l);
        }
        if (invisibleMan) {
            auto l = gameMap->getLocation(im.currentLocationName);
            if (l) invisibleMan->setCurrentLocation(l);
        }

        // Restore villagers
        villagerManager = VillagerManager();
        for (const auto& vs : gameState.getVillagerStates()) {
            if (!vs.currentLocationName.empty() && vs.currentLocationName != "Defeated") {
                auto l = gameMap->getLocation(vs.currentLocationName);
                if (l) villagerManager.addVillager(vs.villagerName, l);
            }
        }

        // Taskboard
        auto tb = gameState.getTaskBoardState();
        taskBoard.setDraculaCoffins(tb.draculaCoffins);
        taskBoard.setInvisibleManCluesDelivered(tb.invisibleManCluesDelivered);
        taskBoard.setDraculaDefeat(tb.draculaDefeat);
        taskBoard.setInvisibleManDefeat(tb.invisibleManDefeat);
        taskBoard.setInvisibleManDefeated(tb.invisibleManDefeated);

        // Frenzy
        int frenzyLevel = gameState.getFrenzyLevel();
        if (frenzyMarker) {
            // 1=Dracula, 2=Invisible Man, else none
            if (frenzyLevel == 1) {
                frenzyMarker->setCurrentFrenzied(static_cast<Dracula*>(dracula.get()));
            } else if (frenzyLevel == 2) {
                frenzyMarker->setCurrentFrenzied(static_cast<InvisibleMan*>(invisibleMan.get()));
            }
            currentFrenziedMonster = frenzyMarker->getCurrentFrenzied() ? frenzyMarker->getCurrentFrenzied()->getMonsterName() : "";
        }

        // Items on map
        if (itemBag && gameMap) {
        }

        // Rebuild map characters for drawing
        initializeLocations();
        addGameMessage("Game loaded.", 2.0f);
    } catch (const std::exception& e) {
        std::cout << "Error restoring from state: " << e.what() << std::endl;
        addGameMessage("Failed to restore game.");
    }
}

void GameScreen::openSaveSlots() {
    showSaveSlots = true;
    saveSlotButtons.clear();
    // Layout similar to overlays: centered modal
    float overlayWidth = screenWidth * 0.6f;
    float overlayHeight = screenHeight * 0.7f;
    float overlayX = (screenWidth - overlayWidth) / 2;
    float overlayY = (screenHeight - overlayHeight) / 2;

    // Create 5 slot button rects stacked
    float padding = screenHeight * 0.02f;
    float buttonHeight = (overlayHeight - padding * 6 - 80) / 5.0f; // leave space for footer buttons
    float buttonWidth = overlayWidth - padding * 2;
    for (int i = 0; i < 5; ++i) {
        Rectangle r{overlayX + padding, overlayY + padding + i * (buttonHeight + padding), buttonWidth, buttonHeight};
        saveSlotButtons.push_back(r);
    }
    // Footer buttons
    float footerY = overlayY + overlayHeight - padding - 40;
    float footerW = (buttonWidth - padding) / 2.0f;
    saveBackToGameButton = {overlayX + padding, footerY, footerW, 40};
    saveGoToMenuButton = {overlayX + padding + footerW + padding, footerY, footerW, 40};
}

void GameScreen::drawSaveSlotsOverlay() {
    // Dark backdrop
    DrawRectangle(0, 0, screenWidth, screenHeight, {0,0,0,180});

    float overlayWidth = screenWidth * 0.6f;
    float overlayHeight = screenHeight * 0.7f;
    float overlayX = (screenWidth - overlayWidth) / 2;
    float overlayY = (screenHeight - overlayHeight) / 2;
    DrawRectangleRounded({overlayX, overlayY, overlayWidth, overlayHeight}, 0.05f, 8, {60,60,60,255});
    DrawRectangleLinesEx({overlayX, overlayY, overlayWidth, overlayHeight}, 2, WHITE);

    // Title
    std::string title = "SAVE GAME";
    Vector2 tsize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {overlayX + (overlayWidth - tsize.x)/2, overlayY + 12}, titleFont.baseSize, 1, titleColor);

    // Slots list
    auto slots = saveManager->getSaveSlots();
    for (int i = 0; i < 5 && i < (int)saveSlotButtons.size(); ++i) {
        const Rectangle& r = saveSlotButtons[i];
        bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        Color col = hovered ? buttonHoverColor : buttonColor;
        DrawRectangleRec(r, col);
        DrawRectangleLinesEx(r, 1, WHITE);

        // Compose slot label
        const SaveSlot& s = slots[i];
        std::string line1 = std::string("Slot ") + std::to_string(i+1) + (s.hasSave ? std::string(" - ") + s.saveName : " - EMPTY");
        std::string line2 = s.hasSave ? (s.saveDate + " | Turn " + std::to_string(s.turnCount) + ", Terror " + std::to_string(s.terrorLevel)) : "Click to save here";
        Vector2 l1 = MeasureTextEx(gameFont, line1.c_str(), gameFont.baseSize * 1.1f, 1);
        DrawTextEx(gameFont, line1.c_str(), {r.x + 12, r.y + 8}, gameFont.baseSize * 1.1f, 1, WHITE);
        DrawTextEx(gameFont, line2.c_str(), {r.x + 12, r.y + 12 + gameFont.baseSize * 1.1f}, gameFont.baseSize * 0.9f, 1, GRAY);
    }

    // Footer buttons
    bool backHover = CheckCollisionPointRec(GetMousePosition(), saveBackToGameButton);
    bool menuHover = CheckCollisionPointRec(GetMousePosition(), saveGoToMenuButton);
    DrawRectangleRec(saveBackToGameButton, backHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(saveBackToGameButton, 1, WHITE);
    DrawTextEx(gameFont, "Back to Game", {saveBackToGameButton.x + 10, saveBackToGameButton.y + 10}, gameFont.baseSize, 1, WHITE);

    DrawRectangleRec(saveGoToMenuButton, menuHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(saveGoToMenuButton, 1, WHITE);
    DrawTextEx(gameFont, "Main Menu", {saveGoToMenuButton.x + 10, saveGoToMenuButton.y + 10}, gameFont.baseSize, 1, WHITE);
}

void GameScreen::handleSaveSlotsClick(Vector2 mousePos) {
    // Footer actions
    if (CheckCollisionPointRec(mousePos, saveBackToGameButton)) {
        showSaveSlots = false;
        return;
    }
    if (CheckCollisionPointRec(mousePos, saveGoToMenuButton)) {
        showSaveSlots = false;
        quitGame();
        return;
    }

    // Slot clicks
    auto slots = saveManager->getSaveSlots();
    for (int i = 0; i < 5 && i < (int)saveSlotButtons.size(); ++i) {
        if (CheckCollisionPointRec(mousePos, saveSlotButtons[i])) {
            int slotNumber = i + 1;
            bool willOverwrite = saveManager->hasSave(slotNumber);
            if (willOverwrite) {
                showConfirmation(
                    std::string("Overwrite slot ") + std::to_string(slotNumber) + "?",
                    [this, slotNumber]() {
                        try {
                            GameState snapshot = buildCurrentGameStateSnapshot();
                            std::string saveName = "Save " + std::to_string(slotNumber);
                            if (saveManager->saveGame(snapshot, slotNumber, saveName)) {
                                addGameMessage("Game saved to slot " + std::to_string(slotNumber) + ".", 2.0f);
                            } else {
                                addGameMessage("Failed to save game.", 3.0f);
                            }
                        } catch (const std::exception& e) {
                            addGameMessage("Error during save.");
                        }
                        showSaveSlots = false;
                    },
                    [this]() {
                        // Do nothing on cancel
                    }
                );
            } else {
                try {
                    GameState snapshot = buildCurrentGameStateSnapshot();
                    std::string saveName = "Save " + std::to_string(slotNumber);
                    if (saveManager->saveGame(snapshot, slotNumber, saveName)) {
                        addGameMessage("Game saved to slot " + std::to_string(slotNumber) + ".", 2.0f);
                    } else {
                        addGameMessage("Failed to save game.", 3.0f);
                    }
                } catch (const std::exception& e) {
                    addGameMessage("Error during save.");
                }
                showSaveSlots = false;
            }
            return;
        }
    }
}

GameState GameScreen::buildCurrentGameStateSnapshot() {
    GameState gs;
    try {
        // Players
        std::string p1 = players.size() > 0 ? players[0].name : "Player1";
        std::string p2 = players.size() > 1 ? players[1].name : "Player2";
        std::string startP = startingPlayer;
        std::string otherP = (players.size() > 1 ? (players[0].name == startingPlayer ? players[1].name : players[0].name) : p2);
        std::string startHero = currentHero ? currentHero->getHeroName() : "";
        std::string otherHeroName = otherHero ? otherHero->getHeroName() : "";
        int p1Garlic = players.size() > 0 ? players[0].garlicTime : 0;
        int p2Garlic = players.size() > 1 ? players[1].garlicTime : 0;
        gs.setPlayerInfo(p1, p2, startP, otherP, startHero, otherHeroName, p1Garlic, p2Garlic);

        // Core game state
        gs.setGameState(currentTurn, terrorTracker.getLevel(), gameRunning);
        gs.setCurrentHeroIndex(0); // currentHero is the active hero in this UI

        // Heroes
        if (currentHero) gs.setHeroState(currentHero, true);
        if (otherHero) gs.setHeroState(otherHero, false);

        // Monsters
        if (dracula) gs.setMonsterState(dracula.get(), true);
        if (invisibleMan) gs.setMonsterState(invisibleMan.get(), false);

        // Villagers
        gs.setVillagerStates(villagerManager);

        // Items and map
        if (itemBag && gameMap) {
            gs.setItemStates(*itemBag, *gameMap);
            gs.setMapState(*gameMap);
        }

        // Task board
        gs.setTaskBoardState(taskBoard);

        // Monster manager and perk deck
        gs.setMonsterManagerState(monsterManager);
        gs.setPerkDeckState(perkDeck);

        // Frenzy marker
        if (frenzyMarker) gs.setFrenzyMarkerState(*frenzyMarker);
    } catch (const std::exception& e) {
        std::cout << "Error building snapshot: " << e.what() << std::endl;
    }
    return gs;
}

std::string GameScreen::convertMonsterCardNameToImage(const std::string& cardName) {
    // Convert monster card names to image filenames
    if (cardName == "Form Of The Bat") return "FormOfTheBat.png";
    if (cardName == "Sunrise") return "Sunrise.png";
    if (cardName == "Thief") return "Thief.png";
    if (cardName == "The Delivery") return "TheDelivery.png";
    if (cardName == "Fortune Teller") return "FortuneTeller.png";
    if (cardName == "Former Employer") return "FomerEmployer.png";
    if (cardName == "Hurried Assistant") return "HurriedAssistant.png";
    if (cardName == "The Innocent") return "TheInnocent.png";
    if (cardName == "Egyptian Expert") return "EgyptianExpert.png";
    if (cardName == "The Ichthyologist") return "TheIchtyologist.png";
    if (cardName == "Hypnotic Gaze") return "HypnoticGaze.png";
    if (cardName == "On The Move") return "OnTheMove.png";
    
    // Default fallback
    return "FormOfTheBat.png";
}

void GameScreen::quitGame() {
   std::cout << "Quitting game and returning to main menu..." << std::endl;
    returnToMenu = true; // New, consistent way
}

void GameScreen::updateItemTooltip(const std::string& locationName, const std::vector<Item>& items) {
    itemTooltip.locationName = locationName;
    itemTooltip.items = items;
    itemTooltip.isVisible = true;
    itemTooltip.displayTime = 0.0f;
}

void GameScreen::startHeroDefense(Hero* hero,
                                  std::function<void(int)> onItemSelected,
                                  std::function<void()> onCanceled) {
    if (!hero) return;

    // Store the defending hero and callbacks
    defendingHero = hero;
    onItemSelectedIndex = onItemSelected;
    onDefenseCanceled = onCanceled;

    // Show the Yes/No choice first
    isHeroDefenseChoiceActive = true;
}

void GameScreen::useItemForDefense(const Item& item) {
    std::cout << "Warning: useItemForDefense(const Item&) is deprecated. Use useItemForDefenseByIndex(int) instead." << std::endl;
}

void GameScreen::useItemForDefenseByIndex(int itemIndex) {
    try {
        // Check if currentHero is valid
        if (!currentHero) {
            std::cout << "Error: No current hero to use item!" << std::endl;
            addGameMessage("Error: No current hero to use item!");
            return;
        }
        
        // Check if item index is valid
        const auto& heroItems = currentHero->getItems();
        if (itemIndex < 0 || itemIndex >= static_cast<int>(heroItems.size())) {
            std::cout << "Error: Invalid item index " << itemIndex << "!" << std::endl;
            addGameMessage("Error: Invalid item index!");
            return;
        }
        
        // Get the item to use
        const Item& itemToUse = heroItems[itemIndex];
        // Scientist ability applies when using an item
        if (currentHero->getHeroName() == std::string("Scientist")) {
            currentHero->ability(static_cast<size_t>(itemIndex));
        }
        
        // Remove the used item from hero's inventory
        currentHero->removeItem(itemIndex);
        
        // Show success message
        addGameMessage("Hero successfully defended using " + itemToUse.getItemName() + "!");
    } catch (const std::exception& e) {
        std::cout << "Error in useItemForDefenseByIndex: " << e.what() << std::endl;
        addGameMessage("Error: Could not use item for defense");
    }
}

void GameScreen::updateTerrorLevel(int level) {
    currentTerrorLevel = level;
    terrorTracker.setLevel(level);
}

void GameScreen::handleItemHover(Vector2 mousePos) {
    // Check if any item is hovered
    for (const auto& [name, location] : mapLocations) {
        if (!location.items.empty()) {
            Vector2 itemPos = {mapArea.x + location.position.x, mapArea.y + location.position.y + 10};
            if (CheckCollisionPointCircle(mousePos, itemPos, 6)) {
                itemTooltip.name = name;
                itemTooltip.description = "Items at " + name;
                itemTooltip.position = itemPos;
                itemTooltip.isVisible = true;
                itemTooltip.displayTime = 0.0f;
                break;
            }
        }
    }
}

void GameScreen::handleMapRightClick(Vector2 mousePos) {
    // If help menu is active, ignore right-clicks
    if (helpMenuActive) return;

    // Only consider right-clicks within the map area
    if (!CheckCollisionPointRec(mousePos, mapArea)) return;

    // Find the first location under the cursor
    std::string clickedLocationName;
    for (const auto& [name, location] : mapLocations) {
        Vector2 locationPos = {mapArea.x + location.position.x, mapArea.y + location.position.y};
        if (CheckCollisionPointCircle(mousePos, locationPos, location.radius)) {
            clickedLocationName = name;
            break;
        }
    }

    if (clickedLocationName.empty()) {
        return; // Not on a location
    }

    // Toggle logic: if already open for this location, close; otherwise open/switch
    if (locationPopupOpen && locationPopupLocationName == clickedLocationName) {
        locationPopupOpen = false;
        locationPopupLocationName.clear();
    } else {
        locationPopupLocationName = clickedLocationName;
        locationPopupOpen = true;
        // Ensure other overlays are hidden while modal is open
        showHeroInventoryOverlay = false;
    }
}

void GameScreen::handleHeroInventoryHover(Vector2 mousePos) {
    // By default, do not show the overlay
    showHeroInventoryOverlay = false;
    hoveredHero = nullptr;

    // The overlay should not appear if the help menu is active
    if (helpMenuActive) return;

    // Check if the mouse is over the main hero info panel
    if (!CheckCollisionPointRec(mousePos, heroInfoPanel)) {
        return;
    }

    // Define the sub-panel areas for each hero
    float leftWidth = heroInfoPanel.width * 0.49f;
    float rightWidth = heroInfoPanel.width * 0.49f;
    float gap = heroInfoPanel.width * 0.02f;
    float padding = screenWidth * 0.005f;

    Rectangle leftPanel = {heroInfoPanel.x + padding, heroInfoPanel.y + padding, leftWidth - 10, heroInfoPanel.height - padding * 2};
    Rectangle rightPanel = {heroInfoPanel.x + leftWidth + gap + padding - 10, heroInfoPanel.y + padding , rightWidth - 10, heroInfoPanel.height - padding * 2};

    // Determine which hero's panel is being hovered over
    Hero* targetHero = nullptr;
    if (CheckCollisionPointRec(mousePos, leftPanel)) {
        targetHero = currentHero;
    } else if (CheckCollisionPointRec(mousePos, rightPanel)) {
        targetHero = otherHero;
    }

    // If we are hovering over a hero, and they have perks or items, activate the overlay
    if (targetHero && (!targetHero->getPerkCards().empty() || !targetHero->getItems().empty())) {
        hoveredHero = targetHero;
        showHeroInventoryOverlay = true;
    }
}

void GameScreen::loadHeroImages() {
    // Create mapping between character names and image file names
    std::vector<std::pair<std::string, std::string>> heroMappings = {
        {"Mayor", "Mayor"},
        {"Archeologist", "Archaeologist"},  // Fix the spelling difference
        {"Courier", "Courier"},
        {"Scientist", "Scientist"}
    };
    
    for (const auto& [charName, fileName] : heroMappings) {
        std::string imagePath = getAssetPath("Horrified_Assets/Heros/" + fileName + ".png");
        if (FileExists(imagePath.c_str())) {
            heroImages[charName] = LoadTexture(imagePath.c_str());
            std::cout << "Loaded hero image: " << imagePath << " for " << charName << std::endl;
        } else {
            std::cout << "Failed to load hero image: " << imagePath << " for " << charName << std::endl;
        }
    }
    heroImagesLoaded = !heroImages.empty();
}

void GameScreen::loadMonsterImages() {
    std::vector<std::string> monsterNames = {"Dracula", "InvisibleMan"};
    
    for (const auto& monster : monsterNames) {
        std::string imagePath = getAssetPath("Horrified_Assets/Monsters/" + monster + ".png");
        if (FileExists(imagePath.c_str())) {
            monsterImages[monster] = LoadTexture(imagePath.c_str());
            std::cout << "Loaded monster image: " << imagePath << std::endl;
        } else {
            std::cout << "Failed to load monster image: " << imagePath << std::endl;
        }
    }
    monsterImagesLoaded = !monsterImages.empty();
}

void GameScreen::loadVillagerImages() {
    // Load villager images with correct file names
    std::vector<std::pair<std::string, std::string>> villagerMappings = {
        {"Dr.Cranley", "DrCranly"},
        {"Dr.Reed", "DrReed"},
        {"Prof.Pearson", "ProfPearson"},
        {"Maleva", "Maleva"},
        {"Fritz", "Fritz"},
        {"Wilbur And Chick", "WilburAndChick"},
        {"Maria", "Maria"}
    };
    
    for (const auto& [codeName, fileName] : villagerMappings) {
        std::string imagePath = getAssetPath("Horrified_Assets/Villager/" + fileName + ".png");
        if (FileExists(imagePath.c_str())) {
            villagerImages[codeName] = LoadTexture(imagePath.c_str());
            std::cout << "Loaded villager image: " << imagePath << " for " << codeName << std::endl;
        } else {
            std::cout << "Failed to load villager image: " << imagePath << " for " << codeName << std::endl;
        }
    }
    villagerImagesLoaded = !villagerImages.empty();
}

void GameScreen::loadItemImages() {
    std::vector<std::pair<std::string, std::string>> itemMappings = {
        // Red items
        {"Torch", "Torch"},
        {"Dart", "Dart"},
        {"Fire Pocker", "FirePoker"},
        {"Rapier", "Rapier"},
        {"Shovel", "Shovel"},
        {"Pitchfork", "Pitchfork"},
        {"Rifle", "Rifle"},
        {"Silver Cane", "SilverCane"},
        {"Knife", "Knife"},
        {"Pistol", "Pistol"},
        {"Bear Trap", "BearTrap"},
        {"Speargun", "Speargun"},
        
        // Blue items
        {"Anatomy Test", "AnatomyText"},
        {"Centrifuge", "Centrifuge"},
        {"Kite", "Kite"},
        {"Research", "Research"},
        {"Telescope", "Telescope"},
        {"Searchlight", "Searchlight"},
        {"Experiment", "Experiments"},
        {"Analysis", "Analysis"},
        {"Rotenone", "Rotenone"},
        {"Cosmic Ray Diffuser", "CosmicRayDiffuser"},
        {"Nebularium", "Nebularium"},
        {"Fossil", "Fossil"},
        {"Monocane Mixture", "MonocaneMixture"},
        
        // Yellow items
        {"Flower", "Flower"},
        {"Tarot Deck", "Tarot"},
        {"Garlic", "Garlic"},
        {"Mirrored Box", "Mirrored_Box"},
        {"Stake", "Stake"},
        {"Scroll Of Thoth", "Scroll"},
        {"Violin", "Violin"},
        {"Wolfsbane", "Wolfsbane"},
        {"Charm", "Charm"},
        {"Tablet", "Tablet"}
    };
    
    // Load all items with mapping
    for (const auto& [codeName, fileName] : itemMappings) {
        std::string imagePath = getAssetPath("Horrified_Assets/Items/");
        
        // Determine color folder based on item name
        std::string colorFolder;
        if (codeName == "Torch" || codeName == "Dart" || codeName == "Fire Pocker" || 
            codeName == "Rapier" || codeName == "Shovel" || codeName == "Pitchfork" || 
            codeName == "Rifle" || codeName == "Silver Cane" || codeName == "Knife" || 
            codeName == "Pistol" || codeName == "Bear Trap" || codeName == "Speargun") {
            colorFolder = "Red/";
        } else if (codeName == "Anatomy Test" || codeName == "Centrifuge" || codeName == "Kite" || 
                   codeName == "Research" || codeName == "Telescope" || codeName == "Searchlight" || 
                   codeName == "Experiment" || codeName == "Analysis" || codeName == "Rotenone" || 
                   codeName == "Cosmic Ray Diffuser" || codeName == "Nebularium" || codeName == "Fossil" || 
                   codeName == "Monocane Mixture") {
            colorFolder = "Blue/";
        } else {
            colorFolder = "Yellow/";
        }
        
        imagePath += colorFolder + fileName + ".png";
        
        std::cout << "Attempting to load item image: " << imagePath << " for " << codeName << std::endl;
        
        if (FileExists(imagePath.c_str())) {
            itemImages[codeName] = LoadTexture(imagePath.c_str());
            std::cout << "Successfully loaded item image: " << imagePath << " for " << codeName << std::endl;
        } else {
            std::cout << "FAILED to load item image: " << imagePath << " for " << codeName << std::endl;
        }
    }
    
    itemImagesLoaded = !itemImages.empty();
    std::cout << "Total item images loaded: " << itemImages.size() << std::endl;
}

void GameScreen::loadPerkCardImages() {
    std::vector<std::string> perkCardNames = {
        "BreakOfDawn.png", "Hurry.png", "LateIntoTheNight.png",
        "Overstock.png", "Repel.png", "VisitFromTheDetective.png"
    };

    for (const auto& perk : perkCardNames) {
        std::string imagePath = getAssetPath("Horrified_Assets/Perk_Cards/" + perk);
        if (FileExists(imagePath.c_str())) {
            perkCardImages[perk] = LoadTexture(imagePath.c_str());
            std::cout << "Loaded perk card image: " << imagePath << std::endl;
        } else {
            std::cout << "Failed to load perk card image: " << imagePath << std::endl;
        }
    }
    perkCardImagesLoaded = !perkCardImages.empty();
}

void GameScreen::loadCoffinImages() {
    // Load coffin texture
    std::string coffinPath = getAssetPath("Horrified_Assets/Items/Coffins/Coffin.png");
    if (FileExists(coffinPath.c_str())) {
        coffinTexture = LoadTexture(coffinPath.c_str());
        std::cout << "Loaded coffin image: " << coffinPath << std::endl;
    } else {
        std::cout << "Failed to load coffin image: " << coffinPath << std::endl;
    }
    
    // Load smashed coffin texture
    std::string smashedCoffinPath = getAssetPath("Horrified_Assets/Items/Coffins/SmashedCoffin.png");
    if (FileExists(smashedCoffinPath.c_str())) {
        smashedCoffinTexture = LoadTexture(smashedCoffinPath.c_str());
        std::cout << "Loaded smashed coffin image: " << smashedCoffinPath << std::endl;
    } else {
        std::cout << "Failed to load smashed coffin image: " << smashedCoffinPath << std::endl;
    }
    
    coffinImagesLoaded = (coffinTexture.id != 0 && smashedCoffinTexture.id != 0);
}

bool GameScreen::isCoffinLocation(const std::string& location) const {
    return std::find(coffinLocations.begin(), coffinLocations.end(), location) != coffinLocations.end();
}

void GameScreen::unloadImages() {
    for (auto& [name, texture] : heroImages) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
    heroImages.clear();
    
    for (auto& [name, texture] : monsterImages) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
    monsterImages.clear();
    
    // Unload coffin textures
    if (coffinTexture.id != 0) {
        UnloadTexture(coffinTexture);
    }
    if (smashedCoffinTexture.id != 0) {
        UnloadTexture(smashedCoffinTexture);
    }
    
    for (auto& [name, texture] : perkCardImages) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
    perkCardImages.clear();
    
    // Unload villager images
    for (auto& [name, texture] : villagerImages) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
    villagerImages.clear();
    
    // Unload item images
    for (auto& [name, texture] : itemImages) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
    itemImages.clear();
}

std::string GameScreen::getAssetPath(const std::string& relativePath) {
    // Try different possible paths
    std::vector<std::string> possiblePaths = {
        relativePath,                    // Current directory
        "../" + relativePath,           // Parent directory
        "../../" + relativePath,        // Grandparent directory
        "./" + relativePath             // Explicit current directory
    };
    
    for (const auto& path : possiblePaths) {
        if (FileExists(path.c_str())) {
            return path;
        }
    }
    return relativePath; // Return original if none found
}

std::string GameScreen::getPerkCardImageName(PerkType type) {
    switch (type) {
        case PerkType::VisitFromTheDetective: return "VisitFromTheDetective.png";
        case PerkType::BreakOfDawn: return "BreakOfDawn.png";
        case PerkType::Overstock: return "Overstock.png";
        case PerkType::LateIntoTheNight: return "LateIntoTheNight.png";
        case PerkType::Repel: return "Repel.png";
        case PerkType::Hurry: return "Hurry.png";
        default: return "";
    }
}

void GameScreen::drawHeroInventoryOverlay() {
    if (!showHeroInventoryOverlay || !hoveredHero) {
        return;
    }

    // --- 1. Define a unified structure for display entries ---
    struct DisplayEntry {
        Texture2D* texture;
        std::string line1; // Perk Name or Item Name
        std::string line2; // Perk Description or Item Color
        std::string line3; // Empty for Perks, Item Power for Items
    };

    std::vector<DisplayEntry> entries;

    // --- 2. Populate the entries vector with perks and items ---
    // Add Perks
    for (const auto& perk : hoveredHero->getPerkCards()) {
        Texture2D* tex = nullptr;
        auto it = perkCardImages.find(getPerkCardImageName(perk.getType()));
        if (it != perkCardImages.end()) {
            tex = &it->second;
        }
        entries.push_back({tex, PerkCard::perkTypeToString(perk.getType()), perk.getDescription(), ""});
    }

    // Add Items
    for (const auto& item : hoveredHero->getItems()) {
        Texture2D* tex = nullptr;
        auto it = itemImages.find(item.getItemName());
        if (it != itemImages.end()) {
            tex = &it->second;
        }
        entries.push_back({tex, item.getItemName(), "Color: " + itemColorToString(item.getColor()), "Power: " + std::to_string(item.getPower())});
    }

    // --- 3. Draw the overlay background and main container ---
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Define container dimensions (centered)
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.9f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;
    
    DrawRectangleRec({boxX, boxY, boxWidth, boxHeight}, {20, 20, 30, 250});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Draw Title
    std::string title = hoveredHero->getHeroName() + "'s Inventory";
    float titleX = boxX + (boxWidth - MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 2).x) / 2;
    DrawTextEx(titleFont, title.c_str(), {titleX, boxY + 20}, titleFont.baseSize, 2, titleColor);

    // --- 4. Two-column layout with scroll if content exceeds available space ---
    float padding = 20.0f;
    float headerHeight = 70.0f;
    heroOverlayRect = {boxX + padding, boxY + headerHeight, boxWidth - 2 * padding, boxHeight - headerHeight - padding};
    heroOverlayContentRect = heroOverlayRect;

    float entryHeight = screenHeight * 0.12f; // Height of one entry (image + text)
    float columnWidth = heroOverlayRect.width * 0.48f;     // Width of one column
    float columnSpacing = heroOverlayRect.width * 0.04f;   // Space between columns

    int rowsNeeded = static_cast<int>((entries.size() + 1) / 2); // two columns
    heroOverlayMaxScroll = std::max(0.0f, rowsNeeded * entryHeight - heroOverlayRect.height);
    if (heroOverlayScroll < 0.0f) heroOverlayScroll = 0.0f;
    if (heroOverlayScroll > heroOverlayMaxScroll) heroOverlayScroll = heroOverlayMaxScroll;

    BeginScissorMode((int)heroOverlayRect.x, (int)heroOverlayRect.y, (int)heroOverlayRect.width, (int)heroOverlayRect.height);

    for (size_t i = 0; i < entries.size(); ++i) {
        int col = static_cast<int>(i % 2);
        int row = static_cast<int>(i / 2);

        // Calculate position of the current entry relative to scroll
        float currentX = heroOverlayRect.x + (col * (columnWidth + columnSpacing));
        float currentY = heroOverlayRect.y + (row * entryHeight) - heroOverlayScroll;
        
        DisplayEntry& entry = entries[i];

        // Draw Image (on the left of the entry)
        float imageSize = entryHeight * 0.8f;
        float imageX = currentX + padding * 0.5f;
        float imageY = currentY + (entryHeight - imageSize) / 2; // Vertically center image
        if (entry.texture) {
            DrawTexturePro(*entry.texture, 
                         {0, 0, (float)entry.texture->width, (float)entry.texture->height},
                         {imageX, imageY, imageSize, imageSize}, 
                         {0, 0}, 0.0f, WHITE);
        }

        // Draw Text (on the right of the image)
        float textX = imageX + imageSize + padding * 0.6f;
        float textY = currentY + padding * 0.5f;
        
        DrawTextEx(gameFont, entry.line1.c_str(), {textX, textY}, gameFont.baseSize * 1.1f, 1, titleColor);
        textY += gameFont.baseSize * 1.2f;
        DrawTextEx(gameFont, entry.line2.c_str(), {textX, textY}, gameFont.baseSize * 0.9f, 1, textColor);
        textY += gameFont.baseSize * 1.0f;
        if (!entry.line3.empty()) {
            DrawTextEx(gameFont, entry.line3.c_str(), {textX, textY}, gameFont.baseSize * 0.9f, 1, textColor);
        }
    }
    EndScissorMode();
     // Add a close instruction
    const char* closeText = "Move mouse away to close";
    float closeTextWidth = MeasureTextEx(gameFont, closeText, gameFont.baseSize * 0.9f, 1).x;
    DrawTextEx(gameFont, closeText, {boxX + (boxWidth - closeTextWidth) / 2, boxY + boxHeight - 25}, gameFont.baseSize * 0.9f, 1, GRAY);
}

void GameScreen::drawLocationDetailPopup() {
    if (!locationPopupOpen || locationPopupLocationName.empty()) return;

    // Dim the main screen
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Container dimensions (match hero inventory overlay style)
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.9f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRec({boxX, boxY, boxWidth, boxHeight}, {20, 20, 30, 250});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Title
    std::string title = std::string("Location: ") + locationPopupLocationName;
    float titleX = boxX + (boxWidth - MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 2).x) / 2;
    DrawTextEx(titleFont, title.c_str(), {titleX, boxY + 20}, titleFont.baseSize, 2, titleColor);

    // Content layout
    float padding = screenWidth * 0.012f;
    float headerHeight = screenHeight * 0.1f;
    float contentX = boxX + padding;
    float contentY = boxY + headerHeight;
    float contentWidth = boxWidth - 2 * padding;
    float contentHeight = boxHeight - headerHeight - padding * 2.0f;

    float entryHeight = screenHeight * 0.12f; // baseline height per entry
    float imageSize = entryHeight * 0.8f;

    // Build entries
    struct Entry { enum Kind { Character, Item, Coffin } kind; std::string name; const Texture2D* texture; std::string sub1; std::string sub2; };
    std::vector<Entry> entries;

    // Characters
    auto itLoc = mapLocations.find(locationPopupLocationName);
    if (itLoc != mapLocations.end()) {
        for (const auto& charName : itLoc->second.characters) {
            const Texture2D* tex = nullptr;
            // Hero
            for (const auto& kv : heroImages) {
                if (charName.find(kv.first) != std::string::npos) { tex = &kv.second; break; }
            }
            // Monster
            if (!tex) {
                for (const auto& kv : monsterImages) {
                    std::string searchName = (charName == "Invisible man") ? std::string("InvisibleMan") : charName;
                    if (searchName.find(kv.first) != std::string::npos) { tex = &kv.second; break; }
                }
            }
            // Villager
            if (!tex) {
                for (const auto& kv : villagerImages) {
                    if (charName.find(kv.first) != std::string::npos) { tex = &kv.second; break; }
                }
            }
            
            // Add safe place information for villagers
            std::string sub1, sub2;
            if (charName != "Archeologist" && charName != "Mayor" && charName != "Scientist" && charName != "Courier" && 
                charName != "Dracula" && charName != "Invisible man") {
                try {
                    auto villager = villagerManager.getVillager(charName);
                    std::string villagerName = villager->getVillagerName();
                    std::string safePlace;
                    if (villagerName == "Dr.Cranley") {
                        safePlace = "Precinct";
                    } else if (villagerName == "Dr.Reed") {
                        safePlace = "Camp";
                    } else if (villagerName == "Prof.Pearson") {
                        safePlace = "Museum";
                    } else if (villagerName == "Maleva") {
                        safePlace = "Shop";
                    } else if (villagerName == "Fritz") {
                        safePlace = "Institute";
                    } else if (villagerName == "Wilbur And Chick") {
                        safePlace = "Dungeon";
                    } else if (villagerName == "Maria") {
                        safePlace = "Camp";
                    }
                    sub1 = "Safe Place: " + safePlace;
                } catch (const std::exception& e) {
                    // Villager not found, leave sub1 empty
                }
            } else if (charName == "Dracula") {
                // Show Dracula current damage progress
                int dmg = taskBoard.getDraculaDefeatStrength();
                sub1 = std::string("Damage: ") + std::to_string(dmg) + "/6";
            } else if (charName == "Invisible man") {
                // Show Invisible Man current damage progress
                int dmg = taskBoard.getInvisibleManDefeatStrength();
                sub1 = std::string("Damage: ") + std::to_string(dmg) + "/9";
            }
            
            entries.push_back({Entry::Character, charName, tex, sub1, sub2});
        }
    }

    // Items (from the map location itself)
    auto locItForItems = gameMap ? gameMap->locations.find(locationPopupLocationName) : gameMap->locations.end();
    if (locItForItems != gameMap->locations.end()) {
        const auto& itemsAtLoc = locItForItems->second->getItems();
        for (const auto& item : itemsAtLoc) {
            const Texture2D* tex = nullptr;
            auto itImg = itemImages.find(item.getItemName());
            if (itImg != itemImages.end()) tex = &itImg->second;
            std::string sub1 = std::string("Item Color: ") + itemColorToString(item.getColor());
            std::string sub2 = std::string("Item Power: ") + std::to_string(item.getPower());
            entries.push_back({Entry::Item, item.getItemName(), tex, sub1, sub2});
        }
    }

    // Coffin (Dracula)
    if (isCoffinLocation(locationPopupLocationName)) {
        bool destroyed = taskBoard.isCoffinDestroyed(locationPopupLocationName);
        const auto& coffins = taskBoard.getDraculaCoffins();
        int current = 0;
        auto itC = coffins.find(locationPopupLocationName);
        if (itC != coffins.end()) current = itC->second.currentStrength;
        const Texture2D* tex = destroyed ? &smashedCoffinTexture : &coffinTexture;
        std::string sub1 = std::string("Damage: ") + std::to_string(current) + "/6";
        entries.push_back({Entry::Coffin, destroyed ? std::string("Coffin (Destroyed)") : std::string("Coffin"), tex, sub1, std::string()});
    }

    // Multi-column rendering like hero overlay
    int maxEntriesPerColumn = static_cast<int>(contentHeight / entryHeight);
    if (maxEntriesPerColumn < 1) maxEntriesPerColumn = 1;

    float columnWidth = contentWidth * 0.48f;
    float columnSpacing = contentWidth * 0.04f;

    for (size_t i = 0; i < entries.size(); ++i) {
        int col = static_cast<int>(i / maxEntriesPerColumn);
        int row = static_cast<int>(i % maxEntriesPerColumn);

        float currentX = contentX + (col * (columnWidth + columnSpacing));
        float currentY = contentY + (row * entryHeight);

        // Image
        float imgX = currentX + 12.0f;
        float imgY = currentY + (entryHeight - imageSize) / 2.0f;
        if (entries[i].texture) {
            DrawTexturePro(*entries[i].texture,
                           {0, 0, (float)entries[i].texture->width, (float)entries[i].texture->height},
                           {imgX, imgY, imageSize, imageSize},
                           {0, 0}, 0.0f, WHITE);
        }

        // Text
        float textX = imgX + imageSize + 12.0f;
        float textY = currentY + 10.0f;
        DrawTextEx(gameFont, entries[i].name.c_str(), {textX, textY}, gameFont.baseSize * 1.1f, 1, titleColor);
        textY += gameFont.baseSize * 1.2f;
        if (!entries[i].sub1.empty()) {
            DrawTextEx(gameFont, entries[i].sub1.c_str(), {textX, textY}, gameFont.baseSize * 0.95f, 1, textColor);
            textY += gameFont.baseSize * 1.05f;
        }
        if (!entries[i].sub2.empty()) {
            DrawTextEx(gameFont, entries[i].sub2.c_str(), {textX, textY}, gameFont.baseSize * 0.95f, 1, textColor);
        }
    }

    // Footer note
    const char* note = "Right-click the same location to close this window.";
    float noteW = MeasureTextEx(gameFont, note, gameFont.baseSize * 0.95f, 1).x;
    DrawTextEx(gameFont, note, {boxX + (boxWidth - noteW) / 2, boxY + boxHeight - padding * 1.2f}, gameFont.baseSize * 0.95f, 1, GRAY);
}

// Add this helper function to game_screen.cpp
std::string GameScreen::itemColorToString(ItemColor color) {
    switch (color) {
        case ItemColor::Red: return "Red";
        case ItemColor::Blue: return "Blue";
        case ItemColor::Yellow: return "Yellow";
        default: return "Unknown";
    }
}

void GameScreen::initializeHelpMenu() {
    // Initialize help menu area (centered on screen)
    float menuWidth = screenWidth * 0.8f;  // 80% of screen width (was 0.7f)
    float menuHeight = screenHeight * 0.9f; // 90% of screen height (was 0.8f)
    float menuX = (screenWidth - menuWidth) / 2;
    float menuY = (screenHeight - menuHeight) / 2;
    
    helpMenuArea = {menuX, menuY, menuWidth, menuHeight};
    
    // Initialize close button (bottom center of help menu)
    float buttonWidth = screenWidth * 0.15f;  // 15% of screen width
    float buttonHeight = screenHeight * 0.05f; // 5% of screen height
    float buttonX = menuX + (menuWidth - buttonWidth) / 2;
    float buttonY = menuY + menuHeight - buttonHeight - screenHeight * 0.025f; // slightly more margin
    helpCloseButton = {buttonX, buttonY, buttonWidth, buttonHeight};

    // Initialize navigation arrows (centered vertically on sides)
    float arrowWidth = screenWidth * 0.022f; // reduced from 0.04f
    float arrowHeight = screenHeight * 0.08f;
    float arrowY = menuY + (menuHeight - arrowHeight) / 2;
    float arrowMargin = screenWidth * 0.012f; // slightly more margin from edge
    helpPrevButton = {menuX + arrowMargin, arrowY, arrowWidth, arrowHeight};
    helpNextButton = {menuX + menuWidth - arrowWidth - arrowMargin, arrowY, arrowWidth, arrowHeight};
}

void GameScreen::drawHelpMenu() {
    if (!helpMenuActive) return;
    
    // Draw semi-transparent background overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 150});
    
    // Draw help menu background
    DrawRectangleRec(helpMenuArea, {40, 40, 40, 255});
    DrawRectangleLinesEx(helpMenuArea, 3, WHITE);
    
    // Draw title
    const char* title = "GAME HELP";
    float titleX = helpMenuArea.x + (helpMenuArea.width - MeasureTextEx(titleFont, title, titleFont.baseSize * 1.5f, 2).x) / 2;
    float titleY = helpMenuArea.y + screenHeight * 0.025f; // slightly more margin
    DrawTextEx(titleFont, title, Vector2{titleX, titleY}, titleFont.baseSize * 1.5f, 2, titleColor);
    
    // Draw help content or images depending on page
    if (currentHelpPage == 1) {
        float contentX = helpMenuArea.x + screenWidth * 0.035f; // more margin from left
        float contentY = titleY + screenHeight * 0.07f;
        float lineHeight = screenHeight * 0.032f;
        std::vector<std::string> helpText = {
            "GAME OBJECTIVE:",
            "Defeat Dracula and the Invisible Man by completing their specific requirements.",
            "HERO ACTIONS:",
            "- Move: Move to an adjacent locations. If villagers are present in your current location, you can choose to move them with you",
            "- Guide: Guide a villager either from your current location to an adjacent one, or from an adjacent location into your current one",
            "- Pick Up: Pick up any number of items from your current location. These items can be used later to complete monster tasks",
            "- Advance: Use a red item to destroy Dracula's coffins or deliver evidence items to the Precinct to weaken the Invisible Man",
            "- Defeat: After completing all tasks related to a monster and if the monster is at your location, use specific colored items to defeat it",
            "          Use yellow items to defeat Dracula, and red items to defeat the Invisible Man",
            "- Hero Abilities and Special Actions:",
            "       - Archeologist: Special Action - Pick up an item from a neighboring location without moving",
            "       - Courier     : Special Action - Move directly to the other hero's location",
            "       - Scientist   : Ability        - Increase the power of an item when using it",
            "       - Mayor       : No special action or ability",
            "- Use Perk Card: Use a perk card you've collected. These don't consume actions and can have helpful effects",
            "MONSTERS:",
            "- Dracula: Defeat by destroying all 4 coffins",
            "- Invisible Man: Defeat by delivering 5 evidences",
            "WINNING:",
            "Defeat both monsters before terror level reaches maximum!",
            "TIPS:",
            "Right-click a location to view its details. Right-click the same location again to close the window."
        };
        for (const auto& line : helpText) {
            if (line.empty()) {
                contentY += lineHeight / 2.0f;
            } else {
                DrawTextEx(gameFont, line.c_str(), Vector2{contentX, contentY}, gameFont.baseSize, 1, textColor);
                contentY += lineHeight;
            }
        }
    } else {
        int imageIndex = currentHelpPage - 2; // pages 2..4 map to indices 0..2
        if (helpImagesLoaded && imageIndex >= 0 && imageIndex < (int)helpPageImages.size()) {
            const Texture2D& tex = helpPageImages[imageIndex];
            // Compute fit into inner box while preserving aspect
            float margin = screenWidth * 0.04f; // more margin for larger menu
            float innerX = helpMenuArea.x + margin;
            float innerY = titleY + screenHeight * 0.05f;
            float innerW = helpMenuArea.width - 2 * margin;
            float innerH = helpCloseButton.y - innerY - screenHeight * 0.05f;
            float texW = (float)tex.width;
            float texH = (float)tex.height;
            float scale = std::min(innerW / texW, innerH / texH);
            float drawW = texW * scale;
            float drawH = texH * scale;
            float drawX = innerX + (innerW - drawW) / 2.0f;
            float drawY = innerY + (innerH - drawH) / 2.0f;
            DrawTexturePro(tex, {0,0,texW,texH}, {drawX, drawY, drawW, drawH}, {0,0}, 0.0f, WHITE);
        } else {
            // Fallback text
            const char* msg = "Image not available";
            float mx = helpMenuArea.x + (helpMenuArea.width - MeasureTextEx(gameFont, msg, gameFont.baseSize, 1).x) / 2.0f;
            float my = helpMenuArea.y + helpMenuArea.height / 2.0f - gameFont.baseSize / 2.0f;
            DrawTextEx(gameFont, msg, {mx, my}, gameFont.baseSize, 1, GRAY);
        }
    }
    // Draw navigation arrows
    auto drawArrow = [&](const Rectangle& rect, bool right) {
        Color bg = CheckCollisionPointRec(GetMousePosition(), rect) ? buttonHoverColor : buttonColor;
        DrawRectangleRec(rect, bg);
        DrawRectangleLinesEx(rect, 2, WHITE);
        // Simple triangle arrow
        Vector2 a, b, c;
        if (right) {
            a = {rect.x + rect.width * 0.3f, rect.y + rect.height * 0.2f};
            b = {rect.x + rect.width * 0.3f, rect.y + rect.height * 0.8f};
            c = {rect.x + rect.width * 0.7f, rect.y + rect.height * 0.5f};
        } else {
            a = {rect.x + rect.width * 0.7f, rect.y + rect.height * 0.2f};
            b = {rect.x + rect.width * 0.7f, rect.y + rect.height * 0.8f};
            c = {rect.x + rect.width * 0.3f, rect.y + rect.height * 0.5f};
            std::swap(a, c);
        }
        DrawTriangle(a, b, c, WHITE);
    };
    drawArrow(helpPrevButton, false);
    drawArrow(helpNextButton, true);
    // Draw close button
    Color buttonColor = CheckCollisionPointRec(GetMousePosition(), helpCloseButton) ? buttonHoverColor : this->buttonColor;
    DrawRectangleRec(helpCloseButton, buttonColor);
    DrawRectangleLinesEx(helpCloseButton, 2, WHITE);
    const char* closeText = "CLOSE";
    float closeTextX = helpCloseButton.x + (helpCloseButton.width - MeasureTextEx(gameFont, closeText, gameFont.baseSize, 1).x) / 2;
    float closeTextY = helpCloseButton.y + (helpCloseButton.height - gameFont.baseSize) / 2;
    DrawTextEx(gameFont, closeText, Vector2{closeTextX, closeTextY}, gameFont.baseSize, 1, WHITE);
    // Page indicator
    char pageBuf[32];
    snprintf(pageBuf, sizeof(pageBuf), "Page %d of 4", currentHelpPage);
    float piX = helpMenuArea.x + (helpMenuArea.width - MeasureTextEx(gameFont, pageBuf, gameFont.baseSize, 1).x) / 2.0f;
    float piY = helpCloseButton.y - gameFont.baseSize - screenHeight * 0.012f;
    DrawTextEx(gameFont, pageBuf, {piX, piY}, gameFont.baseSize, 1, GRAY);
}

void GameScreen::handleHelpMenuClick(Vector2 mousePos) {
    if (!helpMenuActive) return;
    
    if (CheckCollisionPointRec(mousePos, helpPrevButton)) {
        currentHelpPage = (currentHelpPage == 1) ? 4 : (currentHelpPage - 1);
        return;
    }
    if (CheckCollisionPointRec(mousePos, helpNextButton)) {
        currentHelpPage = (currentHelpPage == 4) ? 1 : (currentHelpPage + 1);
        return;
    }
    if (CheckCollisionPointRec(mousePos, helpCloseButton)) {
        helpMenuActive = false;
    }
}

void GameScreen::showHelpMenu() {
    helpMenuActive = true;
    currentHelpPage = 1;
}

void GameScreen::loadHelpImages() {
    if (helpImagesLoaded) return;
    std::vector<std::string> files = {
        getAssetPath("Horrified_Assets/Monster_Mat/DraculaMat.png"),
        getAssetPath("Horrified_Assets/Monster_Mat/InvisibleManMat.png"),
        getAssetPath("Horrified_Assets/Monster_Mat/Precinct.png")
    };
    helpPageImages.clear();
    helpPageImages.reserve(files.size());
    for (const auto& f : files) {
        Texture2D tex{};
        if (FileExists(f.c_str())) {
            tex = LoadTexture(f.c_str());
            if (tex.id != 0) {
                SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                helpPageImages.push_back(tex);
            }
        }
    }
    helpImagesLoaded = helpPageImages.size() == files.size();
}

void GameScreen::unloadHelpImages() {
    for (auto& tex : helpPageImages) {
        if (tex.id != 0) UnloadTexture(tex);
    }
    helpPageImages.clear();
    helpImagesLoaded = false;
}

void GameScreen::loadFrenziedMonsterTexture() {
    if (currentFrenziedMonster.empty()) return;
    
    // Unload existing texture first
    unloadFrenziedMonsterTexture();
    
    std::string monsterImagePath = "";
    if (currentFrenziedMonster.find("Dracula") != std::string::npos) {
        monsterImagePath = getAssetPath("Horrified_Assets/Monsters/Dracula.png");
    } else if (currentFrenziedMonster.find("Invisible") != std::string::npos) {
        monsterImagePath = getAssetPath("Horrified_Assets/Monsters/InvisibleMan.png");
    }
    
    if (!monsterImagePath.empty() && FileExists(monsterImagePath.c_str())) {
        frenziedMonsterTexture = LoadTexture(monsterImagePath.c_str());
        if (frenziedMonsterTexture.id != 0) {
            frenziedMonsterTextureLoaded = true;
            std::cout << "Loaded frenzied monster texture: " << monsterImagePath << std::endl;
        }
    }
}

void GameScreen::unloadFrenziedMonsterTexture() {
    if (frenziedMonsterTextureLoaded && frenziedMonsterTexture.id != 0) {
        UnloadTexture(frenziedMonsterTexture);
        frenziedMonsterTextureLoaded = false;
        std::cout << "Unloaded frenzied monster texture" << std::endl;
    }
}

void GameScreen::showConfirmation(const std::string& question, std::function<void()> onYes, std::function<void()> onNo) {
    confirmationPromptText = question;
    onConfirmYes = onYes;
    onConfirmNo = onNo;
    showConfirmationPrompt = true;
}

void GameScreen::hideConfirmation() {
    showConfirmationPrompt = false;
    confirmationPromptText.clear();
    onConfirmYes = nullptr; // Clear the stored actions
    onConfirmNo = nullptr;
}

void GameScreen::drawConfirmationPrompt() {
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Message box
    float boxWidth = screenWidth * 0.45f;
    float boxHeight = screenHeight * 0.3f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Draw the question using the stored text
    Vector2 titleSize = MeasureTextEx(titleFont, confirmationPromptText.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, confirmationPromptText.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 40}, titleFont.baseSize, 1, WHITE);

    // "Yes" and "No" buttons
    float buttonWidth = boxWidth * 0.3f;
    float buttonHeight = boxHeight * 0.2f;
    float buttonY = boxY + boxHeight - buttonHeight - 30;
    float buttonSpacing = boxWidth * 0.1f;
    
    float yesButtonX = boxX + (boxWidth / 2) - buttonWidth - (buttonSpacing / 2);
    float noButtonX = boxX + (boxWidth / 2) + (buttonSpacing / 2);

    confirmYesButton = {yesButtonX, buttonY, buttonWidth, buttonHeight};
    confirmNoButton = {noButtonX, buttonY, buttonWidth, buttonHeight};

    Vector2 mousePos = GetMousePosition();

    // Draw Yes button
    bool yesHover = CheckCollisionPointRec(mousePos, confirmYesButton);
    DrawRectangleRec(confirmYesButton, yesHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(confirmYesButton, 2, WHITE);
    Vector2 yesTextSize = MeasureTextEx(gameFont, "Yes", gameFont.baseSize, 1);
    DrawTextEx(gameFont, "Yes", {yesButtonX + (buttonWidth - yesTextSize.x) / 2, buttonY + (buttonHeight - yesTextSize.y) / 2}, gameFont.baseSize, 1, WHITE);

    // Draw No button
    bool noHover = CheckCollisionPointRec(mousePos, confirmNoButton);
    DrawRectangleRec(confirmNoButton, noHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(confirmNoButton, 2, WHITE);
    Vector2 noTextSize = MeasureTextEx(gameFont, "No", gameFont.baseSize, 1);
    DrawTextEx(gameFont, "No", {noButtonX + (buttonWidth - noTextSize.x) / 2, buttonY + (buttonHeight - noTextSize.y) / 2}, gameFont.baseSize, 1, WHITE);
}

void GameScreen::handleConfirmationPromptClick(Vector2 mousePos) {
    if (CheckCollisionPointRec(mousePos, confirmYesButton)) {
        if (onConfirmYes) {
            onConfirmYes();
        }
        hideConfirmation(); 
    } else if (CheckCollisionPointRec(mousePos, confirmNoButton)) {
        if (onConfirmNo) {
            onConfirmNo();
        }
        hideConfirmation();
    }
}

void GameScreen::drawGuideVillagersOverlay() {
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Main overlay box
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Title
    std::string title = "Choose a villager to guide:";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    // Calculate grid layout
    int villagersPerRow = 4;
    float imageSize = screenWidth * 0.12f;  // 12% of screen width
    float spacing = screenWidth * 0.02f;    // 2% spacing
    float startX = boxX + (boxWidth - (villagersPerRow * imageSize + (villagersPerRow - 1) * spacing)) / 2;
    float startY = boxY + 80;

    // Draw villagers in a grid
    for (size_t i = 0; i < guidableVillagers.size(); i++) {
        int row = i / villagersPerRow;
        int col = i % villagersPerRow;
        
        float x = startX + col * (imageSize + spacing);
        float y = startY + row * (imageSize + spacing + 30); // Extra space for name

        // Draw villager image
        std::string villagerName = guidableVillagers[i]->getVillagerName();
        bool imageFound = false;
        
        for (const auto& [name, texture] : villagerImages) {
            if (villagerName.find(name) != std::string::npos) {
                Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
                Rectangle destRect = {x, y, imageSize, imageSize};
                DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                imageFound = true;
                break;
            }
        }
        
        // Fallback circle if no image found
        if (!imageFound) {
            DrawCircle(x + imageSize/2, y + imageSize/2, imageSize/2, BLUE);
        }

        // Draw villager name
        Vector2 nameSize = MeasureTextEx(gameFont, villagerName.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, villagerName.c_str(), 
            {x + (imageSize - nameSize.x) / 2, y + imageSize + 5}, 
            gameFont.baseSize, 1, WHITE);

        // Draw location info
        std::string locationInfo = "at " + guidableVillagers[i]->getCurrentLocation()->getName();
        Vector2 locSize = MeasureTextEx(gameFont, locationInfo.c_str(), gameFont.baseSize * 0.8f, 1);
        DrawTextEx(gameFont, locationInfo.c_str(), 
            {x + (imageSize - locSize.x) / 2, y + imageSize + 25}, 
            gameFont.baseSize * 0.8f, 1, GRAY);

        // Draw selection number
        std::string number = std::to_string(i + 1);
        Vector2 numSize = MeasureTextEx(gameFont, number.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, number.c_str(), 
            {x + 5, y + 5}, 
            gameFont.baseSize, 1, YELLOW);
    }

    // Cancel button
    float cancelWidth = 100;
    float cancelHeight = 40;
    float cancelX = boxX + (boxWidth - cancelWidth) / 2;
    float cancelY = boxY + boxHeight - cancelHeight - 20;
    
    Vector2 mousePos = GetMousePosition();
    Rectangle cancelButton = {cancelX, cancelY, cancelWidth, cancelHeight};
    bool cancelHover = CheckCollisionPointRec(mousePos, cancelButton);
    
    DrawRectangleRec(cancelButton, cancelHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(cancelButton, 2, WHITE);
    Vector2 cancelTextSize = MeasureTextEx(gameFont, "Cancel", gameFont.baseSize, 1);
    DrawTextEx(gameFont, "Cancel", 
        {cancelX + (cancelWidth - cancelTextSize.x) / 2, cancelY + (cancelHeight - cancelTextSize.y) / 2}, 
        gameFont.baseSize, 1, WHITE);
}

void GameScreen::drawGuideLocationsOverlay() {
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Main overlay box
    float boxWidth = screenWidth * 0.6f;
    float boxHeight = screenHeight * 0.5f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Title
    std::string title = "Where do you want to guide " + selectedVillager->getVillagerName() + "?";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    // Draw location options
    float startY = boxY + 80;
    float optionHeight = 50;
    float spacing = 10;

    for (size_t i = 0; i < availableGuideLocations.size(); i++) {
        float y = startY + i * (optionHeight + spacing);
        
        std::string locationName = availableGuideLocations[i]->getName();
        
        // Draw location button
        Rectangle optionRect = {boxX + 20, y, boxWidth - 40, optionHeight};
        Vector2 mousePos = GetMousePosition();
        bool isHovered = CheckCollisionPointRec(mousePos, optionRect);
        
        DrawRectangleRec(optionRect, isHovered ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(optionRect, 2, WHITE);
        
        // Draw location name
        Vector2 textSize = MeasureTextEx(gameFont, locationName.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, locationName.c_str(), 
            {boxX + 30, y + (optionHeight - textSize.y) / 2}, 
            gameFont.baseSize, 1, WHITE);
        
        // Draw selection number
        std::string number = std::to_string(i + 1);
        Vector2 numSize = MeasureTextEx(gameFont, number.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, number.c_str(), 
            {boxX + boxWidth - 50, y + (optionHeight - numSize.y) / 2}, 
            gameFont.baseSize, 1, YELLOW);
    }

    // Cancel button
    float cancelWidth = 100;
    float cancelHeight = 40;
    float cancelX = boxX + (boxWidth - cancelWidth) / 2;
    float cancelY = boxY + boxHeight - cancelHeight - 20;
    
    Vector2 mousePos = GetMousePosition();
    Rectangle cancelButton = {cancelX, cancelY, cancelWidth, cancelHeight};
    bool cancelHover = CheckCollisionPointRec(mousePos, cancelButton);
    
    DrawRectangleRec(cancelButton, cancelHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(cancelButton, 2, WHITE);
    Vector2 cancelTextSize = MeasureTextEx(gameFont, "Cancel", gameFont.baseSize, 1);
    DrawTextEx(gameFont, "Cancel", 
        {cancelX + (cancelWidth - cancelTextSize.x) / 2, cancelY + (cancelHeight - cancelTextSize.y) / 2}, 
        gameFont.baseSize, 1, WHITE);
}

void GameScreen::drawGuideMessage() {
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Message box
    float boxWidth = screenWidth * 0.5f;
    float boxHeight = screenHeight * 0.2f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Draw the message
    Vector2 textSize = MeasureTextEx(titleFont, guideMessage.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, guideMessage.c_str(),
        {boxX + (boxWidth - textSize.x) / 2, boxY + (boxHeight - textSize.y) / 2},
        titleFont.baseSize, 1, WHITE);
}

void GameScreen::handleGuideVillagersClick(Vector2 mousePos) {
    // Check if cancel button was clicked
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;
    
    float cancelWidth = 100;
    float cancelHeight = 40;
    float cancelX = boxX + (boxWidth - cancelWidth) / 2;
    float cancelY = boxY + boxHeight - cancelHeight - 20;
    
    Rectangle cancelButton = {cancelX, cancelY, cancelWidth, cancelHeight};
    if (CheckCollisionPointRec(mousePos, cancelButton)) {
        cancelGuideAction();
        return;
    }

    // Check if a villager was clicked
    int villagersPerRow = 4;
    float imageSize = screenWidth * 0.12f;
    float spacing = screenWidth * 0.02f;
    float startX = boxX + (boxWidth - (villagersPerRow * imageSize + (villagersPerRow - 1) * spacing)) / 2;
    float startY = boxY + 80;

    for (size_t i = 0; i < guidableVillagers.size(); i++) {
        int row = i / villagersPerRow;
        int col = i % villagersPerRow;
        
        float x = startX + col * (imageSize + spacing);
        float y = startY + row * (imageSize + spacing + 30);

        Rectangle villagerRect = {x, y, imageSize, imageSize + 30};
        if (CheckCollisionPointRec(mousePos, villagerRect)) {
            selectVillagerToGuide(i);
            return;
        }
    }
}

void GameScreen::handleGuideLocationsClick(Vector2 mousePos) {
    // Check if cancel button was clicked
    float boxWidth = screenWidth * 0.6f;
    float boxHeight = screenHeight * 0.5f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;
    
    float cancelWidth = 100;
    float cancelHeight = 40;
    float cancelX = boxX + (boxWidth - cancelWidth) / 2;
    float cancelY = boxY + boxHeight - cancelHeight - 20;
    
    Rectangle cancelButton = {cancelX, cancelY, cancelWidth, cancelHeight};
    if (CheckCollisionPointRec(mousePos, cancelButton)) {
        cancelGuideAction();
        return;
    }

    // Check if a location was clicked
    float startY = boxY + 80;
    float optionHeight = 50;
    float spacing = 10;

    for (size_t i = 0; i < availableGuideLocations.size(); i++) {
        float y = startY + i * (optionHeight + spacing);
        Rectangle optionRect = {boxX + 20, y, boxWidth - 40, optionHeight};
        
        if (CheckCollisionPointRec(mousePos, optionRect)) {
            selectLocationToGuide(i);
            return;
        }
    }
}

void GameScreen::drawPickUpItemsOverlay() {
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Main overlay box
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Title
    std::string title = "Pick up items from " + pickUpLocation + ":";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    // Calculate grid layout
    int itemsPerRow = 4;
    float imageSize = screenWidth * 0.12f;  // 12% of screen width
    float spacing = screenWidth * 0.02f;    // 2% spacing
    float startX = boxX + (boxWidth - (itemsPerRow * imageSize + (itemsPerRow - 1) * spacing)) / 2;
    float startY = boxY + 80;

    // Draw items in a grid
    for (size_t i = 0; i < availableItems.size(); i++) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        
        float x = startX + col * (imageSize + spacing);
        float y = startY + row * (imageSize + spacing + 60); // Extra space for name and details

        // Draw item image
        std::string itemName = availableItems[i].getItemName();
        bool imageFound = false;
        
        for (const auto& [name, texture] : itemImages) {
            if (itemName.find(name) != std::string::npos) {
                Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
                Rectangle destRect = {x, y, imageSize, imageSize};
                DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                imageFound = true;
                break;
            }
        }
        
        // Fallback circle if no image found
        if (!imageFound) {
            Color itemColor;
            switch (availableItems[i].getColor()) {
                case ItemColor::Red: itemColor = RED; break;
                case ItemColor::Blue: itemColor = BLUE; break;
                case ItemColor::Yellow: itemColor = YELLOW; break;
                default: itemColor = WHITE; break;
            }
            DrawCircle(x + imageSize/2, y + imageSize/2, imageSize/2, itemColor);
        }

        // Draw item name
        Vector2 nameSize = MeasureTextEx(gameFont, itemName.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, itemName.c_str(), 
            {x + (imageSize - nameSize.x) / 2, y + imageSize + 5}, 
            gameFont.baseSize, 1, WHITE);

        // Draw item color
        std::string colorInfo = "Color: " + itemColorToString(availableItems[i].getColor());
        Vector2 colorSize = MeasureTextEx(gameFont, colorInfo.c_str(), gameFont.baseSize * 0.8f, 1);
        DrawTextEx(gameFont, colorInfo.c_str(), 
            {x + (imageSize - colorSize.x) / 2, y + imageSize + 25}, 
            gameFont.baseSize * 0.8f, 1, GRAY);

        // Draw item power
        std::string powerInfo = "Power: " + std::to_string(availableItems[i].getPower());
        Vector2 powerSize = MeasureTextEx(gameFont, powerInfo.c_str(), gameFont.baseSize * 0.8f, 1);
        DrawTextEx(gameFont, powerInfo.c_str(), 
            {x + (imageSize - powerSize.x) / 2, y + imageSize + 45}, 
            gameFont.baseSize * 0.8f, 1, GRAY);

        // Draw selection number
        std::string number = std::to_string(i + 1);
        Vector2 numSize = MeasureTextEx(gameFont, number.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, number.c_str(), 
            {x + 5, y + 5}, 
            gameFont.baseSize, 1, YELLOW);
    }

    // Close button
    float closeWidth = 100;
    float closeHeight = 40;
    float closeX = boxX + (boxWidth - closeWidth) / 2;
    float closeY = boxY + boxHeight - closeHeight - 20;
    
    Vector2 mousePos = GetMousePosition();
    Rectangle closeButton = {closeX, closeY, closeWidth, closeHeight};
    bool closeHover = CheckCollisionPointRec(mousePos, closeButton);
    
    DrawRectangleRec(closeButton, closeHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(closeButton, 2, WHITE);
    Vector2 closeTextSize = MeasureTextEx(gameFont, "Close", gameFont.baseSize, 1);
    DrawTextEx(gameFont, "Close", 
        {closeX + (closeWidth - closeTextSize.x) / 2, closeY + (closeHeight - closeTextSize.y) / 2}, 
        gameFont.baseSize, 1, WHITE);
}

void GameScreen::drawPickUpMessage() {
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Message box
    float boxWidth = screenWidth * 0.5f;
    float boxHeight = screenHeight * 0.2f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Draw the message
    Vector2 textSize = MeasureTextEx(titleFont, pickUpMessage.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, pickUpMessage.c_str(),
        {boxX + (boxWidth - textSize.x) / 2, boxY + (boxHeight - textSize.y) / 2},
        titleFont.baseSize, 1, WHITE);
}

void GameScreen::handlePickUpItemsClick(Vector2 mousePos) {
    // Check if close button was clicked
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;
    
    float closeWidth = 100;
    float closeHeight = 40;
    float closeX = boxX + (boxWidth - closeWidth) / 2;
    float closeY = boxY + boxHeight - closeHeight - 20;
    
    Rectangle closeButton = {closeX, closeY, closeWidth, closeHeight};
    if (CheckCollisionPointRec(mousePos, closeButton)) {
        cancelPickUpAction();
        return;
    }

    // Check if an item was clicked
    int itemsPerRow = 4;
    float imageSize = screenWidth * 0.12f;
    float spacing = screenWidth * 0.02f;
    float startX = boxX + (boxWidth - (itemsPerRow * imageSize + (itemsPerRow - 1) * spacing)) / 2;
    float startY = boxY + 80;

    for (size_t i = 0; i < availableItems.size(); i++) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        
        float x = startX + col * (imageSize + spacing);
        float y = startY + row * (imageSize + spacing + 60);

        Rectangle itemRect = {x, y, imageSize, imageSize + 60};
        if (CheckCollisionPointRec(mousePos, itemRect)) {
            pickUpItem(i);
            return;
        }
    }
}

void GameScreen::addGameMessage(const std::string& message, float duration) {
    GameMessage msg;
    msg.text = message;
    msg.timer = duration;
    msg.requiresAction = false;
    msg.onConfirm = nullptr;
    msg.onCancel = nullptr;
    gameMessages.push_back(msg);
    
    if (!showGameMessage) {
        processNextGameMessage();
    }
}

void GameScreen::addGameMessageWithAction(const std::string& message, std::function<void()> onConfirm, std::function<void()> onCancel) {
    GameMessage msg;
    msg.text = message;
    msg.timer = 0.0f; // No auto-dismiss for action messages
    msg.requiresAction = true;
    msg.onConfirm = onConfirm;
    msg.onCancel = onCancel;
    gameMessages.push_back(msg);
    
    if (!showGameMessage) {
        processNextGameMessage();
    }
}

void GameScreen::processNextGameMessage() {
    if (gameMessages.empty()) {
        showGameMessage = false;
        return;
    }
    
    currentGameMessage = gameMessages.front();
    gameMessages.erase(gameMessages.begin());
    showGameMessage = true;
}

void GameScreen::drawGameMessage() {
    if (!showGameMessage) return;

    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Determine if this is a long message (more than 100 characters)
    bool isLongMessage = currentGameMessage.text.length() > 100;
    
    // Adjust box size based on message length
    float boxWidth = isLongMessage ? screenWidth * 0.8f : screenWidth * 0.6f;
    float boxHeight = isLongMessage ? screenHeight * 0.5f : screenHeight * 0.3f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Choose font based on message length
    Font& messageFont = titleFont;
    float fontSize = titleFont.baseSize;

    // Draw the message with word wrapping
    float padding = 20.0f;
    float textX = boxX + padding;
    float textY = boxY + padding;
    float maxWidth = boxWidth - padding * 2;
    
    std::vector<std::string> lines;
    std::string currentLine = "";
    std::string word = "";
    
    for (char c : currentGameMessage.text) {
        if (c == ' ' || c == '\n') {
            if (!word.empty()) {
                std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                float lineWidth = MeasureTextEx(messageFont, testLine.c_str(), fontSize, 1).x;
                if (lineWidth <= maxWidth) {
                    currentLine = testLine;
                } else {
                    if (!currentLine.empty()) {
                        lines.push_back(currentLine);
                    }
                    currentLine = word;
                }
                word = "";
            }
            if (c == '\n') {
                lines.push_back(currentLine);
                currentLine = "";
            }
        } else {
            word += c;
        }
    }
    
    if (!word.empty()) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float lineWidth = MeasureTextEx(messageFont, testLine.c_str(), fontSize, 1).x;
        if (lineWidth <= maxWidth) {
            currentLine = testLine;
        } else {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }
            lines.push_back(word);
            currentLine = "";
        }
    }
    
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    // Draw lines
    float lineHeight = fontSize + 8;
    for (size_t i = 0; i < lines.size() && textY + i * lineHeight < boxY + boxHeight - padding - 50; i++) {
        DrawTextEx(messageFont, lines[i].c_str(), {textX, textY + i * lineHeight}, fontSize, 1, WHITE);
    }

    // Draw action buttons if required
    if (currentGameMessage.requiresAction) {
        float buttonWidth = 120;
        float buttonHeight = 40;
        float buttonY = boxY + boxHeight - buttonHeight - padding;
        float buttonSpacing = 20;
        
        float yesButtonX = boxX + (boxWidth / 2) - buttonWidth - (buttonSpacing / 2);
        float noButtonX = boxX + (boxWidth / 2) + (buttonSpacing / 2);

        Vector2 mousePos = GetMousePosition();

        // Yes button
        Rectangle yesButton = {yesButtonX, buttonY, buttonWidth, buttonHeight};
        bool yesHover = CheckCollisionPointRec(mousePos, yesButton);
        DrawRectangleRec(yesButton, yesHover ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(yesButton, 2, WHITE);
        Vector2 yesTextSize = MeasureTextEx(gameFont, "Yes", gameFont.baseSize, 1);
        DrawTextEx(gameFont, "Yes", {yesButtonX + (buttonWidth - yesTextSize.x) / 2, buttonY + (buttonHeight - yesTextSize.y) / 2}, gameFont.baseSize, 1, WHITE);

        // No button
        Rectangle noButton = {noButtonX, buttonY, buttonWidth, buttonHeight};
        bool noHover = CheckCollisionPointRec(mousePos, noButton);
        DrawRectangleRec(noButton, noHover ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(noButton, 2, WHITE);
        Vector2 noTextSize = MeasureTextEx(gameFont, "No", gameFont.baseSize, 1);
        DrawTextEx(gameFont, "No", {noButtonX + (buttonWidth - noTextSize.x) / 2, buttonY + (buttonHeight - noTextSize.y) / 2}, gameFont.baseSize, 1, WHITE);
    } else {
        // Draw close instruction for timed messages
        const char* closeText = "Click to continue";
        float closeTextWidth = MeasureTextEx(gameFont, closeText, gameFont.baseSize * 0.9f, 1).x;
        DrawTextEx(gameFont, closeText, {boxX + (boxWidth - closeTextWidth) / 2, boxY + boxHeight - 25}, gameFont.baseSize * 0.9f, 1, GRAY);
    }
}

void GameScreen::handleGameMessageClick(Vector2 mousePos) {
    if (!currentGameMessage.requiresAction) {
        // Auto-dismiss non-action messages
        processNextGameMessage();
        return;
    }

    // Handle action buttons
    float boxWidth = screenWidth * 0.6f;
    float boxHeight = screenHeight * 0.3f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;
    float padding = 20.0f;
    
    float buttonWidth = 120;
    float buttonHeight = 40;
    float buttonY = boxY + boxHeight - buttonHeight - padding;
    float buttonSpacing = 20;
    
    float yesButtonX = boxX + (boxWidth / 2) - buttonWidth - (buttonSpacing / 2);
    float noButtonX = boxX + (boxWidth / 2) + (buttonSpacing / 2);

    Rectangle yesButton = {yesButtonX, buttonY, buttonWidth, buttonHeight};
    Rectangle noButton = {noButtonX, buttonY, buttonWidth, buttonHeight};

    if (CheckCollisionPointRec(mousePos, yesButton)) {
        if (currentGameMessage.onConfirm) {
            currentGameMessage.onConfirm();
        }
        processNextGameMessage();
    } else if (CheckCollisionPointRec(mousePos, noButton)) {
        if (currentGameMessage.onCancel) {
            currentGameMessage.onCancel();
        }
        processNextGameMessage();
    }
}

void GameScreen::addImportantGameMessages(int initialTerrorLevel, const std::string& initialFrenziedMonster) {
    try {
        // Check terror level changes
        int currentTerrorLevel = terrorTracker.getLevel();
        if (currentTerrorLevel > initialTerrorLevel) {
            addGameMessage("Terror level increased to " + std::to_string(currentTerrorLevel) + "!");
            
            // Check if terror level reached maximum
            if (currentTerrorLevel >= 5) {
                addGameMessage("The terror has reached its peak! The monsters have won!");
            }
        }
        
        // Check frenzy marker changes
        if (currentFrenziedMonster != initialFrenziedMonster) {
            if (!currentFrenziedMonster.empty()) {
                addGameMessage("Frenzy marker given to " + currentFrenziedMonster + "!");
            }
        }
        
        // Check for monster card effects that add villagers
        std::string currentCardName = monsterManager.getCurrentCardName();
        if (currentCardName == "The Delivery") {
            addGameMessage("Wilbur And Chick was placed in Docks!");
        } else if (currentCardName == "Fortune Teller") {
            addGameMessage("Maleva was placed in Camp!");
        } else if (currentCardName == "Former Employer") {
            addGameMessage("Dr.Cranley was placed in Laboratory!");
        } else if (currentCardName == "Hurried Assistant") {
            addGameMessage("Fritz was placed in Tower!");
        } else if (currentCardName == "The Innocent") {
            addGameMessage("Maria was placed in Barn!");
        } else if (currentCardName == "Egyptian Expert") {
            addGameMessage("Prof.Pearson was placed in Cave!");
        } else if (currentCardName == "The Ichthyologist") {
            addGameMessage("Dr.Reed was placed in Institute!");
        } 
        
        // Check if monster deck is empty
        if (monsterManager.isEmpty()) {
            addGameMessage("The monster card deck is empty!");
        }
    } catch (const std::exception& e) {
        std::cout << "Error adding important game messages: " << e.what() << std::endl;
        addGameMessage("Error: Could not add important game messages");
    }
}

void GameScreen::capturePreMonsterPhaseState() {
    preMonsterPhaseState.heroLocations.clear();
    preMonsterPhaseState.villagerLocations.clear();
    preMonsterPhaseState.aliveVillagers.clear();
    
    // Capture hero locations
    if (currentHero && currentHero->getCurrentLocation()) {
        try {
            std::string heroName = currentHero->getHeroName();
            std::string locationName = currentHero->getCurrentLocation()->getName();
            preMonsterPhaseState.heroLocations[heroName] = locationName;
        } catch (const std::exception& e) {
            std::cout << "Error capturing current hero location: " << e.what() << std::endl;
        }
    }
    
    if (otherHero && otherHero->getCurrentLocation()) {
        try {
            std::string heroName = otherHero->getHeroName();
            std::string locationName = otherHero->getCurrentLocation()->getName();
            preMonsterPhaseState.heroLocations[heroName] = locationName;
        } catch (const std::exception& e) {
            std::cout << "Error capturing other hero location: " << e.what() << std::endl;
        }
    }
    
    // Capture villager locations and alive villagers
    try {
        for (const auto& [locName, location] : gameMap->locations) {
            if (location) {
                for (const auto& character : location->getCharacters()) {
                    // Check if it's a villager (not hero or monster)
                    if (character != "Archeologist" && character != "Mayor" && character != "Scientist" && character != "Courier" && 
                        character != "Dracula" && character != "Invisible man") {
                        preMonsterPhaseState.villagerLocations[character] = locName;
                        preMonsterPhaseState.aliveVillagers.push_back(character);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error capturing villager locations: " << e.what() << std::endl;
    }
}

void GameScreen::compareAndAddStateChangeMessages() {
    // Check for hero hospital movements
    if (currentHero && currentHero->getCurrentLocation()) {
        std::string currentLocation = currentHero->getCurrentLocation()->getName();
        std::string heroName = currentHero->getHeroName();
        
        // Check if hero name exists in the map before accessing it
        auto it = preMonsterPhaseState.heroLocations.find(heroName);
        if (it != preMonsterPhaseState.heroLocations.end()) {
            std::string previousLocation = it->second;
            
            if (currentLocation == "Hospital" && previousLocation != "Hospital") {
                addGameMessage(heroName + " was moved to Hospital due to monster attack!");
            }
        }
    }
    
    // Check for villager deaths
    std::vector<std::string> currentAliveVillagers;
    for (const auto& [locName, location] : gameMap->locations) {
        for (const auto& character : location->getCharacters()) {
            if (character != "Archeologist" && character != "Mayor" && character != "Scientist" && character != "Courier" && 
                character != "Dracula" && character != "Invisible man") {
                currentAliveVillagers.push_back(character);
            }
        }
    }
    
    // Find villagers that were alive before but are now dead
    for (const auto& villager : preMonsterPhaseState.aliveVillagers) {
        if (std::find(currentAliveVillagers.begin(), currentAliveVillagers.end(), villager) == currentAliveVillagers.end()) {
            addGameMessage(villager + " was killed by monster attack!");
        }
    }
}

void GameScreen::showHeroDefenseChoice(const std::vector<Item>& items, std::function<void(int)> onItemSelected, std::function<void()> onDefenseCanceled) {
    try {
        // Check if items vector is valid
        if (items.empty()) {
            std::cout << "Error: No items provided for hero defense choice!" << std::endl;
            if (onDefenseCanceled) {
                onDefenseCanceled();
            }
            return;
        }
        
        // Check if callbacks are valid
        if (!onItemSelected || !onDefenseCanceled) {
            std::cout << "Error: Invalid callbacks for hero defense choice!" << std::endl;
            return;
        }
        
        // Check if currentHero is valid
        if (!currentHero) {
            std::cout << "Error: No current hero for defense choice!" << std::endl;
            if (onDefenseCanceled) {
                onDefenseCanceled();
            }
            return;
        }
        
        // Store the items for display
        heroDefenseItems = items;
        
        // Store the actual hero item indices that correspond to these items
        heroDefenseItemIndices.clear();
        const auto& heroItems = currentHero->getItems();
        for (const auto& defenseItem : items) {
            for (size_t i = 0; i < heroItems.size(); i++) {
                if (heroItems[i].getItemName() == defenseItem.getItemName() && 
                    heroItems[i].getColor() == defenseItem.getColor() && 
                    heroItems[i].getPower() == defenseItem.getPower()) {
                    heroDefenseItemIndices.push_back(i);
                    break;
                }
            }
        }
        
        this->onItemSelectedIndex = onItemSelected;
        this->onDefenseCanceled = onDefenseCanceled;
        showHeroDefense = true;
    } catch (const std::exception& e) {
        std::cout << "Error in showHeroDefenseChoice: " << e.what() << std::endl;
        if (onDefenseCanceled) {
            onDefenseCanceled();
        }
    }
}

void GameScreen::cancelHeroDefense() {
    showHeroDefense = false;
    showHeroDefenseYesNoOverlay = false; // Reset the new state flag
    
    heroDefenseItems.clear();
    heroDefenseItemIndices.clear();

    // Clear the hero pointer and callbacks
    defendingHero = nullptr;
    onItemSelectedCallback = nullptr;
    onDefenseCanceledCallback = nullptr;
}

void GameScreen::drawHeroDefenseOverlay() {
    // Check if hero defense is active and items exist
    if (!showHeroDefense || heroDefenseItems.empty()) {
        return;
    }
    
    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Main overlay box
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Title
    std::string title = "Choose an item to defend yourself:";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    // Calculate grid layout
    int itemsPerRow = 4;
    float imageSize = screenWidth * 0.12f;  // 12% of screen width
    float spacing = screenWidth * 0.02f;    // 2% spacing
    float startX = boxX + (boxWidth - (itemsPerRow * imageSize + (itemsPerRow - 1) * spacing)) / 2;
    float startY = boxY + 80;

    // Draw items in a grid
    for (size_t i = 0; i < heroDefenseItems.size(); i++) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        
        float x = startX + col * (imageSize + spacing);
        float y = startY + row * (imageSize + spacing + 60); // Extra space for name and details

        // Draw item image
        std::string itemName = heroDefenseItems[i].getItemName();
        bool imageFound = false;
        
        for (const auto& [name, texture] : itemImages) {
            if (itemName.find(name) != std::string::npos) {
                Rectangle sourceRect = {0, 0, (float)texture.width, (float)texture.height};
                Rectangle destRect = {x, y, imageSize, imageSize};
                DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
                imageFound = true;
                break;
            }
        }
        
        // Fallback circle if no image found
        if (!imageFound) {
            Color itemColor;
            switch (heroDefenseItems[i].getColor()) {
                case ItemColor::Red: itemColor = RED; break;
                case ItemColor::Blue: itemColor = BLUE; break;
                case ItemColor::Yellow: itemColor = YELLOW; break;
                default: itemColor = WHITE; break;
            }
            DrawCircle(x + imageSize/2, y + imageSize/2, imageSize/2, itemColor);
        }

        // Draw item name
        Vector2 nameSize = MeasureTextEx(gameFont, itemName.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, itemName.c_str(), 
            {x + (imageSize - nameSize.x) / 2, y + imageSize + 5}, 
            gameFont.baseSize, 1, WHITE);

        // Draw item color
        std::string colorInfo = "Color: " + itemColorToString(heroDefenseItems[i].getColor());
        Vector2 colorSize = MeasureTextEx(gameFont, colorInfo.c_str(), gameFont.baseSize * 0.8f, 1);
        DrawTextEx(gameFont, colorInfo.c_str(), 
            {x + (imageSize - colorSize.x) / 2, y + imageSize + 25}, 
            gameFont.baseSize * 0.8f, 1, GRAY);

        // Draw item power
        std::string powerInfo = "Power: " + std::to_string(heroDefenseItems[i].getPower());
        Vector2 powerSize = MeasureTextEx(gameFont, powerInfo.c_str(), gameFont.baseSize * 0.8f, 1);
        DrawTextEx(gameFont, powerInfo.c_str(), 
            {x + (imageSize - powerSize.x) / 2, y + imageSize + 45}, 
            gameFont.baseSize * 0.8f, 1, GRAY);

        // Draw selection number
        std::string number = std::to_string(i + 1);
        Vector2 numSize = MeasureTextEx(gameFont, number.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, number.c_str(), 
            {x + 5, y + 5}, 
            gameFont.baseSize, 1, YELLOW);
    }

    // Cancel button
    float cancelWidth = 100;
    float cancelHeight = 40;
    float cancelX = boxX + (boxWidth - cancelWidth) / 2;
    float cancelY = boxY + boxHeight - cancelHeight - 20;
    
    Vector2 mousePos = GetMousePosition();
    Rectangle cancelButton = {cancelX, cancelY, cancelWidth, cancelHeight};
    bool cancelHover = CheckCollisionPointRec(mousePos, cancelButton);
    
    DrawRectangleRec(cancelButton, cancelHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(cancelButton, 2, WHITE);
    Vector2 cancelTextSize = MeasureTextEx(gameFont, "Cancel", gameFont.baseSize, 1);
    DrawTextEx(gameFont, "Cancel", 
        {cancelX + (cancelWidth - cancelTextSize.x) / 2, cancelY + (cancelHeight - cancelTextSize.y) / 2}, 
        gameFont.baseSize, 1, WHITE);
}

void GameScreen::handleHeroDefenseClick(Vector2 mousePos) {
    // Check if hero defense is active
    if (!showHeroDefense || heroDefenseItems.empty()) {
        return;
    }
    
    // Check if cancel button was clicked
    float boxWidth = screenWidth * 0.8f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;
    
    float cancelWidth = 100;
    float cancelHeight = 40;
    float cancelX = boxX + (boxWidth - cancelWidth) / 2;
    float cancelY = boxY + boxHeight - cancelHeight - 20;
    
    Rectangle cancelButton = {cancelX, cancelY, cancelWidth, cancelHeight};
    if (CheckCollisionPointRec(mousePos, cancelButton)) {
        cancelHeroDefense();
        return;
    }

    int itemsPerRow = 4;
    float imageSize = screenWidth * 0.12f;
    float spacing = screenWidth * 0.02f;
    float startX = boxX + (boxWidth - (itemsPerRow * imageSize + (itemsPerRow - 1) * spacing)) / 2;
    float startY = boxY + 80;

    for (size_t i = 0; i < heroDefenseItems.size(); i++) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        
        float x = startX + col * (imageSize + spacing);
        float y = startY + row * (imageSize + spacing + 60);

        Rectangle itemRect = {x, y, imageSize, imageSize + 60};
        if (CheckCollisionPointRec(mousePos, itemRect)) {
            // Check if callback exists before calling it
            if (onItemSelectedIndex && i < heroDefenseItemIndices.size()) {
                onItemSelectedIndex(heroDefenseItemIndices[i]);
            }
            cancelHeroDefense();
            return;
        }
    }
}

void GameScreen::setGameOver(const std::string& message, const std::string& reason) {
    // Prevent the game over state from being triggered multiple times
    if (isGameOver) return;

    isGameOver = true;
    gameOverMessage = message;
    gameOverReason = reason;
    std::cout << "Game Over: " << message << " - " << reason << std::endl;
}

void GameScreen::drawGameOverScreen() {
    // Draw a semi-transparent background to dim the main game screen
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Define the dimensions and position of the pop-up box
    float boxWidth = screenWidth * 0.5f;
    float boxHeight = screenHeight * 0.4f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    // Draw the main panel
    DrawRectangleRec({boxX, boxY, boxWidth, boxHeight}, {40, 40, 40, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Draw the primary Game Over message (e.g., "VICTORY!")
    float titleFontSize = titleFont.baseSize * 2.0f;
    Vector2 titleSize = MeasureTextEx(titleFont, gameOverMessage.c_str(), titleFontSize, 2);
    float titleX = boxX + (boxWidth - titleSize.x) / 2;
    float titleY = boxY + screenHeight * 0.05f;
    DrawTextEx(titleFont, gameOverMessage.c_str(), {titleX, titleY}, titleFontSize, 2, titleColor);

    // Draw the reason for the game ending
    float reasonFontSize = gameFont.baseSize * 1.2f;
    Vector2 reasonSize = MeasureTextEx(gameFont, gameOverReason.c_str(), reasonFontSize, 1);
    float reasonX = boxX + (boxWidth - reasonSize.x) / 2;
    float reasonY = titleY + titleSize.y + screenHeight * 0.04f;
    DrawTextEx(gameFont, gameOverReason.c_str(), {reasonX, reasonY}, reasonFontSize, 1, textColor);

    // Define and draw the "Return to Main Menu" button
    float buttonWidth = screenWidth * 0.15f;
    float buttonHeight = screenHeight * 0.05f;
    float buttonX = boxX + (boxWidth - buttonWidth) / 2;
    float buttonY = boxY + boxHeight - buttonHeight - screenHeight * 0.03f;
    gameOverCloseButton = {buttonX, buttonY, buttonWidth, buttonHeight};

    bool isHovered = CheckCollisionPointRec(GetMousePosition(), gameOverCloseButton);
    Color currentButtonColor = isHovered ? buttonHoverColor : buttonColor;
    DrawRectangleRec(gameOverCloseButton, currentButtonColor);
    DrawRectangleLinesEx(gameOverCloseButton, 2, WHITE);

    const char* closeText = "MAIN MENU";
    float closeTextFontSize = gameFont.baseSize * 1.2f;
    Vector2 closeTextSize = MeasureTextEx(gameFont, closeText, closeTextFontSize, 1);
    float closeTextX = buttonX + (buttonWidth - closeTextSize.x) / 2;
    float closeTextY = buttonY + (buttonHeight - closeTextSize.y) / 2;
    DrawTextEx(gameFont, closeText, {closeTextX, closeTextY}, closeTextFontSize, 1, textColor);
}

void GameScreen::handleGameOverClick(Vector2 mousePos) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, gameOverCloseButton)) {
            returnToMenu = true;
        }
    }
}

void GameScreen::drawVisitFromDetectiveSelectionOverlay() {
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    float boxWidth = screenWidth * 0.5f;
    float boxHeight = screenHeight * 0.3f;
    float boxX = (screenWidth - boxWidth) / 2.0f;
    float boxY = (screenHeight - boxHeight) / 2.0f;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.08f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    std::string title = "Choose a location to place the Invisible Man";
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2, boxY + 20}, titleFont.baseSize, 1, WHITE);

    std::string instruction = "Click on any location on the map";
    Vector2 instructionSize = MeasureTextEx(gameFont, instruction.c_str(), gameFont.baseSize, 1);
    DrawTextEx(gameFont, instruction.c_str(), {boxX + (boxWidth - instructionSize.x) / 2, boxY + 60}, gameFont.baseSize, 1, {230, 230, 230, 255});

    // Cancel button
    float btnW = 140.0f, btnH = 44.0f;
    visitFromDetectiveCancelButton = {boxX + boxWidth - btnW - 20.0f, boxY + boxHeight - btnH - 20.0f, btnW, btnH};
    bool hovered = CheckCollisionPointRec(GetMousePosition(), visitFromDetectiveCancelButton);
    DrawRectangleRec(visitFromDetectiveCancelButton, hovered ? dangerColor : buttonColor);
    DrawRectangleLinesEx(visitFromDetectiveCancelButton, 2, WHITE);
    std::string cancel = "Cancel";
    Vector2 csz = MeasureTextEx(gameFont, cancel.c_str(), gameFont.baseSize, 1);
    DrawTextEx(gameFont, cancel.c_str(), {visitFromDetectiveCancelButton.x + (visitFromDetectiveCancelButton.width - csz.x) / 2.0f, visitFromDetectiveCancelButton.y + (visitFromDetectiveCancelButton.height - csz.y) / 2.0f}, gameFont.baseSize, 1, WHITE);
}

void GameScreen::handleVisitFromDetectiveSelectionClick(Vector2 mousePos) {
    // Cancel
    if (CheckCollisionPointRec(mousePos, visitFromDetectiveCancelButton)) {
        showVisitFromDetectiveSelection = false;
        showPerkSelection = true; // Go back to perk selection
        return;
    }

    // Check if clicked on a map location
    for (const auto& [name, location] : mapLocations) {
        if (CheckCollisionPointCircle(mousePos, location.position, location.radius)) {
            try {
                // Use the perk card with the selected location
                auto targetLocation = gameMap->getLocation(name);
                if (taskBoard.isInvisibleManDefeated()) {
                    addGameMessage("Invisible Man is already defeated. Perk has no effect.");
                } else if (invisibleMan && invisibleMan->getCurrentLocation()) {
                    auto currentLocation = invisibleMan->getCurrentLocation();
                    currentLocation->removeCharacter("Invisible man");
                    targetLocation->addCharacter("Invisible man");
                    invisibleMan->setCurrentLocation(targetLocation);
                    addGameMessage("Invisible Man moved to " + name);
                } else {
                    addGameMessage("Invisible Man is defeated.");
                }
                
                // Remove the perk card
                const auto& perks = currentHero->getPerkCards();
                for (size_t i = 0; i < perks.size(); ++i) {
                    if (perks[i].getType() == PerkType::VisitFromTheDetective) {
                        currentHero->removePerkCard(i);
                        break;
                    }
                }
                
                showVisitFromDetectiveSelection = false;
                initializeLocations();
            } catch (const std::exception& e) {
                addGameMessage(std::string("Error moving Invisible Man: ") + e.what());
            }
            break;
        }
    }
}

void GameScreen::drawAdvanceDefeatItemSelectionOverlay() {
    // Dim background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Main box
    float boxWidth = screenWidth * 0.75f;
    float boxHeight = screenHeight * 0.7f;
    float boxX = (screenWidth - boxWidth) / 2.0f;
    float boxY = (screenHeight - boxHeight) / 2.0f;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.08f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    std::string title;
    if (advanceDefeatAction == "advance") {
        title = (advanceDefeatTarget == "invisibleman") ? "Deliver Evidence at Precinct" : "Use Red Item to Damage Coffin";
    } else {
        title = (advanceDefeatTarget == "invisibleman") ? "Defeat Invisible Man (Use Red Item)" : "Defeat Dracula (Use Yellow Item)";
    }
    Vector2 titleSize = MeasureTextEx(titleFont, title.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, title.c_str(), {boxX + (boxWidth - titleSize.x) / 2.0f, boxY + 18.0f}, titleFont.baseSize, 1, WHITE);

    // Build eligible item list
    std::vector<Item> eligibleItems;
    itemSelectionButtons.clear();

    if (!currentHero) return;

    const auto heroItems = currentHero->getItems();

    auto addButton = [&](const Item& item, float x, float y, float w, float h) {
        Rectangle r = {x, y, w, h};
        itemSelectionButtons.push_back(r);
        bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRec(r, hovered ? buttonHoverColor : buttonColor);
        DrawRectangleLinesEx(r, 2, WHITE);
        // item name
        std::string name = item.getItemName();
        Vector2 nsz = MeasureTextEx(gameFont, name.c_str(), gameFont.baseSize, 1);
        DrawTextEx(gameFont, name.c_str(), {r.x + 12.0f, r.y + 10.0f}, gameFont.baseSize, 1, WHITE);
        // color/power
        std::string meta = std::string("Color: ") + itemColorToString(item.getColor()) + ", Power: " + std::to_string(item.getPower());
        DrawTextEx(gameFont, meta.c_str(), {r.x + 12.0f, r.y + 12.0f + nsz.y}, gameFont.baseSize * 0.9f, 1, {230,230,230,255});
        // origin location (for evidences)
        if (item.getLocation()) {
            std::string src = std::string("From: ") + item.getLocation()->getName();
            DrawTextEx(gameFont, src.c_str(), {r.x + 12.0f, r.y + 12.0f + nsz.y + gameFont.baseSize * 0.95f}, gameFont.baseSize * 0.85f, 1, GRAY);
        }
    };

    // Determine eligibility
    std::vector<int> eligibleIndices; // indices in heroItems corresponding to buttons

    if (advanceDefeatAction == "advance" && advanceDefeatTarget == "invisibleman") {
        // Evidence delivery at Precinct: items whose original location is one of the five and not yet delivered
        std::vector<std::string> clueLocations = {"Inn", "Mansion", "Barn", "Laboratory", "Institute"};
        for (size_t i = 0; i < heroItems.size(); ++i) {
            auto locPtr = heroItems[i].getLocation();
            if (!locPtr) continue;
            std::string locName = locPtr->getName();
            if (std::find(clueLocations.begin(), clueLocations.end(), locName) != clueLocations.end()) {
                if (!taskBoard.isClueDelivered(locName)) {
                    eligibleIndices.push_back((int)i);
                }
            }
        }
    } else if (advanceDefeatAction == "advance" && advanceDefeatTarget == "coffin") {
        for (size_t i = 0; i < heroItems.size(); ++i) {
            if (heroItems[i].getColor() == ItemColor::Red) eligibleIndices.push_back((int)i);
        }
    } else if (advanceDefeatAction == "defeat" && advanceDefeatTarget == "invisibleman") {
        if (taskBoard.allCluesDelivered()) {
            for (size_t i = 0; i < heroItems.size(); ++i) {
                if (heroItems[i].getColor() == ItemColor::Red) eligibleIndices.push_back((int)i);
            }
        }
    } else if (advanceDefeatAction == "defeat" && advanceDefeatTarget == "dracula") {
        if (taskBoard.allCoffinsDestroyed()) {
            for (size_t i = 0; i < heroItems.size(); ++i) {
                if (heroItems[i].getColor() == ItemColor::Yellow) eligibleIndices.push_back((int)i);
            }
        }
    }

    for (int idx : eligibleIndices) eligibleItems.push_back(heroItems[idx]);

    // grid
    const int columns = 2;
    float padding = 20.0f;
    float cardW = (boxWidth - padding * (columns + 1)) / columns;
    float cardH = 90.0f;
    float gridTop = boxY + 70.0f;

    itemSelectionButtons.clear();
    for (size_t i = 0; i < eligibleItems.size(); ++i) {
        int row = static_cast<int>(i) / columns;
        int col = static_cast<int>(i) % columns;
        float x = boxX + padding + col * (cardW + padding);
        float y = gridTop + row * (cardH + padding);
        addButton(eligibleItems[i], x, y, cardW, cardH);
    }

    // Cancel button
    float btnW = 140.0f, btnH = 44.0f;
    itemSelectionCancelButton = {boxX + boxWidth - btnW - 20.0f, boxY + boxHeight - btnH - 20.0f, btnW, btnH};
    bool hovered = CheckCollisionPointRec(GetMousePosition(), itemSelectionCancelButton);
    DrawRectangleRec(itemSelectionCancelButton, hovered ? dangerColor : buttonColor);
    DrawRectangleLinesEx(itemSelectionCancelButton, 2, WHITE);
    std::string cancel = "Cancel";
    Vector2 csz = MeasureTextEx(gameFont, cancel.c_str(), gameFont.baseSize, 1);
    DrawTextEx(gameFont, cancel.c_str(), {itemSelectionCancelButton.x + (itemSelectionCancelButton.width - csz.x) / 2.0f, itemSelectionCancelButton.y + (itemSelectionCancelButton.height - csz.y) / 2.0f}, gameFont.baseSize, 1, WHITE);
}

void GameScreen::handleAdvanceDefeatItemSelectionClick(Vector2 mousePos) {
    // Cancel
    if (CheckCollisionPointRec(mousePos, itemSelectionCancelButton)) {
        showAdvanceItemSelection = false;
        showDefeatItemSelection = false;
        return;
    }

    if (!currentHero) return;
    auto heroItems = currentHero->getItems();

    // Rebuild eligible indices in same order used in draw
    std::vector<int> eligibleIndices;
    if (advanceDefeatAction == "advance" && advanceDefeatTarget == "invisibleman") {
        std::vector<std::string> clueLocations = {"Inn", "Mansion", "Barn", "Laboratory", "Institute"};
        for (size_t i = 0; i < heroItems.size(); ++i) {
            auto locPtr = heroItems[i].getLocation();
            if (!locPtr) continue;
            std::string locName = locPtr->getName();
            if (std::find(clueLocations.begin(), clueLocations.end(), locName) != clueLocations.end()) {
                if (!taskBoard.isClueDelivered(locName)) eligibleIndices.push_back((int)i);
            }
        }
    } else if (advanceDefeatAction == "advance" && advanceDefeatTarget == "coffin") {
        for (size_t i = 0; i < heroItems.size(); ++i) {
            if (heroItems[i].getColor() == ItemColor::Red) eligibleIndices.push_back((int)i);
        }
    } else if (advanceDefeatAction == "defeat" && advanceDefeatTarget == "invisibleman") {
        if (taskBoard.allCluesDelivered()) {
            for (size_t i = 0; i < heroItems.size(); ++i) {
                if (heroItems[i].getColor() == ItemColor::Red) eligibleIndices.push_back((int)i);
            }
        }
    } else if (advanceDefeatAction == "defeat" && advanceDefeatTarget == "dracula") {
        if (taskBoard.allCoffinsDestroyed()) {
            for (size_t i = 0; i < heroItems.size(); ++i) {
                if (heroItems[i].getColor() == ItemColor::Yellow) eligibleIndices.push_back((int)i);
            }
        }
    }

    // Map click to selected item index
    for (size_t i = 0; i < itemSelectionButtons.size() && i < eligibleIndices.size(); ++i) {
        if (CheckCollisionPointRec(mousePos, itemSelectionButtons[i])) {
            int invIndex = eligibleIndices[i];
            // Scientist ability
            if (currentHero->getHeroName() == "Scientist") {
                try { currentHero->ability(invIndex); } catch (...) {}
            }

            Item selected = heroItems[invIndex];
            try {
                if (advanceDefeatAction == "advance" && advanceDefeatTarget == "invisibleman") {
                    std::string srcLoc = selected.getLocation() ? selected.getLocation()->getName() : "";
                    taskBoard.deliverClue(srcLoc);
                    addGameMessage(currentHero->getPlayerName() + std::string(" (") + currentHero->getHeroName() + ") delivered evidence from " + srcLoc + ".");
                    currentHero->removeItem(invIndex);
                    remainingActions--;
                } else if (advanceDefeatAction == "advance" && advanceDefeatTarget == "coffin") {
                    taskBoard.addStrengthToCoffin(currentHero->getCurrentLocation()->getName(), selected.getPower());
                    addGameMessage("Coffin at " + currentHero->getCurrentLocation()->getName() + " damaged by " + std::to_string(selected.getPower()) + ".");
                    currentHero->removeItem(invIndex);
                    remainingActions--;
                } else if (advanceDefeatAction == "defeat" && advanceDefeatTarget == "invisibleman") {
                    if (!taskBoard.allCluesDelivered()) {
                        addGameMessage("All evidences are not delivered yet.");
                    } else {
                        taskBoard.addStrengthToInvisibleMan(selected.getPower());
                        addGameMessage("Invisible Man damaged by " + std::to_string(selected.getPower()) + ".");
                        currentHero->removeItem(invIndex);
                        remainingActions--;
                        if (taskBoard.getInvisibleManDefeatStrength() >= 9) {
                            taskBoard.defeatInvisibleMan();
                            addGameMessage("Invisible Man has been defeated!");
                            // Remove Invisible Man from the board and disable further actions
                            try {
                                if (invisibleMan && invisibleMan->getCurrentLocation()) {
                                    invisibleMan->getCurrentLocation()->removeCharacter("Invisible man");
                                }
                            } catch (...) {}
                            invisibleMan.reset();
                            // Update frenzy marker if needed
                            if (frenzyMarker) {
                                if (dracula) {
                                    frenzyMarker->setCurrentFrenzied(static_cast<Dracula*>(dracula.get()));
                                } else {
                                    frenzyMarker->setCurrentFrenzied(nullptr);
                                    currentFrenziedMonster.clear();
                                }
                            }
                        }
                    }
                } else if (advanceDefeatAction == "defeat" && advanceDefeatTarget == "dracula") {
                    if (!taskBoard.allCoffinsDestroyed()) {
                        addGameMessage("All coffins must be destroyed first.");
                    } else {
                        taskBoard.addStrengthToDracula(selected.getPower());
                        addGameMessage("Dracula damaged by " + std::to_string(selected.getPower()) + ".");
                        currentHero->removeItem(invIndex);
                        remainingActions--;
                        if (taskBoard.getDraculaDefeatStrength() >= 6) {
                            addGameMessage("Dracula has been defeated!");
                            // Remove Dracula from the board and disable further actions
                            try {
                                if (dracula && dracula->getCurrentLocation()) {
                                    dracula->getCurrentLocation()->removeCharacter("Dracula");
                                }
                            } catch (...) {}
                            dracula.reset();
                            // Update frenzy marker if needed
                            if (frenzyMarker) {
                                if (invisibleMan) {
                                    frenzyMarker->setCurrentFrenzied(static_cast<InvisibleMan*>(invisibleMan.get()));
                                } else {
                                    frenzyMarker->setCurrentFrenzied(nullptr);
                                    currentFrenziedMonster.clear();
                                }
                            }
                        }
                    }
                }
            } catch (const std::exception& e) {
                addGameMessage(std::string("Error applying item: ") + e.what());
            }

            showAdvanceItemSelection = false;
            showDefeatItemSelection = false;
            initializeLocations();
            if (remainingActions <= 0) { showEndTurnPrompt = true; endTurnPromptTimer = 3.0f; }
            return;
        }
    }
}

void GameScreen::showHeroDefenseYesNoChoice(Hero* hero, 
                                            std::function<void(int)> onItemSelected, 
                                            std::function<void()> onCanceled) {
    if (!hero) return;

    // Store the hero and the final callbacks
    defendingHero = hero;
    onItemSelectedCallback = onItemSelected;
    onDefenseCanceledCallback = onCanceled;

    // Activate the Yes/No overlay
    showHeroDefenseYesNoOverlay = true;
}

void GameScreen::drawHeroDefenseYesNoOverlay() {
    if (!showHeroDefenseYesNoOverlay) return;

    // Dark semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Message box
    float boxWidth = screenWidth * 0.5f;
    float boxHeight = screenHeight * 0.3f;
    float boxX = (screenWidth - boxWidth) / 2;
    float boxY = (screenHeight - boxHeight) / 2;

    DrawRectangleRounded({boxX, boxY, boxWidth, boxHeight}, 0.1f, 8, {60, 60, 60, 255});
    DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 3, WHITE);

    // Draw the question text
    std::string message = defendingHero->getHeroName() + " is under attack!\nUse an item to defend?";
    Vector2 textSize = MeasureTextEx(titleFont, message.c_str(), titleFont.baseSize, 1);
    DrawTextEx(titleFont, message.c_str(),
               {boxX + (boxWidth - textSize.x) / 2, boxY + 40},
               titleFont.baseSize, 1, WHITE);

    // "Yes" and "No" buttons
    float buttonWidth = boxWidth * 0.3f;
    float buttonHeight = boxHeight * 0.2f;
    float buttonY = boxY + boxHeight - buttonHeight - 30;
    float buttonSpacing = boxWidth * 0.1f;
    
    float yesButtonX = boxX + (boxWidth / 2) - buttonWidth - (buttonSpacing / 2);
    float noButtonX = boxX + (boxWidth / 2) + (buttonSpacing / 2);

    defenseYesButton = {yesButtonX, buttonY, buttonWidth, buttonHeight};
    defenseNoButton = {noButtonX, buttonY, buttonWidth, buttonHeight};

    Vector2 mousePos = GetMousePosition();

    // Draw Yes button
    bool yesHover = CheckCollisionPointRec(mousePos, defenseYesButton);
    DrawRectangleRec(defenseYesButton, yesHover ? buttonHoverColor : buttonColor);
    DrawRectangleLinesEx(defenseYesButton, 2, WHITE);
    DrawTextEx(gameFont, "Yes", {yesButtonX + 60, buttonY + 15}, gameFont.baseSize, 1, WHITE);

    // Draw No button
    bool noHover = CheckCollisionPointRec(mousePos, defenseNoButton);
    DrawRectangleRec(defenseNoButton, noHover ? dangerColor : buttonColor);
    DrawRectangleLinesEx(defenseNoButton, 2, WHITE);
    DrawTextEx(gameFont, "No", {noButtonX + 70, buttonY + 15}, gameFont.baseSize, 1, WHITE);
}

void GameScreen::handleHeroDefenseYesNoClick(Vector2 mousePos) {
    if (CheckCollisionPointRec(mousePos, defenseYesButton)) {
        handleHeroDefenseYesNoChoice(true);
    } else if (CheckCollisionPointRec(mousePos, defenseNoButton)) {
        handleHeroDefenseYesNoChoice(false);
    }
}

void GameScreen::handleHeroDefenseYesNoChoice(bool useItem) {
    // Hide the Yes/No prompt regardless of the choice
    showHeroDefenseYesNoOverlay = false;

    if (useItem) {
        // Player chose YES. Now show the item selection grid.
        // We pass the stored callbacks to the next stage.
        showHeroDefenseChoice(defendingHero->getItems(), onItemSelectedCallback, onDefenseCanceledCallback);
    } else {
        // Player chose NO. Execute the cancel/failure callback immediately.
        if (onDefenseCanceledCallback) {
            onDefenseCanceledCallback();
        }
        // Clean up the entire defense state
        cancelHeroDefense();
    }
}
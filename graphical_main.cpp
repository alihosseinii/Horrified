#include <raylib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include "game.hpp"
#include "game_screen.hpp"
#include "savemanager.hpp"
#include "gamestate.hpp"

struct MenuPlayerInfo {
    std::string name;
    std::string lastGarlicTime;
};

class GraphicalMainMenu {
private:
    const int screenWidth = 2000;
    const int screenHeight = 1000;
    
    enum MenuState {
        MAIN_MENU,
        LOAD_GAME,
        PLAYER_SETUP_1,
        PLAYER_SETUP_2,
        HERO_SELECTION_1,
        HERO_SELECTION_2,
        GAME_SCREEN,
        EXIT
    };
    
    MenuState currentState = MAIN_MENU;
    
    int buttonWidth;
    int buttonHeight;
    int buttonSpacing;
    
    Color backgroundColor = {20, 20, 20, 255};
    Color buttonColor = {60, 60, 60, 255};
    Color buttonHoverColor = {80, 80, 80, 255};
    Color textColor = {255, 255, 255, 255};
    Color titleColor = {200, 50, 50, 255};
    Color inputBoxColor = {40, 40, 40, 255};
    Color inputBoxActiveColor = {60, 60, 60, 255};
    
    Texture2D backgroundTexture;
    bool backgroundLoaded = false;
    
    Font titleFont;
    Font menuFont;
    
    std::vector<std::string> saveSlotNames;
    std::vector<bool> saveSlotExists;
    
    std::vector<MenuPlayerInfo> players;
    int currentPlayerIndex = 0;
    char playerNameInput[256] = "";
    char garlicTimeInput[256] = "";
    bool nameInputActive = false;
    bool garlicInputActive = false;
    
    std::string startingPlayerName;
    std::string otherPlayerName;
    std::string selectedHero;
    std::string firstPlayerHero;
    std::string secondPlayerHero;
    bool heroSelectionComplete = false;
    
    std::unique_ptr<GameScreen> gameScreen;
    
    std::string nameError = "";
    std::string garlicError = "";
    bool showNameError = false;
    bool showGarlicError = false;
    
public:
    GraphicalMainMenu() {
        InitWindow(screenWidth, screenHeight, "Horrified");
        SetTargetFPS(60);
        
        buttonWidth = screenWidth * 0.2; 
        buttonHeight = screenHeight * 0.06;  
        buttonSpacing = screenHeight * 0.08;  
        
        std::string titleFontPath = getAssetPath("fonts/Creepster-Regular.ttf");
        std::string menuFontPath = getAssetPath("fonts/Rubik-Regular.ttf");
        
        int titleFontSize = screenHeight * 0.08;  
        int menuFontSize = screenHeight * 0.025;  
        
        titleFont = LoadFontEx(titleFontPath.c_str(), titleFontSize, 0, 0);
        menuFont = LoadFontEx(menuFontPath.c_str(), menuFontSize, 0, 0);
        
        if (titleFont.texture.id == 0) {
            std::cout << "Failed to load title font: " << titleFontPath << std::endl;
            titleFont = GetFontDefault();
        } else {
            std::cout << "Title font loaded successfully from: " << titleFontPath << std::endl;
        }
        if (menuFont.texture.id == 0) {
            std::cout << "Failed to load menu font: " << menuFontPath << std::endl;
            menuFont = GetFontDefault();
        } else {
            std::cout << "Menu font loaded successfully from: " << menuFontPath << std::endl;
        }
        
        std::string backgroundPath = getAssetPath("images/background.jpg");
        if (FileExists(backgroundPath.c_str())) {
            backgroundTexture = LoadTexture(backgroundPath.c_str());
            backgroundLoaded = true;
            std::cout << "Background image loaded successfully from: " << backgroundPath << std::endl;
        } else {
            std::cout << "Failed to load background image: " << backgroundPath << std::endl;
        }
        
        updateSaveSlots();
        
        players.clear();
        players.resize(2); 
        strcpy(playerNameInput, "");
        strcpy(garlicTimeInput, "");
    }
    
    ~GraphicalMainMenu() {
        if (backgroundLoaded && backgroundTexture.id != 0) {
            UnloadTexture(backgroundTexture);
        }
        if (titleFont.texture.id != 0 && titleFont.texture.id != GetFontDefault().texture.id) {
            UnloadFont(titleFont);
        }
        if (menuFont.texture.id != 0 && menuFont.texture.id != GetFontDefault().texture.id) {
            UnloadFont(menuFont);
        }
        CloseWindow();
    }
    
    bool validatePlayerName(const std::string& name) {
        if (name.empty()) {
            nameError = "Player name cannot be empty";
            showNameError = true;
            return false;
        }
        if (name.length() < 2) {
            nameError = "Player name must be at least 2 characters";
            showNameError = true;
            return false;
        }
        
        for (int i = 0; i < players.size(); i++) {
            if (i != currentPlayerIndex && players[i].name == name) {
                nameError = "Player name already exists";
                showNameError = true;
                return false;
            }
        }
        
        showNameError = false;
        return true;
    }
    
    std::string getAssetPath(const std::string& relativePath) {
        std::vector<std::string> possiblePaths = {
            relativePath,                  
            "../" + relativePath,           
            "../../" + relativePath,     
            "./" + relativePath           
        };
        
        for (const auto& path : possiblePaths) {
            if (FileExists(path.c_str())) {
                return path;
            }
        }
        return relativePath; 
    }
    
    bool validateGarlicTime(const std::string& garlicTime) {
        if (garlicTime.empty()) {
            garlicError = "Garlic time cannot be empty";
            showGarlicError = true;
            return false;
        }
        
        for (char c : garlicTime) {
            if (!isdigit(c)) {
                garlicError = "Garlic time must be a number (no letters allowed)";
                showGarlicError = true;
                return false;
            }
        }
        
        int time = std::stoi(garlicTime);
        if (time < 0) {
            garlicError = "Garlic time cannot be negative";
            showGarlicError = true;
            return false;
        }
        if (time > 1000) {
            garlicError = "Garlic time seems too high (max 1000 hours)";
            showGarlicError = true;
            return false;
        }
        
        showGarlicError = false;
        return true;
    }
    
    void restorePlayerData() {
        if (currentPlayerIndex < players.size()) {
            strcpy(playerNameInput, players[currentPlayerIndex].name.c_str());
            strcpy(garlicTimeInput, players[currentPlayerIndex].lastGarlicTime.c_str());
        } else {
            strcpy(playerNameInput, "");
            strcpy(garlicTimeInput, "");
        }
        showNameError = false;
        showGarlicError = false;
    }
    
    void updateSaveSlots() {
        saveSlotNames.clear();
        saveSlotExists.clear();
        
        for (int i = 1; i <= 5; i++) {
            std::string filename = "saves/save_" + std::to_string(i) + ".bin";
            bool exists = FileExists(filename.c_str());
            saveSlotExists.push_back(exists);
            
            if (exists) {
                saveSlotNames.push_back("Save " + std::to_string(i));
            } else {
                saveSlotNames.push_back("Empty Slot " + std::to_string(i));
            }
        }
    }
    
    void drawBackground() {
        if (backgroundLoaded) {
            DrawTexturePro(backgroundTexture, 
                Rectangle{0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
                Rectangle{0, 0, (float)screenWidth, (float)screenHeight},
                Vector2{0, 0}, 0.0f, WHITE);
        } else {
            for (int y = 0; y < screenHeight; y++) {
                float ratio = (float)y / screenHeight;
                Color color = {
                    (unsigned char)(20 + ratio * 30),
                    (unsigned char)(20 + ratio * 20),
                    (unsigned char)(20 + ratio * 40),
                    255
                };
                DrawLine(0, y, screenWidth, y, color);
            }
        }
    }
    
    void drawTitle() {
        const char* title = "HORRIFIED";
        float titleFontSize = titleFont.baseSize * 2.0f;
        Vector2 titleSize = MeasureTextEx(titleFont, title, titleFontSize, 2);
        int titleX = (screenWidth - titleSize.x) / 2;
        int titleY = screenHeight * 0.05;  
        
        DrawTextEx(titleFont, title, {(float)(titleX + 3), (float)(titleY + 3)}, titleFontSize, 2, BLACK);
        DrawTextEx(titleFont, title, {(float)titleX, (float)titleY}, titleFontSize, 2, titleColor);
    }
    
    void drawMainMenu() {
        const char* buttons[] = {"START GAME", "LOAD GAME", "EXIT"};
        int startY = screenHeight * 0.6; 
        
        for (int i = 0; i < 3; i++) {
            int buttonX = (screenWidth - buttonWidth) / 2;
            int buttonY = startY + i * buttonSpacing;
            
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), 
                Rectangle{(float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight});
            
            Color buttonColorCurrent = isHovered ? buttonHoverColor : buttonColor;
            DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, buttonColorCurrent);
            DrawRectangleLines(buttonX, buttonY, buttonWidth, buttonHeight, WHITE);
            
            float buttonFontSize = menuFont.baseSize * 1.5f;
            Vector2 textSize = MeasureTextEx(menuFont, buttons[i], buttonFontSize, 1);
            int textX = buttonX + (buttonWidth - textSize.x) / 2;
            int textY = buttonY + (buttonHeight - textSize.y) / 2;
            
            DrawTextEx(menuFont, buttons[i], {(float)textX, (float)textY}, buttonFontSize, 1, textColor);
            
            if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                switch (i) {
                    case 0: 
                        currentState = PLAYER_SETUP_1;
                        currentPlayerIndex = 0;
                        restorePlayerData();
                        break;
                    case 1: 
                        currentState = LOAD_GAME;
                        updateSaveSlots();
                        break;
                    case 2:
                        currentState = EXIT;
                        break;
                }
            }
        }
    }
    
    void drawPlayerSetup() {
        const char* title = "PLAYER SETUP";
        float titleFontSize = menuFont.baseSize * 2.0f;
        Vector2 titleSize = MeasureTextEx(menuFont, title, titleFontSize, 2);
        int titleX = (screenWidth - titleSize.x) / 2;
        DrawTextEx(menuFont, title, {(float)titleX, (float)(screenHeight * 0.20)}, titleFontSize, 2, textColor);
        
        std::string playerText = "Player " + std::to_string(currentPlayerIndex + 1);
        float playerFontSize = menuFont.baseSize * 1.8f;
        Vector2 playerTextSize = MeasureTextEx(menuFont, playerText.c_str(), playerFontSize, 1);
        int playerTextX = (screenWidth - playerTextSize.x) / 2;
        int playerTextY = screenHeight * 0.25;
        DrawTextEx(menuFont, playerText.c_str(), {(float)playerTextX, (float)playerTextY}, playerFontSize, 1, textColor);
        
        int inputWidth = screenWidth * 0.4;
        int inputX = (screenWidth - inputWidth) / 2;
        int labelY = screenHeight * 0.35;
        int inputY = screenHeight * 0.4;
        float labelFontSize = menuFont.baseSize * 1.5f;
        float inputFontSize = menuFont.baseSize * 1.3f;
        
        DrawTextEx(menuFont, "Player Name:", {(float)inputX, (float)labelY}, labelFontSize, 1, textColor);
        
        Rectangle nameBox = {(float)inputX, (float)inputY + 5, (float)inputWidth, (float)(screenHeight * 0.04)};
        bool nameHovered = CheckCollisionPointRec(GetMousePosition(), nameBox);
        Color nameBoxColor = nameInputActive ? inputBoxActiveColor : (nameHovered ? buttonHoverColor : inputBoxColor);
        if (showNameError) {
            nameBoxColor = {150, 50, 50, 255}; 
        }
        DrawRectangleRec(nameBox, nameBoxColor);
        DrawRectangleLinesEx(nameBox, 2, WHITE);
        
        DrawTextEx(menuFont, playerNameInput, {(float)(inputX + 10), (float)(inputY + 10)}, inputFontSize, 1, textColor);
        
        if (showNameError) {
            DrawTextEx(menuFont, nameError.c_str(), {(float)inputX, (float)(inputY + screenHeight * 0.05)}, menuFont.baseSize * 0.8f, 1, RED);
        }
        
        int garlicLabelY = screenHeight * 0.5;
        int garlicInputY = screenHeight * 0.55;
        
        DrawTextEx(menuFont, "Last Time You Ate Garlic (in hours):", {(float)inputX, (float)garlicLabelY}, labelFontSize, 1, textColor);
        
        Rectangle garlicBox = {(float)inputX, (float)garlicInputY + 5, (float)inputWidth, (float)(screenHeight * 0.04)};
        bool garlicHovered = CheckCollisionPointRec(GetMousePosition(), garlicBox);
        Color garlicBoxColor = garlicInputActive ? inputBoxActiveColor : (garlicHovered ? buttonHoverColor : inputBoxColor);
        if (showGarlicError) {
            garlicBoxColor = {150, 50, 50, 255}; 
        }
        DrawRectangleRec(garlicBox, garlicBoxColor);
        DrawRectangleLinesEx(garlicBox, 2, WHITE);
        
        DrawTextEx(menuFont, garlicTimeInput, {(float)(inputX + 10), (float)(garlicInputY + 10)}, inputFontSize, 1, textColor);
        
        if (showGarlicError) {
            DrawTextEx(menuFont, garlicError.c_str(), {(float)inputX, (float)(garlicInputY + screenHeight * 0.05)}, menuFont.baseSize * 0.8f, 1, RED);
        }
        
        const char* submitText = currentPlayerIndex == 0 ? "NEXT PLAYER" : "START GAME";
        const char* backText = "BACK";
        
        int submitButtonX = (screenWidth - buttonWidth) / 2;
        int submitButtonY = screenHeight * 0.7;
        Rectangle submitButton = {(float)submitButtonX, (float)submitButtonY, (float)buttonWidth, (float)buttonHeight};
        bool submitHovered = CheckCollisionPointRec(GetMousePosition(), submitButton);
        
        Color submitColor = submitHovered ? buttonHoverColor : buttonColor;
        DrawRectangleRec(submitButton, submitColor);
        DrawRectangleLinesEx(submitButton, 2, WHITE);
        
        float submitFontSize = menuFont.baseSize * 1.5f;
        Vector2 submitTextSize = MeasureTextEx(menuFont, submitText, submitFontSize, 1);
        int submitTextX = submitButtonX + (buttonWidth - submitTextSize.x) / 2;
        int submitTextY = submitButtonY + (buttonHeight - submitTextSize.y) / 2;
        DrawTextEx(menuFont, submitText, {(float)submitTextX, (float)submitTextY}, submitFontSize, 1, textColor);
        
        float backFontSize = menuFont.baseSize * 1.3f;
        Vector2 backTextSize = MeasureTextEx(menuFont, backText, backFontSize, 1);
        int backX = screenWidth * 0.05;  
        int backY = screenHeight * 0.9; 
        
        bool backHovered = CheckCollisionPointRec(GetMousePosition(), 
            Rectangle{(float)backX - 10, (float)backY - 5, (float)(backTextSize.x + 20), (float)(backTextSize.y + 10)});
        
        if (backHovered) {
            DrawTextEx(menuFont, backText, {(float)(backX - 10), (float)(backY - 5)}, backFontSize, 1, buttonHoverColor);
        } else {
            DrawTextEx(menuFont, backText, {(float)backX, (float)backY}, backFontSize, 1, textColor);
        }
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (nameHovered) {
                nameInputActive = true;
                garlicInputActive = false;
            } else if (garlicHovered) {
                nameInputActive = false;
                garlicInputActive = true;
            } else {
                nameInputActive = false;
                garlicInputActive = false;
            }
            
            if (submitHovered) {
                bool nameValid = validatePlayerName(std::string(playerNameInput));
                bool garlicValid = validateGarlicTime(std::string(garlicTimeInput));
                
                if (nameValid && garlicValid) {
                    players[currentPlayerIndex].name = std::string(playerNameInput);
                    players[currentPlayerIndex].lastGarlicTime = std::string(garlicTimeInput);
                    
                    if (currentPlayerIndex == 0) {
                        currentPlayerIndex = 1;
                        restorePlayerData();
                    } else {
                        int player1GarlicTime = std::stoi(players[0].lastGarlicTime);
                        int player2GarlicTime = std::stoi(players[1].lastGarlicTime);
                        
                        if (player1GarlicTime < player2GarlicTime) {
                            startingPlayerName = players[0].name;
                            otherPlayerName = players[1].name;
                        } else {
                            startingPlayerName = players[1].name;
                            otherPlayerName = players[0].name;
                        }
                        
                        currentState = HERO_SELECTION_1;
                        return;
                    }
                }
            }
            
            if (backHovered) {
                if (currentPlayerIndex == 0) {
                    currentState = MAIN_MENU;
                } else {
                    currentPlayerIndex = 0;
                    restorePlayerData();
                }
            }
        }
        
        if (nameInputActive) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (strlen(playerNameInput) < 255)) {
                    strcat(playerNameInput, (char*)&key);
                }
                key = GetCharPressed();
            }
            
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(playerNameInput);
                if (len > 0) {
                    playerNameInput[len - 1] = '\0';
                }
            }
        }
        
        if (garlicInputActive) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= '0' && key <= '9') && (strlen(garlicTimeInput) < 255)) {
                    strcat(garlicTimeInput, (char*)&key);
                }
                key = GetCharPressed();
            }
            
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(garlicTimeInput);
                if (len > 0) {
                    garlicTimeInput[len - 1] = '\0';
                }
            }
        }
    }
    
    void drawLoadGameMenu() {
        const char* title = "LOAD GAME";
        float titleFontSize = menuFont.baseSize * 2.0f;
        Vector2 titleSize = MeasureTextEx(menuFont, title, titleFontSize, 2);
        int titleX = (screenWidth - titleSize.x) / 2;
        DrawTextEx(menuFont, title, {(float)titleX, (float)(screenHeight * 0.25)}, titleFontSize, 2, textColor);
        
        int startY = screenHeight / 2;
        for (int i = 0; i < 5; i++) {
            int buttonX = (screenWidth - buttonWidth) / 2;  
            int buttonY = startY + i * (buttonHeight + 20);
            
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), 
                Rectangle{(float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight});
            
            Color buttonColorCurrent = isHovered ? buttonHoverColor : buttonColor;
            if (!saveSlotExists[i]) {
                buttonColorCurrent = {40, 40, 40, 255};
            }
            
            DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, buttonColorCurrent);
            DrawRectangleLines(buttonX, buttonY, buttonWidth, buttonHeight, WHITE);
            
            const char* slotText = saveSlotNames[i].c_str();
            int textWidth = MeasureTextEx(menuFont, slotText, menuFont.baseSize, 1).x;
            int textX = buttonX + (buttonWidth - textWidth) / 2 - 50; 
            int textY = buttonY + (buttonHeight - menuFont.baseSize * 2) / 2;
            
            Color textColorCurrent = saveSlotExists[i] ? textColor : GRAY;
            DrawTextEx(menuFont, slotText, {(float)textX, (float)textY}, menuFont.baseSize * 2, 1, textColorCurrent);
            
            if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && saveSlotExists[i]) {
                int slot = i + 1;
                std::cout << "Loading game from slot " << slot << std::endl;
                GameState loaded;
                SaveManager sm;
                if (sm.loadGame(loaded, slot)) {
                    std::string p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHero;
                    int p1Garlic, p2Garlic;
                    loaded.restorePlayerInfo(p1Name, p2Name, startPlayer, otherPlayer, startHero, otherHero, p1Garlic, p2Garlic);

                    std::vector<PlayerInfo> gamePlayers;
                    gamePlayers.push_back(PlayerInfo(p1Name, startPlayer == p1Name ? startHero : otherHero, std::to_string(p1Garlic)));
                    gamePlayers.push_back(PlayerInfo(p2Name, startPlayer == p2Name ? startHero : otherHero, std::to_string(p2Garlic)));

                    gameScreen = std::make_unique<GameScreen>(gamePlayers, startPlayer, screenWidth, screenHeight);
                    gameScreen->run();
                    gameScreen->restoreFromGameState(loaded);
                    currentState = GAME_SCREEN;
                } else {
                    std::cout << "Failed to load game from slot " << slot << std::endl;
                }
                return; 
            }
        }
        
        const char* backText = "BACK";
        int backX = screenWidth * 0.05;  
        int backY = screenHeight * 0.9;  
        bool backHovered = CheckCollisionPointRec(GetMousePosition(), 
            Rectangle{(float)backX - 10, (float)backY - 5, (float)(MeasureTextEx(menuFont, backText, menuFont.baseSize, 1).x + 20), (float)(menuFont.baseSize * 2 + 10)});
        
        if (backHovered) {
            DrawTextEx(menuFont, backText, {(float)(backX - 10), (float)(backY - 5)}, menuFont.baseSize * 2, 1, buttonHoverColor);
        } else {
            DrawTextEx(menuFont, backText, {(float)backX, (float)backY}, menuFont.baseSize * 2, 1, textColor);
        }
        
        if (backHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentState = MAIN_MENU;
        }
    }
    
    void drawHeroSelection() {
        const char* title = "HERO SELECTION";
        float titleFontSize = menuFont.baseSize * 2.0f;
        Vector2 titleSize = MeasureTextEx(menuFont, title, titleFontSize, 2);
        int titleX = (screenWidth - titleSize.x) / 2;
        DrawTextEx(menuFont, title, {(float)titleX, (float)(screenHeight * 0.25)}, titleFontSize, 2, textColor);
        
        std::string currentPlayer = (currentState == HERO_SELECTION_1) ? startingPlayerName : otherPlayerName;
        std::string playerInfo = currentPlayer + ", choose your hero:";
        float playerFontSize = menuFont.baseSize * 1.2f;
        Vector2 playerInfoSize = MeasureTextEx(menuFont, playerInfo.c_str(), playerFontSize, 1);
        int playerInfoX = (screenWidth - playerInfoSize.x) / 2;
        DrawTextEx(menuFont, playerInfo.c_str(), {(float)playerInfoX, (float)(screenHeight * 0.3)}, playerFontSize, 1, textColor);
        
        std::vector<std::string> heroOptions = {"Mayor", "Archeologist", "Courier", "Scientist"};
        
        if (currentState == HERO_SELECTION_2) {
            heroOptions.erase(std::remove(heroOptions.begin(), heroOptions.end(), firstPlayerHero), heroOptions.end());
        }
        
        int buttonSpacing = screenWidth * 0.02;  
        int totalWidth = 2 * buttonWidth + buttonSpacing;
        int startX = (screenWidth - totalWidth) / 2;
        int startY = screenHeight * 0.35;
        
        for (int i = 0; i < heroOptions.size(); i++) {
            int row = i / 2;
            int col = i % 2;
            
            int buttonX = startX + col * (buttonWidth + buttonSpacing);
            int buttonY = startY + row * (buttonHeight + buttonSpacing);
            
            bool buttonHovered = CheckCollisionPointRec(GetMousePosition(), 
                Rectangle{(float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight});
            
            Color buttonColor = buttonHovered ? this->buttonHoverColor : this->buttonColor;
            DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, buttonColor);
            DrawRectangleLines(buttonX, buttonY, buttonWidth, buttonHeight, WHITE);
            
            const char* heroText = heroOptions[i].c_str();
            float heroFontSize = menuFont.baseSize * 1.2f;
            Vector2 heroTextSize = MeasureTextEx(menuFont, heroText, heroFontSize, 1);
            int textX = buttonX + (buttonWidth - heroTextSize.x) / 2;
            int textY = buttonY + (buttonHeight - heroTextSize.y) / 2;
            DrawTextEx(menuFont, heroText, {(float)textX, (float)textY}, heroFontSize, 1, textColor);
            
            if (buttonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (currentState == HERO_SELECTION_1) {
                    firstPlayerHero = heroOptions[i];
                    currentState = HERO_SELECTION_2;
                } else if (currentState == HERO_SELECTION_2) {
                    secondPlayerHero = heroOptions[i];
                    heroSelectionComplete = true;
                    startGame();
                }
            }
        }
        
        const char* backText = "BACK";
        float backFontSize = menuFont.baseSize * 1.3f;
        Vector2 backTextSize = MeasureTextEx(menuFont, backText, backFontSize, 1);
        int backX = screenWidth * 0.05;  
        int backY = screenHeight * 0.9;  
        bool backHovered = CheckCollisionPointRec(GetMousePosition(), 
            Rectangle{(float)backX, (float)backY, (float)(backTextSize.x + 20), (float)(backTextSize.y + 10)});
        
        if (backHovered) {
            DrawTextEx(menuFont, backText, {(float)(backX - 10), (float)(backY - 5)}, backFontSize, 1, buttonHoverColor);
        } else {
            DrawTextEx(menuFont, backText, {(float)backX, (float)backY}, backFontSize, 1, textColor);
        }
        
        if (backHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (currentState == HERO_SELECTION_2) {
                currentState = HERO_SELECTION_1;
            } else {
                currentState = PLAYER_SETUP_2;
                currentPlayerIndex = 1;
                restorePlayerData();
            }
        }
    }
    
    void startGame() {
        std::vector<PlayerInfo> gamePlayers;
        for (const auto& player : players) {
            PlayerInfo gamePlayer;
            gamePlayer.name = player.name;
            gamePlayer.garlicTime = std::stoi(player.lastGarlicTime);
            
            if (player.name == startingPlayerName) {
                gamePlayer.hero = firstPlayerHero;
            } else {
                gamePlayer.hero = secondPlayerHero;
            }
            
            gamePlayers.push_back(gamePlayer);
        }
        
        gameScreen = std::make_unique<GameScreen>(gamePlayers, startingPlayerName, screenWidth, screenHeight);
        gameScreen->run();
        
        currentState = GAME_SCREEN;
    }
    
    void run() {
        while (!WindowShouldClose()) {
            BeginDrawing();
            
            switch (currentState) {
                case MAIN_MENU:
                    drawBackground();
                    drawTitle();
                    drawMainMenu();
                    break;
                case LOAD_GAME:
                    drawBackground();
                    drawTitle();
                    drawLoadGameMenu();
                    break;
                case PLAYER_SETUP_1:
                case PLAYER_SETUP_2:
                    drawBackground();
                    drawTitle();
                    drawPlayerSetup();
                    break;
                case HERO_SELECTION_1:
                case HERO_SELECTION_2:
                    drawBackground();
                    drawTitle();
                    drawHeroSelection();
                    break;
                case GAME_SCREEN:
                    if (gameScreen) {
                        gameScreen->handleInput();
                        gameScreen->updateGame();
                        gameScreen->draw();
                        
                        if (gameScreen->shouldReturnToMenu()) {
                            gameScreen.reset();
                            currentState = MAIN_MENU;
                        }
                    }
                    break;
                case EXIT:
                    CloseWindow();
                    return;
            }
            
            EndDrawing();
        }
    }
    
    bool shouldStartGame() const {
        return currentState == MAIN_MENU; 
    }
    
    bool shouldLoadGame() const {
        return currentState == LOAD_GAME; 
    }
    
    int getSelectedSaveSlot() const {
        return 1; 
    }
};

int main() {
    GraphicalMainMenu menu;
    menu.run();

    return 0;
}
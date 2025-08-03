#include "TUI.hpp"
#include "item.hpp"
#include "map.hpp"
#include "location.hpp"
#include "hero.hpp"
#include "monster.hpp"
#include "taskboard.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

TUI::TUI() {}

void TUI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void TUI::showWelcomeScreen() {
    cout << "============= Welcome to HORRIFIED =============\n";
    cout << "Prepare to defeat Dracula and the Invisible Man!\n";
    cout << "================================================\n";
}

void TUI::showMessage(const string& message) {
    cout << "\n[!] " << message << "\n";
}

void TUI::showHelpMenu() {
    int choice;

    cout << "\n=== ACTION HELP MENU ===" << endl;
    cout << "Type the number of the action to see its description, or 8 to return.\n";
    cout << "1. Move\n";
    cout << "2. Guide\n";
    cout << "3. Pick Up\n";
    cout << "4. Advance\n";
    cout << "5. Defeat\n";
    cout << "6. Special Action\n";
    cout << "7. Use Perk\n";
    cout << "8. Return\n";
    while (true) {
        cout << "Type your choice: ";
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "\n[Move] - Move to an neighbor location. If villagers are present in your current location, you can choose to move them with you.\n";
                break;
            case 2:
                cout << "\n[Guide] - Guide a villager either from your current location to a neighboring one, or from a neighboring location into your current one.\n";
                break;
            case 3:
                cout << "\n[Pick Up] - Pick up any number of items from your current location. These items can be used later to complete monster tasks.\n";
                break;
            case 4:
                cout << "\n[Advance] - Use a red item to destroy Dracula's coffins or deliver clue items to the Precinct to weaken the Invisible Man.\n";
                break;
            case 5:
                cout << "[Defeat] - After completing all tasks related to a monster and if the monster is at your location, use specific colored items to defeat it.\n";
                cout << "           Use yellow items to defeat Dracula, and red items to defeat the Invisible Man.\n";
                break;
            case 6:
                cout << "[Special Action] - The Archeologist can pick up an item from a neighboring location without moving.\n";
                cout << "                 - The Mayor has no special ability.\n";
                break;
            case 7:
                cout << "\n[Use Perk] - Use a perk card you've collected. These don't consume actions and can have helpful effects.\n";
                break;
            case 8:
                return;
            default:
                cout << "Invalid input. Please type a number between 1 and 8.\n";
                break;
        }   
    }
}

void TUI::showTerrorLevelAndTurn(int terror, int maxTerror, int turn) {
    cout << "|" << std::string(60, '=') << " HORRIFIED " << std::string(67, '=') << "|" << std::endl;;
    cout << "| Terror Level: [" << terror << "/" << maxTerror << "]" << std::string(44, ' ') << "| Turn: " << turn << std::string(65, ' ') << "|" << std::endl;
    std::cout << "|" << std::string(64, ' ') << "|" << std::string(73, ' ') << "|" << std::endl;
}void TUI::showMapWithHeroInfo(const Hero* hero1, const Hero* hero2) {
    std::vector<std::string> mapLines = {
        "|============================= MAP ===============================",
        "| Cave --- Camp --- Precinct --- Inn        Barn       Dungeon   |",
        "|           |          |          |           |           |      |",
        "|           ------------------------------- Theater --- Tower    |",
        "|                             |               |           |      |",
        "| Abbey ------------------ Mansion -----------           Docks   |",
        "|  |                          |               |                  |",
        "|  |     Museum ---------------------------- Shop --- Laboratory |",
        "| Crypt                        |                          |      |",
        "|                           Church                     Institute |",
        "|                            |  |                                |",
        "|                 Hospital ---  --- Graveyard                    |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |"
    };   

    std::vector<std::string> hero1Lines;
    hero1Lines.push_back("============================ HERO INFO ");
    if (hero1) {
        hero1Lines.push_back(" Current hero:");
        hero1Lines.push_back(" Name: " + hero1->getHeroName());
        hero1Lines.push_back(" Player: " + hero1->getPlayerName());
        hero1Lines.push_back(" Actions: " + std::to_string(hero1->getRemainingActions()) + "/" + std::to_string(hero1->getMaxActions()));
        hero1Lines.push_back(" Location: " + hero1->getCurrentLocation()->getName());
        const auto& perkCards = hero1->getPerkCards();
        if (perkCards.empty()) {
            hero1Lines.push_back(" Perks: -");
        } else {
            hero1Lines.push_back(" Perks:");
            for (const auto& perk : perkCards) {
                hero1Lines.push_back("  " + PerkCard::perkTypeToString(perk.getType()));
            }
        }
        const auto& itemsVec = hero1->getItems();
        if (itemsVec.empty()) {
            hero1Lines.push_back(" Items: -");
        } else {
            hero1Lines.push_back(" Items:");
            for (const auto& item : itemsVec) {
                hero1Lines.push_back("  " + item.getItemName() + "(" + std::to_string(item.getPower()) + ")");
            }
        }
    }
std::vector<std::string> hero2Lines;
    hero2Lines.push_back("==================================|");
    if (hero2) {
        hero2Lines.push_back(" Name: " + hero2->getHeroName());
        hero2Lines.push_back(" Player: " + hero2->getPlayerName());
        hero2Lines.push_back(" Actions: " + std::to_string(hero2->getRemainingActions()) + "/" + std::to_string(hero2->getMaxActions()));
        hero2Lines.push_back(" Location: " + hero2->getCurrentLocation()->getName());
        const auto& perkCards = hero2->getPerkCards();
        if (perkCards.empty()) {
            hero2Lines.push_back("Perks: -");
        } else {
            hero2Lines.push_back("Perks:");
            for (const auto& perk : perkCards) {
                hero2Lines.push_back("  " + PerkCard::perkTypeToString(perk.getType()));
            }
        }
        const auto& itemsVec = hero2->getItems();
        if (itemsVec.empty()) {
            hero2Lines.push_back(" Items: -");
        } else {
            hero2Lines.push_back(" Items:");
            for (const auto& item : itemsVec) {
                hero2Lines.push_back("  " + item.getItemName() + "(" + std::to_string(item.getPower()) + ", " + Item::colorToString(item.getColor()) + ")");
            }
        }
    }

    size_t maxLines = std::max({mapLines.size(), hero1Lines.size(), hero2Lines.size()});
    if (mapLines.size() >= hero1Lines.size() && mapLines.size() >= hero2Lines.size()) maxLines = 12;
    for (size_t i = 0; i < maxLines; ++i) {
        if (i < mapLines.size()) {
            std::cout << mapLines[i];
            if (mapLines[i].size() < 63) std::cout << std::string(63 - mapLines[i].size(), ' ');
        } else {
            std::cout << std::string(63, ' ');
        }

        if (i < hero1Lines.size()) {
            std::cout << std::left << std::setw(35) << hero1Lines[i];
        } else {
            std::cout << std::string(35, ' ');
        }

        if (i < hero2Lines.size()) {
            if (i == 0) {
                std::cout << hero2Lines[i] << std::endl;
                continue;
            }
            std::cout << "   " << hero2Lines[i];
            if (hero2Lines[i].size() < 35) std::cout << std::string(35 - hero2Lines[i].size(), ' ') << "|";
        } else {
            std::cout << std::string(38, ' ') << "|";
        }
        std::cout << std::endl;
    }
    std::cout << "|" << std::string(64, ' ') << "|" << std::string(73, ' ') << "|" << std::endl;
}

void TUI::showActionMenuWithVillagers(const Map& map) {
    std::vector<std::string> actionMenu = {
        "|========================= ACTION MENU ===========================",
        "|      [M]ove     |  [G]uide   |  [P]ick Up                      |",
        "|      [A]dvance  |  [D]efeat  |  [S]pecial Action               |",
        "|      [U]se Perk |  [H]elp    |  [E]nd Turn                     |",
        "|      [Q]uit                                                    |",
        "|                                                                |",
        "|                                                                |",
        "|                                                                |"
    };

    std::vector<std::string> villagerLines;
    villagerLines.push_back("============================== VILLAGERS ================================|");
    for (const auto& loc : map.locations) {
        for (const auto& character : loc.second->getCharacters()) {
            if (character != "Dracula" && character != "Invisible man" && character != "Archeologist" && character != "Mayor") {
                villagerLines.push_back(character + ": " + loc.second->getName());
            }
        }
    }
    if (villagerLines.size() == 1) villagerLines.push_back("No villagers on the board.");

    size_t maxLines = std::max(actionMenu.size(), villagerLines.size());
    if (actionMenu.size() >= villagerLines.size()) maxLines = 5;
    for (size_t i = 0; i < maxLines; ++i) {
        if (i < actionMenu.size()) {
            std::cout << actionMenu[i];
            if (actionMenu[i].size() < 63)std::cout << std::string(63 - actionMenu[i].size(), ' ');
        } else {
            std::cout << std::string(63, ' ');
        }
        if (i < villagerLines.size()) {
            if (i == 0) {
                std::cout << villagerLines[i] << std::endl;
                continue;
            }
            std::cout << " " << villagerLines[i];
            if (villagerLines[i].size() < 72) std::cout << std::string(72 - villagerLines[i].size(), ' ') << "|";
        } else {
            std::cout << std::string(73, ' ') << "|";
        }
        std::cout << std::endl;
    }
    std::cout << "|" << std::string(64, ' ') << "|" << std::string(73, ' ') << "|" << std::endl;
}
void TUI::showItemsOnBoard(const ItemBag& itembag, const Map& map) {
    std::cout << "|" << std::string(62, '=') << " Items " << std::string(69, '=') << "|" << std::endl;
    for (const auto& pair : map.locations) {
        auto loc = pair.second;
        const auto& items = loc->getItems();
        if (!items.empty()) {
            std::string prefix = "| " + std::string(12, ' ');
            std::ostringstream line;
            line << "| " << std::setw(12) << std::left << loc->getName() << ": ";
            size_t currentLen = line.str().size();
            bool first = true;
            for (size_t i = 0; i < items.size(); ++i) {
                std::string itemStr = items[i].getItemName() + "(" + std::to_string(items[i].getPower()) + ", " + Item::colorToString(items[i].getColor()) + ")";
                if (!first) itemStr = ", " + itemStr;
                if (currentLen + itemStr.size() > 139) {
                    std::cout << line.str() << std::string(139 - currentLen, ' ') << "|" << std::endl;
                    line.str("");
                    line.clear();
                    line << "| " << std::setw(12) << "" << ": ";
                    currentLen = line.str().size();
                    first = true;
                }
                line << itemStr;
                currentLen += itemStr.size();
                first = false;
            }
            std::cout << line.str() << std::string(139 - currentLen, ' ') << "|" << std::endl;
        }
    }
    std::cout << "|" << std::string(138, ' ') << "|" << std::endl;
}

void TUI::showMonsterStatus(const std::vector<Monster*>& monsters, const Map& map, const TaskBoard& taskBoard) {
    std::vector<std::string> draculaLines, invisibleManLines;
    for (const auto& monster : monsters) {
        if (!monster) continue;
        std::string name = monster->getMonsterName();
        if (name == "Dracula" && !taskBoard.isDraculaDefeated()) {
            auto loc = monster->getCurrentLocation();
            draculaLines.push_back(name + " Location: " + (loc ? loc->getName() : "-"));
            std::string coffinStatus = taskBoard.getDraculaTaskStatus();
            size_t start = 0, end = 0;
            while ((end = coffinStatus.find('\n', start)) != std::string::npos) {
                draculaLines.push_back(coffinStatus.substr(start, end - start));
                start = end + 1;
            }
            if (start < coffinStatus.size())
                draculaLines.push_back(coffinStatus.substr(start));
        } else if (name == "Invisible man" && !taskBoard.isInvisibleManDefeated()) {
            auto loc = monster->getCurrentLocation();
            invisibleManLines.push_back(name + " Location: " + (loc ? loc->getName() : "-"));
            std::string clueStatus = taskBoard.getInvisibleManClueStatus();
            size_t start = 0, end = 0;
            while ((end = clueStatus.find('\n', start)) != std::string::npos) {
                invisibleManLines.push_back(clueStatus.substr(start, end - start));
                start = end + 1;
            }
            if (start < clueStatus.size())
                invisibleManLines.push_back(clueStatus.substr(start));
        }
    }
    size_t maxLines = std::max(draculaLines.size(), invisibleManLines.size());
    std::cout << "|============================ DRACULA ====================================================== INVISIBLE MAN ================================|" << std::endl;
    for (size_t i = 0; i < maxLines; ++i) {
        std::string left = (i < draculaLines.size()) ? draculaLines[i] : "";
        std::string right = (i < invisibleManLines.size()) ? invisibleManLines[i] : "";
        std::cout << "| " << std::left << std::setw(63) << left << "| " << std::left << std::setw(72) << right << "|" << std::endl;
    }
    std::cout << "|" << std::string(64, ' ') << "|" << std::string(73, ' ') << "|" << std::endl;
    std::cout << "|" << std::string(138, '=') << "|" << std::endl;
}void TUI::showDiceRoll(const std::vector<std::string>& diceResults) {
    if (diceResults.empty()) {
        std::cout << "|" << std::string(58, '=') << " DICE ROLLED " << std::string(67, '=') << "|" << std::endl;
        std::cout << "| No dice were rolled this turn." << std::string(107, ' ') << "|" << std::endl;
        std::cout << "|" << std::string(138, ' ') << "|" << std::endl;
        return;
    }
    
    std::cout << "|" << std::string(58, '=') << " DICE ROLLED " << std::string(67, '=') << "|" << std::endl;
    std::cout << "| Dice Results: ";
    for (size_t i = 0; i < diceResults.size(); ++i) {
        std::cout << "[" << diceResults[i] << "]";
        if (i < diceResults.size() - 1) std::cout << " ";
    }
    std::cout << std::string(123 - (diceResults.size() * 3 + diceResults.size() - 1), ' ') << "|" << std::endl;
    std::cout << "| [!] = Power, [*] = Strike, [ ] = Empty" << std::string(99, ' ') << "|" << std::endl;
    std::cout << "|" << std::string(138, '=') << "|" << std::endl;
}

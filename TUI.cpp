#include "TUI.hpp"
#include "GameManager.hpp"
#include "Location.hpp"
#include "Hero.hpp"
#include "Item.hpp"
#include "Villager.hpp"
#include "Monster.hpp"
#include "MonsterCard.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

TUI::TUI() {}

void TUI::showWelcomeScreen() {
    cout << "\n🎮 Welcome to HORRIFIED (Terminal Edition) 🎮\n";
    cout << "Prepare to defeat Dracula and the Invisible Man!\n";
    cout << "----------------------------------------------\n";
}

void TUI::showActionMenu() {
    cout << "\n------------ ACTION MENU ------------\n";
    cout << "[M]ove | [G]uide | [P]ick Up | [A]dvance\n";
    cout << "[D]efeat | [U]se Perk | [H]elp | [Q]uit\n";
    cout << "-------------------------------------\n";
}

void TUI::showMessage(const string& message) {
    cout << "\n[!] " << message << "\n";
}

string TUI::askForCommand() {
    cout << "\nEnter your command: ";
    string cmd;
    getline(cin, cmd);
    return cmd;
}

int TUI::askForNumber(const string& prompt, int min, int max) {
    int num;
    do {
        cout << prompt << " (" << min << "-" << max << "): ";
        cin >> num;
    } while (num < min || num > max);
    cin.ignore();
    return num;
}

string TUI::askForLocationName() {
    cout << "Enter location name: ";
    string name;
    getline(cin, name);
    return name;
}
void TUI::showMap() {
    std::cout << "\n========== MAP ==========\n";
    std::cout << "          Laboratory\n";
    std::cout << "               │\n";
    std::cout << "  Camp ─── Tower ─── Dungeon\n";
    std::cout << "   │         │         │\n";
    std::cout << "Barn ─── Mansion ─── Crypt\n";
    std::cout << "   │         │         │\n";
    std::cout << "  Church ── Graveyard ── Cave\n";
    std::cout << "     │                   │\n";
    std::cout << "  Hospital           Lagoon\n";
    std::cout << "     │                   │\n";
    std::cout << "   Inn ───── Docks ───── Shop\n";
    std::cout << "     │         │         │\n";
    std::cout << "  Station ─── Theater ─── Precinct\n";
    std::cout << "                      │\n";
    std::cout << "                  Institute\n";
    std::cout << "===========================\n";
}


void TUI::showLocationOverview(const vector<Location*>& locations) {
    cout << "\n========= Location Overview =========\n";
    cout << left << setw(12) << "Location" << setw(20) << "Items" << setw(15) << "Monsters" << "Villagers" << "\n";
    cout << string(60, '-') << "\n";

    for (const auto& loc : locations) {
        string itemList, monsterList, villagerList;

        for (const auto& item : loc->getItems()) {
            itemList += item.getName() + "(" + to_string(item.getPower()) + ") ";
        }
        for (const auto& mon : loc->getMonsters()) {
            monsterList += mon->getName() + " ";
        }
        for (const auto& vill : loc->getVillagers()) {
            villagerList += vill->getName() + "(" + vill->getHome() + ") ";
        }

        cout << left << setw(12) << loc->getName()
             << setw(20) << itemList
             << setw(15) << monsterList
             << villagerList << "\n";
    }

    cout << "\nKey Locations:\n";
    cout << "[Castle] Coffins smashed: ■■■□ (3/4)\n";
    cout << "[Lab] Evidence collected: (3/5)\n";
    cout << "====================================\n";
}

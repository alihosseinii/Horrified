#ifndef TUI_HPP
#define TUI_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

class Location;
class Hero;
class Item;
class Villager;
class Monster;
class ItemBag;
class Map;
class TaskBoard;

class TUI {
public:
    TUI();
    void clearScreen();
    void showMessage(const std::string& message);
    void showWelcomeScreen();
    void showHelpMenu();
    void showTerrorLevelAndTurn(int terror, int maxTerror, int turn);
    void showMapWithHeroInfo(const Hero* hero1, const Hero* hero2);
    void showActionMenuWithVillagers(const Map& map);
    void showItemsOnBoard(const ItemBag& itembag, const Map& map);
    void showMonsterStatus(const std::vector<Monster*>& monsters, const Map& map, const TaskBoard& taskBoard);
    void showDiceRoll(const std::vector<std::string>& diceResults);
};

#endif

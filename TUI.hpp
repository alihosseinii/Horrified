#ifndef TUI_HPP
#define TUI_HPP

#include <string>
#include <vector>

class GameManager;
class Location;
class Hero;
class Item;
class Villager;
class Monster;
class MonsterCard;

class TUI {
public:
    TUI();

    void showWelcomeScreen();
    void showGameStatus(const GameManager& game);
    void showHeroStatus(const Hero& hero);
    void showMapASCII();
    void showActionMenu();
    void showMonsterCard(const MonsterCard& card);
    void showVillagerList(const std::vector<Villager*>& villagers);
    void showLocationOverview(const std::vector<Location*>& locations);

    void showMessage(const std::string& message);
    std::string askForCommand();
    int askForNumber(const std::string& prompt, int min, int max);
    std::string askForLocationName();
};

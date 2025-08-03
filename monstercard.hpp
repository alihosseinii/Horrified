#ifndef MONSTERCARD_HPP
#define MONSTERCARD_HPP

#include <iostream>
#include <vector>

enum class MonsterType {
    Dracula,
    InvisibleMan,
    FrenziedMonster
};

struct Strike {
    MonsterType monster;
    int moveCount;
    int diceCount;
};

class MonsterCard {
private:
    std::string name;
    int itemCount;
    std::string eventText;
    std::vector<Strike> strikeList;

public:
    MonsterCard(const std::string& name, int itemCount, const std::string& eventText, const std::vector<Strike>& strikeList);

    std::string getName() const;
    int getItemCount() const;
    std::string getEventText() const;
    std::vector<Strike> getStrikeList() const;
};

#endif

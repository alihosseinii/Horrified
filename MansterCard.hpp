#include <iostream>

using namespace std;

enum class MonsterType {
    Dracula,
    InvisibleMan,
    FrenziedMonster
};

struct strike
{
    MonsterType monster;
    int movecount;
    int dicecount;
};

class MonsterCard {
private:
    int itemCount;
    string eventText;
    vector<strike> strikeList;

public:
    MonsterCard(int itemCount,
                const string& eventText,
                const vector<strike>& strikeList);

    int getItemCount() const;
    string getEventText() const;
    vector<strike> getStrikeList() const;
};

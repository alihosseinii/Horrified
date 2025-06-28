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
    string name;
    int itemCount;
    string eventText;
    vector<strike> strikeList;

public:
    MonsterCard(const string& name,
                int itemCount,
                const string& eventText,
                const vector<strike>& strikeList);

    string getname() const;
    int getItemCount() const;
    string getEventText() const;
    vector<strike> getStrikeList() const;
};

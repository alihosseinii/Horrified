#include "MonsterCard.hpp"
MonsterCard::MonsterCard(const string& name, int itemCount, const string& eventText, const vector<strike>& strikeList) :name(name), itemCount(itemCount), eventText(eventText), strikeList(strikeList) {}

string MonsterCard::getname() const { return name; }

int MonsterCard::getItemCount() const { return itemCount; }

string MonsterCard::getEventText() const { return eventText; }

vector<strike> MonsterCard::getStrikeList() const { return strikeList; }

#include "monstercard.hpp"

using namespace std;

MonsterCard::MonsterCard(const std::string& name, int itemCount, const string& eventText, const vector<Strike>& strikeList) : name(name), itemCount(itemCount), eventText(eventText), strikeList(strikeList) {}

string MonsterCard::getName() const { return name; }

int MonsterCard::getItemCount() const { return itemCount; }

string MonsterCard::getEventText() const { return eventText; }

vector<Strike> MonsterCard::getStrikeList() const { return strikeList; }

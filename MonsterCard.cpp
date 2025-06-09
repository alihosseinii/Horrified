#include "MonsterCard.hpp"

MonsterCard::MonsterCard(int itemCount, const string& eventText, const vector<strike>& strikeList) : itemCount(itemCount), eventText(eventText), strikeList(strikeList) {}

int MonsterCard::getItemCount() const { return itemCount; }

string MonsterCard::getEventText() const { return eventText; }

vector<strike> MonsterCard::getStrikeList() const { return strikeList; }

#ifndef TASKBOARD_H
#define TASKBOARD_H

#include <string>
#include <unordered_map>

using namespace std;

enum class MonsterType {
    Dracula,
    InvisibleMan
};

class TaskBoard {
private:
    unordered_map<string, bool> draculaCoffins;  
    unordered_map<string, bool> invisibleManEvidence; 

public:
    TaskBoard();

    void destroyCoffinAt(const string& location);
    bool isCoffinDestroyed(const string& location) const;
    bool allCoffinsDestroyed() const;

    void placeEvidenceFrom(const string& location);
    bool isEvidencePlaced(const string& location) const;
    bool allEvidencePlaced() const;

    string getDraculaTaskStatus() const;
    string getInvisibleManTaskStatus() const;
};

#endif

#ifndef TASKBOARD_HPP
#define TASKBOARD_HPP

#include <string>
#include <unordered_map>

struct TaskStatus {
    int currentStrength = 0;
    bool completed = false;
};

class TaskBoard {
private:
    std::unordered_map<std::string, TaskStatus> draculaCoffins;  
    std::unordered_map<std::string, bool> invisibleManCluesDelivered;
    TaskStatus draculaDefeat;
    TaskStatus invisibleManDefeat;
    bool invisibleManDefeated = false;

public:
    TaskBoard();

    void addStrengthToCoffin(const std::string& location, int strength);
    bool isCoffinLocation(const std::string& location) const;
    bool isCoffinDestroyed(const std::string& location) const;
    bool allCoffinsDestroyed() const;

    void addStrengthToDracula(int strength);
    int getDraculaDefeatStrength() const;
    bool isDraculaDefeated() const;

    bool isClueDelivered(const std::string& location) const;
    void deliverClue(const std::string& location);
    bool allCluesDelivered() const;
    void addStrengthToInvisibleMan(int strength);
    int getInvisibleManDefeatStrength() const;
    void defeatInvisibleMan();
    bool isInvisibleManDefeated() const;

    std::string getDraculaTaskStatus() const;
    std::string getInvisibleManClueStatus() const;
};

#endif

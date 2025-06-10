#ifndef TERRORTRACKER_H
#define TERRORTRACKER_H

#include <string>

class TerrorTracker {
private:
    int level;

public:
    TerrorTracker();

    void reset();
    void increase();  
    void increase(int n);
    int getLevel() const;

    bool isMax() const;
    string getStatus() const;
};

#endif 

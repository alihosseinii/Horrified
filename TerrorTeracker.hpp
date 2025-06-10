#ifndef TERRORTRACKER_HPP
#define TERRORTRACKER_HPP

#include <iostream>
#include <string>

using namespace std;

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

#pragma once

enum class Level : char 
{
    LEVEL_COMMON,
    LEVEL_BOSS
};

class AICounter
{
public:
    
    static AICounter* getInstance();
    

    int getValue() const { return value;}
    void setValue(int x) {value = x;}
    void increaceValue();
    void decreaseValue();
    void setZero();
    void nextLevel();
    Level getLevel() const {return mLevel;}
    void setLevel(Level level) {mLevel = level;}
private:
    AICounter() : value(0) {}
    AICounter(int initialValue) : value(initialValue) {}
    ~AICounter() {}

    AICounter(const AICounter&) = delete;
    AICounter& operator=(const AICounter&) = delete;
    
    static AICounter* sInstance;
    int value;
    Level mLevel = Level::LEVEL_COMMON;
};

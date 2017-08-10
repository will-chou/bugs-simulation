#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Compiler.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject {
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* sw);
    virtual ~Actor();
    virtual void doSomething() = 0;
    virtual bool isAlive();
    virtual bool blocksInsect();
    virtual bool isInsect();
    virtual bool isPoison();
    virtual bool isPheromone();
    virtual bool isFood();
    
    virtual bool alreadyMoved();
    virtual void setAlreadyMoved(bool a);
    
    StudentWorld* sw();
private:
    StudentWorld* m_sw;
    bool moved;
};

class EnergyHolder: public Actor {
public:
    EnergyHolder(int imageID, int startX, int startY, Direction startDirection, int energy, int depth, StudentWorld* sw);
    int getHitPoints();
    void setHitPoints(int hp);
    virtual bool isAlive();
private:
    int hitPoints;
};

class Pebble: public Actor {
public:
    Pebble(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool blocksInsect();
};

class Insect: public EnergyHolder {
public:
    Insect(int imageID, int startX, int startY, Direction startDirection, int energy, int depth, StudentWorld* sw);
    virtual void doSomething() = 0;
    
    virtual bool canBeStunned();
    virtual void setCanBeStunned(bool s);
    int getSleepTicks();
    void setSleepTicks(int st);
    
    void dropFood(int amt);
    
    virtual void attemptEat(int amt);
    
    virtual bool bite(int damage);
    
    virtual bool isInsect();
    virtual bool isAnt();
    
    bool isBitten();
    void setBitten(bool b);
    
private:
    int sleepTicks;
    bool canGetStunned;
    bool bitten;
};


class BabyGrasshopper: public Insect {
public:
    BabyGrasshopper(int imageID, int x, int y, int energy, StudentWorld* sw);
    virtual void doSomething();
    void move();
    virtual void attemptEat(int amt);
    void setDistance(int d);
    int getDistance();
    virtual bool bite(int damage);
private:
    int distance;
};

class AdultGrasshopper: public BabyGrasshopper {
public:
    AdultGrasshopper(int imageID, int x, int y, StudentWorld* sw);
    virtual void doSomething();
    bool jump();
    virtual bool canBeStunned();
    virtual bool bite(int damage);
};

class Ant: public Insect {
public:
    Ant(int imageID, int x, int y, StudentWorld* sw, int colonyNumber, Compiler* c);
    virtual void doSomething();
    int getColonyNumber();
    virtual bool isAnt();
private:
    int lastRandomValue;
    int instructionValue;
    Compiler* m_c;
    
    bool interpretCommand(const Compiler::Command& cmd);
    
    bool standingWithEnemy(int x, int y);
    bool dangerInFront();
    void moveForward();
    void pickupFood();
    void emitPheromone();
    bool pheromoneInFront();
    
    bool isBlocked();
    void setBlocked(bool b);
    
    int foodCarried();
    void setFoodCarried(int f);
    
    bool blocked;
    int food;
    
    int m_colonyNumber;
};

class Pheromone: public EnergyHolder  {
public:
    Pheromone(int imageID, int x, int y, StudentWorld* sw, int colonyNum);
    virtual void doSomething();
    int getColonyNum();
    virtual bool isPheromone();
private:
    int m_colonyNum;
};

class Anthill: public EnergyHolder {
public:
    Anthill(int x, int y, StudentWorld* sw, int colonyNumber, Compiler* c);
    virtual void doSomething();
    int getColonyNumber();
private:
    int m_colonyNumber;
    void giveBirth();
    void eat(int amt);
    Compiler* m_c;
};

class Food: public EnergyHolder {
public:
    Food(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool isFood();
};

class Pool: public Actor {
public:
    Pool(int x, int y, StudentWorld* sw);
    virtual void doSomething();
};

class Poison: public Actor {
public:
    Poison(int x, int y, StudentWorld* sw);
    virtual void doSomething();
    virtual bool isPoison();
};

#endif // ACTOR_H_

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <vector>
#include <cmath>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Auxiliary
bool isOutOfBounds(int x, int y);

//Actor implementations
Actor::Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* sw)
    : GraphObject(imageID, startX, startY, startDirection, depth)
{
    m_sw = sw;
    moved = false;
}

Actor::~Actor() {
    
}

bool Actor::isAlive() {
    return true;
}

bool Actor::blocksInsect() {
    return false;
}

bool Actor::isInsect() {
    return false;
}

bool Actor::isPoison() {
    return false;
}

bool Actor::isFood() {
    return false;
}

bool Actor::isPheromone() {
    return false;
}

StudentWorld* Actor::sw() {
    return m_sw;
}

void Actor::setAlreadyMoved(bool a) {
    moved = a;
}

bool Actor::alreadyMoved() {
    return moved;
}


//EnergyHolder implementations
EnergyHolder::EnergyHolder(int imageID, int startX, int startY, Direction startDirection, int energy, int depth, StudentWorld* sw)
: Actor(imageID, startX, startY, startDirection, depth, sw)
{
    hitPoints = energy;
}

int EnergyHolder::getHitPoints() {
    return hitPoints;
}

void EnergyHolder::setHitPoints(int hp) {
    hitPoints = hp;
}

bool EnergyHolder::isAlive() {
    return hitPoints > 0;
}

//Pebble implementations
Pebble::Pebble(int x, int y, StudentWorld* sw)
    : Actor(IID_ROCK, x, y, right, 1, sw)
{
}

void Pebble::doSomething() {
    return;
}

bool Pebble::blocksInsect() {
    return true;
}

//Insect base class implementations
Insect::Insect(int imageID, int startX, int startY, Direction startDirection, int energy, int depth, StudentWorld* sw)
    : EnergyHolder(imageID, startX, startY, startDirection, energy, depth, sw)
{
    canGetStunned = true;
    bitten = false;
}

bool Insect::canBeStunned() {
    return canGetStunned;
}

void Insect::setCanBeStunned(bool s) {
    canGetStunned = s;
}

bool Insect::isInsect() {
    return true;
}

bool Insect::isAnt() {
    return false;
}

int Insect::getSleepTicks() {
    return sleepTicks;
}

void Insect::setSleepTicks(int st) {
    sleepTicks = st;
}

//add food to food on square if there is, otherwise add new food
void Insect::dropFood(int amt) {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    if(sw()->isFoodAt(getX(), getY())) {
        while(it != li.end()) {
            if((*it)->isFood()) {
                EnergyHolder* eh = dynamic_cast<EnergyHolder*> (*it);
                eh->setHitPoints(eh->getHitPoints() + amt);
                return;
            }
            it++;
        }
    }
    
    Food* newFood = new Food(getX(), getY(), sw());
    newFood->setHitPoints(amt);
    li.push_back(newFood);
}

//attempt to eat any food on the square
void Insect::attemptEat(int amt) {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        Food* f = dynamic_cast<Food*> (*it);
        if(f != nullptr) {
            if(f->getHitPoints() < amt) {
                setHitPoints(getHitPoints() + f->getHitPoints());
                f->setHitPoints(0);
            }
            else {
                setHitPoints(getHitPoints() + amt);
                f->setHitPoints(f->getHitPoints() - amt);
            }
            return;
        }
        it++;
    }
}

bool Insect::bite(int damage) {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    int numInsects = 1;
    
    std::vector<Insect*> insectsOnSquare;
    insectsOnSquare.push_back(this);
    
    Ant* an = dynamic_cast<Ant*> (this);
    //if called from an ant
    if(an != nullptr) {
        while(it != li.end()) {
            Insect* ins = dynamic_cast<Insect*> (*it);
            Ant* a = dynamic_cast<Ant*> (*it);
            if(ins != nullptr && ins != this) {
                if(a == nullptr || (a != nullptr && (a->getColonyNumber() != an->getColonyNumber()))) {
                    numInsects++;
                    insectsOnSquare.push_back(ins);
                }
            }
            it++;
        }
    }
    //else
    else {
        while(it != li.end()) {
            Insect* ins = dynamic_cast<Insect*> (*it);
            if(ins != nullptr && ins != this) {
                numInsects++;
                insectsOnSquare.push_back(ins);
            }
            it++;
        }
    }
    
    if(numInsects <= 1) {
        return false;
    }
    
    int r = randInt(0, 2);
    if(r == 1 || r == 2) {
        return false;
    }
    
    r = randInt(1, numInsects - 1);
    
    insectsOnSquare[r]->setHitPoints(insectsOnSquare[r]->getHitPoints() - damage);
    insectsOnSquare[r]->setBitten(true);
    
    if(insectsOnSquare[r]->getHitPoints() <= 0) {
        dropFood(100);
        return true;
    }
    
    //bite back
    AdultGrasshopper* ag = dynamic_cast<AdultGrasshopper*> (insectsOnSquare[r]);
    if(ag != nullptr) {
        int randBite = randInt(0, 1);
        if(randBite == 0) {
            ag->bite(50);
            ag->setSleepTicks(2);
        }
    }
    
    return true;
}

bool Insect::isBitten() {
    return bitten;
}

void Insect::setBitten(bool b) {
    bitten = b;
}

//BabyGrasshopper implementations
BabyGrasshopper::BabyGrasshopper(int imageID, int x, int y, int energy, StudentWorld* sw)
    : Insect(imageID, x, y, Direction(randInt(1, 4)), energy, 1, sw)
{
    distance = randInt(2, 10);
    setSleepTicks(0);
}

int BabyGrasshopper::getDistance() {
    return distance;
}

void BabyGrasshopper::setDistance(int d) {
    distance = d;
}

void BabyGrasshopper::move() {
    if(distance == 0) {
        setDirection(Direction(randInt(1, 4)));
        distance = randInt(2, 10);
    }
    
    if(getDirection() == up) {
        if(getY() + 1 < VIEW_HEIGHT && !(sw()->posBlocksInsect(getX(), getY() + 1))) {
            moveTo(getX(), getY() + 1);
            distance--;
        }
        else {
            distance = 0;
        }
    }
    else if(getDirection() == right) {
        if(getX() + 1 < VIEW_WIDTH && !(sw()->posBlocksInsect(getX() + 1, getY()))) {
            moveTo(getX() + 1, getY());
            distance--;
        }
        else {
            distance = 0;
        }
    }
    else if(getDirection() == down) {
        if(getY() - 1 >= 0 && !(sw()->posBlocksInsect(getX(), getY() - 1))) {
            moveTo(getX(), getY() - 1);
            distance--;
        }
        else {
            distance = 0;
        }
    }
    else if(getDirection() == left) {
        if(getX() - 1 >= 0 && !(sw()->posBlocksInsect(getX() - 1, getY()))) {
            moveTo(getX() - 1, getY());
            distance--;
        }
        else {
            distance = 0;
        }
    }
    
    setSleepTicks(2);
    return;
}

//call insect base classes eat, then randomly sleep
void BabyGrasshopper::attemptEat(int amt) {
    int oldHitPoints = getHitPoints();
    
    Insect::attemptEat(amt);

    if(oldHitPoints != getHitPoints()) {
        int x = randInt(0, 1);
        if(x == 0) {
            setSleepTicks(2);
            return;
        }
    }
    return;
}

//baby grasshopper can't bite
bool BabyGrasshopper::bite(int damage) {
    return false;
}

void BabyGrasshopper::doSomething() {

    setHitPoints(getHitPoints() - 1);
    
    if(!isAlive()) {
        dropFood(100);
        return;
    }
    
    if(getSleepTicks() > 0) {
        setSleepTicks(getSleepTicks() - 1);
        return;
    }
    
    if(getHitPoints() >= 1600) {
        //change to adult grasshopper
        std::list<Actor*>& curList = sw()->getListAt(getX(), getY());
        curList.push_back(new AdultGrasshopper(IID_ADULT_GRASSHOPPER, getX(), getY(), sw()));
        
        //set dead
        setHitPoints(0);
        dropFood(100);
        
        return;
    }
    
    attemptEat(200);
    
    if(getSleepTicks() == 0) {
        int oldX = getX();
        int oldY = getY();
        
        move();
        
        if(oldX != getX() || oldY != getY())
            setCanBeStunned(true);
    }
}

//AdultGrasshopper implementations
AdultGrasshopper::AdultGrasshopper(int imageID, int x, int y, StudentWorld* sw)
    : BabyGrasshopper(imageID, x, y, 1600, sw)
{
    setDistance(randInt(2, 10));
    setSleepTicks(0);
}

//jump within 10 square radius
bool AdultGrasshopper::jump() {
    int r = randInt(0, 9);
    if(r > 0) {
        return false;
    }
    
    int radians = randInt(0, 2 * M_PI);
    int dist = randInt(1, 10);
    
    int randX = getX() + dist * cos(radians);
    int randY = getY() + dist * sin(radians);
    
    if(isOutOfBounds(randX, randY) || sw()->posBlocksInsect(randX, randY)) {
       return false;
    }
       
    moveTo(randX, randY);
    
    setSleepTicks(2);
    return true;
}

bool AdultGrasshopper::bite(int damage) {
    return Insect::bite(damage);
}

bool AdultGrasshopper::canBeStunned() {
    return false;
}

void AdultGrasshopper::doSomething() {
    setHitPoints(getHitPoints() - 1);
    
    if(!isAlive()) {
        dropFood(100);
        return;
    }
    
    if(getSleepTicks() > 0) {
        setSleepTicks(getSleepTicks() - 1);
        return;
    }
    
    if(bite(50)) {
        setSleepTicks(2);
        return;
    }

    if(jump()) {
        return;
    }

    attemptEat(200);
    
    if(getSleepTicks() == 0) {
        move();
    }
}

bool isOutOfBounds(int x, int y) {
    return x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT;
}

//Ant implementations
Ant::Ant(int imageID, int x, int y, StudentWorld* sw, int colonyNumber, Compiler* c)
    : Insect(imageID, x, y, Direction(randInt(1, 4)), 1500, 1, sw)
{
    lastRandomValue = 0;
    instructionValue = 0;
    setSleepTicks(0);
    m_c = c;
    blocked = false;
    food = 0;
    m_colonyNumber = colonyNumber;
}

bool Ant::isAnt() {
    return true;
}

int Ant::getColonyNumber() {
    return m_colonyNumber;
}

bool Ant::isBlocked() {
    return blocked;
}

void Ant::setBlocked(bool b) {
    blocked = b;
}

int Ant::foodCarried() {
    return food;
}

void Ant::setFoodCarried(int f) {
    food = f;
}

//move forward if not blocked, otherwise set blocked to true
void Ant::moveForward() {
    int oldX = getX();
    int oldY = getY();
    
    if(getDirection() == up) {
        if(getY() + 1 < VIEW_HEIGHT && !(sw()->posBlocksInsect(getX(), getY() + 1))) {
            moveTo(getX(), getY() + 1);
        }
        else {
            setBlocked(true);
        }
    }
    else if(getDirection() == right) {
        if(getX() + 1 < VIEW_WIDTH && !(sw()->posBlocksInsect(getX() + 1, getY()))) {
            moveTo(getX() + 1, getY());
        }
        else {
            setBlocked(true);
        }
    }
    else if(getDirection() == down) {
        if(getY() - 1 >= 0 && !(sw()->posBlocksInsect(getX(), getY() - 1))) {
            moveTo(getX(), getY() - 1);
        }
        else {
            setBlocked(true);
        }
    }
    else if(getDirection() == left) {
        if(getX() - 1 >= 0 && !(sw()->posBlocksInsect(getX() - 1, getY()))) {
            moveTo(getX() - 1, getY());
        }
        else {
            setBlocked(true);
        }
    }
    
    if(oldX != getX() || oldY != getY()) {
        setBitten(false);
        setCanBeStunned(true);
        setBlocked(false);
    }
}

//picks up any food on current square
void Ant::pickupFood() {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        Food* f = dynamic_cast<Food*> (*it);
        if(f != nullptr) {
            //max food carried is 1800
            if(f->getHitPoints() < 400) {
                if(f->getHitPoints() + foodCarried() > 1800) {
                    int toPickup = 1800 - foodCarried();
                    setFoodCarried(foodCarried() + toPickup);
                    f->setHitPoints(f->getHitPoints() - toPickup);
                }
                else {
                    setFoodCarried(foodCarried() + f->getHitPoints());
                    f->setHitPoints(0);
                }
            }
            else {
                if(f->getHitPoints() + 400 > 1800) {
                    int toPickup = 1800 - foodCarried();
                    setFoodCarried(foodCarried() + toPickup);
                    f->setHitPoints(f->getHitPoints() - toPickup);
                }
                else {
                    setFoodCarried(foodCarried() + 400);
                    f->setHitPoints(f->getHitPoints() - 400);
                }
            }
        }
        it++;
    }
}

//checks if there is enemy on ants square
bool Ant::standingWithEnemy(int x, int y) {
    std::list<Actor*>& li = sw()->getListAt(x, y);
    std::list<Actor*>::iterator it = li.begin();
    
    std::vector<Insect*> insectsOnSquare;
    insectsOnSquare.push_back(this);
    
    Ant* an = dynamic_cast<Ant*> (this);
    if(an != nullptr) {
        while(it != li.end()) {
            Insect* ins = dynamic_cast<Insect*> (*it);
            Ant* a = dynamic_cast<Ant*> (*it);
            if(ins != nullptr && ins != this) {
                if(a == nullptr || (a != nullptr && (a->getColonyNumber() != an->getColonyNumber()))) {
                    insectsOnSquare.push_back(ins);
                }
            }
            it++;
        }
    }
    return insectsOnSquare.size() > 1;
}

//see if there is danger in front using studentworlds method
bool Ant::dangerInFront() {
    if(getDirection() == up) {
        if(getY() + 1 < VIEW_HEIGHT && (standingWithEnemy(getX(), getY() + 1) || sw()->isPoisonAt(getX(), getY() + 1))) {
            return true;
        }
    }
    else if(getDirection() == right) {
        if(getX() + 1 < VIEW_WIDTH && (standingWithEnemy(getX() + 1, getY()) || sw()->isPoisonAt(getX() + 1, getY()))) {
            return true;
        }
    }
    else if(getDirection() == down) {
        if(getY() - 1 >= 0 && (standingWithEnemy(getX(), getY() - 1) || sw()->isPoisonAt(getX(), getY() - 1))) {
            return true;
        }
    }
    else if(getDirection() == left) {
        if(getX() - 1 >= 0 && (standingWithEnemy(getX() - 1, getY()) || sw()->isPoisonAt(getX() - 1, getY()))) {
            return true;
        }
    }
    return false;
}

//see if theres a pheromone in front using studentworlds method
bool Ant::pheromoneInFront() {
    if(getDirection() == up) {
        if(getY() + 1 < VIEW_HEIGHT && sw()->pheromoneAt(getX(), getY() + 1, getColonyNumber()) != nullptr) {
            return true;
        }
    }
    else if(getDirection() == right) {
        if(getX() + 1 < VIEW_WIDTH && sw()->pheromoneAt(getX() + 1, getY(), getColonyNumber()) != nullptr) {
            return true;
        }
    }
    else if(getDirection() == down) {
        if(getY() - 1 >= 0 && sw()->pheromoneAt(getX(), getY() - 1, getColonyNumber()) != nullptr) {
            return true;
        }
    }
    else if(getDirection() == left) {
        if(getX() - 1 >= 0 && sw()->pheromoneAt(getX() - 1, getY(), getColonyNumber()) != nullptr) {
            return true;
        }
    }
    return false;
}

void Ant::emitPheromone() {
    Pheromone* p = sw()->pheromoneAt(getX(), getY(), getColonyNumber());

    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    if(getColonyNumber() == 0) {
        if(p != nullptr) {
            //max hit points for pheromone is 768
            if(p->getHitPoints() >= 768) {
                return;
            }
            else {
                if(p->getHitPoints() + 256 >= 768) {
                    p->setHitPoints(768);
                }
                else {
                    p->setHitPoints(p->getHitPoints() + 256);
                }
                return;
            }
        }
        li.push_back(new Pheromone(IID_PHEROMONE_TYPE0, getX(), getY(), sw(), 0));
    }
    else if(getColonyNumber() == 1) {
        if(p != nullptr) {
            if(p->getHitPoints() >= 768) {
                return;
            }
            else {
                if(p->getHitPoints() + 256 >= 768) {
                    p->setHitPoints(768);
                }
                else {
                    p->setHitPoints(p->getHitPoints() + 256);
                }
                return;
            }
        }
        li.push_back(new Pheromone(IID_PHEROMONE_TYPE1, getX(), getY(), sw(), 1));
    }
    else if(getColonyNumber() == 2) {
        if(p != nullptr) {
            if(p->getHitPoints() >= 768) {
                return;
            }
            else {
                if(p->getHitPoints() + 256 >= 768) {
                    p->setHitPoints(768);
                }
                else {
                    p->setHitPoints(p->getHitPoints() + 256);
                }
                return;
            }
        }
        li.push_back(new Pheromone(IID_PHEROMONE_TYPE2, getX(), getY(), sw(), 2));
    }
    else if(getColonyNumber() == 3) {
        if(p != nullptr) {
            if(p->getHitPoints() >= 768) {
                return;
            }
            else {
                if(p->getHitPoints() + 256 >= 768) {
                    p->setHitPoints(768);
                }
                else {
                    p->setHitPoints(p->getHitPoints() + 256);
                }
                return;
            }
        }
        li.push_back(new Pheromone(IID_PHEROMONE_TYPE3, getX(), getY(), sw(), 3));
    }
}

bool Ant::interpretCommand(const Compiler::Command &cmd) {
    //return true if not goto or if command
    if(cmd.opcode == Compiler::moveForward) {
        moveForward();
        return true;
    }
    else if(cmd.opcode == Compiler::eatFood) {
        if(foodCarried() >= 100) {
            setHitPoints(getHitPoints() + 100);
            setFoodCarried(foodCarried() - 100);
        }
        else {
            setHitPoints(getHitPoints() + foodCarried());
            setFoodCarried(0);
        }
        return true;
    }
    else if(cmd.opcode == Compiler::dropFood) {
        dropFood(foodCarried());
        setFoodCarried(0);
        return true;
    }
    else if(cmd.opcode == Compiler::bite) {
        bite(15);
        return true;
    }
    else if(cmd.opcode == Compiler::pickupFood) {
        pickupFood();
        return true;
    }
    else if(cmd.opcode == Compiler::emitPheromone) {
        emitPheromone();
        return true;
    }
    else if(cmd.opcode == Compiler::faceRandomDirection) {
        setDirection(Direction(randInt(1, 4)));
        return true;
    }
    else if(cmd.opcode == Compiler::rotateClockwise) {
        if(getDirection() == left) {
            setDirection(Direction(1));
        }
        else {
            setDirection(Direction(getDirection() + 1));
        }
        return true;
    }
    else if(cmd.opcode == Compiler::rotateCounterClockwise) {
        if(getDirection() == up) {
            setDirection(Direction(4));
        }
        else {
            setDirection(Direction(getDirection() - 1));
        }
        return true;
    }
    else if(cmd.opcode == Compiler::generateRandomNumber) {
        if(cmd.operand1 == "0") {
            lastRandomValue = 0;
        }
        else {
            lastRandomValue = randInt(1, stoi(cmd.operand1) - 1);
        }
        return false;
    }
    else if(cmd.opcode == Compiler::goto_command) {
        instructionValue = stoi(cmd.operand1);
        return false;
    }
    else if(cmd.opcode == Compiler::if_command) {
        if(stoi(cmd.operand1) == Compiler::last_random_number_was_zero) {
            if(lastRandomValue == 0) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_am_carrying_food) {
            if(foodCarried() > 0) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_am_hungry) {
            if(getHitPoints() <= 25) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_am_standing_with_an_enemy) {
            if(standingWithEnemy(getX(), getY())) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_am_standing_on_food) {
            if(sw()->isFoodAt(getX(), getY())) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_am_standing_on_my_anthill) {
            if(sw()->isAnthillAt(getX(), getY(), getColonyNumber())) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_smell_pheromone_in_front_of_me) {
            if(pheromoneInFront()) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_smell_danger_in_front_of_me) {
            if(dangerInFront()) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_was_bit) {
            if(isBitten()) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        else if(stoi(cmd.operand1) == Compiler::i_was_blocked_from_moving) {
            if(isBlocked()) {
                instructionValue = stoi(cmd.operand2);
            }
            else {
                instructionValue++;
            }
        }
        return false;
    }
    return false;
}

void Ant::doSomething() {
    setHitPoints(getHitPoints() - 1);
    
    if(!isAlive()) {
        dropFood(100);
        return;
    }
    
    //if is stunned by pool of water
    if(getSleepTicks() > 0) {
        setSleepTicks(getSleepTicks() - 1);
        return;
    }
    
    Compiler::Command cmd;
    
    int numCommands = 0;
    
    //go up to 10 commands
    while(numCommands < 10) {
        //if command is invalid, set dead
        if(!(m_c->getCommand(instructionValue, cmd))) {
            setHitPoints(0);
            return;
        }
        
        //if interpreted command other than goto or if, increment instruction counter and return
        if(interpretCommand(cmd)) {
            instructionValue++;
            return;
        }
        
        if(cmd.opcode != Compiler::goto_command && cmd.opcode != Compiler::if_command) {
            instructionValue++;
        }
        
        if(!isAlive()) {
            dropFood(100);
            return;
        }
        
        numCommands++;
    }
}

//Pheromone implementations
Pheromone::Pheromone(int imageID, int x, int y, StudentWorld* sw, int colonyNum)
    : EnergyHolder(imageID, x, y, right, 256, 2, sw)
{
    m_colonyNum = colonyNum;
}

void Pheromone::doSomething() {
    setHitPoints(getHitPoints() - 1);
}

int Pheromone::getColonyNum() {
    return m_colonyNum;
}

bool Pheromone::isPheromone() {
    return true;
}

//Anthill implementations
Anthill::Anthill(int x, int y, StudentWorld* sw, int colonyNumber, Compiler* c)
    : EnergyHolder(IID_ANT_HILL, x, y, right, 8999, 2, sw)
{
    m_colonyNumber = colonyNumber;
    m_c = c;
}

//get colony number
int Anthill::getColonyNumber() {
    return m_colonyNumber;
}

void Anthill::giveBirth() {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    if(getColonyNumber() == 0) {
        li.push_back(new Ant(IID_ANT_TYPE0, getX(), getY(), sw(), 0, m_c));
    }
    else if(getColonyNumber() == 1) {
        li.push_back(new Ant(IID_ANT_TYPE1, getX(), getY(), sw(), 1, m_c));
    }
    else if(getColonyNumber() == 2) {
        li.push_back(new Ant(IID_ANT_TYPE2, getX(), getY(), sw(), 2, m_c));
    }
    else if(getColonyNumber() == 3) {
        li.push_back(new Ant(IID_ANT_TYPE3, getX(), getY(), sw(), 3, m_c));
    }
}

//attempt to eat whatever food is on that square
void Anthill::eat(int amt) {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        Food* f = dynamic_cast<Food*> (*it);
        if(f != nullptr) {
            if(f->getHitPoints() < amt) {
                setHitPoints(getHitPoints() + f->getHitPoints());
                f->setHitPoints(0);
            }
            else {
                setHitPoints(getHitPoints() + amt);
                f->setHitPoints(f->getHitPoints() - amt);
            }
            return;
        }
        it++;
    }
}

void Anthill::doSomething() {
    setHitPoints(getHitPoints() - 1);
    
    if(getHitPoints() <= 0) {
        return;
    }
    
    //if there is food
    if(sw()->isFoodAt(getX(), getY())) {
        eat(10000);
        return;
    }
    
    if(getHitPoints() >= 2000) {
        //add new ant of same colony
        giveBirth();
        setHitPoints(getHitPoints() - 1500);
        sw()->setAntsInColony(getColonyNumber(), sw()->getAntsInColony(getColonyNumber()) + 1);
    }
}

//Food implementations
Food::Food(int x, int y, StudentWorld* sw)
    : EnergyHolder(IID_FOOD, x, y, right, 6000, 2, sw)
{
}

void Food::doSomething() {
    return;
}

bool Food::isFood() {
    return true;
}

//Pool implementations
Pool::Pool(int x, int y, StudentWorld* sw)
    : Actor(IID_WATER_POOL, x, y, right, 2, sw)
{
}

//stuns any insect on square
void Pool::doSomething() {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        Insect* ins = dynamic_cast<Insect*> (*it);
        if(ins != nullptr) {
            if(ins->canBeStunned()) {
                ins->setSleepTicks(ins->getSleepTicks() + 2);
                ins->setCanBeStunned(false);
            }
        }
        it++;
    }
}

//Poison implementations
Poison::Poison(int x, int y, StudentWorld* sw)
    : Actor(IID_POISON, x, y, right, 2, sw)
{
}

//subtracts 150 hit points from each actor on square
void Poison::doSomething() {
    std::list<Actor*>& li = sw()->getListAt(getX(), getY());
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        Insect* ins = dynamic_cast<Insect*> (*it);
        if(ins != nullptr) {
            if(ins->canBeStunned()) {
                ins->setHitPoints(ins->getHitPoints() - 150);
                if(ins->getHitPoints() <= 0) {
                    ins->dropFood(100);
                }

            }
        }
        it++;
    }
}

bool Poison::isPoison() {
    return true;
}
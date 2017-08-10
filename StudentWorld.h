#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}

    ~StudentWorld() {
        cleanUp();
    }
    
	virtual int init()
	{
        int x = loadGrid();
        if(x == 0 || x == GWSTATUS_LEVEL_ERROR) {
            return GWSTATUS_LEVEL_ERROR;
        };
        ticks = 0;

		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
        ticks++;
        
        //Set all the objects to not moved yet
        for(int i = 0; i < VIEW_WIDTH; i++) {
            for(int j = 0; j < VIEW_HEIGHT; j++) {
                std::list<Actor*>& li = getListAt(i, j);
                std::list<Actor*>::iterator it;
                
                it = li.begin();
                
                while(it != li.end()) {
                    (*it)->setAlreadyMoved(false);
                    it++;
                }
            }
        }
        
        //Move objects and update data structure
        for(int i = 0; i < VIEW_WIDTH; i++) {
            for(int j = 0; j < VIEW_HEIGHT; j++) {
                std::list<Actor*>& li = getListAt(i, j);
                std::list<Actor*>::iterator it;
                
                it = li.begin();
                
                while(it != li.end()) {
                    int oldX = (*it)->getX();
                    int oldY = (*it)->getY();

                    if((*it)->isAlive() && !((*it)->alreadyMoved())) {
                        (*it)->doSomething();
                    }
                    if(oldX != (*it)->getX() || oldY != (*it)->getY()) {
                        
                        (*it)->setAlreadyMoved(true);
                        Actor* a = *it;
                        
                        actorGrid[VIEW_HEIGHT - 1 - (*it)->getY()][(*it)->getX()].push_back(a);
                        
                        it = li.erase(it);
                        
                    }
                    else
                        it++;
                    //}
                }
            }
        }
        
        //remove dead objects
        for(int i = 0; i < VIEW_WIDTH; i++) {
            for(int j = 0; j < VIEW_HEIGHT; j++) {
                std::list<Actor*>& li = getListAt(i, j);
                std::list<Actor*>::iterator it;
                it = li.begin();
                
                while(it != li.end()) {
                    if(!((*it)->isAlive())) {
                        delete (*it);
                        it = li.erase(it);
                    }
                    else
                        it++;
                }
            }
        }
        
        //update display text
        int winningColony = 0;
        int mostAnts = getAntsInColony(0);
        int numWinning = 1;
        for(int i = 1; i < nameOfColony.size(); i++) {
            if(getAntsInColony(i) > mostAnts) {
                winningColony = i;
                mostAnts = getAntsInColony(i);
                numWinning = 1;
            }
            else if(getAntsInColony(i) == mostAnts) {
                numWinning++;
            }
        }
        
        if(numWinning > 1) {
            winningColony = -1;
        }
        
        int numTicks = 2000 - getTicks();
        std::ostringstream oss;
        oss << "Ticks:" << std::setw(5) << numTicks << " -";
        
        for(int i = 0; i < nameOfColony.size(); i++) {
            oss << "  " << getNameOfColony(i);
            if(i == winningColony) {
                oss << "*";
            }
            oss << ": ";
            if(getAntsInColony(i) < 10) {
                oss << "0";
            }
            oss << getAntsInColony(i);
        }
        
        setGameStatText(oss.str());
        
        if(ticks >= 2000) {
            if(winningColony != -1) {
                setWinner(nameOfColony[winningColony]);
                return GWSTATUS_PLAYER_WON;
            }
            return GWSTATUS_NO_WINNER;
        }
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter.
		  // Notice that the return value GWSTATUS_NO_WINNER will cause our framework to end the simulation.
        
        return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
        for(int i = 0; i < VIEW_WIDTH; i++) {
            for(int j = 0; j < VIEW_HEIGHT; j++) {
                std::list<Actor*>& li = getListAt(i, j);
                std::list<Actor*>::iterator it;
                it = li.begin();
                
                while(it != li.end()) {
                    delete (*it);
                    it = li.erase(it);
                }
            }
        }
        
        if(compilerForEntrant0 != nullptr) {
            delete compilerForEntrant0;
            compilerForEntrant0 = nullptr;
        }
        if(compilerForEntrant1 != nullptr) {
            delete compilerForEntrant1;
            compilerForEntrant1 = nullptr;
        }
        if(compilerForEntrant2 != nullptr) {
            delete compilerForEntrant2;
            compilerForEntrant2 = nullptr;
        }
        if(compilerForEntrant3 != nullptr) {
            delete compilerForEntrant3;
            compilerForEntrant3 = nullptr;
        }
	}
    
    std::list<Actor*>& getListAt(int x, int y);
    int getTicks();
    bool posBlocksInsect(int x, int y);
    bool isFoodAt(int x, int y);
    bool isAnthillAt(int x, int y, int colonyNum);
    bool isPoisonAt(int x, int y);
    Pheromone* pheromoneAt(int x, int y, int colonyNum);
    void setAntsInColony(int colonyNum, int num);
    int getAntsInColony(int colonyNum);
    std::string getNameOfColony(int colonyNum);

private:
    std::list<Actor*> actorGrid[VIEW_HEIGHT][VIEW_WIDTH];
    std::vector<int> antsInColony;
    std::vector<std::string> nameOfColony;
    int loadGrid();
    int ticks;
    Compiler *compilerForEntrant0, *compilerForEntrant1, *compilerForEntrant2, *compilerForEntrant3;
};

#endif // STUDENTWORLD_H_

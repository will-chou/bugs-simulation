#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Field.h"
#include "Compiler.h"
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
int StudentWorld::loadGrid() {
    string fieldFileName;
    Field f;
    
    std::string fieldFile = getFieldFilename();
    
    //std::string fieldFile = "/Users/williamchou/Documents/Documents/@William UCLA/Winter 2017/CS32/Project 3/Bugs/field.txt";
    
    if(f.loadField(fieldFile) != Field::LoadResult::load_success) {
        return 0;
    }

//
    Anthill *ah0, *ah1, *ah2, *ah3;
//
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
        
    for(int i = 0; i < fileNames.size(); i++) {
        antsInColony.push_back(0);
    }

    std::string error;
    
    int toCompile = (int)fileNames.size();
    
    if(toCompile > 0) {
        compilerForEntrant0 = new Compiler;
        if(!compilerForEntrant0->compile(fileNames[0], error)) {
            setError(fileNames[0] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        nameOfColony.push_back(compilerForEntrant0->getColonyName());
        
        toCompile--;
    }

    if(toCompile > 0) {
        compilerForEntrant1 = new Compiler;
        if(!compilerForEntrant1->compile(fileNames[1], error)) {
            setError(fileNames[1] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        nameOfColony.push_back(compilerForEntrant1->getColonyName());
        
        toCompile--;
    }
    
    if(toCompile > 0) {
        compilerForEntrant2 = new Compiler;
        if(!compilerForEntrant2->compile(fileNames[2], error)) {
            setError(fileNames[2] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        nameOfColony.push_back(compilerForEntrant2->getColonyName());
        
        toCompile--;
    }
    
    if(toCompile > 0) {
        compilerForEntrant3 = new Compiler;
        if(!compilerForEntrant3->compile(fileNames[3], error)) {
            setError(fileNames[3] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        nameOfColony.push_back(compilerForEntrant3->getColonyName());
        
        toCompile--;
    }
    
    for(int i = 0; i < VIEW_WIDTH; i++) {
        for(int j = 0; j < VIEW_HEIGHT; j++) {
            Field::FieldItem item = f.getContentsOf(i, j);
            if(item == Field::FieldItem::rock) {
                actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(new Pebble(i, j, this));
                //cout << "A pebble should be placed at (" << i << ", " << j << ")." << endl;
            }
            if(item == Field::FieldItem::anthill0) {
                if(fileNames.size() >= 1) {
                    ah0 = new Anthill(i, j, this, 0, compilerForEntrant0);
                    actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(ah0);
                }
                //cout << "The anthill for the first Ant should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::anthill1) {
                if(fileNames.size() >= 2) {
                    ah1 = new Anthill(i, j, this, 1, compilerForEntrant1);
                    actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(ah1);
                }
                //cout << "The anthill for the second Ant should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::anthill2) {
                if(fileNames.size() >= 3) {
                    ah2 = new Anthill(i, j, this, 2, compilerForEntrant2);
                    actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(ah2);
                }
                //cout << "The anthill for the third Ant should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::anthill3) {
                if(fileNames.size() >= 4) {
                    ah3 = new Anthill(i, j, this, 3, compilerForEntrant3);
                    actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(ah3);
                }
                //cout << "The anthill for the fourth Ant should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::food)  {
                actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(new Food(i, j, this));
                //cout << "A food should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::grasshopper) {
                actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(new BabyGrasshopper(IID_BABY_GRASSHOPPER, i, j, 500, this));
                //cout << "A grasshopper should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::water) {
                actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(new Pool(i, j, this));
                //cout << "A water should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::poison) {
                actorGrid[VIEW_HEIGHT - 1 - j][i].push_back(new Poison(i, j, this));
                //cout << "A poison should be placed at (" << i << ", " << j << ")." <<endl;
            }
            if(item == Field::FieldItem::empty)
                ;
                //cout << "empty at (" << i << ", " << j << ")." <<endl;
        }
    }
    
    return 1;
}

std::list<Actor*>& StudentWorld::getListAt(int x, int y) {
    return actorGrid[VIEW_HEIGHT - 1 - y][x];
}

int StudentWorld::getTicks() {
    return ticks;
}

bool StudentWorld::posBlocksInsect(int x, int y) {
    std::list<Actor*>& li = getListAt(x, y);
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        if((*it)->blocksInsect()) {
            return true;
        }
        it++;
    }
    return false;
}

bool StudentWorld::isFoodAt(int x, int y) {
    std::list<Actor*>& li = getListAt(x, y);
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        if((*it)->isFood()) {
            return true;
        }
        it++;
    }
    return false;
}

bool StudentWorld::isAnthillAt(int x, int y, int colonyNum) {
    std::list<Actor*>& li = getListAt(x, y);
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        Anthill* a = dynamic_cast<Anthill*> (*it);
        if(a != nullptr && colonyNum == a->getColonyNumber()) {
            return true;
        }
        it++;
    }
    return false;
}

bool StudentWorld::isPoisonAt(int x, int y) {
    std::list<Actor*>& li = getListAt(x, y);
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        if((*it)->isPoison()) {
            return true;
        }
        it++;
    }
    return false;
}

Pheromone* StudentWorld::pheromoneAt(int x, int y, int colonyNum) {
    std::list<Actor*>& li = getListAt(x, y);
    std::list<Actor*>::iterator it = li.begin();
    
    while(it != li.end()) {
        if((*it)->isPheromone()) {
            Pheromone* p = dynamic_cast<Pheromone*> (*it);
            if(colonyNum == p->getColonyNum())
                return p;
        }
        it++;
    }
    return nullptr;
}

int StudentWorld::getAntsInColony(int colonyNum) {
    return antsInColony[colonyNum];
}

void StudentWorld::setAntsInColony(int colonyNum, int num) {
    antsInColony[colonyNum] = num;
}

std::string StudentWorld::getNameOfColony(int colonyNum) {
    return nameOfColony[colonyNum];
}
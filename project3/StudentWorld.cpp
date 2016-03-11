#include "StudentWorld.h"
#include <string>
#include <stdlib.h>
#include <cmath>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld()
{
    delete player;
    for(int i = 0; i < DIRT_WIDTH; i++)
        for(int j = 0; j < DIRT_HEIGHT; j++)
            delete dirt[i][j];
    for(int i = 0; i < actors.size(); i++)
        delete actors[i];
    actors.clear();
}

void StudentWorld::calculatePathToPlayer()
{
    pathToPlayer.clear(); //path is being recalculated
    std::queue<PathPoint> points;
    points.push(PathPoint(player->getX(),player->getY())); //start from player and search all the paths
    pathToPlayer[PathPoint(player->getX(),player->getY())] = GraphObject::none;
    while(!points.empty())
    {
        PathPoint curr = points.front();
        points.pop();
        for(int i = 0; i < 4; i++)
        {
            GraphObject::Direction dir = GraphObject::none; //try all four directions
            switch(i)
            {
                case 0:
                    dir = GraphObject::up;
                    break;
                case 1:
                    dir = GraphObject::left;
                    break;
                case 2:
                    dir = GraphObject::down;
                    break;
                case 3:
                    dir = GraphObject::right;
                    break;
            }
            int x = curr.m_x;
            int y = curr.m_y;
            posInDirection(x, y, dir);
            if(x >= 0 && x <= 60 &&  y >= 0 && y <= 60 && pathToPlayer.count(PathPoint(x,y)) == 0 && !actorTypeWithin(x, y, "Boulder",3) && !dirtExistsSprite(x, y)) //if it is possible to move in the direction and it has not already been visited
            {
                pathToPlayer[PathPoint(x,y)] = opposite(dir); //the path to get to the player is the opposite of the direction that the search traveled in
                points.push(PathPoint(x,y));
            }
        }
    }
    
}

void StudentWorld::calculatePathToLeave()
{
    //same as path from player but from exit instead
    pathToLeave.clear();
    std::queue<PathPoint> points;
    points.push(PathPoint(60,60));
    pathToLeave[PathPoint(60,60)] = GraphObject::none;
    while(!points.empty())
    {
        PathPoint curr = points.front();
        points.pop();
        for(int i = 0; i < 4; i++)
        {
            GraphObject::Direction dir = GraphObject::none;
            switch(i)
            {
                case 0:
                    dir = GraphObject::up;
                    break;
                case 1:
                    dir = GraphObject::left;
                    break;
                case 2:
                    dir = GraphObject::down;
                    break;
                case 3:
                    dir = GraphObject::right;
                    break;
            }
            int x = curr.m_x;
            int y = curr.m_y;
            posInDirection(x, y, dir);
            if(x >= 0 && x <= 60 &&  y >= 0 && y <= 60 && pathToLeave.count(PathPoint(x,y)) == 0 && !actorTypeWithin(x, y, "Boulder",3) && !dirtExistsSprite(x, y))
            {
                pathToLeave[PathPoint(x,y)] = opposite(dir);
                points.push(PathPoint(x,y));
            }
        }
    }
    
}

//random pickup generation
void StudentWorld::generatePickups()
{
    int Gprob = getLevel() * 25 + 300;
    int n = rand() % Gprob + 1;
    if(n == 1){
        int p = rand() % 5 + 1;
        if( p == 1)
            addActor(new SKit(this,0,60,min(100,300 - 10*static_cast<int>(getLevel()))));
        else
        {
            while(1)
            {
                int randX = rand() % 61;
                int randY = rand() % 57;
                if(!dirtExistsSprite(randX, randY))
                {
                    addActor(new WRefill(this,randX,randY,min(100,300 - 10*static_cast<int>(getLevel()))));
                    break;
                }
            }
        }
    }
}

//random protestor generation
void StudentWorld::generateProtestors()
{
    if(tickCount == -1 || (tickCount >= min(25, 200 - static_cast<int>(getLevel())) && protestorCount < min(15,2+static_cast<int>(getLevel()*1.5))))
    {
        int pOfHardcore = min(90,static_cast<int>(getLevel())*10+30);
        if(rand() % 100 + 1 <= pOfHardcore)
            addActor(new HardcoreProtestor(this,60,60));
        else
            addActor(new Protestor(this,60,60));
        protestorCount++;
        tickCount = -1;
    }
    tickCount++;
}

void StudentWorld::barrelObtained()
{
    barrelsObtained++;
    if(barrelsObtained >= min(static_cast<int>(getLevel())+2,20))
        game_state = GWSTATUS_FINISHED_LEVEL;
}

int StudentWorld::init()
{
    
    tickCount = -1;
    barrelsObtained = 0;
    protestorCount = 0;
    player = new FrackMan(this);
    for(int i = 0; i < DIRT_WIDTH; i++) //create dirt
    {
        if(i >= 30 && i <= 33)
            for( int j = 0; j < 4; j++)
                dirt[i][j] = new Dirt(this,i,j);
        else
            for(int j = 0; j < DIRT_HEIGHT; j++)
                dirt[i][j] = new Dirt(this,i,j);
    }
    //generate the items in the level(boulder,barrels, and nuggets)
    int l = getLevel()/2;
    int B = min(l+2, 6);
    for(int i = 0; i < B; i++)
    {
        while(1)
        {
            int randX = rand() % 61;
            int randY = rand() % 37 + 20; //boulders can only be above Y position of 20
            if(dirtCoveredSprite(randX, randY)) //only place a boulder where dirt is
            {
                vector<Actor*>* a = getActorsWithin(randX, randY, 6);
                if(a->empty()) //can only be placed if theres no other actor within 6 spots
                {
                    actors.push_back(new Boulder(this,randX,randY));
                    for(int i = 0; i < 4; i++)
                        for(int j = 0; j < 4; j++)
                            removeDirt(randX+i, randY+j); //remove dirt around the boulder
                    delete a;
                    break;
                }
                delete a;
            }
        }
    }
    int L = min(static_cast<int>(getLevel()) + 2, 20);
    for(int i = 0; i < L; i++)
    {
        while(1)
        {
            int randX = rand() % 61;
            int randY = rand() % 57;
            if(dirtCoveredSprite(randX,randY)) //only place an oil barrel where there is dirt
            {
                vector<Actor*>* a = getActorsWithin(randX, randY, 6);
                if(a->empty())
                {
                    actors.push_back(new OilBarrel(this,randX,randY));
                    delete a;
                    break;
                }
                delete a;
            }
        }
    }
    int G = max(5-static_cast<int>(getLevel())/2,2);
    for(int i = 0; i < G; i++)
    {
        while(1)
        {
            int randX = rand() % 61;
            int randY = rand() % 57;
            if(dirtCoveredSprite(randX, randY))
            {
                vector<Actor*>* a = getActorsWithin(randX, randY, 6);
                if(a->empty())
                {
                    actors.push_back(new GNugget(this,randX,randY,false,-1));
                    delete a;
                    break;
                }
                delete a;
            }
        }
    }
    calculatePathToLeave(); //calculate the paths
    calculatePathToPlayer();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    for(int i = 0; i < actors.size(); i++)
    {
        if(!actors[i]->toRemove()) //if the actor shouldnt be removed make it do something
        {
            actors[i]->doSomething();
            if(player->toRemove())
            {
                decLives();
                playSound(SOUND_PLAYER_GIVE_UP);
                return GWSTATUS_PLAYER_DIED;
            }
        }
    }
    if(!player->toRemove())
        player->doSomething();
    generateProtestors();
    generatePickups();
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->toRemove())
        {
            if(actors[i]->getType() == "Protestor" || actors[i]->getType() == "HardcoreProtestor")
                protestorCount--;
            else if(actors[i]->getType() == "Boulder")
            {
                calculatePathToLeave();
                calculatePathToPlayer();
            }
            delete actors[i];
            actors.erase(actors.begin()+i);
        }
    }
    if(player->toRemove())
    {
        decLives();
        playSound(SOUND_PLAYER_GIVE_UP);
        return GWSTATUS_PLAYER_DIED;
    }
    if(game_state == GWSTATUS_FINISHED_LEVEL)
        playSound(SOUND_FINISHED_LEVEL);
    setDisplayText();
    return game_state;
}

vector<Actor*>* StudentWorld::getActorsWithin(int x, int y, int distance)
{
    std::vector<Actor*>* within = new std::vector<Actor*>();
    for(int i = 0; i < actors.size(); i++)
    {
        //add all protestors with a euclidian distance less than or equal to distance to the vector
        if(sqrt(pow(actors[i]->getX() - x,2) + pow(actors[i]->getY() - y, 2)) <= static_cast<double>(distance))
            within->push_back(actors[i]);
    }
    if(sqrt(pow(player->getX() - x, 2) + pow(player->getY() - y, 2)) <= static_cast<double>(distance)) //include the frackman
        within->push_back(player);
    return within;
}

//return the actor if theres an actor of a certain type within a distance from x,y
Actor* StudentWorld::getActorTypeWithin(int x, int y, std::string type, int distance)
{
    if(type == "FrackMan")
    {
        if(sqrt(pow(player->getX() - x, 2) + pow(player->getY() - y, 2)) <= static_cast<double>(distance))
            return player;
    }
    else
    {
        std::vector<Actor*>* a = getActorsWithin(x, y, distance);
        for(int i = 0; i < a->size(); i++)
        {
            if(a->at(i)->getType() == type)
            {
                Actor* toReturn = a->at(i);
                delete a;
                return toReturn;
            }
        }
        delete a;
    }
    return nullptr;
}


void StudentWorld::cleanUp()
{
    game_state = GWSTATUS_CONTINUE_GAME;
    delete player;
    for(int i = 0; i < DIRT_WIDTH; i++)
        for(int j = 0; j < DIRT_HEIGHT; j++)
            delete dirt[i][j];
    for(int i = 0; i < actors.size(); i++)
        delete actors[i];
    actors.clear();
}

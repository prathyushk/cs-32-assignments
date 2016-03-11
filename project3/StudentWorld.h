#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <algorithm>
#include <sstream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
const int DIRT_WIDTH = 64;
const int DIRT_HEIGHT = 60;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir), barrelsObtained(0), game_state(GWSTATUS_CONTINUE_GAME), tickCount(-1), protestorCount(0)
	{
	}
    ~StudentWorld();

    //initializes the world depending on the level
    virtual int init();
    //called every tick
    virtual int move();
    //cleans up the world after a level is completed
    virtual void cleanUp();
    //uses a queue based breadth-first search to create a map of directions to move in to reach the player's position
    void calculatePathToPlayer();
    //uses a queue based breadth-first search to create a map of directions to move in to reach the exit point
    void calculatePathToLeave();
    
    //uses a stringstream to create a formatted string to display the game text as in the example program
    void setDisplayText()
    {
        int score = getScore();
        int level = getLevel();
        int lives = getLives();
        int health = player->getHealth();
        int squirts = player->getSquirts();
        int gold = player->getGold();
        int sonar = player->getSonar();
        int barrelsLeft = std::min(static_cast<int>(getLevel())+2,20) - barrelsObtained;
        std::stringstream ss;
        ss << "Scr: ";
        int zeros;
        if(score >= 10)
            zeros = 6 - ceil(log10(score+1));
        else
            zeros = 5;
        for(int i = 0; i < zeros; i++)
            ss << "0";
        ss << score << "  Lvl: ";
        if(level / 10 == 0)
            ss << " " << level;
        else
            ss << level;
        ss << "  Lives: " << lives << "  Hlth: ";
        if(health / 10 == 0)
            ss << " " << health*10;
        else
            ss << health*10;
        ss << "%  Wtr: ";
        if(squirts / 10 == 0)
            ss << " " << squirts;
        else
            ss << squirts;
        ss << "  Gld: ";
        if(gold / 10 == 0)
            ss << " " << gold;
        else
            ss << gold;
        ss << "  Sonar: ";
        if(sonar / 10 == 0)
            ss << " " << sonar;
        else
            ss << sonar;
        ss << "  Oil Left: ";
        if(barrelsLeft / 10 == 0)
            ss << " " << barrelsLeft;
        else
            ss << barrelsLeft;
        this->setGameStatText(ss.str());

    }
    
    //uses the calculated path to find distance from some point on the path to the player
    int distanceToPlayer(int x, int y)
    {
        if(pathToPlayer.count(PathPoint(x,y)) == 0)
            return -1;
        int dist = 0;
        while(x != player->getX() || y != player->getY())
        {
            posInDirection(x, y, pathToPlayer[PathPoint(x,y)]);
            dist++;
        }
        return dist;
    }
    
    int getFrackManX(){return player->getX();}
    int getFrackManY(){return player->getY();}
    void gotSonar(){player->gotSonar();}
    void gotGold(){player->gotGold();}
    void gotWater(){ player->gotWater();}
    void damageFrackMan(int damage){player->takeDamage(damage);}

    //returns the direction an actor must move to be on a path to the player
    GraphObject::Direction dirToPlayer(int x, int y)
    {
        return pathToPlayer[PathPoint(x,y)];
    }
    
    //returns the direction an actor must move to be on a path to the exit point(60,60)
    GraphObject::Direction dirToMove(int x, int y)
    {
        return pathToLeave[PathPoint(x,y)];
    }
    
    //removes the dirt at a specified x,y and recalculates a path to the exit point
    bool removeDirt(int x, int y)
    {
        if(x < 0 || x > 63 || y < 0 || y > 59 || dirt[x][y] == nullptr)
            return false;
        delete dirt[x][y];
        dirt[x][y] = nullptr;
        calculatePathToLeave();
        return true;
    }
    
    //updates state of game when a barrel is obtained
    void barrelObtained();
    
    bool canMove(int x, int y)
    {
        return x >= 0 && x <= 60 && y >= 0 && y <= 60 && !actorTypeWithin(x, y, "Boulder", 3) && !dirtExistsSprite(x, y);
    }
    
    //returns whether there is an actor of a certain type within a certain distance from the specified x,y
    bool actorTypeWithin(int x, int y, std::string type, int distance = 0)
    {
        if(type == "FrackMan")
            return getActorTypeWithin(x, y, "FrackMan", distance) != nullptr;
        std::vector<Actor*>* a = getActorsWithin(x, y, distance);
        for(int i = 0; i < a->size(); i++)
        {
            if(a->at(i)->getType() == type)
            {
                delete a;
                return true;
            }
        }
        delete a;
        return false;
    }
    
    bool dirtExists(int x, int y)
    {
        return dirt[x][y] != nullptr;
    }
    
    //checks if the bounding box of a sprite at x,y overlaps the bounding box of another sprite of a certain type
    int spriteOverlaps(int x, int y, std::string type)
    {
        int count = 0;
        for(int i = 0; i < actors.size(); i++)
        {
            if(abs(actors[i]->getX() - x) < 4 && abs(actors[i]->getY() - y) < 4 && actors[i]->getType() == type)
                count++;
        }
        return count;
    }
    
    //checks if any dirt exists in the space of a sprite at a point x,y
    bool dirtExistsSprite(int x, int y)
    {
        bool dirtExists = false;
        for(int i = 0; i < SPRITE_WIDTH; i++)
            for(int j = 0; j < SPRITE_HEIGHT; j++)
                if(x + i > -1 && x + i < DIRT_WIDTH && y + j > -1 && y + j < DIRT_HEIGHT &&
                   dirt[x+i][y+j] != nullptr)
                    dirtExists = true;
        return dirtExists;
    }
    
    //returns whether the space of a sprite at x,y is completely covered with dirt
    bool dirtCoveredSprite(int x, int y)
    {
        bool dirtCovered = true;
        for(int i = 0; i < SPRITE_WIDTH; i++)
            for(int j = 0; j < SPRITE_HEIGHT; j++)
            {
                if(x + i < 0 || x + i >= DIRT_WIDTH || y + j < 0 || y + j >= DIRT_HEIGHT || dirt[x+i][y+j] == nullptr)
                    dirtCovered = false;
            }
        return dirtCovered;
    }
    
    void addActor(Actor* a)
    {
        actors.push_back(a);
    }
    
    //checks if any protestors are within hit radius for a squirt at x,y and damages them if so
    bool squirtDamageProtestors(int x, int y)
    {
        bool hit = false;
        std::vector<Actor*>* inDir = getActorsWithin(x,y,3);
        for(int i = 0; i < inDir->size(); i++)
        {
            if(inDir->at(i)->getType() == "Protestor" || inDir->at(i)->getType() == "HardcoreProtestor")
            {
                dynamic_cast<Mortal*>(inDir->at(i))->takeDamage(2);
                hit = true;
            }
        }
        delete inDir;
        return hit;
    }
    
    //reveals oil barrels and gold nuggets in a radius of 12 from the specified x,y
    void sonarUsed(int x, int y)
    {
        playSound(SOUND_SONAR);
        std::vector<Actor*>* a = getActorsWithin(x, y, 12);
        for(int i = 0; i < a->size(); i++)
        {
            if(a->at(i)->getType() == "OilBarrel" || a->at(i)->getType() == "GNugget")
            {
                dynamic_cast<Pickup*>(a->at(i))->found();
            }
        }
        delete a;
    }
    
    //kills mortals that are within the range of a falling boulder
    void boulderDamageMortals(int x, int y)
    {
        std::vector<Actor*>* a = getActorsWithin(x, y, 3);
        for(int i = 0; i < a->size(); i++)
        {
            Mortal* m = dynamic_cast<Mortal*>(a->at(i));
            if(m != nullptr)
                m->takeDamage(100);
        }
        delete a;
    }
    
    //checks whether a protestor can pick up gold or not
    bool checkProtestorGold(int x, int y)
    {
        Actor* a = getActorTypeWithin(x, y, "Protestor", 3);
        Actor* b = getActorTypeWithin(x, y, "HardcoreProtestor", 3);
        if(a != nullptr || b != nullptr)
        {
            playSound(SOUND_PROTESTER_FOUND_GOLD);
            Actor* theOne;
            a != nullptr ? theOne = a : b != nullptr ? theOne = b : theOne = nullptr;
            if(theOne != nullptr)
            {
                Protestor* prot = dynamic_cast<Protestor*>(theOne);
                prot->gotGold();
                increaseScore(25);
                return true;
            }
        }
        return false;
    }

private:
    struct PathPoint
    {
        PathPoint(int x, int y)
        : m_x(x),m_y(y){}
        //< operator in order to allow the use of map
        bool operator<(const PathPoint& other) const
        {
            if(m_x == other.m_x)
                return m_y < other.m_y;
            return m_x < other.m_x;
        }
        int m_x;
        int m_y;
    };
    
    //returns a direction that is the opposite of dir
    GraphObject::Direction opposite(GraphObject::Direction dir)
    {
        switch(dir)
        {
            case GraphObject::left:
                return GraphObject::right;
            case GraphObject::right:
                return GraphObject::left;
            case GraphObject::up:
                return GraphObject::down;
            case GraphObject::down:
                return GraphObject::up;
            case GraphObject::none:
                return dir;
        }
    }
    //returns a vector of actors that are within a certain distance to an x,y. Helper function
    std::vector<Actor*>* getActorsWithin(int x, int y, int distance = 0);
    //returns an actor of a certain type in a certain distance to an x,y. Helper function
    Actor* getActorTypeWithin(int x, int y, std::string type, int distance = 0);
    void generatePickups();
    void generateProtestors();
    std::map<PathPoint,GraphObject::Direction> pathToLeave;
    std::map<PathPoint,GraphObject::Direction> pathToPlayer;
    FrackMan* player;
    Dirt* dirt[DIRT_WIDTH][DIRT_HEIGHT];
    std::vector<Actor*> actors;
    int barrelsObtained;
    int game_state;
    int tickCount;
    int protestorCount;
};

#endif // STUDENTWORLD_H_

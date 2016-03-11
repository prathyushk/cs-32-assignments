#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

//updates an x,y depending on the input direction in the number of steps specified
void posInDirection(int& x, int& y, GraphObject::Direction dir, int numSteps = 1);

class StudentWorld;

//base actor class
//keeps track of fields that all actors will need
class Actor : public GraphObject
{
public:
    Actor(std::string name, StudentWorld* world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
    : GraphObject(imageID,startX,startY,dir,size,depth), m_toRemove(false), m_type(name), m_world(world)
    { setVisible(true); }
    virtual ~Actor(){}
    virtual void doSomething() = 0;
    void removeSelf(){ m_toRemove = true; }
    bool toRemove() { return m_toRemove; }
    std::string getType() { return m_type; }
    StudentWorld* getWorld() { return m_world; }
private:
    bool m_toRemove;
    std::string m_type; //in order to avoid dynamic_cast checking
    StudentWorld* m_world;
};

//an actor that has health
class Mortal : public Actor
{
public:
    Mortal(std::string type, StudentWorld* world, int imageID, int startX, int startY, Direction dir, int health)
    : Actor(type, world, imageID, startX, startY,dir), m_health(health)
    {}
    virtual ~Mortal(){}
    int getHealth() { return m_health; }
    void setHealth(int h)
    {
        m_health = h;
    }
    virtual bool takeDamage(int dmg)
    {
        m_health -= dmg;
        if(m_health <= 0)
            removeSelf();
        return toRemove();
    }
private:
    int m_health;
};

//the class that represents the player character
class FrackMan : public Mortal
{
public:
    FrackMan(StudentWorld* world)
    : Mortal("FrackMan", world, IID_PLAYER, 30, 60, right, 10), ticksSinceSoundPlayed(0), m_squirts(5), m_charges(1), m_nuggets(0) {}
    virtual ~FrackMan(){}
    virtual void doSomething();
    int getSquirts(){return m_squirts;}
    int getSonar(){return m_charges;}
    int getGold(){return m_nuggets;}
    void gotGold()
    {
        m_nuggets++;
    }
    void gotSonar()
    {
        m_charges++;
    }
    void gotWater()
    {
        m_squirts+=5;
    }
    
private:
    void moveInDir(Direction dir);
    int ticksSinceSoundPlayed;
    int m_squirts;
    int m_charges;
    int m_nuggets;
};

//base class for protestor as well as the actual regular protestor
class Protestor : public Mortal
{
public:
    Protestor(StudentWorld* world, int startX, int startY, int health=5, int imageID = IID_PROTESTER, std::string type = "Protestor");
    virtual ~Protestor(){}
    virtual void doSomething();
    void move();
    bool leaveField();
    bool attack();
    void update();
    virtual void gotGold();
    int getRestTicks()
    {
        return inBetween;
    }
    void setRest(int s)
    {
        inBetween = s;
    }
    void decRest()
    {
        inBetween--;
    }
    virtual bool takeDamage(int dmg);
private:
    int lastShout;
    int m_moveInDir;
    bool m_leaveField;
    int inBetween;
    int lastPerp;
};

//hardcore extension on top of the protestor functionality
class HardcoreProtestor : public Protestor
{
public:
    HardcoreProtestor(StudentWorld* world, int startX, int startY)
    : Protestor(world, startX, startY, 20, IID_HARD_CORE_PROTESTER, "HardcoreProtestor") {}
    virtual ~HardcoreProtestor(){}
    virtual void gotGold();
    virtual void doSomething();
};


//base class for all objects that can be picked up
//they can have a finite or infinite lifetime as well as be hidden or visible
class Pickup : public Actor
{
public:
    Pickup(std::string type, StudentWorld* world, int imageID, int startX, int startY, int life)
    : Actor(type, world, imageID, startX, startY, right, 1.0, 2), m_found(false), lifeTime(life)
    { setVisible(false); }
    virtual ~Pickup(){}
    bool updateLife()
    {
        if(lifeTime == -1)
            return true;
        lifeTime--;
        return lifeTime != 0;
    }
    bool frackManFind();
    bool isFound()
    {
        return m_found;
    }
    void found()
    {
        setVisible(true);
        m_found = true;
    }
private:
    bool m_found;
    int lifeTime;
};

//represents the oil barrels in game
class OilBarrel : public Pickup
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY)
    : Pickup("OilBarrel",world,IID_BARREL,startX,startY,-1){}
    virtual ~OilBarrel(){}
    virtual void doSomething();
};

//represents the gold nuggets in the game
class GNugget : public Pickup
{
public:
    GNugget(StudentWorld* world, int startX, int startY, bool visible, int life)
    : Pickup("GNugget",world,IID_GOLD,startX,startY,life)
    {
        if(visible)
        {
            forProtestor = true;
            found();
        }
        else
            forProtestor = false;
    }
    
    virtual ~GNugget(){}
    virtual void doSomething();
private:
    bool forProtestor;
};

//represents the sonar kits in the game
class SKit : public Pickup
{
public:
    SKit(StudentWorld* world, int startX, int startY, int life)
    : Pickup("SKit", world, IID_SONAR, startX, startY, life)
    {found();}
    virtual ~SKit() {}
    virtual void doSomething();
};

//represents the water pickups in the game
class WRefill : public Pickup
{
public:
    WRefill(StudentWorld* world, int startX, int startY, int life)
    : Pickup("WRefill", world, IID_WATER_POOL,startX,startY,life)
    {found();}
    virtual ~WRefill() {}
    virtual void doSomething();
    
};

//represents water squirts that damage protestors in the game
class Squirt : public Actor
{
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction dir)
    : Actor("Squirt",world,IID_WATER_SPURT, startX, startY, dir, 1.0, 1), toTravel(4){}
    virtual ~Squirt() {}
    virtual void doSomething();
private:
    int toTravel;
};

//represents boulders in the game
class Boulder : public Actor
{
public:
    Boulder(StudentWorld* world, int startX, int startY)
    : Actor("Boulder",world,IID_BOULDER,startX,startY,down,1.0,1), m_state(stable), waitingTicks(0){}
    virtual ~Boulder() {}
    virtual void doSomething();
private:
    enum State { stable, waiting, falling } m_state;
    int waitingTicks;
};

//represents dirt in the game
class Dirt : public Actor
{
public:
    Dirt(StudentWorld* world, int startX, int startY)
    : Actor("Dirt", world, IID_DIRT, startX, startY, right, 0.25, 3) {}
    virtual ~Dirt() {}
    virtual void doSomething() {}
};

#endif // ACTOR_H_

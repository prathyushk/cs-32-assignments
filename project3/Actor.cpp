
#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;

void posInDirection(int& x, int& y, GraphObject::Direction dir, int numSteps)
{
    switch(dir)
    {
        case GraphObject::up:
            y+=numSteps;
            break;
        case GraphObject::down:
            y-=numSteps;
            break;
        case GraphObject::right:
            x+=numSteps;
            break;
        case GraphObject::left:
            x-=numSteps;
            break;
        case GraphObject::none:
            break;
    }
}

Protestor::Protestor(StudentWorld* world, int startX, int startY, int health, int imageID, std::string type)
: Mortal(type, world, imageID, startX, startY, left, health), m_moveInDir(rand() % 53 + 8), lastShout(99), m_leaveField(false), inBetween(max(0,3-static_cast<int>(getWorld()->getLevel())/4)), lastPerp(999)
{}

bool Protestor::takeDamage(int dmg)
{
    if(getHealth() <= 0)
        return false;
    setHealth(getHealth() - dmg);
    if(getHealth() <= 0)
    {
        //if the protestor dies, leave field, play sound and update score depending on if killed by boulder or squirt
        m_leaveField = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        inBetween = 0;
        if(dmg == 100)
            getWorld()->increaseScore(500);
        else
            getWorld()->increaseScore(100);
    }
    else
    {
        //not dead but damaged so play sound and get stunned
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        inBetween += min(50,100-static_cast<int>(getWorld()->getLevel())*10);
    }
    return m_leaveField;
}

void Protestor::gotGold()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(25);
    m_leaveField = true;
}

void HardcoreProtestor::gotGold()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(50);
    setRest(min(50,100-static_cast<int>(getWorld()->getLevel())*10));
}

void Protestor::move()
{
    int fx = getWorld()->getFrackManX(), fy = getWorld()->getFrackManY();
    if(getX() - fx== 0 || getY() - fy== 0) //check if the frackman is in the same row or column as the protestor
    {
        if(!getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",4))
        {
            int x = getX(); int y = getY(); //see what direction the protestor must move in to find the frackman by comparing x and y values
            Direction dir;
            if(fx > x)
                dir = right;
            else if(fx < x)
                dir = left;
            else if(fy > y)
                dir = up;
            else
                dir = down;
            bool canMove = true;
            while(x != fx|| y != fy) //finds whether there is an unblocked straight path to the frackman
            {
                posInDirection(x, y, dir);
                if(!getWorld()->canMove(x, y))
                    canMove = false;
            }
            if(canMove)
            {
                //moves in that straight path if its possible
                setDirection(dir);
                x = getX();
                y = getY();
                posInDirection(x, y, getDirection());
                moveTo(x, y);
                m_moveInDir = 0;
                return;
            }
        }
    }
    else
    {
        m_moveInDir--;
        if(m_moveInDir <= 0) //need to reset number of steps to take
        {
            while(1)
            {
                //selects a random direction that the protestor can move at least one step in
                Direction dir = none;
                int randDir = rand() % 4;
                switch(randDir)
                {
                    case 0:
                        dir = up;
                        break;
                    case 1:
                        dir = left;
                        break;
                    case 2:
                        dir = down;
                        break;
                    case 3:
                        dir = right;
                        break;
                }
                int x = getX(); int y = getY();
                posInDirection(x, y, dir);
                if(getWorld()->canMove(x, y)) //checks whether the protestor can actually take at least one step in this random direction
                {
                    setDirection(dir);
                    m_moveInDir = rand() % 53 + 8;
                    break;
                }
            }
        }
        else
        {
            Direction dir1, dir2;
            int x = getX(); int y = getY();
            //finds possible intersection path directions depending on current direction
            if(getDirection() == right || getDirection() == left)
            {
                dir1 = up;
                dir2 = down;
            }
            else
            {
                dir1 = left;
                dir2 = right;
            }
            posInDirection(x, y, dir1);
            bool can1 = getWorld()->canMove(x, y); //checks whether it can move in one of those directions
            x = getX(); y = getY();
            posInDirection(x, y, dir2); //check if can move at least one in the other direction
            bool can2 = getWorld()->canMove(x, y);
            if(lastPerp > 200) //if the protestor has not turned in 200 non-resting ticks
            {
                if(can1 || can2) //move in one of those two directions if they are viable
                {
                    if(can1 && can2) //choose one randomly if they are both viable
                    {
                        if(rand() % 2 == 0)
                            setDirection(dir1);
                        else
                            setDirection(dir2);
                    }
                    else if(can1)
                        setDirection(dir1);
                    else if(can2)
                        setDirection(dir2);
                    m_moveInDir = rand() % 53 + 8;
                    lastPerp = 0;
                }
            }
        }
    }
    int x = getX(),y = getY();
    posInDirection(x, y, getDirection()); //move in the set direction
    if(getWorld()->canMove(x, y))
        moveTo(x, y);
    else
        m_moveInDir = 0;
    lastPerp++;
}

bool Protestor::leaveField()
{
    if(m_leaveField)
    {
        if(getX() == 60 && getY() == 60) //protestor made it to end
            removeSelf();
        int x = getX(); int y = getY();
        setDirection(getWorld()->dirToMove(x, y)); //set the direction of the protestor based on the path
        posInDirection(x, y, getDirection()); //move in that direction
        moveTo(x, y);
        return true;
    }
    return false;
}
bool Protestor::attack()
{
    lastShout++;
    if(getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",4)) //if the frackman is within attack range
    {
        Direction toFace;
        int xDiff = getX() - getWorld()->getFrackManX();
        int yDiff = getY() - getWorld()->getFrackManY();
        if(abs(xDiff) > abs(yDiff)) //figure out whether the frackman is to the left or right
        {
            if(xDiff < 0)
                toFace = right; //frackman is to the right
            else
                toFace = left; //frackman is to the left
        }
        else
        {
            if(yDiff > 0)
                toFace = down; //frackman is below
            else
                toFace = up; //frackman is above
        }
        if(getDirection() == toFace) //if the protestor is facing in that direction then attack if it has not attacked in the last 15 ticks
        {
            if(lastShout >= 15)
            {
                getWorld()->playSound(SOUND_PROTESTER_YELL);
                getWorld()->damageFrackMan(2);
                lastShout = 0;
            }
            return true;
        }
    }
    return false;

}
void Protestor::doSomething()
{
    if(toRemove())
        return;
    if(getRestTicks() == 0) //if rest ticks run out
    {
        setRest(max(0,3-static_cast<int>(getWorld()->getLevel())/4)); //reset rest ticks
        if(leaveField()) //see if it needs to leave field and perform it
            return;
        else if(attack()) //see if it can attack and perform it
            return;
        move();
    }
    else
        decRest(); //rest tick decrement
}

void HardcoreProtestor::doSomething()
{
    if(toRemove())
        return;
    if(getRestTicks() == 0)
    {
        setRest(max(0,3-static_cast<int>(getWorld()->getLevel())/4));
        if(leaveField()) //same as regular protestor
            return;
        else if(attack()) //same as regular protestor
            return;
        if(!getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",4)) //hardcore protestor cell signal tracking
        {
            if(getWorld()->distanceToPlayer(getX(), getY()) <= 16 + getWorld()->getLevel()*2) //if number of steps to move to find the frackman is within the range go to the frackman
            {
                setDirection(getWorld()->dirToPlayer(getX(), getY()));
                int x = getX(); int y = getY();
                posInDirection(x, y, getDirection());
                moveTo(x, y);
                return;
            }
        }
        move();
    }
    else
        decRest();
}

bool Pickup::frackManFind()
{
    if(!isFound() && getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",4)) //if the pickup has not already been found and the frackman is within a radius of 4, this pickup has been found
    {
        found();
        return true;
    }
    return false;
}

void WRefill::doSomething()
{
    if(toRemove())
        return;
    if(getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",3)) //check whether the refill has been picked up
    {
        removeSelf();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->gotWater();
        getWorld()->increaseScore(100);
    }
    if(!updateLife())
        removeSelf();
}

void SKit::doSomething()
{
    if(toRemove())
        return;
    if(getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",3)) //check whether the sonar kit has been picked up
    {
        removeSelf();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->gotSonar();
        getWorld()->increaseScore(75);
    }
    if(!updateLife())
        removeSelf();
}

void GNugget::doSomething()
{
    if(toRemove())
        return;
    if(frackManFind())
        return;
    else if(!forProtestor && getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",3)) //check whether it is for the frackman and if its been picked up
    {
        removeSelf();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        getWorld()->gotGold();
    }
    else if(forProtestor) //if it is for the protestor
    {
        if(getWorld()->checkProtestorGold(getX(),getY())) //check if a protestor can pick up the nugget
            removeSelf();
        
    }
    if(!updateLife())
        removeSelf();
}

void OilBarrel::doSomething()
{
    if(toRemove())
        return;
    if(frackManFind()) return;
    else if(getWorld()->actorTypeWithin(getX(), getY(), "FrackMan",3)) //check whether the oil barrel can be picked up
    {
        removeSelf();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->barrelObtained();
    }
}

void Boulder::doSomething()
{
    if(toRemove())
        return;
    if(m_state == stable)
    {
        bool dirtExists = false; //check whether there is dirt under the boulder
        for(int i = 0; i < 4; i++)
        {
            int x = getX() + i;
            int y = getY();
            posInDirection(x, y, getDirection());
            if(getWorld()->dirtExists(x, y))
                dirtExists = true;
        }
        if(!dirtExists) //if there is not, wait to fall
            m_state = waiting;
    }
    else if(m_state == waiting)
    {
        waitingTicks++;
        if(waitingTicks == 30) //if waiting is done, fall
        {
            m_state = falling;
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
    }
    else
    {
        moveTo(getX(), getY()-1); //move down
        getWorld()->calculatePathToLeave(); //recalculate paths because the movement will change it
        getWorld()->calculatePathToLeave();
        if(getY() < 0)
            removeSelf();
        bool dirtExists = false;
        int x = getX();
        int y = getY();
        for(int i = 0; i < 4; i++) //check whether boulder landed on dirt
        {
            if(getWorld()->dirtExists(x+i, y))
                dirtExists = true;
        }
        if(dirtExists || getWorld()->spriteOverlaps(x,y,"Boulder") > 1) //check whether there is overlap with another boulder
            removeSelf();
        
        else
        {
            getWorld()->boulderDamageMortals(getX(),getY()); //check if can damage a mortal
        }
    }
}

void Squirt::doSomething()
{
    if(toTravel == 0)
        removeSelf();
    int x = getX();
    int y = getY();
    if(getWorld()->squirtDamageProtestors(getX(),getY())) //check if the squirt can damage protestors and do it if possible
       removeSelf();
    posInDirection(x, y, getDirection());
    if(getWorld()->dirtExistsSprite(x, y)) //if it hits dirt, die
        removeSelf();
    else
    {
        if(getWorld()->actorTypeWithin(x, y, "Boulder",3)) //check if it hits a boulder
           removeSelf();
    }
    moveTo(x, y); //move
    toTravel--;
}

void FrackMan::moveInDir(Direction dir)
{
    if(getDirection() != dir) //if its not currently facing the direction, face it
        setDirection(dir);
    else
    {
        //move in the direction if it can
        int x = getX(), y =getY();
        posInDirection(x, y, dir);
        if(x < 0 || x > VIEW_WIDTH-SPRITE_WIDTH || y < 0 || y > VIEW_HEIGHT-SPRITE_HEIGHT)
            moveTo(getX(), getY());
        else if(getWorld()->actorTypeWithin(x, y, "Boulder",3));
        else
            moveTo(x, y);
    }
}

void FrackMan::doSomething()
{
    if(toRemove())
        return;
    ticksSinceSoundPlayed++;
    bool playedSound = false;
    //remove dirt that the frackman overlaps
    for(int i = getX(); i < getX() + SPRITE_WIDTH; i++)
        for(int j = getY(); j < getY() + SPRITE_HEIGHT; j++)
            if(getWorld()->removeDirt(i, j))
            {
                if(!playedSound && ticksSinceSoundPlayed > 3)
                {
                    ticksSinceSoundPlayed = 0;
                    playedSound = true;
                    getWorld()->playSound(SOUND_DIG);
                }
            }
    int ch;
    if(getWorld()->getKey(ch) == true)
    {
        switch(ch)
        {
            case KEY_PRESS_SPACE:
                if(m_squirts > 0)
                {
                    //shoot a squirt if there is no boulder or dirt in the way
                    m_squirts--;
                    int x = getX(); int y = getY();
                    posInDirection(x, y, getDirection(),4);
                    if(!getWorld()->actorTypeWithin(x, y, "Boulder",3) && !getWorld()->dirtExistsSprite(x, y))
                        getWorld()->addActor(new Squirt(getWorld(),x,y,getDirection()));
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                }
                break;
            case KEY_PRESS_TAB:
                //drop a gold nugget if possible
                if(m_nuggets > 0)
                {
                    m_nuggets--;
                    getWorld()->addActor(new GNugget(getWorld(),getX(),getY(),true,100));
                }
                break;
            case 122: //z
            case 90: //Z
                //use a sonar kit if possible
                if(m_charges > 0)
                {
                    m_charges--;
                    getWorld()->sonarUsed(getX(), getY());
                }
                break;
            //move in the direction specified by the key
            case KEY_PRESS_LEFT:
                moveInDir(left);
                getWorld()->calculatePathToPlayer();
                break;
            case KEY_PRESS_RIGHT:
                moveInDir(right);
                getWorld()->calculatePathToPlayer();
                break;
            case KEY_PRESS_DOWN:
                moveInDir(down);
                getWorld()->calculatePathToPlayer();
                break;
            case KEY_PRESS_UP:
                moveInDir(up);
                getWorld()->calculatePathToPlayer();
                break;
        }
    }
}

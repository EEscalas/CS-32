#ifndef ACTOR_H_
#define ACTOR_H_

#include <string>
#include "GraphObject.h"

class StudentWorld;

///////////////////////////////////////////////////////////
/////////////////     A C T O R      //////////////////////
///////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir = right, float size = 1.0, unsigned int depth = 1);
	virtual void doSomething() = 0;
	StudentWorld* getWorld() const;
	bool isAlive() const;
	void kill();
	bool isActorVisible() const;
	void setInvisible();
	void makeVisible();
	int getXValue() const;
	int getYValue() const;
private:
	StudentWorld* m_studentWorld;
	bool m_alive;
	bool m_visible;
};


////////////////      D I R T       ///////////////////////

class Dirt : public Actor
{
public:
	Dirt(StudentWorld* world, int startX, int startY);
	virtual ~Dirt();
	virtual void doSomething() {}

private:
};

//////////////     B O U L D E R       ////////////////////

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* world, int startX, int startY);
	virtual ~Boulder();
	virtual void doSomething() {}
};

/////////////   S Q U I R T G U N    //////////////////////

class SquirtGun : public Actor
{

public:
	SquirtGun(StudentWorld* world);
	virtual void doSomething() {}
	void setSquirts(int squirts);
	int getSquirts() const;
private:
	int m_travelDistance;
	int m_squirts;
};

///////////////////////////////////////////////////////////
//////////////   H U M A N A C T O R    ///////////////////
///////////////////////////////////////////////////////////

class HumanActor : public Actor
{
public:
	HumanActor(StudentWorld* world, int imageID, int startX, int startY, Direction dir);
	virtual ~HumanActor();
	virtual void doSomething() {}
	void decHitPoints();
	void setHitPointsTo(int hitPoints);
	bool zeroHitPoints() const;
	void annoy();
	void setHealth(int health);
	int getHealth() const;
private:
	int m_hitPoints;
	int m_health;
};

/////////////      F R A C K M A N       /////////////////

class FrackMan : public HumanActor
{
public:
	FrackMan(StudentWorld* world);
	virtual ~FrackMan();
	virtual void doSomething();
	SquirtGun* getGun() const;
private:
	int m_sonarCharge;
	SquirtGun* gun;
};

////////////////    P R O T E S T O R    //////////////////

class Protestor : public HumanActor
{
public:
	Protestor(StudentWorld* world, int startX, int startY, int imageID = IID_PROTESTER);
	virtual ~Protestor();
};

/////////   H A R D C O R E P R O T E S T O R    //////////

class HardcoreProtestor : public Protestor
{
public:
	HardcoreProtestor(StudentWorld* world, int startX, int startY);
	virtual ~HardcoreProtestor();
};


///////////////////////////////////////////////////////////
////////////    G O O D I E A C T O R    //////////////////
///////////////////////////////////////////////////////////

class GoodieActor : public Actor
{
public:
	GoodieActor(FrackMan* hunter, StudentWorld* world, int imageID, int startX, int startY);
	virtual ~GoodieActor();
	virtual void doSomething() = 0;
	void getFrackManPosition(int &x, int &y) const;
private:
	FrackMan* m_hunter;
};

////////////   B A R R E L L O F O I L    /////////////////

class BarrelOfOil : public GoodieActor
{
public:
	BarrelOfOil(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~BarrelOfOil();
	virtual void doSomething();
};


//////////////   G O L D N U G G E T     //////////////////

class GoldNugget : public GoodieActor
{
public:
	GoldNugget(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~GoldNugget();
	virtual void doSomething();
private:

};


////////////////   S O N A R K I T   //////////////////////

class SonarKit : public GoodieActor
{
public:
	SonarKit(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~SonarKit();
	virtual void doSomething();
};


///////////////   W A T E R P O O L    ////////////////////

class WaterPool : public GoodieActor
{
public:
	WaterPool(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~WaterPool();
	virtual void doSomething();
};

#endif // ACTOR_H_

#ifndef ACTOR_H_
#define ACTOR_H_

#include <string>
#include <stack>
#include "GraphObject.h"
using namespace std;

class StudentWorld;

///////////////////////////////////////////////////////////
/////////////////     A C T O R      //////////////////////
///////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir = right, float size = 1.0, unsigned int depth = 1);		// constructor
	virtual ~Actor();


	// functions used by studentWorld
	virtual void doSomething() = 0;		// pure virtual function
	
	bool isAlive() const;				// uses duplicate visiblity marker to have this function
	void setInvisible();				// changes duplicate visibility marker
	void makeVisible();					// changes duplicate visibility marker
	virtual void kill();				// sets actors as dead

	virtual void squirtProtester() {}			// squirt does nothing unless you are squirting a protester class
	virtual void instersectNugget() {}			// does nothing unless you are a humanActor
	virtual void sendHome() { kill(); }			// snedHome means kill unless you are dealing with a protester class

	virtual bool isHuman() const { return false; }		// only returns true for humanActor classes
	virtual bool isBoulder() const { return false; }	// only returns true for boulder class
	virtual bool isFalling() const { return true; }			// only returns proper answer when dealing with a Boulder class
	virtual bool isLeaving() const { return false; }			// only returns proper answer when dealing with a protester class
	virtual bool isSquirt() const { return false; }			// only returns 
		
	// functions that recieve information from studentWorld
	virtual bool isDirtBelow(int x, int y) const;
	StudentWorld* getWorld() const;

	// functions used by derived classes only
	bool isActorVisible() const;

	void incTicks();
	void setTicksToZero();
	int getTickCount() const;

	virtual void annoy() {}						// only responds when called for a HumanActor
	virtual void bribed() {}					// only responds when called for a HumanActor

private:
	StudentWorld* m_studentWorld;
	bool m_alive;
	bool m_visible;
	int m_tickCount;
};


////////////////      D I R T       ///////////////////////

class Dirt : public Actor
{
public:
	Dirt(StudentWorld* world, int startX, int startY);
	virtual ~Dirt();
	virtual void doSomething() {}	// doesn't do anything, but this must be defined in order for dirt to not be an abstract base class
};

//////////////     B O U L D E R       ////////////////////

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* world, int startX, int startY);
	virtual ~Boulder();
	virtual void doSomething();
	virtual bool isBoulder() const { return true; }
	virtual bool isFalling() const;
private:
	bool m_falling;
	bool m_stable;
	int m_tickCount;
};

/////////////   S Q U I R T G U N    //////////////////////

class SquirtGun : public Actor
{

public:
	SquirtGun(StudentWorld* world, int x, int y, Direction dir);
	virtual void doSomething();
	virtual bool isSquirt() const { return true; }
private:
	int m_travelDistance;
};

///////////////////////////////////////////////////////////
//////////////   H U M A N A C T O R    ///////////////////
///////////////////////////////////////////////////////////

class HumanActor : public Actor
{
public:
	HumanActor(StudentWorld* world, int imageID, int startX, int startY, Direction dir);
	virtual ~HumanActor();
	virtual void doSomething() = 0;
	virtual bool isHuman() const { return true; }

	void decHitPoints();
	void setHitPointsTo(int hitPoints);
	bool zeroHitPoints() const;
	int getHitPoints() const;

	virtual void annoy() = 0;
	
	void incGoldNuggets();
	void decGoldNuggets();
	bool noGoldNuggets() const;
	int getNuggets() const;

	virtual void kill();

private:
	int m_hitPoints;
	int m_nuggets;
};

/////////////      F R A C K M A N       /////////////////

class FrackMan : public HumanActor
{
public:
	FrackMan(StudentWorld* world);
	virtual ~FrackMan();
	virtual void doSomething();

	bool doesFrackmanOverlap(int x, int y);

	int getSonar() const;
	void decSonar();
	void incSonar();

	void incSquirt();
	int getSquirts() const;
	void squirt();

	virtual void annoy();
private:
	SquirtGun* gun;
	int m_sonar;
	int m_squirts;
};

////////////////    P R O T E S T E R    //////////////////

class Protester : public HumanActor
{
public:
	Protester(StudentWorld* world, int imageID = IID_PROTESTER);
	virtual ~Protester();
	virtual void doSomething();

	// used by other actor classes
	virtual void annoy();
	void setStunned(int stunAmt);

	// used by studentWorld
	void squirtProtester();
	virtual void instersectNugget();
	virtual void sendHome();
	virtual bool isLeaving() const { return m_leaving; }

	// modified by hardcore protester
	virtual void bribed();
	virtual bool inLineOfSight();
	virtual void moveTowardsFrackman();


private:
	void leaveFromShaft();
	void randDir(Direction& dir);
	bool canMove(Direction dir);
	void takeStepForward();
	bool perpendicularTurn(const Direction &newD);
	void changeToPerpDir();
	void changeDir();
	void leaveField();
	bool isActive() const;
	bool frackmanWithinRange() const;
	void shout();


	bool m_leaving;
	int m_stunned;
	int m_numSquaresToMoveInCurrentDirection;
	bool m_shout;
	int m_nonRestingTicks;
	bool m_goNextTick;
	bool m_squirted;
	int m_perpendicularTurnCount;
	int m_timeSincePerpTurn;
	struct point
	{
		int m_x; 
		int m_y;
	};
	stack<point> m_leavingPath;
};

/////////   H A R D C O R E P R O T E S T E R    //////////

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world);
	virtual ~HardcoreProtester();
	virtual void doSomething();
	virtual void bribed();
//	virtual bool inLineOfSight();
//	virtual void moveTowardsFrackman();

private:
//	int movesToFrackman(int& startX, int& startY);
	struct point
	{
		int m_x;
		int m_y;
	};
	char m_availableSpace[VIEW_HEIGHT][VIEW_WIDTH];
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
	void find();
	void getFrackManPosition(int &x, int &y) const;
	virtual void removeObject() = 0;
	void incSonarKit();
	void incNuggets();
	void addSquirtsToGun();
private:
	FrackMan* m_hunter;
	int m_tickCount;
};

////////////   B A R R E L L O F O I L    /////////////////

class BarrelOfOil : public GoodieActor
{
public:
	BarrelOfOil(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~BarrelOfOil();
	virtual void doSomething();
	virtual void removeObject();
};


//////////////   G O L D N U G G E T     //////////////////

class GoldNugget : public GoodieActor
{
public:
	GoldNugget(FrackMan* hunter, StudentWorld* world, int startX, int startY, bool dropped);
	virtual ~GoldNugget();
	virtual void doSomething();
	virtual void removeObject();
	void drop();
private:
	int m_tickCount;
	bool m_dropped;
};


////////////////   S O N A R K I T   //////////////////////

class SonarKit : public GoodieActor
{
public:
	SonarKit(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~SonarKit();
	virtual void doSomething();
	virtual void removeObject();
};


///////////////   W A T E R P O O L    ////////////////////

class WaterPool : public GoodieActor
{
public:
	WaterPool(FrackMan* hunter, StudentWorld* world, int startX, int startY);
	virtual ~WaterPool();
	virtual void doSomething();
	virtual void removeObject();
};

#endif // ACTOR_H_

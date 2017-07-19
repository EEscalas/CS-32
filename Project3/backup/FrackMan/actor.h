
#ifndef ACTOR_H
#define ACTOR_H

#include <string>

typedef int DIRECTION;

class GraphObject
{
	GraphObject(int imageID, int startX, int startY,
		DIRECTION	startDirection, float	size = 1.0,
		unsigned	int	depth = 0) {}
	void setVisible(bool shouldIDisplay) {}
	void getX()	const {}
	void getY()	const {}
	void moveTo(int x, int y) {}
	DIRECTION	getDirection()	const {}	//	Directions:	up,	down,	left,	right
	void setDirection(DIRECTION	d) {}			//	Directions:	up,	down,	left,	right
};

///////////////////////////////////////////////////////////
///////////////      A C T O R      ///////////////////////
///////////////////////////////////////////////////////////

class Actor //: public GraphObject
{
public:
	Actor();
	virtual ~Actor();
	virtual void doSomething() = 0;
private:
};

///////////////////////////////////////////////////////////
///////      I N A C T I V E A C T O R      ///////////////
///////////////////////////////////////////////////////////

class InactiveActor : public Actor
{
public:
	InactiveActor();
	virtual ~InactiveActor();
	virtual void doSomething() {}
};

////////////////      D I R T       ///////////////////////

class Dirt : public InactiveActor
{
public:
	Dirt();
	virtual ~Dirt();
	virtual void doSomething() {};
	bool isDirt(int x, int y);

};

//////////////     B O U L D E R       ////////////////////

class Boulder : public InactiveActor
{
public:
	Boulder();
	virtual ~Boulder();
};

///////////////////////////////////////////////////////////
//////////////   H U M A N A C T O R    ///////////////////
///////////////////////////////////////////////////////////

class HumanActor : public Actor
{
public:
	HumanActor();
	virtual ~HumanActor();
	virtual void doSomething() {}
};

/////////////      F R A C K M A N       /////////////////

class FrackMan : public HumanActor
{
public:
	FrackMan();
	virtual ~FrackMan();
private:

};

////////////////    P R O T E S T O R    //////////////////

class Protestor : public HumanActor
{
public:
	Protestor();
	virtual ~Protestor();
};

/////////   H A R D C O R E P R O T E S T O R    //////////

class HardcoreProtestor : public Protestor
{
public:
	HardcoreProtestor();
	virtual ~HardcoreProtestor();
};


///////////////////////////////////////////////////////////
////////////    G O O D I E A C T O R    //////////////////
///////////////////////////////////////////////////////////

class GoodieActor : public Actor
{
public:
	GoodieActor();
	virtual ~GoodieActor();
	virtual void doSomething() {}
};

////////////   B A R R E L L O F O I L    /////////////////

class BarrelOfOil : public GoodieActor
{
public: 
	BarrelOfOil();
	virtual ~BarrelOfOil();
};


//////////////   G O L D N U G G E T     //////////////////

class GoldNugget : public GoodieActor
{
public:
	GoldNugget();
	virtual ~GoldNugget();
private:

};


////////////////   S O N A R K I T   //////////////////////

class SonarKit : public GoodieActor
{
public:
	SonarKit();
	virtual ~SonarKit();
};


///////////////   W A T E R P O O L    ////////////////////

class WaterPool : public GoodieActor
{
public:
	WaterPool();
	virtual ~WaterPool();
};

#endif

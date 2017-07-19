#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include <string>


///////////////////////////////////////////////////////////
/////////////////     A C T O R      //////////////////////
///////////////////////////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir, float size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth)
{
	makeVisible();
	m_studentWorld = world;
	m_alive = true;
}

StudentWorld* Actor::getWorld() const
{
	return m_studentWorld;
}

bool Actor::isAlive() const
{
	return m_alive;
}

void Actor::kill()
{
	m_alive = false;
}

bool Actor::isActorVisible() const
{
	return m_visible;
}

void Actor::setInvisible()
{
	setVisible(false);
	m_visible = false;
}


void Actor::makeVisible()
{
	setVisible(true);
	m_visible = true;
}

int Actor::getXValue() const { return getX(); }
int Actor::getYValue() const { return getY(); }

////////////////      D I R T       ///////////////////////

Dirt::Dirt(StudentWorld* world, int startX, int startY)
	:Actor(IID_DIRT, startX, startY, world, right, 0.25, 3)
{

}

Dirt::~Dirt()
{

}

//////////////     B O U L D E R       ////////////////////

Boulder::Boulder(StudentWorld* world, int startX, int startY)
	:Actor(IID_BOULDER, startX, startY, world, down)
{

}

Boulder::~Boulder()
{

}

/////////////   S Q U I R T G U N    //////////////////////

SquirtGun::SquirtGun(StudentWorld* world)
	:Actor(IID_WATER_SPURT, 30, 60, world, right, 1, 1)
{
	setInvisible();
}

void SquirtGun::setSquirts(int squirts)
{
	m_squirts = squirts;
}

int SquirtGun::getSquirts() const { return m_squirts; }

///////////////////////////////////////////////////////////
//////////////   H U M A N A C T O R    ///////////////////
///////////////////////////////////////////////////////////

HumanActor::HumanActor(StudentWorld* world, int imageID, int startX, int startY, Direction dir)
	:Actor(imageID, startX, startY, world, dir)
{

}

HumanActor::~HumanActor()
{}

void HumanActor::decHitPoints()
{
	m_hitPoints--;
}

void HumanActor::setHitPointsTo(int hitPoints)
{
	m_hitPoints = hitPoints;
}

bool HumanActor::zeroHitPoints() const
{
	if (m_hitPoints == 0)
		return true;
	else return false;
}

void HumanActor::annoy()
{
	if (m_hitPoints == 0)
		kill();
	m_hitPoints -= 2;
}

void HumanActor::setHealth(int health)
{
	m_health = health;
}

int HumanActor::getHealth() const
{
	return m_health;
}

/////////////      F R A C K M A N       /////////////////

FrackMan::FrackMan(StudentWorld* world)
	:HumanActor(world, IID_PLAYER, 30, 60, right)
{
	setHitPointsTo(10);
	m_sonarCharge = 1;
	setHealth(10);
	gun = new SquirtGun(getWorld());
	gun->setSquirts(5);
}

FrackMan::~FrackMan()
{}

void FrackMan::doSomething()
{
	if (getHealth() == 0)			// determining whether or not he is dead
		return;
	int x = getX();
	int y = getY();
	StudentWorld* world = getWorld();
	
	// remove dirt if frackman overlaps dirt
	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (world->isDirtInField(x + k, y + j))
			{
				world->removeDirtFrom(x + k, y + j);
				world->playSound(SOUND_DIG);
			}
		}
	}
	
	// respond to user pressing keys
	int key = 0;
	if (world->getKey(key))
	{
		Direction dir = getDirection();
		int x = getX();
		int y = getY();
		switch(key)
		{
		case KEY_PRESS_LEFT:
			if (dir == left)
			{
				if (x - 1 >= 0 && !world->isBoulder(x - 1, y))
					moveTo(x - 1, y);
			}
			else setDirection(left);
			break;
		case KEY_PRESS_RIGHT:
			if (dir == right)
			{
				if (x + 1 <= 60 && !world->isBoulder(x+1, y))
					moveTo(x + 1, y);
			}
			else setDirection(right);
			break;
		case KEY_PRESS_UP:
			if (dir == up)
			{
				if (y + 1 <= 60 && !world->isBoulder(x, y + 1))
					moveTo(x, y + 1);
			}
			else setDirection(up);
			break;
		case KEY_PRESS_DOWN:
			if (dir == down)
			{
				if (y - 1 >= 0 && !world->isBoulder(x, y - 1))
					moveTo(x, y - 1);
			}
			else setDirection(down);
			break;
		case KEY_PRESS_SPACE:
			break;
		case KEY_PRESS_ESCAPE:
			kill();
			break;
		case KEY_PRESS_TAB:
			break;
		case 'z':
			break;
		case 'Z':
			break;
		}
	}
}


SquirtGun* FrackMan::getGun() const
{
	return gun;
}

////////////////    P R O T E S T O R    //////////////////

Protestor::Protestor(StudentWorld* world, int startX, int startY, int imageID)
	:HumanActor(world, imageID, startX, startY, left)
{}

Protestor::~Protestor()
{}

/////////   H A R D C O R E P R O T E S T O R    //////////

HardcoreProtestor::HardcoreProtestor(StudentWorld* world, int startX, int startY)
	:Protestor(world, startX, startY, IID_HARD_CORE_PROTESTER)
{}

HardcoreProtestor::~HardcoreProtestor()
{}

///////////////////////////////////////////////////////////
////////////    G O O D I E A C T O R    //////////////////
///////////////////////////////////////////////////////////

GoodieActor::GoodieActor(FrackMan* hunter, StudentWorld* world, int imageID, int startX, int startY)
	:Actor(imageID, startX, startY, world, right, 1, 2)
{
	m_hunter = hunter;
}

GoodieActor::~GoodieActor()
{}

void GoodieActor::getFrackManPosition(int &x, int &y) const
{
	x = m_hunter->getX();
	y = m_hunter->getY();
}



////////////   B A R R E L L O F O I L    /////////////////

BarrelOfOil::BarrelOfOil(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_BARREL,startX,startY)
{
	setInvisible();
}

BarrelOfOil::~BarrelOfOil()
{}

void BarrelOfOil::doSomething()
{
	if (!isAlive())
		return;
	if (!isActorVisible())
	{
		int x = 0;
		int y = 0;
		getFrackManPosition(x, y);
		for (int k = 0; k <= 4; k++)
		{
			if (getX() == x + k)
			{
				for (int j = 0; j <= 4; j++)
				{
					if (getY() == y + j)
					{	makeVisible(); return;	}
				}
			}
		}	
		for (int k = 0; k <= 3; k++)
		{
			if (getX() == x + k)
			{
				for (int j = 0; j <= 3; j++)
				{
					if (getY() == y + j)
					{
						kill();
						StudentWorld* world = getWorld();
						world->playSound(SOUND_FOUND_OIL);
						world->increaseScore(1000);
						world->decBarrels();
					}
				}
			}
		}
	}
}

//////////////   G O L D N U G G E T     //////////////////

GoldNugget::GoldNugget(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_GOLD, startX, startY)
{}

GoldNugget::~GoldNugget()
{}

void GoldNugget::doSomething()
{}

////////////////   S O N A R K I T   //////////////////////

SonarKit::SonarKit(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_SONAR, startX, startY)
{}

SonarKit::~SonarKit()
{}

void SonarKit::doSomething()
{

}

///////////////   W A T E R P O O L    ////////////////////

WaterPool::WaterPool(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_WATER_POOL, startX, startY)
{}

WaterPool::~WaterPool()
{}

void WaterPool::doSomething()
{

}
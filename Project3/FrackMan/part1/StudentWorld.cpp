#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
	m_frackman = new FrackMan(this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////      I N I T      /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//////////////// init implementation ///////////////////////

int StudentWorld::init()		// create's current level's oil field
{
	// populates oil field with: Dirt, Boulders, Barrels of Oil, and Gold Nuggets
	
	fillOilFieldWithDirt(m_dirt);
	
	m_totBarrels = 2 + getLevel();
	if (m_barrels > 20) m_barrels = 20;
	addBarrelsOfOil(m_barrels);
	m_barrels = m_totBarrels;

	int numBoulders = getLevel() / 2 + 2;
	if (numBoulders > 6) numBoulders = 6;
//	addBoulders(numBoulders);

	int numNuggets = 5 - getLevel() / 2;
	if (numNuggets < 2) numNuggets = 2;
//	addGoldNuggets(numNuggets);

	
	return GWSTATUS_CONTINUE_GAME;

}

////////////// function implementation /////////////////////

void StudentWorld::fillOilFieldWithDirt(Dirt* arr[][VIEW_WIDTH])
{
	// fill rest of the array with Dirt
	for (int row = 0; row < VIEW_HEIGHT; row++)
	{
		for (int col = VIEW_WIDTH - 1; col >= 0; col--)
		{
			int y = (VIEW_HEIGHT) - (row + 1);
			int x = col;
			
			if (x < 0 || y < 0)
				m_dirt[row][col] = nullptr;
			else if (x >= 30 && x <= 33 && y > 3)
				m_dirt[row][col] = nullptr;
			else if (y >= 60)
				m_dirt[row][col] = nullptr;
			else 
				m_dirt[row][col] = new Dirt(this, x, y);
		}
	}
}

void StudentWorld::removeDirtFrom(int x, int y)
{
	int row = (VIEW_HEIGHT)-(y + 1);
	int col = x;

	if (m_dirt[row][col] == nullptr)
		return;

	delete m_dirt[row][col];
	m_dirt[row][col] = nullptr;
}

bool StudentWorld::isDirtInField(int x, int y) const
{
	int row = (VIEW_HEIGHT)-(y + 1);
	int col = x;

	if (m_dirt[row][col] == nullptr)
		return false;

	return true;
}

bool StudentWorld::isBoulder(int x, int y) const
{
	return false;
}

void StudentWorld::addBarrelsOfOil(int numBarrels)
{
	for (int k = 1; k <= numBarrels; k++)
	{
		int x = 20 + k;
		int y = 40 + k;
		// get startX and startY
		// make sure there's enough of a radius between objects
		// make sure you're not in the shaft!!!
		if (!isObject(x, y))
		{
			BarrelOfOil* barrel = new BarrelOfOil(m_frackman, this, x, y);
			m_objects.push_back(barrel);
		}
	}

}

bool StudentWorld::isObject(int x, int y) const
{
	if (x >= 30 && x <= 33)
		return false;
	if (y > 59)
		return false;
	for (int k = 0; k < m_objects.size(); k++)
	{
		int objectX, objectY;
		objectX = m_objects[k]->getXValue();
		objectY = m_objects[k]->getYValue();
		if (objectX == x && objectY == y)
			return true;
	}
	return false;
}

void StudentWorld::decBarrels()
{
	m_barrels--;
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////      M O V E      /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//////////////// function implementation //////////////////////
 
string StudentWorld::setDisplayText() const
{
	int score = getScore();  // make sure is 8 digits long
	int level = getLevel();
	int lives = getLives();
	int health = (((m_frackman->getHealth()) * 100) /10) ;
	int squirts = m_frackman->getGun()->getSquirts();
	int gold = 0;												// do later
	int sonar = 0;												// do later
	int barrelsLeft = m_totBarrels - m_barrels;

	return "Scr: " + to_string(score) + " Lvl: " + to_string(level) +
		" Lives: " + to_string(lives) + " Hlth: " + to_string(health) + "%" +
		" Water: " + to_string(squirts) + " Gld: " + to_string(gold) +
		" Sonar: " + to_string(sonar) + " Oil Left: " + to_string(barrelsLeft);
}

//////////////// move implementation ///////////////////////

int StudentWorld::move() // run a single tick of the game
{
	// ask game's actors to try and do something
	// deleting actors
	setGameStatText(setDisplayText());
	m_frackman->doSomething();
	for (int k = 0; k < m_objects.size(); k++)
	{
		m_objects[k]->doSomething();
	}
	if (m_barrels == 0)
		m_level++;
	if (!m_frackman->isAlive())
		return GWSTATUS_PLAYER_DIED;
	//return GWSTATUS_FINISHED_LEVEL;
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////     C L E A N U P     ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//////////////// function declaration //////////////////////



////////////// cleanup implementation //////////////////////

void StudentWorld::cleanUp()	// player loses a life or successfully completes a level
{
	// frees all actors
	// includes actors created in init() or modified in move()

	// delete frackman
	delete m_frackman;

/*	// delete dirt
	for (int row = 0; row > 0; row--)
	{
		for (int col = 0; col < VIEW_WIDTH; col++)
		{
			int y = VIEW_HEIGHT - row;
			int x = col;

			delete m_dirt[row][col];
			m_dirt[row][col] = nullptr;
		}
	}*/

/*	// delete objects
	for (int k = 0; k < m_objects.size(); k++)
	{
		delete m_objects[k];
	}
	while (!m_objects.empty())
	{
		m_objects.pop_back();
	}*/

}

////////////// function implementation /////////////////////

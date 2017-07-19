#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
	m_frackman = new FrackMan(this);
	m_addNewProtester = 0;
	m_ticks = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////      I N I T      /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//////////////// init implementation ///////////////////////

int StudentWorld::init()		// create's current level's oil field
{
	// populates oil field with: Dirt, Boulders, Barrels of Oil, and Gold Nuggets
	
	int num_barrels = 2 + getLevel();
	if (num_barrels > 20) num_barrels = 20;
	addObject('O', num_barrels);
	m_barrels = num_barrels;

	int numBoulders = getLevel() / 2 + 2;
	if (numBoulders > 6) numBoulders = 6;
	addObject('B', numBoulders);

	int numNuggets = 5 - getLevel() / 2;
	if (numNuggets < 2) numNuggets = 2;
	addObject('N', numNuggets);

	fillOilFieldWithDirt(m_dirt);
	
	return GWSTATUS_CONTINUE_GAME;
}

////////////// function implementation /////////////////////

void StudentWorld::fillOilFieldWithDirt(Dirt* arr[][VIEW_WIDTH])
{
	// fill array with Dirt
	for (int row = 0; row < VIEW_HEIGHT; row++)
	{
		for (int col = VIEW_WIDTH - 1; col >= 0; col--)
		{
			int y = (VIEW_HEIGHT) - (row + 1);
			int x = col;

			if (x >= 30 && x <= 33 && y > 3)
				m_dirt[row][col] = nullptr;
			else if (y >= 60)
				m_dirt[row][col] = nullptr;
			else if (!BoulderpointIsFree(x, y))
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

bool StudentWorld::isShaft(int x, int y) const
{
	if (x >= 30 && x <= 33 && y > 3)
		return true;
	else if (y >= 60)
		return true;
	else return false;
}

void StudentWorld::annoyFrackman()
{
	m_frackman->annoy();
}


void StudentWorld::addObject(char objectType, int numObjects)
{
	for (int k = 0; k < numObjects; k++)
	{
		int xPlacement = rand() % 61;
		int yPlacement = rand() % 61;

		if (isValidSpot(xPlacement, yPlacement))
		{
			switch (objectType)
			{
			case 'O':
				m_objects.push_back(new BarrelOfOil(m_frackman, this, xPlacement, yPlacement));
				break;
			case 'B':
			{
				Boulder* boulder = new Boulder(this, xPlacement, yPlacement);
				m_objects.push_back(boulder);
			}
				break;
			case 'N':
				m_objects.push_back(new GoldNugget(m_frackman, this, xPlacement, yPlacement, false));
				break;
			}
		}
		else k--;
	}
}

void StudentWorld::addObject(Actor* object)
{
	m_objects.push_back(object);
}

bool StudentWorld::isValidSpot(int x, int y) const
{
	if (x >= 27 && x <= 33)
		return false;
	if (y > 56)
		return false;
	for (int k = 0; k < m_objects.size(); k++)
	{
		int objectX, objectY;
		objectX = m_objects[k]->getX();
		objectY = m_objects[k]->getY();
		double changeInX = static_cast<double>(objectX) - static_cast<double>(x);
		double changeInY = static_cast<double>(objectY) - static_cast<double>(y);
		double distance = sqrt((changeInX*changeInX) + (changeInY*changeInY));
		if (distance <= 6.0)
			return false;
	}
	return true;
}

void StudentWorld::decBarrels()
{
	m_barrels--;
}

bool StudentWorld::BoulderpointIsFree(int x, int y) const
{
	for (int k = 0; k < m_objects.size(); k++)
	{
		if (m_objects[k]->isBoulder())
		{
			int boulderX = m_objects[k]->getX();
			int boulderY = m_objects[k]->getY();
			for (int m = 0; m <= 3; m++)
			{
				for (int j = 0; j <= 3; j++)
				{
					if (boulderX + m == x && boulderY + j == y)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool StudentWorld::ProtesterpointIsFree(int x, int y) const
{
	for (int k = 0; k < m_objects.size(); k++)
	{
		if (m_objects[k]->isHuman())
		{
			int protesterX = m_objects[k]->getX();
			int protesterY = m_objects[k]->getY();
			for (int m = 0; m <= 3; m++)
			{
				for (int j = 0; j <= 3; j++)
				{
					if (protesterX + m == x && protesterY + j == y)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool StudentWorld::annoyProtester(int x, int y)
{
	bool inRange = false;
	int overlapX = 0;
	int overlapY = 0;

	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (!ProtesterpointIsFree(x + k, y + j))
			{
				overlapX = x + k;
				overlapY = y + j;
				inRange = true;
				break;
			}
		}
	}
	if (inRange)
	{
		for (int ob = 0; ob < m_objects.size(); ob++)
		{
			if (m_objects[ob]->isHuman())
			{
				double changeInX = static_cast<double>(overlapX) - static_cast<double>(m_objects[ob]->getX());
				double changeInY = static_cast<double>(overlapY) - static_cast<double>(m_objects[ob]->getY());
				double distance = sqrt((changeInX*changeInX) + (changeInY*changeInY));

				if (distance <= 3)
				{
					for (int k = 0; k < m_objects.size(); k++)
					{
						if (m_objects[k]->isSquirt())
							m_objects[k]->makeVisible();
					}
					m_objects[ob]->squirtProtester();
					return true;
				}
			}
		}
	}
	return false;
}

void StudentWorld::dropNugget(int x, int y)
{
	if (m_frackman->noGoldNuggets()) return;
	GoldNugget* nugget = new GoldNugget(m_frackman, this, x, y, true);
	nugget->makeVisible();
	m_objects.push_back(nugget);
	m_frackman->decGoldNuggets();
}

bool StudentWorld::intersectProtester(int x, int y)
{
	for (int k = 0; k < m_objects.size(); k++)
	{
		if (m_objects[k]->isHuman())
		{
			double changeInX = static_cast<double>(x) - static_cast<double>(m_objects[k]->getX());
			double changeInY = static_cast<double>(y) - static_cast<double>(m_objects[k]->getY());
			double distance = sqrt((changeInX*changeInX) + (changeInY*changeInY));

			if (distance <= 3.0)
			{
				if (!m_objects[k]->isLeaving())
					m_objects[k]->instersectNugget();
				return true;
			}
		}
	}
	return false;
}

bool StudentWorld::frackmanIsAlive() const
{
	return m_frackman->isAlive();
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////      M O V E      /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//////////////// function implementation //////////////////////
 
string StudentWorld::setDisplayText() const
{
	int score = getScore();  // make sure is 8 digits long
	string score_type = "000000";
	if (score < 10)
		score_type = "00000" + to_string(score);
	else if (score < 100)
		score_type = "0000" + to_string(score);
	else if (score < 1000)
		score_type = "000" + to_string(score);
	else if (score < 10000)
		score_type = "00" + to_string(score);
	else if (score < 100000)
		score_type = "0" + to_string(score);
	else if (score < 1000000);
	else score_type = to_string(score);
	string level = to_string(getLevel());
	if (getLevel() < 10)
		level = " " + level;
	int lives = getLives();
	string health = to_string(((m_frackman->getHitPoints()) * 100) / 10) + "%";
	if (m_frackman->getHitPoints() < 10)
		health = " " + health;
	string squirts = to_string(m_frackman->getSquirts());
	if (m_frackman->getSquirts() < 10)
		squirts = " " + squirts;
	string gold = to_string(m_frackman->getNuggets());
	if (m_frackman->getNuggets() < 10)
		gold = " " + gold;
	string sonar = to_string(m_frackman->getSonar());
	if (m_frackman->getSonar() < 10)
		sonar = " " + sonar;
	string barrelsLeft = to_string(m_barrels);
	if (m_barrels < 10)
		barrelsLeft = " " + barrelsLeft;

	return "Scr: " + score_type + "  Lvl:" + level +
		"  Lives: " + to_string(lives) + "  Hlth: " + health +
		"  Wtr: " + squirts + "  Gld: " + gold +
		"  Sonar: " + sonar + "  Oil Left: " + barrelsLeft;
}

//////////////// move implementation ///////////////////////

int StudentWorld::move() // run a single tick of the game
{
	m_ticks++;
	
	// ask game's actors to try and do something
	// deleting actors
	setGameStatText(setDisplayText());
	m_frackman->doSomething();
	
	// have every object do something
	for (int k = 0; k < m_objects.size(); k++)
	{
		m_objects[k]->doSomething();
		if (!m_objects[k]->isAlive())
		{
			delete m_objects[k];
			m_objects.erase(m_objects.begin() + k);
		}
	}

	// add protesters and goodies when appropriate
	int G = getLevel() * 25 + 300;
	if (rand() % G + 1 == 1)
	{
		addWaterPool();
	}

	m_addNewProtester++;
	int T = 200 - getLevel();
	if (T < 25) T = 25;

	if (m_addNewProtester == T || m_ticks == 1)
	{
		addProtester();
		m_addNewProtester = 0;
	}

	killHumanActorIfBoulderIsFalling(m_frackman);
	for (int j = 0; j < m_objects.size(); j++)
	{
		if (m_objects[j]->isHuman())
		{
			killHumanActorIfBoulderIsFalling(m_objects[j]);
		}
	}

	removeDeadGameObjects();

	if (!m_frackman->isAlive())
	{
		playSound(SOUND_PLAYER_GIVE_UP);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	if (m_barrels == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

}

void StudentWorld::addWaterPool()
{
	int probability = rand() % 5 + 1;
	if (probability == 1)
		m_objects.push_back(new SonarKit(m_frackman, this, 0, 60));
	else
	{
		int openSpots = 0;
		for (;;)
		{
			openSpots = 0;
			int freeX = rand() % 61;
			int freeY = rand() % 61;
			for (int k = 0; k < 4; k++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (!isDirtInField(freeX + k, freeY + j))
						openSpots++;
				}
			}
			if (openSpots >= 16)
			{
				m_objects.push_back(new WaterPool(m_frackman, this, freeX, freeY));
				break;
			}
			else continue;
		}
	}
}


void StudentWorld::addProtester()
{
	int numProtesters = 0;
	int P = 2 + getLevel() * 1.5;
	if (P > 15) P = 15;
	for (int j = 0; j < m_objects.size(); j++)
	{
		if (m_objects[j]->isHuman())
			numProtesters++;
	}
	if (numProtesters < P)
	{
		int probabilityOfHardcore = getLevel() * 10 + 30;
		if (probabilityOfHardcore > 90) probabilityOfHardcore = 90;
		
		int probOfH = rand() % 100;
		if (probOfH <= probabilityOfHardcore)
			m_objects.push_back(new HardcoreProtester(this));
		else
			m_objects.push_back(new Protester(this));
	}
}

void StudentWorld::removeDeadGameObjects()
{
	for (int k = 0; k < m_objects.size(); k++)
	{
		if (!m_objects[k]->isAlive())
		{
			m_objects[k]->setInvisible();
			delete m_objects[k];
			m_objects[k] = nullptr;
		}
	}
}

void StudentWorld::killHumanActorIfBoulderIsFalling(Actor* person)
{
	for (int k = 0; k < m_objects.size(); k++)
	{
		if (m_objects[k]->isBoulder())
		{
			if (m_objects[k]->isFalling())
			{
				double changeX = m_objects[k]->getX() - person->getX();
				double changeY = m_objects[k]->getY() - person->getY();
				double distance = sqrt((changeX*changeX) + (changeY*changeY));
				if (distance <= 3)
				{
					if (!person->isLeaving())
						person->sendHome();
				}
			}
		}
	}
}

bool StudentWorld::isSonarKitEmpty() const
{
	if (m_frackman->getSonar() == 0)
		return true;
	else return false;
}

void StudentWorld::getFrackmanPosition (int &x, int &y)
{
	x = m_frackman->getX();
	y = m_frackman->getY();
}

bool StudentWorld::isDirtOrBoulderInField(int x, int y) const
{
	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (isDirtInField(x + k, y + j) || !BoulderpointIsFree(x + k, y + j))
				return true;
			else if (x < 0 || x + k > 64 || y < 0 || y + j >= 64)
				return true;
		}
	}
	return false;
	
}

void StudentWorld::useSonar()
{
	for (int k = 0; k < m_objects.size(); k++)
	{
		double changeInX = static_cast<double>(m_frackman->getX()) - static_cast<double>(m_objects[k]->getX());
		double changeInY = static_cast<double>(m_frackman->getY()) - static_cast<double>(m_objects[k]->getY());
		double distance = sqrt((changeInX*changeInX) + (changeInY*changeInY));

		if (distance < 12)
			m_objects[k]->makeVisible();
	}
	playSound(SOUND_SONAR);
	m_frackman->decSonar();
}

void StudentWorld::incSonarKit()
{
	m_frackman->incSonar();
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

	m_ticks = 0;

	// delete dirt
	for (int row = 0; row < VIEW_HEIGHT; row++)
	{
		for (int col = VIEW_WIDTH - 1; col >= 0; col--)
		{
			int y = (VIEW_HEIGHT)-(row + 1);
			int x = col;

			delete m_dirt[row][col];
			m_dirt[row][col] = nullptr;
		}
	}

	// delete objects
	for (int k = 0; k < m_objects.size(); k++)
	{
		delete m_objects[k];
		m_objects[k] = nullptr;
	}
	while (!m_objects.empty())
	{
		m_objects.pop_back();
	}

	// delete frackman
	delete m_frackman;
	m_frackman = new FrackMan(this);
}

////////////// function implementation /////////////////////

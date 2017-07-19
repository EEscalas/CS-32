#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include <stack>
#include <queue>
#include <string>


///////////////////////////////////////////////////////////
/////////////////     A C T O R      //////////////////////
///////////////////////////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir, float size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth)
{
	m_tickCount = 0;
	makeVisible();
	m_studentWorld = world;
	m_alive = true;
}

Actor::~Actor()
{

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

void Actor::setTicksToZero()
{
	m_tickCount = 0;
}


bool Actor::isDirtBelow(int x, int y) const
{
	for (int k = 0; k < 4; k++)
	{
		if (m_studentWorld->isDirtInField(x + k, y))
		{
			return true;
		}
	}
	return false;
}

int Actor::getTickCount() const
{
	return m_tickCount;
}

void Actor::incTicks()
{
	m_tickCount++;
}

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
	m_stable = true;
	m_falling = false;
}

Boulder::~Boulder()
{

}

void Boulder::doSomething()
{
	if (!isAlive())
		return;
	int boulderX = getX();
	int boulderY = getY();
	StudentWorld* world = getWorld();

	if (!isDirtBelow(boulderX, boulderY - 1) && m_stable == true)
	{
		m_stable = false;
		return;
	}

	if (m_stable == false)
		incTicks();

	if (getTickCount() >= 30 && (!isDirtBelow(boulderX, boulderY - 1)))
	{
		moveTo(boulderX, boulderY - 1);
		world->playSound(SOUND_FALLING_ROCK);
		m_falling = true;
		return;
	}
	else if (m_stable == false && (!world->BoulderpointIsFree(boulderX, boulderY - 1) || isDirtBelow(boulderX, boulderY - 1)))
	{
		kill();
	}
}

bool Boulder::isFalling() const
{
	return m_falling;
}

/////////////   S Q U I R T G U N    //////////////////////

SquirtGun::SquirtGun(StudentWorld* world, int x, int y, Direction dir)
	:Actor(IID_WATER_SPURT, x, y, world, dir, 1, 1)
{
	m_travelDistance = 4;
}


void SquirtGun::doSomething()
{
	if (!isAlive())
		return;

	m_travelDistance--;

	int squirtX = getX();
	int squirtY = getY();

	if (getWorld()->annoyProtester(squirtX, squirtY))
	{
		kill();
		return;
	}

	if (m_travelDistance < 0)
	{
		kill();
		return;
	}
	
	switch(getDirection())
	{
	case right:
		squirtX++;
		break;
	case left:
		squirtX--;
		break;
	case up:
		squirtY++;
		break;
	case down:
		squirtY--;
		break;
	}
	for (int k = 0; k < 4; k++)
		for (int j = 0; j < 4; j++)
			if (getWorld()->isDirtInField(squirtX + k, squirtY + j) || !getWorld()->BoulderpointIsFree(squirtX + k, squirtY + k))
				return;

	moveTo(squirtX, squirtY);
	
}

///////////////////////////////////////////////////////////
//////////////   H U M A N A C T O R    ///////////////////
///////////////////////////////////////////////////////////

HumanActor::HumanActor(StudentWorld* world, int imageID, int startX, int startY, Direction dir)
	:Actor(imageID, startX, startY, world, dir)
{
	m_nuggets = 0;
}

HumanActor::~HumanActor()
{}

void HumanActor::decHitPoints()
{
	m_hitPoints -= 2;
}

int HumanActor::getHitPoints() const
{
	return m_hitPoints;
}

int HumanActor::getNuggets() const
{
	return m_nuggets;
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

void HumanActor::incGoldNuggets()
{
	m_nuggets++;
}

void HumanActor::decGoldNuggets()
{
	m_nuggets--;
}

bool HumanActor::noGoldNuggets() const
{
	return m_nuggets == 0;
}

void HumanActor::kill()
{
	m_hitPoints = 0;
	Actor::kill();
}

/////////////      F R A C K M A N       /////////////////

FrackMan::FrackMan(StudentWorld* world)
	:HumanActor(world, IID_PLAYER, 30, 60, right)
{
	setHitPointsTo(10);
	m_sonar = 0;
	m_squirts = 5;
}

FrackMan::~FrackMan()
{
	
}

void FrackMan::doSomething()
{
	if (!isAlive())
		return;
	
	if (zeroHitPoints())			// determining whether or not he is dead
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
				if (x - 1 >= 0 && !doesFrackmanOverlap(x - 1, y))
					moveTo(x - 1, y);
				else  if (!doesFrackmanOverlap(x - 1, y))
					moveTo(x, y);
			}
			else setDirection(left);
			break;
		case KEY_PRESS_RIGHT:
			if (dir == right)
			{
				if (x + 1 <= 60 && !doesFrackmanOverlap(x + 1, y))
					moveTo(x + 1, y);
				else if(!doesFrackmanOverlap(x + 1, y))
					moveTo(x, y);
			}
			else setDirection(right);
			break;
		case KEY_PRESS_UP:
			if (dir == up)
			{
				if (y + 1 <= 60 && !doesFrackmanOverlap(x, y + 1))
					moveTo(x, y + 1);
				else if (!doesFrackmanOverlap(x, y + 1))
					moveTo(x, y);
			}
			else setDirection(up);
			break;
		case KEY_PRESS_DOWN:
			if (dir == down)
			{
				if (y - 1 >= 0 && !doesFrackmanOverlap(x, y - 1))
					moveTo(x, y - 1);
				else if (!doesFrackmanOverlap(x, y - 1))
					moveTo(x, y);
			}
			else setDirection(down);
			break;
		case KEY_PRESS_SPACE:
			if (getSquirts() > 0)
				squirt();
			break;
		case KEY_PRESS_ESCAPE:
			kill();
			break;
		case KEY_PRESS_TAB:
			world->dropNugget(x, y);
			break;
		case 'z':
		case 'Z':
			if (!world->isSonarKitEmpty())
				world->useSonar();
			break;
		}
	}
}

bool FrackMan::doesFrackmanOverlap(int x, int y)
{
	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			StudentWorld* world = getWorld();
			if (!world->BoulderpointIsFree(x + k, y + j))
			{
				return true;
			}
		}
	}
	return false;
}

void FrackMan::annoy()
{
	decHitPoints();
	if (zeroHitPoints())
		kill();
}

int FrackMan::getSonar() const
{
	return m_sonar;
}

void FrackMan::decSonar()
{
	m_sonar--;
}

void FrackMan::incSonar()
{
	m_sonar++;
}

void FrackMan::incSquirt()
{
	m_squirts += 5;
}

int FrackMan::getSquirts() const
{
	return m_squirts;
}

void FrackMan::squirt()
{
	Direction dir = getDirection();
	int squirtX = getX();
	int squirtY = getY();
	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
	m_squirts--;
	switch (dir)
	{
	case right:
		squirtX += 3;
		break;
	case left:
		squirtX -= 3;
		break;
	case up:
		squirtY += 3;
		break;
	case down:
		squirtY -= 3;
		break;
	}
	if (squirtX > 60 || squirtY > 60)
		return;
	else
	{
		for (int k = 0; k < 4; k++)
			for (int j = 0; j < 4; j++)
				if (getWorld()->isDirtInField(squirtX + k, squirtY + j) || !getWorld()->BoulderpointIsFree(squirtX + k, squirtY + k))
					return;
	}
	gun = new SquirtGun(getWorld(), squirtX, squirtY, dir);
	gun->makeVisible();
	getWorld()->addObject(gun);
}

////////////////    P R O T E S T E R    //////////////////

Protester::Protester(StudentWorld* world, int imageID)
	:HumanActor(world, imageID, 60, 60, left)
{
	makeVisible();
	setHitPointsTo(5);
	m_shout = false;
	m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
	m_nonRestingTicks = 0;
	m_stunned = 0;
	m_goNextTick = false;
	m_leaving = false;
	setTicksToZero();
	incTicks();
	m_timeSincePerpTurn = 0;
}

Protester::~Protester()
{}

void Protester::doSomething()
{
	if (!isAlive())
		return;

	int ticksToWaitBetweenMoves = 3 - getWorld()->getLevel() / 4;
	if (ticksToWaitBetweenMoves < 0)
		ticksToWaitBetweenMoves = 0;

	incTicks();

	if (m_goNextTick)
	{
		setTicksToZero();
		m_goNextTick = false;
	}

	if (getTickCount() < ticksToWaitBetweenMoves && getTickCount() != 0)
		return;

	setTicksToZero();
	
	if (m_stunned > 0)
	{
		m_stunned--;
		return;
	}

	m_timeSincePerpTurn++;
	m_nonRestingTicks++;

	if (m_leaving)
	{
		if (getWorld()->isShaft(getX(), getY()))
			leaveFromShaft();
		else
			leaveField();
		return;
	}

	if (m_shout && m_nonRestingTicks < 15)
		return;

	m_shout = false;
	
	if (frackmanWithinRange())
	{
		shout();
		return;
	}
	else if (inLineOfSight())
	{
		moveTowardsFrackman();
		m_numSquaresToMoveInCurrentDirection = 0;
		return;
	}

	if (m_numSquaresToMoveInCurrentDirection == 0)
	{
		changeDir();
		return;
	}

	takeStepForward();

	if (m_perpendicularTurnCount <= 0 && m_nonRestingTicks >= 200)						// needs work
		changeToPerpDir();

	setTicksToZero();
}

void Protester::changeToPerpDir()
{
	Direction viabledir = none;
	switch (getDirection())
	{
	case right:
	case left:
		if (!getWorld()->isDirtOrBoulderInField(getX(), getY() + 1))
			viabledir = up;
		else if (!getWorld()->isDirtOrBoulderInField(getX(), getY() - 1))
			viabledir = down;
		break;
	case up:
	case down:
		if (!getWorld()->isDirtOrBoulderInField(getX() + 1, getY()))
			viabledir = right;
		if (!getWorld()->isDirtOrBoulderInField(getX() - 1, getY()))
			viabledir = left;
		break;
	}
	if (viabledir == none)
	{
		m_perpendicularTurnCount = -1;
		return;
	}
	else 
	{
		setDirection(viabledir);
		m_timeSincePerpTurn = 0;
		m_perpendicularTurnCount = 0;
		m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
	}
}

void Protester::takeStepForward()	// step 8
{
	int x = getX();
	int y = getY();

	switch (getDirection())
	{
	case right:
		x++;
		break;
	case left:
		x--;
		break;
	case up:
		y++;
		break;
	case down:
		y--;
		break;
	}

	if (getWorld()->isDirtOrBoulderInField(x, y))
	{
		m_numSquaresToMoveInCurrentDirection = 0;
		return;
	}
	else moveTo(x,y);
	m_numSquaresToMoveInCurrentDirection--;
	
	point p;
	p.m_x = x;
	p.m_y = y;
	m_leavingPath.push(p);
}

bool Protester::frackmanWithinRange() const
{
	int frackX = 0;
	int frackY = 0;
	
	getWorld()->getFrackmanPosition(frackX, frackY);

	double changeInX = static_cast<double>(frackX) - static_cast<double>(getX());
	double changeInY = static_cast<double>(frackY) - static_cast<double>(getY());
	double distance = sqrt((changeInX*changeInX) + (changeInY*changeInY));

	if (distance > 4)
		return false;

	switch (getDirection())
	{
	case right:
		return frackX > getX();
		break;
	case left:
		return frackX < getX();
		break;
	case up:
		return frackY > getY();
		break;
	case down:
		return frackY < getY();
		break;
	}

	return false;
}



void Protester::moveTowardsFrackman()
{
	int x = getX();
	int y = getY();
	

	
	switch (getDirection())
	{
	case right:
		x++;
		break;
	case left:
		x--;
		break;
	case up:
		y++;
		break;
	case down:
		y--;
		break;
	}

	if (getWorld()->isDirtOrBoulderInField(x, y))
		return;
	else moveTo(x, y);

	point p;
	p.m_x = x;
	p.m_y = y;

	m_leavingPath.push(p);


}

bool Protester::inLineOfSight()
{
	int frackX = 0;
	int frackY = 0; 
	
	getWorld()->getFrackmanPosition(frackX, frackY);

	if (getX() != frackX && getY() != frackY)
		return false;

	Direction dirTowardsFrackman = left;
	
	if (getX() == frackX)
	{
		dirTowardsFrackman = up;
		if (getY() > frackY)
			dirTowardsFrackman = down;
		
		int distanceBetween = abs(frackY - getY());

		for (int i = 0; i <= distanceBetween; i++)
		{
			if (dirTowardsFrackman == up)
			{
				if (getWorld()->isDirtOrBoulderInField(getX(), getY() + i))
					return false;
			}
			else if (dirTowardsFrackman == down)
			{
				if (getWorld()->isDirtOrBoulderInField(getX(), getY() - i))
					return false;
			}
		}
	}
	else if (getY() == frackY)
	{
		dirTowardsFrackman = left;
		if (getX() < frackX)
			dirTowardsFrackman = right;
		
		int distanceBetween = abs(frackX - getX());

		for (int i = 0; i <= distanceBetween; i++)
		{
			if (dirTowardsFrackman == right)
			{
				if (getWorld()->isDirtOrBoulderInField(getX() + i, getY()))
					return false;
			}
			else if (dirTowardsFrackman == left)
			{
				if (getWorld()->isDirtOrBoulderInField(getX() - i, getY()))
					return false;
			}
		}
	}
	setDirection(dirTowardsFrackman);
	return true;
}

void Protester::shout()
{
	getWorld()->playSound(SOUND_PROTESTER_YELL);
	getWorld()->annoyFrackman();
	m_nonRestingTicks = 0;
	m_shout = true;
}

bool Protester::isActive() const
{
	return !m_leaving;
}

void Protester::randDir(Direction& dir)
{
	int direction = rand() % 4;
	if (direction == 0)
		dir = right;
	else if (direction == 1)
		dir = left;
	else if (direction == 2)
		dir = up;
	else if (direction == 3)
		dir = down;
}

bool Protester::canMove(Direction dir)
{
	point p;
	int x = getX();
	int y = getY();
	switch (dir)
	{
	case right:
		x++;
		break;
	case left:
		x--;
		break;
	case up:
		y++;
		break;
	case down:
		y--;
		break;
	}
	if (!getWorld()->isDirtOrBoulderInField(x, y))
		return true;
	else return false;
}

void Protester::changeDir()			// changes direction if the protester should no longer take steps in current direction
{

	Direction newDirection = down;
	while (!canMove(newDirection))
		randDir(newDirection);
	if (perpendicularTurn(newDirection))
		m_perpendicularTurnCount++;
	setDirection(newDirection);

	m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
}

bool Protester::perpendicularTurn(const Direction& newD)
{
	switch (newD)
	{
	case right:
	case left:
		if (getDirection() == down || getDirection() == up)
			return true;
		break;
	case up:
	case down:
		if (getDirection() == right || getDirection() == left)
			return true;
		break;
	}
	return false;
}

void Protester::leaveField()
{	
	int x = getX();
	int y = getY();

	point p;
	p = m_leavingPath.top();
	m_leavingPath.pop();
	if (p.m_x == x)
	{
		if (p.m_y < y)
			setDirection(down);
		else if (p.m_y > y)
			setDirection(up);
	}
	else if (p.m_y == y)
	{
		if (p.m_x < x)
			setDirection(left);
		else if (p.m_x > x)
			setDirection(right);
	}
	

	moveTo(p.m_x, p.m_y);

}

void Protester::leaveFromShaft()
{	
	int x = getX();
	int y = getY() ;
	if (y < 60)
	{
		setDirection(up);
		moveTo(x, y + 1);
	}
	else
	{
		setDirection(right);
		moveTo(x + 1, y);
	}
	
	if (getX() == 60 && getY() == 60)
		kill();
}
void Protester::squirtProtester()
{
	m_squirted = true;
	annoy();
}

void Protester::annoy()
{
	if (m_leaving == true)
		return;
	decHitPoints();
	if (zeroHitPoints())
	{
		m_leaving = true;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_goNextTick = true;
		if (m_squirted)
			getWorld()->increaseScore(100);
		else getWorld()->increaseScore(500);
		return;
	}
	else
	{
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		int N = 100 - getWorld()->getLevel() * 10;
		if (N < 50) N = 50;
		setStunned(N);
	}


}

void Protester::sendHome()
{
	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	m_leaving = true;
}


void Protester::bribed()
{
	m_leaving = true;
}

void Protester::instersectNugget()
{
	if (!isLeaving()) 
	{
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		getWorld()->increaseScore(25);
		bribed();
	}
}

void Protester::setStunned(int stunAmt)
{
	m_stunned = stunAmt;
}

/////////   H A R D C O R E P R O T E S T E R    //////////

HardcoreProtester::HardcoreProtester(StudentWorld* world)
	:Protester(world, IID_HARD_CORE_PROTESTER)
{
	setHitPointsTo(20);
}

HardcoreProtester::~HardcoreProtester()
{

}

void HardcoreProtester::doSomething()
{
	Protester::doSomething();

}

void HardcoreProtester::bribed()
{
	int N = 100 - getWorld()->getLevel() * 10;
	if (N < 50) N = 50;
	setStunned(N);
}

/*bool HardcoreProtester::inLineOfSight()
{
	// in line of sight for hardcore protester means far enough for him to find
	int nextMoveX = 0;
	int nextMoveY = 0;

	int M = 16 + getWorld()->getLevel() * 2;
	if (movesToFrackman(nextMoveX, nextMoveY) <= M)
		return true;
	else return false;
}

int HardcoreProtester::movesToFrackman(int& startX, int& startY)
{
	int frackX = 0;
	int frackY = 0;
	getWorld()->getFrackmanPosition(frackX, frackY);
	queue<point> points;
	point p;					// starting point
	p.m_x = getX();
	p.m_y = getY();
	m_availableSpace[p.m_x][p.m_y] = '#';
	points.push(p);
	int moves = 0;
	while (m_availableSpace[frackX][frackY] != '#')
	{
		if (points.empty())
			return 16 + getWorld()->getLevel() * 2 + 1;
		p = points.front();
		points.pop();

		if (!getWorld()->isDirtOrBoulderInField(p.m_x, p.m_y - 1))
			p.m_y--;
		else if (!getWorld()->isDirtOrBoulderInField(p.m_x + 1, p.m_y))
			p.m_x++;
		else if (!getWorld()->isDirtOrBoulderInField(p.m_x - 1, p.m_y))
			p.m_x--;
		else if (!getWorld()->isDirtOrBoulderInField(p.m_x, p.m_y + 1))
			p.m_y++;
		
		if (!getWorld()->isDirtOrBoulderInField(p.m_x, p.m_y))
		{
			m_availableSpace[p.m_x][p.m_y] = '#';
			points.push(p);
		}
	}
	
	m_availableSpace[frackX][frackY] = '$';
	p.m_x = frackX;
	p.m_y = frackY;
	while (m_availableSpace[getX()][getY()] != '$')
	{
		if (m_availableSpace[p.m_x][p.m_y - 1] == '#')
			p.m_y--;
		else if (m_availableSpace[p.m_x + 1][p.m_y] == '#')
			p.m_x++;
		else if (m_availableSpace[p.m_x - 1][p.m_y] == '#')
			p.m_x--;
		else if (m_availableSpace[p.m_x][p.m_y + 1] == '#')
			p.m_y++;

		m_availableSpace[p.m_x][p.m_y] = '$';
	}

	for (int k = 0; k < VIEW_WIDTH; k++)
	{
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
			if (m_availableSpace[k][j] == '$')
				moves++;
		}
	}

	return moves;

}

void HardcoreProtester::moveTowardsFrackman()
{
	int x = getX();
	int y = getY();

	if (m_availableSpace[x + 1][y] == '$')
	{
		setDirection(right);
		moveTo(x + 1, y);
		return;
	}

	if (m_availableSpace[x - 1][y] == '$')
	{
		setDirection(left);
		moveTo(x - 1, y);
		return;
	}

	if (m_availableSpace[x][y + 1] == '$')
	{
		setDirection(up);
		moveTo(x, y + 1);
		return;
	}
	if (m_availableSpace[x][y - 1] == '$')
	{
		moveTo(x, y - 1);
		return;
	}
}*/

///////////////////////////////////////////////////////////
////////////    G O O D I E A C T O R    //////////////////
///////////////////////////////////////////////////////////

GoodieActor::GoodieActor(FrackMan* hunter, StudentWorld* world, int imageID, int startX, int startY)
	:Actor(imageID, startX, startY, world, right, 1, 2)
{
	setInvisible();
	m_hunter = hunter;
}

GoodieActor::~GoodieActor()
{}

void GoodieActor::getFrackManPosition(int &x, int &y) const
{
	x = m_hunter->getX();
	y = m_hunter->getY();
}

void GoodieActor::find()
{
	if (!getWorld()->frackmanIsAlive())
	{
		removeObject();
		kill();
	}

	int frackX = 0;
	int frackY = 0;
	getFrackManPosition(frackX, frackY);

	// if close enough to object, make object visible
	int goodieX = getX();
	int goodieY = getY();

	double changeInX = static_cast<double>(frackX) - static_cast<double>(goodieX);
	double changeInY = static_cast<double>(frackY) - static_cast<double>(goodieY);
	double distance = sqrt((changeInX*changeInX) + (changeInY*changeInY));

	if (distance <= 4.0)
		makeVisible();
	if (distance <= 3.0)
		{
			removeObject();
			kill();
		}
}

void GoodieActor::incSonarKit()
{
	m_hunter->incSonar();
}

void GoodieActor::incNuggets()
{
	m_hunter->incGoldNuggets();
}

void GoodieActor::addSquirtsToGun()
{
	m_hunter->incSquirt();
}

////////////   B A R R E L L O F O I L    /////////////////

BarrelOfOil::BarrelOfOil(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_BARREL,startX,startY)
{
	setInvisible();
}

BarrelOfOil::~BarrelOfOil()
{}

void BarrelOfOil::removeObject()
{
	getWorld()->decBarrels();
	getWorld()->playSound(SOUND_FOUND_OIL);
	getWorld()->increaseScore(1000);
}

void BarrelOfOil::doSomething() 
{
	if (!isAlive())
		return;
	find();
}



//////////////   G O L D N U G G E T     //////////////////

GoldNugget::GoldNugget(FrackMan* hunter, StudentWorld* world, int startX, int startY, bool dropped)
	:GoodieActor(hunter, world, IID_GOLD, startX, startY)
{
	m_dropped = dropped;
}

GoldNugget::~GoldNugget()
{}

void GoldNugget::doSomething()
{
	if (!isAlive())
		return;
	if (isVisible() && m_dropped)
	{
		incTicks();
		if (getWorld()->intersectProtester(getX(), getY()))
			kill();
		if (getTickCount() == 100)
			kill();
	}
	else find();
}

void GoldNugget::removeObject()
{
	incNuggets();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(10);
}

void GoldNugget::drop()
{
	m_dropped = true;
}

////////////////   S O N A R K I T   //////////////////////

SonarKit::SonarKit(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_SONAR, startX, startY)
{}

SonarKit::~SonarKit()
{}

void SonarKit::doSomething()
{
	if (!isAlive())
		return;
	
	//if time for sonar to come out
		makeVisible();
		incTicks();

	int ticks = 300 - 10 * getWorld()->getLevel();
	if (ticks < 100)
		ticks = 100;
	if (getTickCount() == ticks)
	{
		kill();
	}

	if(isVisible())
		find();
}

void SonarKit::removeObject()
{
	incSonarKit();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(75);
}

///////////////   W A T E R P O O L    ////////////////////

WaterPool::WaterPool(FrackMan* hunter, StudentWorld* world, int startX, int startY)
	:GoodieActor(hunter, world, IID_WATER_POOL, startX, startY)
{
	makeVisible();
}

WaterPool::~WaterPool()
{}

void WaterPool::doSomething()
{
	if (!isAlive())
		return;

	find();

	incTicks();
	int T = 300 - 10 * getWorld()->getLevel();
	if (T < 100) T = 100;
	if (getTickCount() == T)
		kill();
}

void WaterPool::removeObject()
{
	getWorld()->playSound(SOUND_GOT_GOODIE);
	addSquirtsToGun();
	getWorld()->increaseScore(100);
	setInvisible();
	kill();
}
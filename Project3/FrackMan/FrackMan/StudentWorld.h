#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

class Dirt;
class FrackMan;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(string assetDir);
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// functions used by actor.cpp
	void removeDirtFrom(int x, int y);
	bool isDirtInField(int x, int y) const;
	bool BoulderpointIsFree(int x, int y) const;
	void addObject(Actor* object);
	void decBarrels();
	void dropNugget(int x, int y);
	void incSonarKit();
	bool isSonarKitEmpty() const;
	void useSonar();
	bool ProtesterpointIsFree(int x, int y) const;
	bool annoyProtester(int x, int y);
	bool frackmanIsAlive() const;
	bool isShaft(int x, int y) const;
	void annoyFrackman();
	void getFrackmanPosition(int &x, int &y);
	bool isDirtOrBoulderInField(int x, int y) const;
	bool intersectProtester(int x, int y);

private:
	void killHumanActorIfBoulderIsFalling(Actor* person);
	void addObject(char objectType, int numBarrels);
	void fillOilFieldWithDirt(Dirt* arr[][VIEW_WIDTH]);
	string setDisplayText() const;
	void removeDeadGameObjects();
	bool isValidSpot(int x, int y) const;
	void addProtester();
	void addWaterPool();

	vector <Actor*> m_objects;
	FrackMan* m_frackman;
	Dirt* m_dirt[VIEW_HEIGHT][VIEW_WIDTH];
	int m_curScore;
	int m_level;
	int m_lives;
	int m_barrels;
	int m_addNewProtester;
	int m_ticks;
};

#endif // STUDENTWORLD_H_

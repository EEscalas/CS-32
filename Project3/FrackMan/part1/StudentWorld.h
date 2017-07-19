#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;

class Dirt;
class FrackMan;
class Actor;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(string assetDir);
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	void fillOilFieldWithDirt(Dirt* arr[][VIEW_WIDTH]);
	bool isBoulder(int x, int y) const;
	void removeDirtFrom(int x, int y);
	bool isDirtInField(int x, int y) const;
	string setDisplayText() const;
	void addBarrelsOfOil(int numBarrels);
	bool isObject(int x, int y) const;
	void decBarrels();
private:
	vector <Actor*> m_objects;
	FrackMan* m_frackman;
	Dirt* m_dirt[VIEW_HEIGHT][VIEW_WIDTH];
	int m_curScore;
	int m_level;
	int m_lives;
	int m_totBarrels;
	int m_barrels;
};

#endif // STUDENTWORLD_H_

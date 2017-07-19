
#ifndef STUDENT_WORLD_H
#define STUDENT_WORLD_H

#include <string>
using namespace std;

class StudentWorld //: public GameWorld
{
public:
	StudentWorld();
	~StudentWorld();
	virtual void init();
	virtual int move();
	virtual void cleanup();
};







#endif

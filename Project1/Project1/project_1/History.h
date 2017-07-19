// History header file

#include "globals.h"

#ifndef HISTORY_H
#define HISTORY_H

class History
{
public:
	History(int nRows, int nCols);
	bool record(int r, int c);
	void display() const;
private:
	int m_nRows;
	int m_nCols;
	int deathCount[MAXROWS][MAXCOLS] = {{ 0 }};
};

#endif

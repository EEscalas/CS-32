#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>

#include "History.h"
#include "globals.h"

using namespace std;

History::History(int nRows, int nCols)
{
	m_nRows = nRows;
	m_nCols = nCols;
}

bool History::record(int r, int c)
{
	if (r < 1 || r > m_nRows || c < 1 || c > m_nCols)
		return false;
	else
	{
		deathCount[r-1][c-1] += 1;
		return true;
	}
}

void History::display() const
{
	// clears the screen
	clearScreen();

	for (int row = 0; row < m_nRows; row++)
	{
		for (int col = 0; col < m_nCols; col++)
		{
			if (deathCount[row][col] > 26)
				cout << 'Z';
			else
				cout << alphabet[deathCount[row][col]];
		}
		cout << endl;
	}
}
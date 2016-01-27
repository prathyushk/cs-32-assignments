#include "History.h"
#include "globals.h"
#include <iostream>
#include <vector>
using namespace std;

History::History(int nRows, int nCols)
  :m_rows(nRows),m_cols(nCols)
{
}

bool History::record(int r, int c)
{
  if(r > m_rows || r < 1 || c > m_cols || c < 1)
    return false;
  point p;
  p.x = r;
  p.y = c;
  hist.push_back(p);
  return true;
}

void History::display() const
{
  char grid[MAXROWS][MAXCOLS];
  for(int r = 0; r < m_rows; r++)
    for(int c = 0; c < m_cols; c++)
      grid[r][c] = '.';
  for(int i = 0; i < hist.size(); i++)
    {
      if(grid[hist[i].x-1][hist[i].y-1] == '.')
	grid[hist[i].x-1][hist[i].y-1] = 'A';
      else if(grid[hist[i].x-1][hist[i].y-1] == 'Z');
      else
	grid[hist[i].x-1][hist[i].y-1] += 1;
    }
  clearScreen();
  for(int r = 0; r < m_rows; r++)
    {
      for(int c = 0; c < m_cols; c++)
	cout << grid[r][c];
      cout << endl;
    }
  cout << endl;
}

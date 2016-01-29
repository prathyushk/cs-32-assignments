#include <queue>
#include <string>
#include <iostream>

const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;

using namespace std;

class Coord
{
public:
  Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
  int r() const { return m_r; }
  int c() const { return m_c; }
private:
  int m_r;
  int m_c;
};

bool canMove(string maze[], int nRows, int nCols, int r, int c, int dir, Coord& updated)
{
  switch(dir)
    {
    case NORTH:
      if(r-1 >= 0 && maze[r-1][c] == '.')
	{
	  updated = Coord(r-1,c);
	  return true;
	}
      break;
    case EAST:
      if(c+1 < nCols && maze[r][c+1] == '.')
	{
	  updated = Coord(r,c+1);
	  return true;
	}
      break;
    case SOUTH:
      if(r+1 < nRows && maze[r+1][c] == '.')
	{
	  updated = Coord(r+1,c);
	  return true;
	}
      break;
    case WEST:
      if(c-1 >= 0 && maze[r][c-1] == '.')
	{
	  updated = Coord(r,c-1);
	  return true;
	}
      break;
    }
  return false;
}

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
  queue<Coord> coords;
  coords.push(Coord(sr,sc));
  maze[sr][sc] = '*';
  while(!coords.empty())
    {
      Coord curr = coords.front();
      coords.pop();
      if(curr.r() == er && curr.c() == ec)
	return true;
      for(int i = 0; i < 4; i++)
	{
	  Coord updated(-1,-1);
	  if(canMove(maze,nRows,nCols,curr.r(),curr.c(),i,updated))
	    {
	      coords.push(updated);
	      maze[updated.r()][updated.c()] = '*';
	    }
	}
    }
  return false;
}

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
  if(sr == er && sc == ec)
    return true;
  maze[sr][sc] = '*';
  for(int i = 0; i < 4; i++)
    {
      int nr=sr,nc=sc;
      bool canMove = false;
      switch(i)
	{
	case 0:
	  if(sr-1 >= 0 && maze[sr-1][sc] == '.')
	    {
	      nr--;
	      canMove = true;
	    }
	  break;
	case 1:
	  if(sc+1 < nCols && maze[sr][sc+1] == '.')
	    {
	      nc++;
	      canMove = true;
	    }
	  break;
	case 2:
	  if(sr+1 < nRows && maze[sr+1][sc] == '.')
	    {
	      nr++;
	      canMove = true;
	    }
	  break;
	case 3:
	  if(sc-1 >= 0 && maze[sr][sc-1] == '.')
	    {
	      nc--;
	      canMove = true;
	    }
	  break;
	}
      if(canMove)
	if(pathExists(maze,nRows,nCols,nr,nc,er,ec))
	  return true;
    }
  return false;
}

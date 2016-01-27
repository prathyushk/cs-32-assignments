#ifndef HISTORY_H_
#define HISTORY_H_

#include <vector>

class History
{
 public:
  History(int nRows, int nCols);
  bool record(int r, int c);
  void display() const;
 private:
  int m_rows;
  int m_cols;
  struct point
  {
    int x;
    int y;
  };
  std::vector<point> hist;
};

#endif //HISTORY_H_

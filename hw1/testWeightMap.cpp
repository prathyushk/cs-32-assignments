#include "WeightMap.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
  WeightMap wm;
  assert(wm.size() == 0);
  assert(wm.enroll("John0", 140.5) == true);
  assert(wm.weight("John0") == 140.5);
  assert(wm.weight("Mark") == -1);
  assert(wm.adjustWeight("Mark", 50.3) == false);
  assert(wm.adjustWeight("John0", -150) == false);
  assert(wm.adjustWeight("John0", 15.5) == true);
  assert(wm.enroll("John0", 150.5) == false);
  assert(wm.enroll("Mark", -150.2) == false);
  for(int i = 1; i <= 199; i++)
    assert(wm.enroll("John" + to_string(i), 112.3) == true);
  assert(wm.enroll("John200", 114) == false);
  assert(wm.size() == 200);
  wm.print();
  cout << "Passed all tests" << endl;
}

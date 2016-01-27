#include "Map.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
  Map m;  // maps strings to doubles
  assert(m.empty());
  ValueType v = -1234.5;
  assert( !m.get("abc", v)  &&  v == -1234.5); // v unchanged by get failure
  assert(m.insert("xyz", 9876.5) == true);
  assert(m.insert("xyz",9234) == false);
  assert(m.update("xyz",9122) == true);
  assert(m.update("zyx",2.23) == false);
  assert(m.size() == 1);
  assert(m.contains("xyz") == true);
  assert(m.contains("zyx") == false);
  assert(m.erase("zyx") == false);
  KeyType k = "hello";
  assert(m.get(0, k, v)  &&  k == "xyz"  &&  v == 9122);
  assert(m.erase("xyz"));
  assert(m.empty());
  cout << "Passed all tests" << endl;
}

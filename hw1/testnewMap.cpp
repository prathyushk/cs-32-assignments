#include "newMap.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
  Map m;  // maps strings to doubles
  assert(m.empty());
  ValueType v1 = -1234.5;
  assert( !m.get("abc", v1)  &&  v1 == -1234.5); // v unchanged by get failure
  assert(m.insert("xyz", 9876.5) == true);
  assert(m.insert("xyz",9234) == false);
  assert(m.update("xyz",9122) == true);
  assert(m.update("zyx",2.23) == false);
  assert(m.size() == 1);
  assert(m.contains("xyz") == true);
  assert(m.contains("zyx") == false);
  assert(m.erase("zyx") == false);
  KeyType k1 = "hello";
  assert(m.get(0, k1, v1)  &&  k1 == "xyz"  &&  v1 == 9122);
  assert(m.erase("xyz"));
  assert(m.empty());
  Map a(1000);   // a can hold at most 1000 key/value pairs
  Map b(5);      // b can hold at most 5 key/value pairs
  Map c;         // c can hold at most DEFAULT_MAX_ITEMS key/value pairs
  std::string k[6] = { "a" , "ab", "abc", " aa", "bb", "cc"};
  double v = 5.0;;

  // No failures inserting pairs with 5 distinct keys into b
  for (int n = 0; n < 5; n++)
    assert(b.insert(k[n], v));

  // Failure if we try to insert a pair with a sixth distinct key into b
  assert(!b.insert(k[5], v));

  // When two Maps' contents are swapped, their capacities are swapped
  // as well:
  a.swap(b);
  assert(!a.insert(k[5], v)  &&  b.insert(k[5], v));
  cout << "Passed all tests" << endl;
}

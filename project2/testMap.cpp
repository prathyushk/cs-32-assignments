#include "Map.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
  Map m;  // maps strings to doubles
  Map m2;
  assert(m.empty()); //nothing has been inserted so the map should be empty
  ValueType v = -1234.5;
  assert( !m.get("abc", v)  &&  v == -1234.5); //m is empty so get abc should return false and v should remain unchanged
  assert(m.insert("xyz",9876.5) == true); //the key-value pair will be inserted and return tru
  assert(m.insert("xyz",9234) == false); //the key already exists in the map so it will not be inserted and return false
  assert(m.update("xyz",91242) == true); //the key exists so it will update the value and return true
  assert(m.insertOrUpdate("xyz",9122) == true); //the key will be updated since it already exists(insertOrUpdate should not return false because there is no max capacity
  assert(m.update("zyx",2.23) == false); //the key does not exist so nothing will be done and update will return false
  assert(m.size() == 1); //there is one item in m so size will be 1
  assert(m.contains("xyz") == true); //m contains the key "xyz" so return true
  assert(m.contains("zyx") == false); //m does not contain "zyx" so return false
  assert(m.erase("zyx") == false); //m does not contain "zyx" so it cannot be erased, return false
  KeyType k = "hello";
  assert(m.get(0, k, v)  &&  k == "xyz"  &&  v == 9122); //nothing has been inserted or removed successfully since the first insert so it will be the only and 0th item in the map
  assert(m2.insert("xyz",2392) == true); //m2 does not contain "xyz" so it can be inserted
  assert(m2.insert("zyx",2483) == true); //m2 does not contain "zyx" so it can be inserted
  assert(m.insert("yxs",21233) == true); //m does not contain "yxs" so it can be inserted
  assert(m.size() == 2); //now m and m2 have two items each
  assert(m2.size() == 2);
  Map combined, subtracted;
  assert(combine(m,m2,combined) == false); //m and m2 both contain "xyz" but with different values so theres a mismatch so returns false
  assert(combined.contains("xyz") == false); //the combined map should not contain "xyz" because there was a value mismatch
  assert(combined.contains("yxs") == true); //however, it should contain "yxs" from m and " and "zyx" from m2
  assert(combined.contains("zyx") == true);
  assert(combined.size() == 2); //it contains two items
  subtract(m,m2,subtracted);
  assert(subtracted.size() == 1);
  assert(subtracted.contains("yxs"));
  assert(combine(m,m2,m) == false); //now m should contain the same thing as combined and there are no errors due to aliasing
  assert(m.contains("yxs"));
  assert(m.contains("zyx"));
  subtract(m,m2,m); //now m should contain "yxs" because after m2 contains "xyz" and "zyx" so "zyx" will be removed from m
  assert(m.size() == 1);
  assert(m.contains("yxs"));
  m.swap(m2); //now m will contain "xyz" and "zyx" and m2 contains "yxs"
  assert(m.contains("xyz"));
  assert(m.contains("zyx"));
  assert(m.size() == 2);
  assert(m2.contains("yxs"));
  assert(m2.size() == 1);
  assert(m2.erase("yxs"));
  assert(!m2.erase("xyz"));
  assert(m2.empty());
    assert(m.erase("zyx")); //test erase from end
  assert(m.erase("xyz")); //test when head is the one being erased
  assert(m.empty());
  m.insert("no",1234);
  m.insert("yes",5432);
  Map m3 = m; //m3 should now contain the same things as m
  assert(m3.size() == 2);
  assert(m3.get(0,k,v) && k == "no" && v == 1234);
  assert(m3.get(1,k,v) && k == "yes" && v == 5432);
  m2 = m; //m2 should now contain the same things as m
  assert(m2.size() == 2);
  assert(m2.get(0,k,v) && k == "no" && v == 1234);
  assert(m2.get(1,k,v) && k == "yes" && v == 5432);
  m = m; //m should be unchanged
  assert(m.size() == 2);
  assert(m.get(0,k,v) && k == "no" && v == 1234);
  assert(m.get(1,k,v) && k == "yes" && v == 5432);
  cout << "Passed all tests" << endl;
}

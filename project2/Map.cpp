#include "Map.h"
#include <iostream>

bool combine(const Map& m1, const Map& m2, Map& result)
{
  Map tempResult;
  bool mismatch = false;
  int m1size = m1.size();
  int m2size = m2.size();
  KeyType k;
  ValueType v,v2;
  for(int i = 0; i < m1size; i++) //first add all pairs in m1 to the temp map
    {
      m1.get(i,k,v);
      tempResult.insert(k,v);
    }
  for(int i = 0; i < m2size; i++) //iterate through all of the pairs in m2
    {
      m2.get(i,k,v); 
      if(tempResult.get(k,v2)) //if the key already exists in the temp map
	{
	  if(v != v2) //and the values are not the same
	    {
	      tempResult.erase(k); //should not be in the combined map and there was a mismatch
	      mismatch = true;
	    }
	}
      else
	{
	  tempResult.insert(k,v); //key is unique to m2, so insert
	}
    }
  result.swap(tempResult); //swap the maps so result contains the combination
  return !mismatch; 
}

void subtract(const Map& m1, const Map& m2, Map& result)
{
  Map tempResult;
  int m1size = m1.size();
  int m2size = m2.size();
  KeyType k;
  ValueType v;
  for(int i = 0; i < m1size; i++) //first add everything in m1 to the temp results
    {
      m1.get(i,k,v);
      tempResult.insert(k,v);
    }
  for(int i = 0; i < m2size; i++) //remove everything with the keys in m2 in the temp results
    {
      m2.get(i,k,v);
      tempResult.erase(k);
    } //this leaves only the pairs where the keys are unique to m1
  result.swap(tempResult);
}

Map::Map()
  :head(nullptr),tail(nullptr)
{}

Map::Map(const Map& other)
{
  Node *p = other.head;
  while(p != nullptr) //insert each pair that exists in the other map, one by one
    {
      insert(p->data.key,p->data.value);
      p = p->next;
    }
}

Map::~Map()
{
  Node *p = head;
  while(p != nullptr)
    {
      Node *temp = p; //hold temp while moving to next node
      p = p->next;
      delete temp; //delete temp
    }
}

Map& Map::operator=(const Map& other)
{
  if(&other == this) //check for aliasing
    return *this;
  Node *p = head;
  while(p != nullptr) //delete all of the existing nodes
    {
      Node *temp = p;
      p = p->next;
      delete temp;
    }
  head = nullptr; //reset head and tail because they after deletion, they are dangling
  tail = nullptr;
  p = other.head;
  while(p != nullptr) //insert each pair that exists in the other map, one by one
    { 
      insert(p->data.key,p->data.value);
      p = p->next;
    }
  return *this;
}

void Map::dump() const
{
  for(int i = 0; i < size(); i++)
    {
      KeyType k;
      ValueType v;
      get(i,k,v);
      std::cerr << k << " : " << v << std::endl;
    }
  std::cerr << std::endl;
}

bool Map::empty() const
{
  return head == nullptr;
}

int Map::size() const
{
  int size = 0;
  Node *p = head;
  while(p != nullptr) //count each node
    {
      size++;
      p = p->next;
    }
  return size;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
  if(contains(key)) 
    return false; //Map only contains unique keys
  Pair p;
  p.key = key;
  p.value = value; //create key/value pair
  Node *newNode = new Node; 
  newNode->data = p;
  newNode->next = nullptr; //create node to hold the pair
  newNode->prev = nullptr;
  if(empty()) //add to beginning if the list is empty
    {
      head = newNode;
      tail = head;
    }
  else //add to end if the list is not empty
    {
      tail->next = newNode;
      newNode->prev = tail;
      tail = newNode;
    }
  return true;
}

bool Map::update(const KeyType& key, const ValueType& value)
{
  Node *p = head;
  while(p != nullptr) //find node and update the value
    {
      if(p->data.key == key)
	{
	  p->data.value = value;
	  return true;
	}
      p = p->next;
    }
  return false;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
  if(!update(key,value))
    return insert(key,value);
  return true;
}

bool Map::erase(const KeyType& key)
{
  if(empty())
    return false;
  if(head->data.key == key) //move head to the next node and delete the original head if deleting from the beginning
    {
      Node *temp = head;
      head = head->next;
      if(head != nullptr)
	head->prev = nullptr;
      delete temp;
      return true;
    }
  Node *p = head;
  while(p != nullptr)
    {
      if(p->next != nullptr && p->next->data.key == key) //find the node where its next is the node we must erase
	{
	  if(p->next == tail) //if erasing the tail, move tail back one node and delete original tail
	    {
	      delete p->next;
	      tail = p;
	      p->next = nullptr;
	    }
	  else //remove node between this node and the one after next
	    {
	      Node *next = p->next;
	      p->next = p->next->next;
	      p->next->prev = p;
	      delete next;
	    }
	  return true;
	}
      p = p->next;
    }
  return false;
}

bool Map::contains(const KeyType& key) const
{
  Node *p = head;
  while(p != nullptr) //iterate through list until key is found
    {
      if(p->data.key == key)
	return true;
      p = p->next;
    }
  return false; //key was not found
}

bool Map::get(const KeyType& key, ValueType& value) const
{
  Node* p = head;
  while(p != nullptr) //iterate through until key is found
    {
      if(p->data.key == key)
	{
	  value = p->data.value; //set value to the value associated with the key and return
	  return true;
	}
      p = p->next;
    }
  return false; //key not found
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
  if(i < 0 || i >= size()) //check if the ith index is valid
    return false;
  int iter = 0;
  Node *p = head;
  while(iter < i) //set p to point to the ith node
    {
      iter++;
      p = p->next;
    }
  key = p->data.key;
  value = p->data.value;
  return true;
}

void Map::swap(Map& other)
{
  //swap heads and tails
  Node *htemp = head;
  Node *ttemp = tail;
  head = other.head;
  tail = other.tail;
  other.head = htemp;
  other.tail = ttemp;
}

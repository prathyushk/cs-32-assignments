#include "newMap.h"
#include <iostream>
#include <cstdlib>

Map::Map(int max)
{
  if(max < 0)
    {
      std::cout << "Size of map cannot be negative!" << std::endl;
      exit(EXIT_FAILURE);
    }
  else
    {
      m_max = max;
      m_size = 0;
      m_items = new Pair[m_max];
    }
}

Map::Map(const Map& other)
  :m_items(new Pair[other.m_max]), m_size(other.m_size),m_max(other.m_max)
{
  for(int i = 0; i < m_size; i++)
    m_items[i] = other.m_items[i];
}

Map& Map::operator=(const Map& other)
{
  if(&other == this)
    return *this;
  delete [] m_items;
  m_items = new Pair[other.m_max];
  m_size = other.m_size;
  m_max = other.m_max;
  return *this;
}

Map::~Map()
{
  delete [] m_items;
}

bool Map::empty() const
{
  return m_size == 0;
}

int Map::size() const
{
  return m_size;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
  if(m_size == m_max)
    return false;
  if(contains(key))
    return false;
  Pair p;
  p.key = key;
  p.value = value;
  m_items[m_size] = p;
  m_size++;
  return true;
}

bool Map::update(const KeyType& key, const ValueType& value)
{
  for(int i = 0; i < m_size; i++)
    {
      if(m_items[i].key == key)
	{
	  m_items[i].value = value;
	  return true;
	}
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
  for(int i = 0; i < m_size; i++)
    {
      if(m_items[i].key == key)
	{
	  m_items[i] = m_items[m_size-1];
	  m_size--;
	  return true;
	}
    }
  return false;
}

bool Map::contains(const KeyType& key) const
{
  for(int i = 0; i < m_size; i++)
    {
      if(m_items[i].key == key)
	return true;
    }
  return false;
}

bool Map::get(const KeyType& key, ValueType& value) const
{
  for(int i = 0; i < m_size; i++)
    {
      if(m_items[i].key == key)
	{
	  value = m_items[i].value;
	  return true;
	}
    }
  return false;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
  if(i < 0 || i >= m_size)
    return false;
  key = m_items[i].key;
  value = m_items[i].value;
  return true;
}

void Map::swap(Map& other)
{
  Pair *temp = m_items;
  m_items = other.m_items;
  other.m_items = temp;
  int t_size = m_size ,t_max = m_max;
  m_size = other.m_size;
  m_max = other.m_max;
  other.m_size = t_size;
  other.m_max = t_max;
}

#include "Map.h"

Map::Map()
  :m_size(0)
{}

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
  if(m_size == DEFAULT_MAX_ITEMS)
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
  int max_size;
  m_size > other.m_size ? max_size = m_size : max_size = other.m_size;
  for(int i = 0; i < max_size; i++)
    {
      Pair tmp = m_items[i];
      m_items[i] = other.m_items[i];
      other.m_items[i] = tmp;
    }
  int tempSize = m_size;
  m_size = other.m_size;
  other.m_size = tempSize;
}

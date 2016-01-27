#include "WeightMap.h"
#include <iostream>

WeightMap::WeightMap()
{}

bool WeightMap::enroll(std::string name, double startWeight)
{
  if(startWeight < 0)
    return false;
  return m_map.insert(name, startWeight);
}

double WeightMap::weight(std::string name) const
{
  double weight;
  if(m_map.get(name,weight))
    return weight;
  return -1;
}

bool WeightMap::adjustWeight(std::string name, double amt)
{
  double curWeight;
  if(!m_map.get(name,curWeight)) return false;
  curWeight += amt;
  if(curWeight < 0)
    return false;
  return m_map.update(name,curWeight);
}

int WeightMap::size() const
{
  return m_map.size();
}

void WeightMap::print() const
{
  for(int i = 0; i < m_map.size(); i++)
    {
      std::string name;
      double weight;
      m_map.get(i,name,weight);
      std::cout << name << " " << weight << std::endl;
    }
}

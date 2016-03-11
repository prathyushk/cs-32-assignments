#include "IntelWeb.h"
#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

IntelWeb::IntelWeb()
{}

IntelWeb::~IntelWeb()
{}

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
  if(!forward.createNew(filePrefix+".f.dat",maxDataItems*4/3)) //create a map for forward from->to associations with max load factor of 0.75
    return false;
  if(!reverse.createNew(filePrefix+".r.dat",maxDataItems*4/3)) //create a map for reverse to->from associations with max load factor of 0.75
    {
      forward.close(); //reverse failed to be created to close forward and return false
      return false;
    }
  return true; //successfully created empty crawling database
}

bool IntelWeb::openExisting(const std::string& filePrefix)
{
  if(!forward.openExisting(filePrefix+".f.dat")) //open the forward from->to associations map
    return false;
  if(!reverse.openExisting(filePrefix+".r.dat")) //open the reverse to->from associations map
    {
      forward.close();
      return false;
    }
  return true;
}

void IntelWeb::close()
{
  //close both maps
  forward.close();
  reverse.close();
}

bool IntelWeb::ingest(const std::string& telemetryFile)
{
  std::ifstream inf(telemetryFile); //file read stream
  if(!inf)
    return false; //file could not be opened
  std::string line;
  while(std::getline(inf,line)) //get each line one by one
    {
      std::istringstream iss(line); //stringstream to read in formatted ata
      std::string context,value,key; 
      if(!(iss >> context >> key >> value)) //read in each value from the line, if it is not successful the line is incorrectly formatted so just ignore it
	continue;
      char dummy; 
      if(iss >> dummy) //make sure there are no extra characters
	;
      forward.insert(key,value,context); //insert an association for this interaction mapping from->to
      reverse.insert(value,key,context); //insert an association for this interaction mapping to->from
    }
  return true;
}

unsigned int IntelWeb::prevalence(const std::string& key)
{
  std::unordered_map<std::string,unsigned int>::iterator it = prevalences.find(key); //checks if a prevalence for this key has already been calculated
  if(it != prevalences.end())
    return it->second; //a prevalence already exists so return it
  DiskMultiMap::Iterator fi = forward.search(key); //find the key in the forward associations
  DiskMultiMap::Iterator ri = reverse.search(key); //find the key in the reverse associations
  unsigned int count = 0;
  while(fi.isValid()) //count each forward association
    {
      count++;
      ++fi;
    }
  while(ri.isValid()) //count each reverse association
    {
      count++;
      ++ri;
    }
  prevalences[key] = count; //store the prevalence in the map and return it
  return count; 
}

bool compareInteractionTuple(const InteractionTuple& f, const InteractionTuple& o)
{
  if(f.context == o.context) //if the contexts are the same, compare the from fields
    {
      if(f.from == o.from) //if the from fields are the same, compare the to fields
	return f.to < o.to;
      else
	return f.from < o.from;
    }
  else
    return f.context < o.context;
}

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators, unsigned int minPrevalenceToBeGood, std::vector<std::string>& badEntitiesFound,std::vector<InteractionTuple>& badInteractions)
{
  std::queue<std::string> bads; //queue for the breadth first search
  std::unordered_set<std::string> badSet; //holds unique malicious entities
  std::unordered_map<std::string,InteractionTuple> badInteractionsMap; //holds the unique interaction tuples to be returned
  for(int i = 0; i < indicators.size(); i++)
    bads.push(indicators[i]); //add all of the indicators into the search
  while(!bads.empty()) //while there are more malicious entities to search
    {
      std::string key = bads.front(); //search using the item at the front of the queue
      bads.pop();
      DiskMultiMap::Iterator fi = forward.search(key); //find all the forward associations
      DiskMultiMap::Iterator ri = reverse.search(key); //find all the reverse associations
      if(fi.isValid() || ri.isValid()) //if at least one association was found, then that means the indicator was in the log so add it to the set of bad entities
	badSet.insert(key);
      while(fi.isValid())
	{
	  MultiMapTuple tpl =  *fi; //extract the current tuple
	  InteractionTuple itpl(tpl.key,tpl.value,tpl.context); //create a interaction tuple and try adding it to the map(the key is the concatenated strings in the tuple because strings can be easily hashed while the InteractionTuple does not have a hash function
	  badInteractionsMap[tpl.key+tpl.value+tpl.context] = itpl;
	  if(prevalence(tpl.value) < minPrevalenceToBeGood && badSet.count(tpl.value) == 0) //if the prevalence of the entity associated with the malicious entity is less than the min and the set of malicious entities does not already contain this entity
	    {
	      bads.push(tpl.value); //add this new malicious entity to the search
	      badSet.insert(tpl.value); //insert this entity as a new malicious entity
	    }
	  ++fi; //keep going
	}
      while(ri.isValid()) //same as the forward association search except that the interaction tuple must switch the value and the key to preserve the correct to and from
	{
	  MultiMapTuple tpl = *ri;
	  InteractionTuple itpl(tpl.value,tpl.key,tpl.context);
	  badInteractionsMap[tpl.value+tpl.key+tpl.context] = itpl; 
	  if(prevalence(tpl.value) < minPrevalenceToBeGood && badSet.count(tpl.value) == 0)
	    {
	      bads.push(tpl.value);
	      badSet.insert(tpl.value);
	    }
	  ++ri;
	}
    }
  badEntitiesFound.clear(); //clear the vector to hold the bad entities
  for(auto it = badSet.begin(); it != badSet.end(); ++it)
    badEntitiesFound.push_back(*it); //add each malicious entity to the vector
  std::sort(badEntitiesFound.begin(),badEntitiesFound.end()); //sort the vector
  badInteractions.clear(); //clear the vector to hold the malicious interactions
  for(auto it = badInteractionsMap.begin(); it != badInteractionsMap.end(); ++it)
    badInteractions.push_back(it->second); //add each malicious interaction to the vector
  std::sort(badInteractions.begin(),badInteractions.end(),compareInteractionTuple); //sort the vector using the compareInteractionTuple function to compare
  return badSet.size(); //return the number of unique bad entities
}
   
bool IntelWeb::purge(const std::string& entity)
{
  DiskMultiMap::Iterator fi = forward.search(entity); //find the item in the forward associations
  DiskMultiMap::Iterator ri = reverse.search(entity); //find the item in the reverse associations
  std::queue<MultiMapTuple> toDelete; //to keep track of which tuples to delete
  bool deleted = false;
  while(fi.isValid()) //add each tuple to the queue
    {
      MultiMapTuple tpl = *fi;
      toDelete.push(tpl);
      ++fi;
    }
  while(ri.isValid()) //add each tuple to the queue but switch key and value to preserve to and from
    {
      MultiMapTuple tpl = *ri;
      std::string tmp = tpl.key;
      tpl.key = tpl.value;
      tpl.value = tmp;
      toDelete.push(tpl);
      ++ri;
    }
  while(!toDelete.empty()) //for each association to delete, remove it from the forward association map and the reverse association map
    {
      deleted = true;
      forward.erase(toDelete.front().key,toDelete.front().value,toDelete.front().context);
      reverse.erase(toDelete.front().value,toDelete.front().key,toDelete.front().context);
      toDelete.pop();
    }
  return deleted;
}

#include "DiskMultiMap.h"
#include <string>
#include <functional>
#include <cstring>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

DiskMultiMap::Iterator::Iterator()
  :p(0),map(nullptr)
{}

DiskMultiMap::Iterator::Iterator(BinaryFile::Offset place, const std::string& sKey, DiskMultiMap* ref)
  :p(place),key(sKey),map(ref)
{}

bool DiskMultiMap::Iterator::isValid() const
{
  return map != nullptr;  //an invalid iterator will have map set to nullptr
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
  if(!isValid()) //if its not valid just return itself, an invalid iterator
    return *this;
  DiskNode curr;
  map->bf.read(curr,p); //read the current node the iterator points to from the disk
  do
    {
      p = curr.next; //move to the next node
      if(p == 0)
	{
	  map = nullptr; //this means that the iterator has reached the end of the list so it is now invalid
	  break;
	}
      map->bf.read(curr,p); //read in the next node which is now the current node that the iterator points to
    }
  while(strcmp(curr.key,key.c_str())); //make sure the key of this node is the key that the iterator is set to find(to iterate over collisions)
  return *this; //return the updated iterator
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
  MultiMapTuple tpl; //empty tuple
  if(!isValid())
    return tpl; //empty tuple is returned if the iterator is invalid
  DiskNode curr; 
  map->bf.read(curr,p); //read the current node using the offset the iterator is currently on from file
  tpl.key = curr.key; tpl.value = curr.value; tpl.context = curr.context; //copy the data from the DiskNode to the MultiMapTuple(the assignment operator for std::string takes care of this)
  return tpl; //the tuple now contains the data that is pointed to by the iterator
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
  Iterator invalidIt; //default invalid iterator
  if(key.size() > 120)
    return invalidIt; //the key is larger than the map will hold so it will not be in the map
  int bucket = std::hash<std::string>()(key) % nBuckets; //compute the hash for this key
  BinaryFile::Offset p;
  bf.read(p,(bucket + 1)*sizeof(BinaryFile::Offset)+sizeof(unsigned int)); //read the offset of the list that belongs to this bucket into p
  DiskNode curr;
  while(p != 0)
    {
      bf.read(curr,p); //read the current node from file
      if(!strcmp(curr.key,key.c_str())) //make sure the key of this node is the key that is being searched for(to iterate over collisions)
	{
	  Iterator it(p,key,this); //create a valid iterator at the current position and return
	  return it;
	}
      p = curr.next; //keep going
    }
  return invalidIt; //key not found, return an invalid iterator
}

void DiskMultiMap::removeNode(DiskNode& n, BinaryFile::Offset& ptr)
{
  BinaryFile::Offset prevNext = n.next; //saves the next of the node to be deleted
  BinaryFile::Offset deleted; 
  bf.read(deleted,0); //finds the head of the deleted node list
  n.next = deleted; //set the next of the node to be deleted to the deleted node list
  deleted = ptr; //set the deleted head to the node to be deleted
  bf.write(n,deleted);  //updated the deleted node
  bf.write(deleted,0); //update the deleted head
  ptr = prevNext; //sets the pointer that used to point to the deleted node to the next of the node that was deleted before it was deleted
}

BinaryFile::Offset DiskMultiMap::nalloc()
{
  BinaryFile::Offset p;
  BinaryFile::Offset deleted;
  bf.read(deleted,0); //reads in the deleted list head
  if(deleted == 0) 
    p = bf.fileLength(); //the deleted list is empty so the file must be grown
  else
    {
      DiskNode d;
      bf.read(d,deleted); //reads in the head node of the deleted list
      p = deleted; //the offset to return will be the head of the deleted list
      deleted = d.next; //removes the node to return from the deleted list
      bf.write(deleted,0); //updates deleted head in file
    }
  return p;
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
  if(key.size() > 120 || value.size() > 120 || context.size() > 120) //cannot insert data with length over 120 characters
    return false;
  int bucket = std::hash<std::string>()(key) % nBuckets; //compute the hash of the key to find the relevant bucket that should carry this data
  DiskNode nNode; //create a new node and copy the input values into it
  strcpy(nNode.key,key.c_str());
  strcpy(nNode.value,value.c_str());
  strcpy(nNode.context,context.c_str());
  BinaryFile::Offset head;
  bf.read(head,(bucket + 1)*sizeof(BinaryFile::Offset)+sizeof(unsigned int)); //find the head of the list to add this new node to by reading in the offset at the relevant bucket
  nNode.next = head; //the new node will point to the rest of the list
  head = nalloc(); //find the next available offset to put the node at
  bf.write(head,(bucket + 1)*sizeof(BinaryFile::Offset)+sizeof(unsigned int)); //the new head of the list is at the offset of the new node
  bf.write(nNode,head); //write the new node into the allocated offset
  return true;
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
  int bucket = std::hash<std::string>()(key) % nBuckets; //compute the hash of the key to find the relevant bucket that should hold this data
  BinaryFile::Offset head;
  bf.read(head,(bucket + 1)*sizeof(BinaryFile::Offset)+sizeof(unsigned int)); //find the head of the list using the bucket
  BinaryFile::Offset p = head;
  if(p == 0) //the list is empty so key cannot be erased
    return false;
  bool removed = false; //keeps track of whether something has been removed or not
  DiskNode curr,next;
  bf.read(curr,p); //read in the current head
  while(curr.next != 0) //look through the rest of the list to find matching nodes to delete
    {
      bf.read(next, curr.next); //read in the next node
      if(!strcmp(next.key,key.c_str()) && !strcmp(next.value,value.c_str()) && !strcmp(next.context,context.c_str())) //see if the next node carries the data that should be deleted
	{
	  removeNode(next,curr.next); //remove the node and update the parent's next pointer
	  bf.write(curr,p); //write the parent with the updated next pointer
	  removed = true; //a node has been removed
	}
      else
	{
	  p = curr.next; //change focus to the next node
	  curr = next;
	}
    }
  if(p != head)
    bf.read(curr,head); //see if the head needs to be deleted as well
  if(!strcmp(curr.key,key.c_str()) && !strcmp(curr.value,value.c_str()) && !strcmp(curr.context,context.c_str())) //check if the head carries the correct data
    {
      removeNode(curr,head); //delete the head
      bf.write(head,(bucket + 1)*sizeof(BinaryFile::Offset)+sizeof(unsigned int)); //update the head of the list in file
      removed = true; //a node has been removed
    }
  return removed;
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
  nBuckets = numBuckets; 
  if(bf.isOpen())
    bf.close(); //if a file is already open, close it
  if(!bf.createNew(filename))
    return false; //create new file and if it fails, return false
  BinaryFile::Offset zero = 0;
  bf.write(zero,0); //write a zero for the deleted list head because it is empty
  bf.write(numBuckets,bf.fileLength()); //write the number of buckets to the file after the deleted list head
  for(int i = 0; i < numBuckets; i++)
    bf.write(zero,bf.fileLength()); //write a zero for each bucket to indicate that each list pointed to by each bucket is empty
  return true; //successfully created the multimap
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
  if(bf.isOpen()) 
    bf.close(); //if a file is already open, close it
  if(!bf.openExisting(filename))
    return false; //create a new file and if it fails, return false
  bf.read(nBuckets,sizeof(BinaryFile::Offset)); //read in the number of buckets that lies right after the deleted list head
  return true; //the file has successfully been opened and can be used
}

void DiskMultiMap::close()
{
  bf.close();
}

DiskMultiMap::DiskMultiMap()
:nBuckets(0){}

DiskMultiMap::~DiskMultiMap()
{
  close(); //make sure the BinaryFile is closed when this is destructed
}

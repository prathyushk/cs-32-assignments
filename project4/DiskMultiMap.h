#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

/* The file created by the DiskMultiMap looks as follows:
deletedHead (BinaryFile::Offset)
numberOfBuckets (unsigned int)
bucket0Head (BinaryFile::Offset)
bucket1Head (BinaryFile::Offset)
...
bucketNHead(BinaryFile::Offset)
---- LINKED LIST NODES --------
*/

class DiskMultiMap
{
 public:

  class Iterator
  {
  public:
    Iterator();
    Iterator(BinaryFile::Offset place, const std::string& sKey, DiskMultiMap* ref);
    bool isValid() const;
    Iterator& operator++();
    MultiMapTuple operator*();
  private:
    BinaryFile::Offset p;
    std::string key;
    DiskMultiMap* map;
  };

  DiskMultiMap();
  ~DiskMultiMap();
  bool createNew(const std::string& filename, unsigned int numBuckets);
  bool openExisting(const std::string& filename);
  void close();
  bool insert(const std::string& key, const std::string& value, const std::string& context);
  Iterator search(const std::string& key);
  int erase(const std::string& key, const std::string& value, const std::string& context);
 private:
  BinaryFile bf;
  unsigned int nBuckets;
  struct DiskNode
  {
    char key[121];
    char value[121];
    char context[121];
    BinaryFile::Offset next;
  };
  BinaryFile::Offset nalloc(); //gets the next available spot for a node in the file
  void removeNode(DiskNode& n, BinaryFile::Offset& ptr); //does the work to keep track of deleted nodes and automatically updates ptrs
};

#endif // DISKMULTIMAP_H_

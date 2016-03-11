#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "MultiMapTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>
#include <unordered_map>

bool compareInteractionTuple(const InteractionTuple& f, const InteractionTuple& o);

class IntelWeb
{
 public:
  IntelWeb();
  ~IntelWeb();
  bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
  bool openExisting(const std::string& filePrefix);
  void close();
  bool ingest(const std::string& telemetryFile);
  unsigned int crawl(const std::vector<std::string>& indicators,
		     unsigned int minPrevalenceToBeGood,
		     std::vector<std::string>& badEntitiesFound,
		     std::vector<InteractionTuple>& interactions
		     );
  bool purge(const std::string& entity);

 private:
  unsigned int prevalence(const std::string& key);
  std::unordered_map<std::string,unsigned int> prevalences;
  DiskMultiMap forward;
  DiskMultiMap reverse;
};

#endif // INTELWEB_H_

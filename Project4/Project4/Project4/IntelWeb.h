#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "BinaryFile.h"
#include <string>
#include <vector>
#include "DiskMultiMap.h"

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
	DiskMultiMap m_TFC_Map;
	DiskMultiMap m_FTC_Map;

	string m_filePrefix;

	bool prevalent(int prevelanceGood, string key);
};

#endif // INTELWEB_H_
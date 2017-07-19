
#include "IntelWeb.h"
#include "DiskMultiMap.h"
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <string>

IntelWeb::IntelWeb()
{

}

IntelWeb::~IntelWeb()
{

}

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
	int m_numBuckets = (4 * maxDataItems) / 3;
	bool valid = true;
	if (!m_TFC_Map.createNew(filePrefix + "toFrom", m_numBuckets))
		valid = false;
	if (!m_FTC_Map.createNew(filePrefix + "fromTo", m_numBuckets))
		valid = false;

	m_filePrefix = filePrefix;

	if (!valid)
	{
		m_TFC_Map.close();
		m_FTC_Map.close();
		return false;
	}
	return true;
}

bool IntelWeb::openExisting(const std::string& filePrefix)
{
	bool valid = true;
	if (!m_TFC_Map.openExisting(m_filePrefix + "toFrom"))
		valid = false;
	if (!m_FTC_Map.openExisting(m_filePrefix + "fromTo"))
		valid = false;

	if (!valid)
	{
		m_TFC_Map.close();
		m_FTC_Map.close();
		return false;
	}
	return true;
}

void IntelWeb::close()
{
	m_TFC_Map.close();
	m_FTC_Map.close();
}

bool IntelWeb::ingest(const std::string& telemetryFile)
{
	ifstream inf(telemetryFile);
	if (!inf)
		return false;
	
	InteractionTuple data;
	string line;
	while(getline(inf, line))
	{
		istringstream iss(line);
		if ((iss >> data.context >> data.to >> data.from))
		{
			if (!m_TFC_Map.insert(data.to, data.from, data.context))
				return false;
			if (!m_FTC_Map.insert(data.from, data.to, data.context))
				return false;
		}
	}
	return true;
}

bool myfunc(InteractionTuple& a, InteractionTuple& b)
{
	if (a.context < b.context)
		return true;
	else if (a.context > b.context)
		return false;
	else if (a.from < b.from)
		return true;
	else if (a.from > b.from)
		return false;
	else if (a.to < b.to)
		return true;
	else return false;
}

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
	unsigned int minPrevalenceToBeGood,
	std::vector<std::string>& badEntitiesFound,
	std::vector<InteractionTuple>& interactions
	)
{
	badEntitiesFound.clear();
	interactions.clear();
	
	unordered_set<string> badEntities;
	unordered_set<string> badInteractions;
	string indicator;
	int maliciousEntities = 0;
	DiskMultiMap::Iterator it_TFC = m_TFC_Map.search(indicator);
	DiskMultiMap::Iterator it_FTC = m_FTC_Map.search(indicator);

	for (int k = 0; k < indicators.size(); k++)
	{
		indicator = indicators[k];
		it_TFC = m_TFC_Map.search(indicator);
		it_FTC = m_FTC_Map.search(indicator);


		string data;
		MultiMapTuple tempData;
		InteractionTuple data2;

		while (it_TFC.isValid())		// crawl through the to -> from set
		{
			if (!prevalent(minPrevalenceToBeGood, indicator))
			{
				tempData = *it_TFC;
				if (badEntities.count(tempData.key) == 0)
				{
					badEntities.insert(tempData.key);
					badEntitiesFound.push_back(tempData.key);
					maliciousEntities++;
				}
				if (!prevalent(minPrevalenceToBeGood, tempData.value) && badEntities.count(tempData.value) == 0)
				{
					badEntities.insert(tempData.value);
					badEntitiesFound.push_back(tempData.value);
				}
			}
			if (!prevalent(minPrevalenceToBeGood, indicator))
			{
				tempData = *it_FTC;
				data = tempData.key + tempData.context + tempData.value;
				if (!prevalent(minPrevalenceToBeGood, tempData.value) && badInteractions.count(data) == 0)
				{
					badInteractions.insert(data);
					data2.from = tempData.key;
					data2.to = tempData.value;
					data2.context = tempData.context;
					interactions.push_back(data2);
				}
			}

			++it_TFC;
		}
		while (it_FTC.isValid())			// crawl through the from->to set
		{
			if (!prevalent(minPrevalenceToBeGood, indicator))
			{
				tempData = *it_TFC;
				if (badEntities.count(tempData.key) == 0)
				{
					badEntities.insert(tempData.key);
					badEntitiesFound.push_back(tempData.key);
					maliciousEntities++;
				}
				if (!prevalent(minPrevalenceToBeGood, tempData.value) && badEntities.count(tempData.value) == 0)
				{
					badEntities.insert(tempData.value);
					badEntitiesFound.push_back(tempData.value);
				}
			}
			if (!prevalent(minPrevalenceToBeGood, indicator))
			{
				tempData = *it_FTC;
				data = tempData.key + tempData.context + tempData.value;
				if (!prevalent(minPrevalenceToBeGood, tempData.value) && badInteractions.count(data) == 0)
				{
					badInteractions.insert(data);
					data2.from = tempData.key;
					data2.to = tempData.value;
					data2.context = tempData.context;
					interactions.push_back(data2);
				}
			}
			++it_FTC;
		}
	}

	sort(badEntitiesFound.begin(), badEntitiesFound.end());
	sort(interactions.begin(), interactions.end(), myfunc);

	return maliciousEntities;
}

bool IntelWeb::prevalent(int prevelanceGood,string key)
{
	int count = 0;
	DiskMultiMap::Iterator it1 = m_TFC_Map.search(key);
	DiskMultiMap::Iterator it2 = m_FTC_Map.search(key);
	while (it1.isValid())
	{
		count++;
		++it1;
	}
	while (it2.isValid())
	{
		count++;
		++it2;
	}
	if (count >= prevelanceGood)
		return true;
	else return false;
}


bool IntelWeb::purge(const std::string& entity)
{
	int purgeCount = 0;
	MultiMapTuple data;
	DiskMultiMap::Iterator it1 = m_TFC_Map.search(entity);
	DiskMultiMap::Iterator it2 = m_FTC_Map.search(entity);
	while (it1.isValid())
	{
		data = *it1;
		purgeCount += m_TFC_Map.erase(data.key, data.value, data.context);
	}
	while (it2.isValid())
	{
		data = *it2;
		purgeCount += m_FTC_Map.erase(data.key, data.value, data.context);
	}
	return purgeCount != 0;
}
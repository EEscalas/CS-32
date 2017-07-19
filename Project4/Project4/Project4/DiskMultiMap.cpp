#include "DiskMultiMap.h"
#include <functional>
#define _CRT_SECURE_NO_WARNINGS

/////////////////////////////////////////////////
///////////ITERATOR IMPLEMENTATION///////////////
/////////////////////////////////////////////////


DiskMultiMap::Iterator::Iterator()
{
	m_map = nullptr;
}

DiskMultiMap::Iterator::Iterator(DiskMultiMap* map)
{
	m_map = map;
	m_valid = false;
}

DiskMultiMap::Iterator::Iterator(DiskMultiMap* map, BinaryFile::Offset pos, MultiMapTuple data)
{
	m_map = map;
	DiskNode node;
	m_curNode = pos;
	m_valid = m_map->m_file.read(node, m_curNode);
	m_data = data;
}

bool DiskMultiMap::Iterator::isValid() const
{
	if (m_map == nullptr)
		return false;
	else return true;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
	if (!m_valid)
		return *this;
	DiskNode node;
	m_valid = m_map->m_file.read(node,m_next);
	while (m_data.key != node.m_key)
	{
		m_next = node.m_next;
		if (node.m_next == 0)
		{
			m_valid = false;
			return *this;
		}
	}

	m_map->m_file.read(node, m_next);
	
	m_data.key = node.m_key;
	m_data.value = node.m_value;
	m_data.context = node.m_context;
	m_next = node.m_next;

	cout << m_data.key << " " << m_data.value << " " << m_data.context << endl;
	return *this;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
	if (m_valid)
		return m_data;
	return m_data;
}


/////////////////////////////////////////////////
/////// DISK MULTI MAP IMPLEMENTATION ///////////
/////////////////////////////////////////////////

DiskMultiMap::DiskMultiMap()
{
	m_created = false;
	m_open = false;
	m_bucketBegin = 0;
}

DiskMultiMap::~DiskMultiMap()
{
	close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
	if (m_created)
		m_file.close();

	if (numBuckets == 0)
		return false;

	m_file.createNew(filename);
	m_created = true;
	m_open = true;
	int m_numBuckets = numBuckets;
	if (!m_file.write(m_numBuckets, 0))
		return false;

	int nodeCount = 0;
	BinaryFile::Offset header = 0;
	if (!m_file.write(header, sizeof(m_numBuckets)))		// the offset number at offset 1 indicates the offset number of a removed node
		return false;
	if (!m_file.write(nodeCount, sizeof(m_numBuckets) + sizeof(header)))
		return false;
	
	m_bucketBegin = sizeof(m_numBuckets) + sizeof(header) + sizeof(nodeCount);	// three headers in binary file: int numBuckets, BinaryFile::Offset m_removedHead, int nodeCount;

	BinaryFile::Offset pos = 0;
	for (int k = 0; k < m_numBuckets; k++)
	{
		if (!m_file.write(pos, m_bucketBegin + (k*sizeof(pos))))
			return false;
	}

	m_bucketEnd = m_bucketBegin + m_numBuckets*sizeof(pos);

	return true;
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
	if (m_open)
		m_file.close();
	
	m_open = true;
	return m_file.openExisting(filename);
}

void DiskMultiMap::close()
{
	if (m_open || m_created)
		m_file.close();
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{	
	if (key.size() >= 120 || value.size() >= 120 || context.size() >= 120)
		return false;

	// create new entry node

	DiskNode newEntry;
	strcpy_s(newEntry.m_key, key.c_str());
	strcpy_s(newEntry.m_value, value.c_str());
	strcpy_s(newEntry.m_context, context.c_str());

	int nodeCount = 0;
	int numBuckets = 0;

	BinaryFile::Offset freeLocation;
	BinaryFile::Offset m_removedHead;
	m_file.read(m_removedHead, sizeof(int));

	m_file.read(nodeCount, sizeof(int) + sizeof(BinaryFile::Offset));
	m_file.read(numBuckets, 0);
	m_file.read(m_removedHead, sizeof(int));
	if (m_removedHead != 0)
	{
		DiskNode temp;
		m_file.read(temp, m_removedHead);
		freeLocation = m_removedHead;
		m_removedHead = temp.m_next;
		m_file.write(m_removedHead, sizeof(int));
	}
	else freeLocation = m_bucketEnd + (nodeCount*sizeof(DiskNode));

	std::hash<std::string> hashKey;						// determine the hash value for the key
	unsigned int hashValue = hashKey(key);
	int m_numBuckets = 1;
	m_file.read(m_numBuckets, 0);
	int bucket = hashValue % m_numBuckets;

	BinaryFile::Offset bucketOffset;
	m_file.read(bucketOffset, m_bucketBegin + (bucket*sizeof(BinaryFile::Offset)));
	if(bucketOffset != 0)
		newEntry.m_next = bucketOffset;
	else newEntry.m_next = 0;

	m_file.write(newEntry, freeLocation);													// put the new entry into the free location
	m_file.write(freeLocation, m_bucketBegin + (bucket*sizeof(BinaryFile::Offset)));		// put the location of the new entry into the proper bucket

	// increase the bucket count by one
	m_file.read(nodeCount, 0 + sizeof(int) + sizeof(BinaryFile::Offset));
	nodeCount++;
	m_file.write(nodeCount, 0 + sizeof(int) + sizeof(BinaryFile::Offset));

	return true;		// check to see when this is supposed to return false
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
	std::hash<std::string> hashKey;
	unsigned int hashValue = hashKey(key);
	int m_numBuckets = 1;
	m_file.read(m_numBuckets, 0);
	unsigned int bucket = hashValue % m_numBuckets;

	BinaryFile::Offset m_head = 0;
	m_file.read(m_head, m_bucketBegin + (bucket*sizeof(BinaryFile::Offset)));
	DiskNode node;
	m_file.read(node, m_head);
	if (node.m_key == key.c_str())
	{
		MultiMapTuple data;
		data.key = node.m_key;
		data.value = node.m_value;
		data.context = node.m_context;
		DiskMultiMap::Iterator it(this, node.m_next, data);
		return it;
	}
	else
	{
		while (node.m_key != key.c_str())
		{
			if (node.m_next == 0)
			{
				DiskMultiMap::Iterator iter;	// return invalid iterator
				return iter;
			}
			m_head = node.m_next;
			m_file.read(node, m_head);
		}
	}

	MultiMapTuple data;
	data.key = node.m_key;
	data.value = node.m_value;
	data.context = node.m_context;
	DiskMultiMap::Iterator it(this, node.m_next, data);
	return it;
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
	std::hash<std::string> hashKey;
	unsigned int hashValue = hashKey(key);
	int m_numBuckets = 1;
	m_file.read(m_numBuckets, 0);
	unsigned int bucket = hashValue % m_numBuckets;
	
	DiskNode node, nextNode;
	int instances = 0;

	BinaryFile::Offset m_head = 0;
	m_file.read(m_head, m_bucketBegin + (bucket*sizeof(BinaryFile::Offset)));	// start searching throgh all keys that hash to the "bucket" numbered bucket

	BinaryFile::Offset m_removedHead;
	m_file.read(m_removedHead, sizeof(int));

	m_file.read(node, m_head);

	if ((strcmp(node.m_key, key.c_str()) == 0) && (strcmp(node.m_value, value.c_str()) == 0) && (strcmp(node.m_context, context.c_str()) == 0))
	{
		BinaryFile::Offset temp = m_head;
		m_head = node.m_next;
		m_removedHead = temp;
		m_file.write(m_removedHead, sizeof(int));
	}

	if (m_head == 0) return 1;

	BinaryFile::Offset curNode = m_head;
	while (curNode != 0)
	{
		m_file.read(node, curNode);
		m_file.read(nextNode, node.m_next);

		if ((strcmp(nextNode.m_key, key.c_str()) == 0) && (strcmp(nextNode.m_value, value.c_str()) == 0) && (strcmp(nextNode.m_context, context.c_str()) == 0))
		{
			instances++;
			BinaryFile::Offset temp = node.m_next;
			node.m_next = nextNode.m_next;
			m_file.write(node, curNode);

			nextNode.m_next = m_removedHead;
			m_file.write(nextNode, temp);
			m_removedHead = temp;
			m_file.write(m_removedHead, sizeof(int));
		}
		else curNode = node.m_next;
	}

	return instances;
}



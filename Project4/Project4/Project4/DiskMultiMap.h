#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"
#include <stack>


class DiskMultiMap
{
public:

	class Iterator
	{
	public:
		Iterator();
		Iterator(DiskMultiMap* map);
		Iterator(DiskMultiMap* map, BinaryFile::Offset pos, MultiMapTuple data);
		// You may add additional constructors
		bool isValid() const;
		Iterator& operator++();
		MultiMapTuple operator*();

	private:
		DiskMultiMap* m_map;
		BinaryFile::Offset m_next;
		BinaryFile::Offset m_curNode;
		MultiMapTuple m_data;
		bool m_valid;
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
	// Your private member declarations will go here
	BinaryFile m_file;
		// three headers in binary file: numberOfBuckets (int), offset of first removed node (BinaryFile::Offset), nodeCount (int)

	bool m_created;
	bool m_open;

	BinaryFile::Offset m_bucketBegin;		// offset location at which headers end and buckets begin
	BinaryFile::Offset m_bucketEnd;			// offset location at which buckets end and data begins
	BinaryFile::Offset m_dataEnd;			// offset location where last piece of data is stored

	struct DiskNode
	{
		BinaryFile::Offset m_next;
		char m_key[120];
		char m_value[120];
		char m_context[120];
	};
};


#endif // DISKMULTIMAP_H_

#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <string>

typedef std::string KeyType;
typedef double ValueType;

class Map
{
public:
	Map();
	Map(const Map &old);
	Map &operator= (const Map& src);
	~Map();
	bool empty() const;
	int size() const;
	bool insert(const KeyType& key, const ValueType& value);
	bool update(const KeyType& key, const ValueType& value);
	bool insertOrUpdate(const KeyType& key, const ValueType& value);
	bool erase(const KeyType& key);
	bool contains(const KeyType& key) const;
	bool get(const KeyType& key, ValueType& value) const;
	bool get(int i, KeyType& key, ValueType& value) const;
	void swap(Map& other);
private:
	struct Node
	{
		KeyType m_key;
		ValueType m_value;
		Node* next;
		Node* prev;
	};
	Node* head;
	Node* tail;
};




#endif


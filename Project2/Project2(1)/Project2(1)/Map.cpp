#include "Map.h"

Map::Map()
{
	head = nullptr;
	tail = nullptr;
}

Map::Map(const Map &old)				// copy constructor
{
	for (int k = old.size() - 1; k >= 0; k--)
	{
		KeyType old_key;
		ValueType old_value;
		old.get(k, old_key, old_value);
		this->insertOrUpdate(old_key, old_value);
	}
}

Map &Map::operator= (const Map &src)			// assignment operator
{
	Map old = src;
	
	KeyType this_key;
	ValueType this_value;
	
	for (int k = this->size() - 1; k >= 0; k--)
	{
		this->get(k, this_key, this_value);
		this->erase(this_key);
	}
	
	for (int k = old.size() - 1; k >= 0; k--)
	{
		KeyType old_key;
		ValueType old_value;
		old.get(k, old_key, old_value);
		this->insertOrUpdate(old_key, old_value);
	}

	return *this;
}

Map::~Map()
{
	Node* p = head;
	while (p != nullptr)		// traverse and delete each node
	{
		Node* n = p->next;
		delete p;
		p = n;
	}
}

bool Map::empty() const
{
	if (head == nullptr)	// the map is empty if the head has yet to be assigned to anything
		return true;
	else return false;
}

int Map::size() const
{
	int size = 0;
	Node* p;
	p = head;
	while (p != nullptr)	// traverse through the nodes of the Map and count each element until it reaches the end of the linked list
	{
		size++;
		p = p->next;
	}
	return size;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
	if (head == nullptr)		// if the map is empty, create a node at the beginning
	{
		Node* ptr;
		ptr = new Node;
		ptr->m_key = key;
		ptr->m_value = value;
		ptr->next = head;
		ptr->prev = nullptr;
		head = ptr;
		return true;
	}
	else
	{
		// Add Node to front
		
		Node* p;
		p = new Node;
		p->m_key = key;
		p->m_value = value;
		p->next = head;
		p->prev = nullptr;
		head->prev = p;
		head = p;
		return true;
	}

}

bool Map::update(const KeyType& key, const ValueType& value)
{
	Node* p;
	p = head;
	while (p != nullptr)
	{
		if (p->m_key == key)		// if a key is found, update the value
		{
			p->m_value = value;
			return true;
		}
		p = p->next;
	}
	return false;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
	Node* p;
	p = head;
	while (p != nullptr)
	{
		if (p->m_key == key)
		{
			p->m_value = value;
			return true;
		}
		p = p->next;
	}
	return insert(key, value);
}

bool Map::erase(const KeyType& key)
{
	Node* p = head;
	if (head == nullptr)
		return false;
	if (head->m_key == key)
	{
		Node* killFirst = head;
		head = killFirst->next;
		delete killFirst;
		return true;
	}
	
	while (p != nullptr)			// traverse to find key
	{
		if (p->next != nullptr && p->next->m_key == key)
			break;
		p = p->next;
	}
	if (p != nullptr)
	{
		Node* killMe = p->next;
		p->next = killMe->next;
		p->prev = killMe->prev;
		delete killMe;
		return true;
	}
	else return false;
}

bool Map::contains(const KeyType& key) const
{
	Node* p;
	p = head;
	while (p != nullptr)		// traverse until you see if one of the nodes contains the desired key
	{
		if (p->m_key == key)
		{
			return true;
		}
		p = p->next;
	}
	return false;
}

bool Map::get(const KeyType& key, ValueType& value) const
{
	Node* p;						// traverse through the nodes until the desired keytype is found
	p = head;
	while (p != nullptr)
	{
		if (p->m_key == key)
		{
			value = p->m_value;		// change the referenced value to the node value
			return true;
		}
		p = p->next;
	}
	return false;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
	int nodeNumber = 0;
	Node* p = head;
	while (p != nullptr)
	{
		if (nodeNumber == i)		// traverse through the nodes until the desired node position is reached
		{
			key = p->m_key;			// change the references key and value to node p's key and value
			value = p->m_value;
			return true;
		}
		nodeNumber++;
		p = p->next;
	}
	return false;
}

void Map::swap(Map& other)
{
	Map temp = other;
	other = *this;
	*this = temp;
}


bool combine(const Map& m1, const Map& m2, Map& result)
{
	bool NoDuplicate = true;

	KeyType result_key;
	ValueType result_value;

	Map mm1 = m1;
	Map mm2 = m2;

	KeyType m1_key, m2_key;
	ValueType m1_value, m2_value;
	for (int j = 0; j < mm1.size(); j++)
	{
		mm1.get(j, m1_key, m1_value);
		for (int k = 0; k < mm2.size(); k++)
		{
			mm2.get(k, m2_key, m2_value);
			if (m1_key == m2_key && m1_value == m2_value)				// if the key and value are the same in m1 and m2, only insert the key and value type once
				result.insertOrUpdate(m1_key, m1_value);
			else if (m1_key == m2_key && m1_value != m2_value)
			{
				result.erase(m1_key);
				NoDuplicate = false;
			}
			else if (j == mm1.size() - 1 || k == mm2.size() - 1)			// if you have gone through all the possible combinations with m1 and m2, insert m1 and m2
			{
				result.insertOrUpdate(m1_key, m1_value);
				result.insertOrUpdate(m2_key, m2_value);
			}
		}
	}
	for (int j = 0; j < mm1.size(); j++)
	{
		mm1.get(j, m1_key, m1_value);
		for (int k = 0; k < mm2.size(); k++)
		{
			mm2.get(k, m2_key, m2_value);
			if (m1_key == m2_key && m1_value != m2_value)
			{
				result.erase(m1_key);								// go through and erase all the places where m1's key equals m2's key, but the values aren't equal
				NoDuplicate = false;
			}
		}
	}
	return NoDuplicate;
}



void subtract(const Map& m1, const Map& m2, Map& result)
{
	Map mm1 = m1;
	Map mm2 = m2;
	
	KeyType m1_key, m2_key, result_key;
	ValueType m1_value, m2_value, result_value;

	for (int j = 0; j < mm1.size(); j++)
	{
		result.get(j, result_key, result_value);
		if (!mm1.contains(result_key))
			result.erase(result_key);
	}
	
	for (int j = 0; j < mm1.size(); j++)
	{
		mm1.get(j, m1_key, m1_value);
		result.insertOrUpdate(m1_key, m1_value);
	}

	for (int j = 0; j < mm1.size(); j++)
	{
		mm1.get(j, m1_key, m1_value);
		for (int k = 0; k < mm2.size(); k++)
		{
			mm2.get(k, m2_key, m2_value);
			if (m1_key == m2_key)
			{
				result.erase(m1_key);			// go through and erase any points where m1's key equals m2's key
			}
		}
	}
}
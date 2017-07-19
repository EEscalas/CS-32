#include "Map.h"

Map::Map()
{
	head = nullptr;
	tail = nullptr;
}

Map::Map(const Map &old)
{
	Node* p_old = old.head;				// create a new node that is the head of the old node to enable traversal later on
	this->head = old.head;				// set the map being constructed's head and tail to old map's head and tail
	this->tail = old.tail;
	while (p_old != nullptr)
	{
		Node* thisNode = new Node;			// traverse through the old map's nodes and copy the nodes into the new map's nodes
		thisNode->m_key = p_old->m_key;
		thisNode->m_value = p_old->m_value;
		thisNode->next = p_old->next;
		thisNode->prev = p_old->prev;

		p_old = p_old->next;
	}
}

Map &Map::operator= (const Map &src)
{
	Node* p_this = this->head;				// delete content of leftmost side of equals sign
	while (p_this != nullptr)
	{
		Node* n = p_this->next;
		delete p_this;
		p_this = n;
	}
	Node* p_src = src.head;
	this->head = src.head;
	this->tail = src.tail;
	while (p_src != nullptr)
	{		
		Node* thisNode = new Node;
		thisNode->m_key = p_src->m_key;
		thisNode->m_value = p_src->m_value;
		thisNode->next = p_src->next;
		thisNode->prev = p_src->prev;

		p_src = p_src->next;
	}
	return *this;
}

Map::~Map()
{
	Node* p = head;
	while (p != nullptr)
	{
		Node* n = p->next;
		delete p;
		p = n;
	}
}

bool Map::empty() const
{
	if (head == nullptr)
		return true;
	else return false;
}

int Map::size() const
{
	int size = 0;
	Node* p;
	p = head;
	while (p != nullptr)
	{
		size++;
		p = p->next;
	}
	return size;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
	if (head == nullptr)
	{
		Node* p;
		p = new Node;
		p->m_key = key;
		p->m_value = value;
		p->next = head;
		p->prev = nullptr;
		head = p;
		return true;
	}
	else
	{
		Node* q;
		q = head;
		while (q != nullptr)
		{
			if (q->m_key == key)
			{
				q->m_value = value;
				return false;
			}
			q = q->next;
		}
	}

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

bool Map::update(const KeyType& key, const ValueType& value)
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
	while (p != nullptr)
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
	while (p != nullptr)
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
	Node* p;
	p = head;
	while (p != nullptr)
	{
		if (p->m_key == key)
		{
			value = p->m_value;
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
		if (nodeNumber == i)
		{
			key = p->m_key;
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
	*this = other;
}


bool combine(const Map& m1, const Map& m2, Map& result)
{
	bool duplicateKeyValuePair = true;

	Map* resultado = new Map;
	result = *resultado;

	KeyType m1_key, m2_key;
	ValueType m1_value, m2_value;
	for (int j = 0; j < m1.size(); j++)
	{
		m1.get(j, m1_key, m1_value);
		for (int k = 0; k < m2.size(); k++)
		{
			m2.get(k, m2_key, m2_value);
			if (m1_key == m2_key && m1_value == m2_value)
				result.insert(m1_key, m1_value); 
			else if (j == m1.size() - 1 || k == m2.size()-1)
			{
				result.insert(m1_key, m1_value);
				result.insert(m2_key, m2_value);
			}
		}
	}
	for (int j = 0; j < m1.size(); j++)
	{
		m1.get(j, m1_key, m1_value);
		for (int k = 0; k < m2.size(); k++)
		{
			m2.get(k, m2_key, m2_value);
			if (m1_key == m2_key && m1_value != m2_value)
			{
				result.erase(m1_key);
				duplicateKeyValuePair = false;
			}
		}
	}
	return duplicateKeyValuePair;
}



void subtract(const Map& m1, const Map& m2, Map& result)
{
	Map* resultado = new Map;
	result = *resultado;

	KeyType m1_key, m2_key;
	ValueType m1_value, m2_value;

	for (int j = 0; j < m1.size(); j++)
	{
		m1.get(j, m1_key, m1_value);
		for (int k = 0; k < m2.size(); k++)
		{
			m2.get(k, m2_key, m2_value);
			if (j == m1.size() - 1 || k == m2.size() - 1)
			{
				result.insert(m1_key, m1_value);
			}
		}
	}

	for (int j = 0; j < m1.size(); j++)
	{
		m1.get(j, m1_key, m1_value);
		for (int k = 0; k < m2.size(); k++)
		{
			m2.get(k, m2_key, m2_value);
			if (m1_key == m2_key)
			{
				result.erase(m1_key);
			}
		}
	}

}
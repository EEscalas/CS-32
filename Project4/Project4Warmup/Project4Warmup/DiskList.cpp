#include <string>
#include <iostream>
#include "BinaryFile.h"

class DiskList
{
public:
	DiskList(const std::string& filename)
	{
		m_file.createNew(filename);
		m_head = 0;
		m_removedHead = 0;
		numElements = 0;
	}
	bool push_front(const char* data)
	{
		if (strlen(data) >= 256)
			return false;
		
		DiskNode newElement;
		newElement.m_next = m_head;
		strcpy_s(newElement.m_name, data);
		BinaryFile::Offset newLocation = m_head;

		if (m_removedHead != 0)
		{
			DiskNode temp;
			m_file.read(temp, m_removedHead);
			newLocation = m_removedHead;
			m_removedHead = temp.m_next;
		}
		else newLocation = sizeof(m_head) + numElements*sizeof(DiskNode);
		
		m_file.write(newElement, newLocation);
		m_head = newLocation;
		numElements++;

		return true;
	}
	bool remove(const char* data)
	{
		DiskNode node, nextNode;
		bool found = false;

		if (m_head == 0) return false;
		
		m_file.read(node, m_head);

		if (strcmp(node.m_name, data) == 0)
		{
			BinaryFile::Offset temp = m_head;
			m_head = node.m_next;

			m_removedHead = temp;
		}

		BinaryFile::Offset curNode = m_head;
		while (curNode != 0)
		{
			m_file.read(node, curNode);
			m_file.read(nextNode, node.m_next);

			if (strcmp(nextNode.m_name, data) == 0)
			{
				found = true;
				BinaryFile::Offset temp = node.m_next;
				node.m_next = nextNode.m_next;
				m_file.write(node, curNode);

				nextNode.m_next = m_removedHead;
				m_file.write(nextNode, temp);
				m_removedHead = temp;
			}
			else curNode = node.m_next;
		}

		return found;
	}
	void printAll()
	{
		BinaryFile::Offset curNode = m_head;
		DiskNode node;
		while (curNode != 0)
		{
			m_file.read(node, curNode);
			int k = 0;
			while (node.m_name[k] != '\0')
			{
				cout << node.m_name[k];
				k++;
			}
			cout << endl;
			curNode = node.m_next;
		}
	}
private:

	BinaryFile m_file;
	BinaryFile::Offset m_head;
	BinaryFile::Offset m_removedHead;
	int numElements;
	struct DiskNode
	{
		char m_name[256];
		BinaryFile::Offset m_next;
	};
};

int main()
{
	DiskList x("mylist.dat");
	x.push_front("Fred");
	x.push_front("Lucy");
	x.push_front("Ethel");
	x.push_front("Ethel");
	x.push_front("Lucy");
	x.push_front("Fred");
	x.push_front("Ethel");
	x.push_front("Ricky");
	x.push_front("Lucy");
	x.remove("Lucy");
	x.push_front("Fred");
	x.push_front("Ricky");
	x.printAll();  // writes, one per line
					   // Ricky  Fred  Ricky  Ethel  Fred  Ethel  Ethel  Fred
}
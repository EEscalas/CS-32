#include "Map.h"

#include <iostream>
#include <string>

using namespace std;

bool combine(const Map& m1, const Map& m2, Map& result);
void subtract(const Map& m1, const Map& m2, Map& result);

void print(const Map& map)
{
	for (int m = 0; m < map.size(); m++)
	{
		KeyType key;
		ValueType value;
		map.get(m, key, value);
		cout << key << " " << value << "     ";
	}
	cout << endl;
}

int main()
{
	Map m1, m2, m3;
	m1.insert("Fred", 123);
	m1.insert("Ethel", 456);
	m1.insert("Lucy", 789);

	m2.insert("Lucy", 789);
	m2.insert("Ricky", 321);
	m2.insert("Ethel", 345);

	m3.insert("Lucy", 624);
	m3.insert("Ricky", 321);

	Map m4 = m1;

	print(m1);
	print(m4);
	//print(m2);
	//print(m3);

	/*
	cout << "Subract:" << endl;
	Map resultados;
	subtract(m1, m2, resultados);
	print(resultados);

	cout << "Combine:" << endl;
	Map result;
	if (combine(m1, m2, result))
		cout << "true" << endl;
	else cout << "false" << endl;
	print(result);*/
}
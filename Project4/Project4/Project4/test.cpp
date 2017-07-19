#include "DiskMultiMap.h"
#include "IntelWeb.h"
#include <iostream>

using namespace std;

int main()
{
	DiskMultiMap map;
	map.createNew("elena.dat", 8);
	map.insert("Elena", "Lucille", "Escalas");
	map.insert("as;dfjk", "lak;sdjf", "a;lskdjf");
	map.insert("Elena", "has", "collision");
	map.insert("m4140", "bad.exe", "awful.com");
	map.erase("m4140", "bad.exe", "awful.com");
	map.insert("Elena", "is", "deflated");
	DiskMultiMap::Iterator it;
	it = map.search("Elena");

	if (it.isValid())
		cout << "is valid!" << endl;
	else cout << "is not valid!" << endl;

	MultiMapTuple test = *it;

	cout << test.key << " " << test.value << " " << test.context << endl;

	IntelWeb web;
	

	cout << "Yay you compile!!!" << endl;
}
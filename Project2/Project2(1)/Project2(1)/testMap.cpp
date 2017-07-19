#include "Map.h"

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

bool combine(const Map& m1, const Map& m2, Map& result);
void subtract(const Map& m1, const Map& m2, Map& result);
void test();
void testCS();

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
//	testCS();
	test();
	cout << "Passed all tests" << endl;
}

void test()
{
	KeyType name, her;
	ValueType herTime, time;	
	
	Map m, m2;		// test default constructor
	
	// for an empty map

	assert(m.size() == 0);      // test size
	assert(m.empty());          // test empty
	assert(!m.erase("Ricky"));  // nothing to erase

	// filling empty map

	m.insert("Elena", 103.12);
	m.insert("Ella", 102.46);
	m.insert("Alex", 101.1);
	m.insert("Allie", 100.3);						// test insert

	assert(m.contains("Allie"));					// test contains
	assert(!m.contains("Suzanna"));
	assert(m.get("Alex", herTime));					// test get
	assert(herTime == 101.1);
	assert(m.insertOrUpdate("Julia Jane", 101.4));	// test update of insertOrUpdate
	assert(m.get("Julia Jane", herTime));
	assert(herTime == 101.4);
	assert(m.insertOrUpdate("Suzanna", 103.3));		// test insert of insertOrUpdate
	assert(m.contains("Suzanna"));
	assert(m.update("Allie", 59.99));				// test update

	// comparing and swapping maps

	m2.insert("Rafa", 1452);

	assert(m2.size() == 1);

	Map m3 = m;		// test copy constructor

	assert(m3.size() == 6);	
	assert(m3.get("Alex", herTime));
	assert(herTime == 101.1);
	assert(m3.contains("Ella"));
	assert(m3.erase("Elena"));			// test erase		
	assert(!m3.contains("Elena"));
	assert(!m3.erase("Suzanne"));
	assert(m3.erase("Suzanna"));

	Map m4 = m3;
	
	assert(m3.size() == 4);
	m3.get(1, name, herTime);				// test integer get function
	m4.get(1, her, time);
	assert(name == her && herTime == time);

	Map oneNode;	// test erasing a one node map

	assert(oneNode.insert("Marina", 110));
	assert(oneNode.erase("Marina"));
	assert(!oneNode.size() > 0);
	assert(oneNode.empty());

	m2 = m3;		// test assignment operator

	assert(m2.size() == 4);
	assert(m2.get("Alex", herTime));
	assert(m2.contains("Ella"));
	
	m2.swap(m);		// test the swap function

	assert(m.size() == 4);
	assert(m2.size() == 6);
	assert(m2.get("Alex", herTime));
	assert(m2.contains("Ella"));
	assert(m2.erase("Allie"));
	assert(m2.size() == 5);

	Map result;		// test combine function
	
	assert(combine(m, m2, result));
	assert(result.size() == 6);					// result contains each unique element only once
	assert(m2.insertOrUpdate("Ella", 100.5));
	assert(!combine(m, m2, result));
	assert(!result.contains("Ella"));			// does not contain the key in which the values are not equal
	assert(result.size() == 5);					// result contains each unique element only once
	assert(result.contains("Allie"));			// result contains element only from m
	assert(result.contains("Julia Jane"));		// result contains element only from m2
	assert(!combine(m, m2, m));					// make sure this unique case still compiles and properly sets m to result
	assert(m.size() == 5);						// m should be the same size as result now
	assert(m.contains("Julia Jane"));			// m now contains element from m2, because it is the result of the combination of the two

	subtract(m, m3, result);	// test subtract function
	
	assert(!result.contains("Ella"));									// the subraction does not include the element in m3 that is not in m
	assert(result.contains("Elena") && result.contains("Suzanna"));		// the subraction successfully eliminated all the values m3 contains that are in m
	assert(result.size() == 2);
	subtract(m, m2, m);							// make sure this unique case compiles and properly sets m to result
	assert(m.size() == 1);
	assert(m.contains("Allie"));

	
	
	
	
}

void testCS()
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

	cout << m3.size() << endl;

	Map m4 = m1;
	m4 = m2;

	cout << "m1: " << endl;
	print(m1);
	cout << "m2" << endl;
	print(m2);
//	print(m4);
//	print(m3);


	cout << "Subract:" << endl;
	Map resultados;
	subtract(m1, m2, resultados);
	print(resultados);

	cout << "Combine:" << endl;
	Map result;
	if (combine(m1, m2, result))
		cout << "true" << endl;
	else cout << "false" << endl;
	print(result);

//	if (combine(m1, m2, m1))
//		cout << "true" << endl;
//	else cout << "false" << endl;
//	print(m1);

	subtract(m1, m2, m1);
	print(m1);
}

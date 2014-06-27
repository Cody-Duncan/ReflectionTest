#include "AnyTest.h"
#include <iostream>

namespace AnyTest
{
	void BasicTest()
	{
		using namespace std;

		Any a = 42;
		cout << a.cast<int>() << endl;

		a = 13;
		cout << a.cast<int>() << endl;

		a = "hello";
		cout << a.cast<const char*>() << endl;

		a = std::string("1234567890");
		cout << a.cast<std::string>() << endl;

		int n = 42;
		a = &n;
		cout << *a.cast<int*>() << endl;

		Any b = true;
		cout << b.cast<bool>() << endl;

		swap(a, b);        
		cout << a.cast<bool>() << endl;

		a.cast<bool>() = false;
		cout << a.cast<bool>() << endl;
	}
}

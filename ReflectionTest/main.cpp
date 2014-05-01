
#include "Test.h"
#include <iostream>
#include <type_traits>
#include "Meta.h"

meta_define(Test);
meta_define(int);

int main(int argc, const char* argv[])
{
    using namespace std;

	bool result = true;
	
	//check meta by type
	if(!meta::has<Test>()) 
		printf("Test not registered by type");
	cout << meta::get<Test>()->Name() << endl << meta::get<Test>()->Size() << endl;
	cout << endl;

	//check meta by object
	Test a;
	if (!meta::has(a))
		printf("Test not registered by object");
	const meta::Type* t = meta::get(a);
	cout << t->Name() << endl << t->Size() << endl;
	cout << endl;

	//check meta by string name
	std::string type = "Test";
	if (!meta::has(type))
		printf("Test not registered by string name");
	cout << meta::get(type)->Name() << endl << meta::get(type)->Size() << endl;
	cout << endl;

	const char* typebychar = "Test";
	
	//check meta by c-string name
	if (!meta::has(typebychar))
		printf("Test not registered by c-string name");
	cout << meta::get(typebychar)->Name() << endl << meta::get(typebychar)->Size() << endl;
	cout << endl;

	//check int type
	cout << meta::get<int>()->Name() << endl << meta::get<int>()->Size() << endl;
	cout << endl;
}
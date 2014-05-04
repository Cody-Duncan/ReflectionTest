
#include "Test.h"
#include <iostream>
#include <type_traits>
#include "Meta.h"



void Test1()
{
	using namespace std;

	bool result = true;

	//check meta by type
	if (!meta::has<Test>())
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
	if (!meta::has_name(type))
		printf("Test not registered by string name");
	cout << meta::get_name(type)->Name() << endl << meta::get_name(type)->Size() << endl;
	cout << endl;

	const char* typebychar = "Test";

	//check meta by c-string name
	if (!meta::has_name(typebychar))
		printf("Test not registered by c-string name");
	cout << meta::get_name(typebychar)->Name() << endl << meta::get_name(typebychar)->Size() << endl;
	cout << endl;

	//check int type
	cout << meta::get<int>()->Name() << endl << meta::get<int>()->Size() << endl;
	cout << endl;


	//print out Test's structure.
	cout << meta::get<Test>()->Name() << " contains members: " << endl;
	for (const meta::Member* m : meta::get<Test>()->members)
	{
		cout << "    " << m->TypeName() << " " << m->Name() << endl;
	}
}



int main(int argc, const char* argv[])
{
	using namespace std;

	Test1();
	cout << endl;

	cout << meta::get("stuff")->Name() << endl;
	cout << meta::get("stuff")->Size() << endl;
}
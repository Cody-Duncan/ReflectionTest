
// https://github.com/EngineArchitectureClub/TalkSlides/tree/master/2013

#include "Test.h"
#include <iostream>
#include <type_traits>
#include "SerializationTest.h"
#include "FunctionTest.h"

void BasicTypeTest()
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
	const meta::TypeInfo* t = meta::get(a);
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

	cout << endl;
}


void TestVariant()
{
	using namespace std;

	meta::RefVariant v = 1;

	std::cout << "RefVariant Value: " << v.GetValue<int>() << endl;

	v = true;
	std::cout << "RefVariant Value: " << v.GetValue<bool>() << endl;

	std::string originalstr("This be a string\n");
	v = originalstr; //must be assigned an lvalue
	std::cout << "RefVariant Value: " << v.GetValue<std::string>() << endl;
}


int main(int argc, const char* argv[])
{
	//BasicTypeTest();
	//TestVariant();
	TestDeSerialization();
	FunctionSignatureTest();

	return 0;
}
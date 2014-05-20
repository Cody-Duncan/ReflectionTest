#include "FunctionMeta2.h"
#include <iostream>

std::string printVars(char c, double d, int i)
{
	std::cout << c + d + i << std::endl;
	return "somestring";
}

void Test1()
{
	/*
	meta::FunctionSignature thefunc = meta::FunctionSignature(printVars);

	printf("Function return type: %s\n", thefunc.returnType->Name().c_str());
	for (unsigned int i = 0; i < thefunc.argCount; ++i)
	{
		printf("   Type%d: %s \n", i, thefunc.argArray[i]->Name().c_str());
	}
	*/
}

class TestWithMethod
{
public:

	std::string printVars(char c, double d, int i)
	{
		std::cout << c + d + i << std::endl;
		return "somestring";
	}

	meta_expose_internal(TestWithMethod);
};

meta_define(TestWithMethod)
{
}

void FunctionSignatureTest()
{
	//VarMethod(const char* name, ReturnType (Type::*method)(Args...))

	meta::Method* m = meta::bindMethod("printVars", &TestWithMethod::printVars);
}

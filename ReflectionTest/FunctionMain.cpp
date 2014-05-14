#include "FunctionMeta.h"
#include <iostream>

std::string printVars(char c, double d, int i)
{
	std::cout << c + d + i << std::endl;
	return "somestring";
}

void FunctionSignatureTest()
{
	meta::FunctionSignature thefunc = meta::FunctionSignature(printVars);

	printf("Function return type: %s\n", thefunc.returnType->Name().c_str());
	for (unsigned int i = 0; i < thefunc.argCount; ++i)
	{
		printf("   Type%d: %s \n", i, thefunc.argArray[i]->Name().c_str());
	}	
}

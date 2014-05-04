#pragma once

#include "Meta.h"

class Test
{
public:
    int a;
    int b;

    Test() : a(0), b(0) {}
    Test(int _a, int _b) : a(_a), b(_b) {}

    int product() { return a*b; }
    double sum() { return (double)a + (double)b; }

	meta_expose_internal(Test);

private:
	int c;
};

meta_define(Test)
{
	meta_add_member(a);
	meta_add_member(b);
	meta_add_member(c);
}
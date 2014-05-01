#pragma once

class Test
{
public:
    int a;
    int b;

    Test() : a(0), b(0) {}
    Test(int _a, int _b) : a(_a), b(_b) {}

    int product() { return a*b; }
    double sum() { return (double)a + (double)b; }
};
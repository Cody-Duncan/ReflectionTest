#pragma once

#include "include\jansson.h"
#include <string>
#include "Meta.h"

struct Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3(const float* arr) : x(arr[0]), y(arr[1]), z(arr[2]) {}

	meta_expose_internal(Vector3);
};

enum ThingType
{
	Thing_Banana,
	Thing_Apple
};

class Thing
{
public:
	int size;
	std::string name;
	float radius;
	double height;
	Vector3 position;

	meta_expose_internal(Thing);
};

void TestSerialization();
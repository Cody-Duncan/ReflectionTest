#include "SerializationTest.h"

meta_define(Vector3)
{
	meta_add_member(x);
	meta_add_member(y);
	meta_add_member(z);
}

meta_define(Thing)
{
	meta_add_member(size);
	meta_add_member(name);
	meta_add_member(radius);
	meta_add_member(height);
	meta_add_member(position);
}

void TestSerialization()
{
	return;
}
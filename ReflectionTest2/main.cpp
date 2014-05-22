
#include "Meta.h"
#include <assert.h>

// a test class
class A1
{
	int a;
	float b;

	int bar(float method) { return (int)(method * 0.5f); }
	float baz(double d, char is_const) { return d > (double)is_const ? (float)(d * 0.5) : 0.f; }

public:
	int getA() const { return a; }
	void setA(int _) { a = _; }

	float getB() const { return b; }

	void foo() { a *= 3; }

	meta_DECLARE(A);
};

meta_DEFINE(A1)
	.member("a", &A1::a) // note that we can bind private m_Members with this style of Get
	.member("b", &A1::b)
	.method("foo", &A1::foo)
	.method("bar", &A1::bar)
	.method("baz", &A1::baz);

// tests that a member variable has a particular value
template <typename T, typename U> static void test_ro_member(T& obj, const char* name, const U& value)
{
	auto m = meta::Get(obj)->FindMember(name);
	assert(m != nullptr);
	assert(m->CanGet(&obj));
	U test = meta::any_cast<U>(m->Get(&obj));
	assert(test == value);
}

int main(int argc, const char* argv[])
{
	A1 a;
	a.setA(12);
	test_ro_member(a, "a", a.getA());
}

#include "Meta.h"
#include <assert.h>
#include <iostream>

// a test class
class A1
{
	int a;
	float b;
	char c;

	int bar(float method) { return (int)(method * 0.5f); }
	float baz(double d, char is_const) { return d > (double)is_const ? (float)(d * 0.5) : 0.f; }

public:
	int getA() const { return a; }
	void setA(int _) { a = _; }

	float getB() const { return b; }

	void foo() { a *= 3; }

	void stuff(int a, double b, char c) { std::cout << a + b + c << std::endl;  }

	meta_DECLARE(A1);
};

meta_DEFINE(A1)
.member("a", &A1::a) // note that we can bind private m_Members with this style of Get
.member("b", &A1::b)
.member("c", &A1::c)
.method("foo", &A1::foo)
.method("bar", &A1::bar)
.method("baz", &A1::baz)
.method("stuff", &A1::stuff);

// tests that a member variable has a particular value
template <typename T, typename U> static void test_ro_member(T& obj, const char* name, const U& value)
{
	auto m = meta::Get(obj)->FindMember(name);
	assert(m != nullptr);
	assert(m->CanGet(&obj));
	U test = meta::any_cast<U>(m->Get(&obj));
	assert(test == value);
}

template <typename T> static void test_method(T& obj, const char* name)
{
	meta::Method* m = meta::Get(obj)->FindMethod(name);
	assert(m != nullptr);
	meta::Any argv[3] = { 1, 2.0, 'a'};
	assert(m->CanCall(&obj, 3, argv));
	assert(m->CanCall(obj, 1, 2.0, 'a'));
	m->Call(obj, argv);
	m->Call(obj, 1, 2.0, 'a');
}

void TestMeta_1()
{
	A1 a;
	a.setA(12);
	test_ro_member(a, "a", a.getA());

	//check A1 type
	const meta::TypeInfo* aInfo = meta::Get<A1>();
	std::cout << "Info on type: " <<  aInfo->GetName() <<  std::endl;
	assert(meta::Get<A1>()->GetSize() == sizeof (A1));

	//check A1 members
	
	for(unsigned int i = 0; i < aInfo->GetMembers().size(); ++i)
	{
		const meta::Member* m = aInfo->GetMembers()[i];
		std::cout << m->GetType()->GetName() << " " << m->GetName() << std::endl;
	}

	//Check A1 methods
	for(unsigned int i = 0; i < aInfo->GetMethods().size(); ++i)
	{
		const meta::Method* m = aInfo->GetMethods()[i];
		std::cout << m->GetReturnType().type->GetName() << " " << m->GetName() << "(";
		for(int k = 0; k < m->GetArity(); ++k)
		{
			std::cout << m->GetParamType(k).type->GetName();
			if(k != m->GetArity() - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << ")" << std::endl;
	}
	
	std::cout << std::endl;
	std::cout << "Get value of \"b\"" << std::endl;
	meta::Member* BMember = meta::Get(a)->FindMember("b");
	meta::Any val = BMember->Get(a);
	std::cout << val.GetReference<float>() << std::endl << std::endl;

	std::cout << "Call private method \"stuff\"" << std::endl;
	test_method(a, "stuff");
	std::cout << std::endl;
}

int main(int argc, const char* argv[])
{
	TestMeta_1();

	std::cout << "any_cast and any(3) to int." << std::endl;
	meta::Any b = 3;
	std::cout << meta::any_cast<int>(b) << std::endl;
	std::cout << std::endl;

	return 0;
}
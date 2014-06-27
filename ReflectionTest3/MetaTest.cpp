#include "MetaTest.h"
#include "Meta.h"
#include <iostream>
#include <assert.h>
#include "MacroHelpers.h"
#include "AnyTest.h"


namespace MetaTest
{
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

		//meta_DECLARE(A1);
		public: 
			struct MetaStaticHolder { static const meta::StaticTypeInfo s_TypeInfo; }; 
			virtual const meta::TypeInfo*  GetType() const { return &MetaStaticHolder::s_TypeInfo; }
	};

	//meta_DEFINE(A1)
	const meta::StaticTypeInfo A1::MetaStaticHolder::s_TypeInfo = meta::internal::TypeInfoBuilder<A1, true>(STR(A1), sizeof(A1))
	.member("a", &A1::a) // note that we can bind private m_Members with this style of Get
	.member("b", &A1::b)
	.member("c", &A1::c)
	.method("foo", &A1::foo)
	.method("bar", &A1::bar)
	.method("baz", &A1::baz)
	.method("stuff", &A1::stuff);

	void Test1()
	{
		A1 a;
		a.setA(12);
	
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
	}
}
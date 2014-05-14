#include "Meta.h"
#include "indices.h"
namespace meta
{
	struct FunctionSignature
	{
		const unsigned argCount;
		const Type** argArray;
		const Type* returnType;
		const Type* context;

		class Method
		{
			const char* m_Name;
			const Type* m_Owner;

			void SetOwner(const Type* m_Type) { m_Owner = m_Type; }

		protected:
			Method(const char* name) : m_Name(name) {}
			virtual ~Method() {}

		};

		//Regular Functions
		template<typename retType, typename... Args>
		FunctionSignature(retType(*)(Args... vs)) :
			returnType(meta::get<retType>()),
			context(NULL),
			argCount(sizeof...(Args))
		{
			static const Type* stat_args[sizeof...(Args)] =
			{
				meta::get<Args>()... //expands to meta::get<Arg_0>(), meta::get<Arg_1>(), and so on
			};
			argArray = stat_args;
		}

		//Class Functions (includes context)
		template<typename contextType, typename retType, typename... Args>
		FunctionSignature(retType(*)(Args... vs)) :
			returnType(meta::get<retType>()),
			context(meta::get<contextType>()),
			argCount(sizeof...(Args))
		{
			static const Type* stat_args[sizeof...(Args)] =
			{
				meta::get<Args>()... //expands to meta::get<Arg_0>(), meta::get<Arg_1>(), and so on
			};
			argArray = stat_args;
		}

	};

#define CALL_PTR(PTR, FN) ((PTR)->*(FN))

	///No Object
	template<typename returnType, typename... Args, unsigned int... Is>
	void Call(
		returnType(*fn)(Args...),
		RefVariant& ret,
		RefVariant* args,
		unsigned argCount,
		indices<Is...>)
	{
		assert(argCount == sizeof...(Args));
		fn(args[Is].GetValue<Args>()...);
	}

	template<typename returnType, typename... Args>
	void Call(
		returnType(*fn)(Args...),
		RefVariant& binding,
		RefVariant& ret,
		RefVariant* args,
		unsigned argCount)
	{
		Call(fn, binding, ret, args, argCount, build_indices<sizeof...(Args)>{});
	}
}

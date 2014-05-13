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

		//Regular Functions
		template<typename retType, typename... Args>
		FunctionSignature(retType (*)(Args... vs)) :
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


	struct GenericBase1{};
	struct GenericBase2{};
	struct GenericBaseMultiVirtualClass : public virtual GenericBase1, public GenericBase2 {};
	struct GenericMultiClass : public GenericBase1, public GenericBase2 {};
	struct GenericClass{};
	struct GenericUnknownClass;

	typedef void(GenericUnknownClass::*UnknownClassFn)		(void);
	typedef void(GenericMultiClass::*MultiClassFn)			(void);
	typedef void(GenericBaseMultiVirtualClass::*MultiVirtFn)(void);
	typedef void(GenericClass::*GenericClassFn)				(void);
	typedef void(*StaticFn)									(void);


	union MultiFnPtr
	{
		UnknownClassFn	UnknownClassFn;
		MultiClassFn	MultiClassFn;
		MultiVirtFn		MultiVirtFn;
		GenericClassFn	GenenricClassFn;
		StaticFn		StaticFn;
	};

#define CALL_PTR(PTR, FN) ((PTR)->*(FN))
	
	template<typename contextType, typename returnType, typename... Args, unsigned int... Is>
	void Call(
		returnType(contextType::*fn)(Args...),
		RefVariant& binding,
		RefVariant& ret,
		RefVariant* args,
		unsigned argCount,
		indices<Is...>)
	{
		assert(argCount == sizeof...(Args));
		CALL_PTR(binding.GetValue<contextType*>(), fn)(args[Is].GetValue<Args>()...);
	}

	template<typename contextType, typename returnType, typename... Args>
	void Call(
		returnType(contextType::*fn)(Args...),
		RefVariant& binding,
		RefVariant& ret,
		RefVariant* args,
		unsigned argCount)
	{
		Call(fn, binding, ret, args, argCount, build_indices<sizeof...(Args)>{});
	}

	//stores a function pointer for proper typecasting of the MultiFnPtr
	template<typename Signature>
	void CallHelper(
		const MultiFnPtr& fn,	//MultiFnPtr union
		RefVariant& binding,	//pointer to object as RefVariant
		RefVariant& ret,		//Copy Return value to here.
		RefVariant* args,		//Pass in arguments
		unsigned argCount)
	{
		Call(*reinterpret_cast<Signature*>(&const_cast<MultiFnPtr&>(fn)), binding, ret, args, argCount);
	}


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

	//stores a function pointer for proper typecasting of the MultiFnPtr
	template<typename Signature>
	void CallHelper(
		const MultiFnPtr& fn,	//MultiFnPtr union
		RefVariant& ret,		//Copy Return value to here.
		RefVariant* args,		//Pass in arguments
		unsigned argCount)
	{
		Call(*reinterpret_cast<Signature*>(&const_cast<MultiFnPtr&>(fn)), ret, args, argCount);
	}
}
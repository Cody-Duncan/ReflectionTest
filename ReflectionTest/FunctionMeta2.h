#pragma once

#include "Meta.h"
#include "indices.h"

namespace meta
{
	class Method
	{
		const char* m_Name;			// The name of the method
		const TypeInfo* m_Owner;	// The type that owns the method.

		void SetOwner(const TypeInfo* m_Type) { m_Owner = m_Type; }

	protected:
		Method(const char* name) : m_Name(name) {}
		virtual ~Method();

		virtual Any DoCall(const Any& obj, int argc, const Any* argv) const = 0;

	public:
		const char* GetName() const { return m_Name; }

		const TypeInfo* GetOwner() const { return m_Owner; }

		virtual TypeRecord GetReturnType() const = 0;

		virtual TypeRecord GetParamType(int i) const = 0;

		virtual int GetArity() const = 0;

		inline Any Call(const Any& obj, int argc, const Any* argv) const;

		inline bool CanCall(const Any& obj, int argc, const Any* argv) const;

		friend class TypeInfo;
	};

	Any Method::Call(const Any& obj, int argc, const Any* argv) const
	{
		return DoCall(obj, argc, argv);
	}

	bool Method::CanCall(const Any& obj, int argc, const Any* argv) const
	{
		//if (!obj.GetType()->IsSameOrDerivedFrom(m_Owner))
			//return false;

		if (argc != GetArity())
			return false;

		for (int i = 0; i < argc; ++i)
		{
			auto tr = GetParamType(i);
			if (argv[i].GetType() != tr.type)
				return false;
			if (argv[i].IsConst() && tr.qualifier == TypeRecord::Pointer)
				return false;
		}

		return true;
	}

	namespace internal
	{
		template <typename Type, typename ReturnType, typename... Args, unsigned int... Is>
		Any Call(ReturnType(Type::*method)(Args...), Type* obj, int argc, const Any* argv, indices<Is...>)
		{
			return internal::make_any<ReturnType>::make(
				(obj->*method)( *argv[Is].GetPointer<Args>()... )
			);
		}

		template <typename Type, typename ReturnType, typename... Args>
		Any Call(ReturnType(Type::*method)(Args...), Type* obj, int argc, const Any* argv)
		{
			return Call(method, obj, argc, argv, build_indices<sizeof...(Args)>{});
		}

		template<typename Type, typename ReturnType, typename... Args>
		class VarMethod : public Method
		{
			ReturnType (Type::*method)(Args...);

		public:
			
			virtual TypeRecord GetReturnType() const override { return internal::make_type_record<ReturnType>::type(); }
			virtual TypeRecord GetParamType(int i) const override { return TypeRecord(); }
			virtual int GetArity() const override { return sizeof...(Args); }

			VarMethod(const char* name, ReturnType (Type::*method)(Args...)) : 
				Method(name), 
				method(method) {}
			
			virtual Any DoCall(const Any& obj, int argc, const Any* argv) const 
			{ 
				return meta::internal::Call(method, obj.GetPointer<Type>(), argc, argv); 
			}
		};
	}

	template<typename Type, typename ReturnType, typename... Args>
	Method* bindMethod(const char* name, ReturnType(Type::*method)(Args...))
	{
		return new meta::internal::VarMethod<Type, ReturnType, Args...>(name, method);
	}
}
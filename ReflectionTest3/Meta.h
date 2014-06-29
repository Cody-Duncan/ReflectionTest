#pragma once

#include <vector>
#include <unordered_map>
#include "MetaUtil.h"
#include "TypeRecord.h"

namespace meta
{

	//////////////////////////////////////////////////////////////////////////////
	//  Member
	//////////////////////////////////////////////////////////////////////////////

	class Member
	{
	protected:
		const char* m_Name;		 
		const TypeInfo* m_Owner; // The type this member is apart of.
		const TypeInfo* m_Type;  // This member's type.

	public:
		Member(const char* name, const TypeInfo* type) : m_Name(name), m_Owner(nullptr), m_Type(type) {}
		virtual ~Member() {}

		void SetOwner(const TypeInfo* newOwner)
		{
			m_Owner = newOwner;
		}

		//Accessors
		const char* GetName() const		{ return m_Name; }
		std::string GetNameStr() const	{ return std::string(m_Name); }
		const TypeInfo* GetType() const { return m_Type; }
		const char* GetTypeName() const;
		const std::string GetTypeNameStr() const;
	};



	//////////////////////////////////////////////////////////////////////////////
	//  Method
	//////////////////////////////////////////////////////////////////////////////

	class Method
	{
	protected:
		const char* m_Name;		 
		const TypeInfo* m_Owner; // The type this method is apart of.

	public:
		Method(const char* name) : m_Name(name), m_Owner(nullptr)
		{}
		virtual ~Method()
		{}

		void SetOwner(const TypeInfo* newOwner)
		{
			m_Owner = newOwner;
		}

		// Accessors

		const char* GetName() const		{ return m_Name; }
		std::string GetNameStr() const	{ return std::string(m_Name); }
		const TypeInfo* GetOwner() const { return m_Owner; }
		virtual TypeRecord GetReturnType() const = 0;
		virtual TypeRecord GetParamType(int i) const = 0;
		virtual int GetArity() const = 0;
	};


	//////////////////////////////////////////////////////////////////////////////
	//  TypeInfo
	//////////////////////////////////////////////////////////////////////////////

	class TypeInfo
	{
	protected:
		static std::unordered_map<std::string, TypeInfo*>* sTypeInfoDictionary;

		const char* m_Name;
		size_t m_ByteSize;

		//base classes
		std::vector<Member*> m_Members;	// Member variables of this type.
		std::vector<Method*> m_Methods; // Methods of this type.

	public:
		static std::unordered_map<std::string, TypeInfo*>& GetTypeInfoDictionary()
		{
			if(sTypeInfoDictionary == nullptr)
			{
				sTypeInfoDictionary = new std::unordered_map<std::string, TypeInfo*>();
				sTypeInfoDictionary->rehash(256);
			}
			return *sTypeInfoDictionary;
		}

		TypeInfo(const char* name, size_t size) : m_ByteSize(size)
		{
			m_Name = metaUtil::TrimTypeToName(name);
		}

		TypeInfo(const TypeInfo& other) :
			m_Name(other.m_Name),
			m_ByteSize(other.m_ByteSize),
			m_Members(other.m_Members),
			m_Methods(other.m_Methods)
		{
			for(auto member : m_Members)
			{
				member->SetOwner(this);
			}

			for(auto method : m_Methods)
			{
				method->SetOwner(this);
			}
		}

		//Accessors
		const char* GetName() const		{ return m_Name; }
		std::string GetNameStr() const	{ return std::string(m_Name); }
		size_t GetSize() const			{ return m_ByteSize; }

		const std::vector<Member*>& GetMembers() const { return m_Members; }
		const std::vector<Method*>& GetMethods() const { return m_Methods; }


		/// <summary>
		/// Finds a member variable of this type.
		/// </summary>
		/// <param name="name">The name of the variable.</param>
		/// <returns>The member named. Nullptr if no such member exists</returns>
		Member* FindMember(const char* name) const;
		Member* FindMember(std::string& name) const;

		/// <summary>
		/// Finds a member variable of this type.
		/// </summary>
		/// <param name="name">The name of the variable.</param>
		/// <returns>The member named. Nullptr if no such member exists</returns>
		Method* FindMethod(const char* name) const;
		Method* FindMethod(std::string& name) const;

	};


	///////////////////////////////////////////////////////////////////////////////////////
	//  StaticTypeInfo - Static TypeInfo objects that act as the base storage of metadata.
	///////////////////////////////////////////////////////////////////////////////////////

	class StaticTypeInfo : public TypeInfo
	{
	public:
		/// <summary>
		/// Constructor for <see cref="TypeInfo"/> class.
		/// </summary>
		/// <param name="name">The name of the type.</param>
		StaticTypeInfo(const char* name, size_t size)
			:TypeInfo(name, size)
		{
			GetTypeInfoDictionary().insert( std::make_pair(GetNameStr(), this) );
		}

		/// <summary>
		/// Copy constructor for <see cref="TypeInfo"/>.
		/// </summary>
		/// <param name="type">The typeinfo to copy.</param>
		StaticTypeInfo(const TypeInfo& type) : 
			TypeInfo(type)
		{
			GetTypeInfoDictionary().insert( std::make_pair(GetNameStr(), this) );
		}
	};

	inline TypeInfo* Get_Name(const std::string& name)
	{
		auto result = TypeInfo::GetTypeInfoDictionary().find(name);
		if( result != TypeInfo::GetTypeInfoDictionary().end() )
		{
			return result->second;
		}
		return nullptr;
	}
	
	inline TypeInfo* Get_Name(const char* name)
	{
		return Get_Name(std::string(name));
	}

	//////////////////////////////////////////////////////////////////////////////
	//  MetaHolder - for info on primitives, and types we have no access to.
	//////////////////////////////////////////////////////////////////////////////

	namespace internal
	{
		/// <summary>
		/// Holder for TypeInfo external to a type, 
		/// used when adding introspection to types that cannot be modified.
		/// </summary>
		template <typename Type>
		struct MetaHolder
		{
			static const StaticTypeInfo s_TypeInfo;
		};
	}


	//////////////////////////////////////////////////////////////////////////////
	//  Meta.inl - define functions that need to access TypeInfo
	//////////////////////////////////////////////////////////////////////////////
	#include "Meta.inl"


	// *********************** Member Implementations ***********************
	namespace internal
	{
		// A member that is accessed directly (not through getter or setter)
		template <typename Type, typename MemberType> 
		class TypeMember : public Member
		{
			MemberType Type::*m_Member;

		public:
			TypeMember(const char* name, const TypeInfo* type, MemberType Type::*member) : 
				Member(name, type), 
				m_Member(member)
			{}

			//Get
			//Set
		};
	}

	// *********************** Variadic Call (Method) Implementation *********************** //
	
	namespace internal
	{
	//	//Call_Internal - The final destination for a Call on a method. Actually calls the method.
	//	
	//	template <typename Type, typename ReturnType, typename... Args, unsigned int... Is>
	//	Any Call_Internal( ReturnType(Type::*method)(Args...), Type* obj, const Any* argv, indices<Is...> )
	//	{
	//		return internal::make_any<ReturnType>::make( (obj->*method)( *argv[Is].GetPointer<Args>()... ) );
	//	}

	//	template <typename Type, typename... Args, unsigned int... Is>
	//	void Call_Internal( void (Type::*method)(Args...), Type* obj, const Any* argv, indices<Is...> )
	//	{
	//		return  (obj->*method)( *argv[Is].GetPointer<Args>()... );
	//	}


	//	//Call - forwards a call to Call_Internal, generating a variadic non-type template containing the indices for the arguments.
	//	// Uses what seems to get dubbed "The indices trick". 

	//	template <typename Type, typename ReturnType, typename... Args>
	//	Any Call(ReturnType(Type::*method)(Args...), Type* obj, const Any* argv)
	//	{
	//		assert((sizeof...(Args) > 0 && argv != nullptr) ||		// if has args, must not have null argv.
	//			(sizeof...(Args) == 0 && argv == nullptr));			// if no args, must have null argv.
	//		return Call_Internal(method, obj, argv, build_indices<sizeof...(Args)>{});
	//	}

	//	template <typename Type, typename... Args>
	//	void Call(void (Type::*method)(Args...), Type* obj, const Any* argv)
	//	{
	//		assert((sizeof...(Args) > 0 && argv != nullptr) ||		// if has args, must not have null argv.
	//			(sizeof...(Args) == 0 && argv == nullptr));			// if no args, must have null argv.
	//		return Call_Internal(method, obj, argv, build_indices<sizeof...(Args)>{});
	//	}
		
		/// <summary>
		/// Variadic Method that can accept any args, and a return type.
		/// </summary>
		template<typename Type, typename ReturnType, typename... Args> class VarMethod : public Method
		{
			ReturnType (Type::*method)(Args...);

		public:
			
			virtual TypeRecord GetReturnType() const override { return internal::make_type_record<ReturnType>::type(); }
			virtual TypeRecord GetParamType(int i) const override 
			{
				return meta::internal::GetTypeRecordByIndex<sizeof...(Args), Args...>(i);
			}
			virtual int GetArity() const override { return sizeof...(Args); }

			VarMethod(const char* name, ReturnType (Type::*method)(Args...)) : 
				Method(name), 
				method(method) {}
			
	//		virtual Any DoCall(const Any& obj, const Any* argv) const 
	//		{ 
	//			return meta::internal::Call(method, obj.GetPointer<Type>(), argv); 
	//		}
		};

		/// <summary>
		/// Variadic Method that can accept any args, and a void return.
		/// </summary>
		template<typename Type, typename... Args> class VarMethod<Type, void, Args...> : public Method
		{
			void (Type::*method)(Args...);

		public:
			
			virtual TypeRecord GetReturnType() const override { return internal::make_type_record<void>::type(); }
			virtual TypeRecord GetParamType(int i) const override 
			{
				return meta::internal::GetTypeRecordByIndex<sizeof...(Args), Args...>(i);
			}
			virtual int GetArity() const override { return sizeof...(Args); }

			VarMethod(const char* name, void (Type::*method)(Args...)) : 
				Method(name), 
				method(method) {}
			
	//		virtual Any DoCall(const Any& obj, const Any* argv) const 
	//		{ 
	//			meta::internal::Call(method, obj.GetPointer<Type>(), argv); 
	//			return Any();
	//		}
		};
	}

	template<typename Type, typename ReturnType, typename... Args>
	Method* bindMethod(const char* name, ReturnType(Type::*method)(Args...))
	{
		return new meta::internal::VarMethod<Type, ReturnType, Args...>(name, method);
	}

	//////////////////////////////////////////////////////////////////////////////
	//  TypeInfoBuilder - Builds the MetaInformation.
	//////////////////////////////////////////////////////////////////////////////

	namespace internal
	{
		template <typename Type, bool IsClass> 
		struct TypeInfoBuilder : public TypeInfo
		{
			TypeInfoBuilder(const char* name, size_t size) : 
				TypeInfo(name, size)
			{}

			/// <summary>
			/// Add a member meta to this type.
			/// </summary>
			/// <param name="name">The name of the member.</param>
			/// <param name="member">A pointer to the member.</param>
			/// <returns>Returns a reference to this TypeInfoBuilder.</returns>
			template <typename MemberType> typename std::enable_if<!std::is_member_function_pointer<MemberType>::value, TypeInfoBuilder&>::type member(const char* name, MemberType Type::*member)
			{
				m_Members.push_back(new TypeMember<Type, typename std::remove_reference<MemberType>::type>(name, Get<MemberType>(), member)); 
				return *this;
			}

			/// <summary>
			/// Add a method meta definition to this type.
			/// </summary>
			/// <param name="name">The name of the method.</param>
			/// <param name="method">A pointer to the member ( &Type::method ).</param>
			/// <returns>Returns a reference to this TypeInfoBuilder.</returns>
			template<typename Type, typename ReturnType, typename... Args> TypeInfoBuilder& method(const char* name, ReturnType(Type::*method)(Args...))
			{
				//Not implemented yet
				m_Methods.push_back(bindMethod(name, method)); 
				return *this;
			}
		};

		// TypeInfoBuilder - Pointer types (for c-string types)
		template <typename Type> struct TypeInfoBuilder<Type*, true> : public TypeInfo
		{
			TypeInfoBuilder(const char* name, size_t size) : TypeInfo(name, size)
			{}
		};

		// TypeInfoBuilder - Primitives and other classes that cannot have base types, member variables, or member functions.
		template <typename Type> struct TypeInfoBuilder<Type, false> : public TypeInfo
		{
			TypeInfoBuilder(const char* name, size_t size) : TypeInfo(name, size)
			{}
		};
	}
}

// Add to a type declaration to make it participate in polymorphic type lookups and to allow binding private members and methods.
// T - The type to annotate.
#define meta_DECLARE(T) \
	public: \
		struct MetaStaticHolder { static const meta::StaticTypeInfo s_TypeInfo; }; \
		virtual const ::meta::TypeInfo*  GetType() const { return &MetaStaticHolder::s_TypeInfo; }

// Put outside of any declaration to begin annotating a type.
// T - The type to annotate.
#define meta_DEFINE_EXTERN(T) template<> const meta::StaticTypeInfo meta::internal::MetaHolder<T>::s_TypeInfo = ::meta::internal::TypeInfoBuilder<T, !std::is_fundamental<T>::value>(#T , sizeof(T))

// Put outside of any declaration to begin annotating a type marked up with META_DECLARE(T)
//T - The type to annotate.
#define meta_DEFINE(T) const ::meta::StaticTypeInfo T::MetaStaticHolder::s_TypeInfo = ::meta::internal::TypeInfoBuilder<T, true>(#T, sizeof(T))
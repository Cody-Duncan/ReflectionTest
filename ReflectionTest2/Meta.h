#pragma once

#include <vector>
#include <assert.h>
#include "Any.h"
#include "Indices.h"
#include <unordered_map>

namespace meta
{
	class TypeInfo;

	//////////////////////////////////////////////////////////////////////////////
	//  Member
	//////////////////////////////////////////////////////////////////////////////

	// Represents a member variable of a type. */
	class Member
	{
		const char* m_Name;		 // The name of this variable.
		const TypeInfo* m_Owner; // The type this member variable belongs to.
		const TypeInfo* m_Type;  // The type of this member variable.

	protected:

		/// <summary>
		/// Constructor for <see cref="Member"/>.
		/// </summary>
		/// <param name="name">The name.</param>
		/// <param name="type">The type.</param>
		Member(const char* name, const TypeInfo* type) : m_Name(name), m_Type(type)
		{}
		virtual ~Member()
		{}

		/// <summary> Reimplement to get the value of a member variable. </summary>
		/// <param name="obj">Object that owns the member.</param>
		/// <returns>Value of the member.</returns>
		virtual Any DoGet(const Any& obj) const = 0;

		/// <summary>
		/// Reimplement to set the value of a member variable.
		/// </summary>
		/// <param name="obj"> Object that owns the member.</param>
		/// <param name="in">[IN] Value to set to.</param>
		virtual void DoSet(const Any& obj, const Any& in) const = 0;

		/// <summary>
		/// Sets the owner type.  Only to be called by TypeInfo.
		/// </summary>
		void SetOwner(const TypeInfo* owner)
		{
			m_Owner = owner;
		}

	public:
		// Checks if the variable is mutable (the value can be set).
		virtual bool IsMutable() const = 0;

		const char* GetName() const { return m_Name; }
		const std::string GetNameStr() const { return std::string(m_Name); }
		const char* GetTypeName() const; 
		const std::string GetTypeNameStr() const;

		// Retrieves the type of the member variable.
		const TypeInfo* GetType() const { return m_Type; }

		// Retrieves the owner of the member variable.
		const TypeInfo* GetOwner() const { return m_Owner; }

		/// <summary>
		/// Tests if the variable value can be retrieved into the given output any ref.
		/// </summary>
		/// <param name="obj">An instance of the owner type.</param>
		/// <returns>True if Get is safe to call with these arguments, false otherwise.</returns>
		inline bool CanGet(const Any& obj) const;

		/// <summary>
		/// Retrieves the value of the member variable.
		/// </summary>
		/// <param name="obj">An instance of the owner type.</param>
		/// <returns></returns>
		inline Any Get(const Any& obj) const;

		/// <summary>
		/// Tests if the variable can be set from the given input value.
		/// </summary>
		/// <param name="obj">An instance of the owner type.</param>
		/// <param name="in">An instance of the variable type to set.</param>
		/// <returns>True if Set is safe to call with these arguments, false otherwise.</returns>
		inline bool CanSet(const Any& obj, const Any& in) const;

		/// <summary>
		/// Sets the value of the member variable.
		/// </summary>
		/// <param name="obj">An instance of the owner type.</param>
		/// <param name="value">An instance of the variable type to set.</param>
		inline void Set(const Any& obj, const Any& value) const;

		friend class TypeInfo;
	};

	



	//////////////////////////////////////////////////////////////////////////////
	//  Method
	//////////////////////////////////////////////////////////////////////////////

	// A method attach to a type.
	class Method
	{
		const char* m_Name;		 // The name of the method.
		const TypeInfo* m_Owner; // The type that owns the method.

		/// <summary>
		/// Sets the owner type.  Only to be called by TypeInfo.
		/// </summary>
		void SetOwner(const TypeInfo* m_Type)
		{
			m_Owner = m_Type;
		}

	protected:
		/// <summary>
		/// Constuctor for <see cref="Method"/>.
		/// </summary>
		/// <param name="name">The name of the method.</param>
		Method(const char* name) : m_Name(name)
		{}

		virtual ~Method()
		{}

		/// <summary>
		/// Override to implement calling the method.
		/// </summary>
		/// <param name="obj">The memory location of an instance of the owner object.</param>
		/// <param name="argv">A list of any references for the arguments.</param>
		/// <returns> The return value as an Any</returns>
		virtual Any DoCall(const Any& obj, const Any* argv) const = 0;

	public:

		const char*     GetName() const { return m_Name; }
		const std::string GetNameStr() const { return std::string(m_Name); }
		const TypeInfo* GetOwner() const { return m_Owner; }

		// Get the TypeInfo of the return value.
		virtual TypeRecord GetReturnType() const = 0;

		/// <summary>
		/// Get the TypeInfo of the parameter at the i-th index.
		/// </summary>
		/// <param name="i">The index of the parameter, starting from 0.</param>
		/// <returns></returns>
		virtual TypeRecord GetParamType(int i) const = 0;

		/// <summary>
		/// Get the number of parameters of the method.
		/// </summary>
		virtual int GetArity() const = 0;

		/// <summary>
		/// Invoke the method.
		/// </summary>
		/// <param name="obj"> The instance of the object to call the method on.</param>
		/// <param name="argc">The number of arguments to pass in to the method.</param>
		/// <param name="argv">A list of any references for the arguments.</param>
		/// <returns>The return value as an Any (can be void)</returns>
		inline Any Call(const Any& obj, const Any* argv) const;

		/// <summary>
		/// Invoke a method. Convienient syntax for passing in args:  func(a, b, c...)
		/// </summary>
		template<class T, typename... Args> Any Call(T& obj, Args... args) const;

		/// <summary>
		/// Invoke a method.
		/// </summary>
		/// <remarks> This overload redirects a call given an object and an Any[] of args. (Wraps obj in Any) </remarks>
		template<class T> Any Call(T& obj, const Any* argv) const;
		template<class T> Any Call(T& obj, Any* argv) const; //same as above with no const

		/// <summary>
		/// Invoke a method. Convienient syntax for a zero arg function: func()
		/// </summary>
		/// <remarks> This overload is for methods with no arguments</remarks>
		template<class T> Any Call(T& obj);

		/// <summary>
		/// Determine if a method can be invoked.
		/// </summary>
		/// <param name="obj">The instance of the object to call the method on.</param>
		/// <param name="argc">The number of arguments to pass in to the method.</param>
		/// <param name="argv">A list of any references for the arguments.</param>
		/// <returns>True if the parameters are valid for a call to succeed.</returns>
		inline bool CanCall(const Any& obj, int argc, const Any* argv) const;

		//overload for variadic template forwarding
		template<class T, typename... Args> bool CanCall(T& obj, Args... args) const;

		//overload for no args
		template<class T> bool CanCall(T& obj);

		friend class TypeInfo;
	};

	
	

	//////////////////////////////////////////////////////////////////////////////
	//  TypeInfo
	//////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// <para>Information about a type. </para>
	/// <para> (Name, Size, IsDerivedFrom, IsSameOrDerivedFrom, FindMember, FindMethod) </para>
	/// </summary>
	class TypeInfo
	{
	private:
		const char* m_Name; // The name of the type.
		size_t byteSize;
		static std::unordered_map<std::string, TypeInfo*>* sTypeInfoDictionary;

	protected:
		std::vector<internal::BaseRecord> m_Bases; // The list of base types.
		std::vector<Member*> m_Members;			   // The list of all members of this type.
		std::vector<Method*> m_Methods;            // The list of all methods of this type.

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

		/// <summary>
		/// Constructor for <see cref="TypeInfo"/> class.
		/// </summary>
		/// <param name="name">The name of the type.</param>
		TypeInfo(const char* name, size_t size)
		{
			int nested = 0;

			// trim off the namespaces from the name; 
			// account for possible template specializations
			const char* trimmedName;
			for (trimmedName = name + std::strlen(name) - 1; trimmedName != name; --trimmedName)
			{
				if (*trimmedName == '>')
					++nested;
				else if (*trimmedName == '<')
					--nested;
				else if(nested == 0 && *trimmedName == ':')
				{
					++trimmedName;
					break;
				}
			}

			this->m_Name = trimmedName;
			byteSize = size;
		}

		/// <summary>
		/// Copy constructor for <see cref="TypeInfo"/>.
		/// </summary>
		/// <param name="type">The typeinfo to copy.</param>
		TypeInfo(const TypeInfo& type) : 
			m_Name(type.m_Name), 
			byteSize(type.byteSize),
			m_Bases(type.m_Bases), 
			m_Members(type.m_Members), 
			m_Methods(type.m_Methods)
		{
			for (auto member : m_Members)
				member->SetOwner(this);

			for (auto member : m_Methods)
				member->SetOwner(this);
		}

		const char* GetName() const { return m_Name; }
		const std::string GetNameStr() const { return std::string(m_Name); }
		const size_t GetSize() const { return byteSize; }

		const std::vector<internal::BaseRecord>& GetBases() const { return m_Bases; }
		const std::vector<Member*> & GetMembers() const { return m_Members; }
		const std::vector<Method*>& GetMethods() const { return m_Methods; }

		/// <summary>
		/// Determines whether Tests if this type is derived from the specified type.
		/// </summary>
		/// <param name="newBase">The type to test if this is derived from.</param>
		/// <returns>True if this type derives from base either directly or indirectly.</returns>
		bool IsDerivedFrom(const TypeInfo* newBase) const
		{
			for (auto& base : m_Bases)
			{
				if (base.type == newBase || base.type->IsDerivedFrom(newBase))
				{
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Tests is this type is derived from another type, or is the same type.
		/// </summary>
		/// <param name="base">The type to test if this is derived from, or the same as.</param>
		/// <returns>True if this type is equal to, or derives from base either directly or indirectly.</returns>
		bool IsSameOrDerivedFrom(const TypeInfo* base) const
		{
			if (base == this)
				return true;
			if (IsDerivedFrom(base))
				return true;
			return false;
		}


		/// <summary>
		/// Adjust a pointer of this type to a derived type.
		/// </summary>
		/// <param name="base">The base type to convert to.</param>
		/// <param name="ptr">The pointer to convert.</param>
		/// <returns>The adjusted pointer, or nullptr if the adjustment is illegal (the given base type is not a base of the type).</returns>
		void* Adjust(const TypeInfo* base, void* ptr) const
		{
			if (base == this)
				return ptr;

			for (auto& b : m_Bases)
			{
				void* rs = b.type->Adjust(base, static_cast<char*>(ptr)+b.offset);
				if (rs != nullptr)
					return rs;
			}

			//LogError( "Called GetReference or GetPointerusing a type that doesn't match, or isn't convertable from the value stored by this Any.
			//  The type is %s", base->GetName());
			
			return nullptr;
		}

		/// <summary>
		/// Adjust a pointer of this type to a derived type.
		/// </summary>
		/// <param name="base">The base type to convert to.</param>
		/// <param name="ptr">The pointer to convert.</param>
		/// <returns>he adjusted pointer, or nullptr if the adjustment is illegal (the given base type is not a base of the type).</returns>
		const void* Adjust(const TypeInfo* base, const void* ptr) const
		{
			return Adjust(base, const_cast<void*>(ptr));
		}

		

		/// <summary>
		/// Find a member of this type or any base type.
		/// </summary>
		/// <param name="name">The name of the member to look up.</param>
		/// <returns>The member named. Nullptr if no such member exists.</returns>
		Member* FindMember(const char* name) const
		{
			for (auto member : m_Members)
			{
				if (std::strcmp(member->GetName(), name) == 0)
				{
					return member;
				}
			}

			for (auto& base : m_Bases)
			{
				auto member = base.type->FindMember(name);
				if (member != nullptr)
				{
					return member;
				}
			}

			return nullptr;
		}

		Member* FindMember(std::string& name) const
		{
			return FindMember(name.c_str());
		}

		/// <summary>
		/// Find a method of this type or any base type.
		/// </summary>
		/// <param name="name">The name of the method to look up.</param>
		/// <returns>The method named. Nullptr if no such method exists.</returns>
		Method* FindMethod(const char* name) const
		{
			for (auto method : m_Methods)
			{ 
				if ( std::strcmp(method->GetName(), name) == 0)
				{
					return method;
				}		
			}

			for (auto& base : m_Bases)
			{
				auto method = base.type->FindMethod(name);
				if (method != nullptr)
				{
					return method;
				}
			}

			return nullptr;
		}
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

	
	#include "Meta.inl"


	// *********************** Member Implementations ***********************

	namespace internal
	{
		// A member that is accessed directly (not through getter or setter)
		template <typename Type, typename MemberType> class TypeMember : public Member
		{
			MemberType Type::*m_Member;

		public:
			TypeMember(const char* name, const TypeInfo* type, MemberType Type::*member) : 
				Member(name, type), 
				m_Member(member)
			{}

			virtual bool IsMutable() const override
			{
				return true;
			}

			virtual Any DoGet(const Any& obj) const override
			{
				return internal::make_any<MemberType>::make(obj.GetPointer<Type>()->*m_Member);
			}

			virtual void DoSet(const Any& obj, const Any& in) const override
			{
				obj.GetPointer<Type>()->*m_Member = in.GetReference<MemberType>();
				//if it breaks here, its because in.GetReference<MemberType>() tried to cast
				// whatever const Any& in holds to a non-compatible type. Check call stack.
			}
		};
	}

	// *********************** Variadic Call (Method) Implementation *********************** 
	namespace internal
	{
		//Call_Internal - The final destination for a Call on a method. Actually calls the method.
		
		template <typename Type, typename ReturnType, typename... Args, unsigned int... Is>
		Any Call_Internal( ReturnType(Type::*method)(Args...), Type* obj, const Any* argv, indices<Is...> )
		{
			return internal::make_any<ReturnType>::make( (obj->*method)( *argv[Is].GetPointer<Args>()... ) );
		}

		template <typename Type, typename... Args, unsigned int... Is>
		void Call_Internal( void (Type::*method)(Args...), Type* obj, const Any* argv, indices<Is...> )
		{
			return  (obj->*method)( *argv[Is].GetPointer<Args>()... );
		}


		//Call - forwards a call to Call_Internal, generating a variadic non-type template containing the indices for the arguments.
		// Uses what seems to get dubbed "The indices trick". 

		template <typename Type, typename ReturnType, typename... Args>
		Any Call(ReturnType(Type::*method)(Args...), Type* obj, const Any* argv)
		{
			assert((sizeof...(Args) > 0 && argv != nullptr) ||		// if has args, must not have null argv.
				(sizeof...(Args) == 0 && argv == nullptr));			// if no args, must have null argv.
			return Call_Internal(method, obj, argv, build_indices<sizeof...(Args)>{});
		}

		template <typename Type, typename... Args>
		void Call(void (Type::*method)(Args...), Type* obj, const Any* argv)
		{
			assert((sizeof...(Args) > 0 && argv != nullptr) ||		// if has args, must not have null argv.
				(sizeof...(Args) == 0 && argv == nullptr));			// if no args, must have null argv.
			return Call_Internal(method, obj, argv, build_indices<sizeof...(Args)>{});
		}
		
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
			
			virtual Any DoCall(const Any& obj, const Any* argv) const 
			{ 
				return meta::internal::Call(method, obj.GetPointer<Type>(), argv); 
			}
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
			
			virtual Any DoCall(const Any& obj, const Any* argv) const 
			{ 
				meta::internal::Call(method, obj.GetPointer<Type>(), argv); 
				return Any();
			}
		};
	}

	template<typename Type, typename ReturnType, typename... Args>
	Method* bindMethod(const char* name, ReturnType(Type::*method)(Args...))
	{
		return new meta::internal::VarMethod<Type, ReturnType, Args...>(name, method);
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
	//  TypeInfoBuilder - Builds the MetaInformation.
	//////////////////////////////////////////////////////////////////////////////

	namespace internal
	{
		
		/// <summary>
		/// TypeInfo object that is created using the Builder Pattern. 
		/// <code> 
		/// <para> meta_DEFINE(Class) </para>
		/// <para>	  .member("a", &amp;Class&#58;&#58;a) </para>
		/// <para>	  .method("foo", &amp;Class&#58;&#58;foo) </para>
		/// </code>
		/// </summary>
		template <typename Type, bool IsClass> struct TypeInfoBuilder : public TypeInfo
		{
			//! \brief Construct the type.
			TypeInfoBuilder(const char* name, size_t size) : TypeInfo(name, size)
			{}

			//! \brief Add a base to this type.
			template <typename BaseType> TypeInfoBuilder& base()
			{
				static_assert(std::is_base_of<BaseType, Type>::value && !std::is_same<BaseType, Type>::value, "incorrect base"); 
				m_Bases.push_back(BaseRecord(Get<BaseType>(), reinterpret_cast<ptrdiff_t>(static_cast<const BaseType*>(reinterpret_cast<const Type*>(0x1000))) - 0x1000)); 
				return *this;
			}

			/// <summary>
			/// Add a read-write member definition to this type which will be accessed directly.
			/// </summary>
			/// <param name="name">The name of the member.</param>
			/// <param name="member">A pointer to the member.</param>
			/// <returns></returns>
			template <typename MemberType> 
			typename std::enable_if<!std::is_member_function_pointer<MemberType>::value, TypeInfoBuilder&>::type member(const char* name, MemberType Type::*member)
			{
				m_Members.push_back(new TypeMember<Type, typename std::remove_reference<MemberType>::type>(name, Get<MemberType>(), member)); return *this;
			}

			/// <summary>
			/// Add a method definition to this type.
			/// </summary>
			/// <param name="name">The name of the method.</param>
			/// <param name="method">A pointer to the member ( &Type::method ).</param>
			/// <returns></returns>
			template<typename Type, typename ReturnType, typename... Args> TypeInfoBuilder& method(const char* name, ReturnType(Type::*method)(Args...))
			{
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
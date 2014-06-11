#pragma once

#include<type_traits>

namespace meta
{
	class TypeInfo;

	
	namespace internal
	{
		// Forward Declaration of MetaHolder
		template <typename T> 
		struct MetaHolder;

		// MetaHolder for void
		template <> 
		struct MetaHolder<void>
		{
			static const TypeInfo s_TypeInfo;
		};

		// Type_Trait check if type has GetMeta() method
		template <typename T> class has_get_meta
		{
			template<typename U> static auto test(int) -> decltype(std::declval<U>().GetType(), std::true_type());
			template<typename> static std::false_type test(...);

		public:
			static const bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
		};

		// Type_Trait check if type has MetaStaticHolder::s_TypeInfo
		template <class T>                                                  
		class has_meta                                                          
		{       
			template<class U, class = typename std::enable_if<!std::is_member_pointer<decltype(&U::MetaStaticHolder::s_TypeInfo)>::value>::type>
				static std::true_type check(int);
			template <class>
				static std::false_type check(...);
		public:
			static const bool value = decltype(check<T>(0))::value;
		};

		//////////////////////////////////////////////////////////////////////
		/////  Meta Lookup : used to retrieve s_TypeInfo from a type.
		/////  If it doesn't have it, refers it to nulltype.

		template <typename Type, bool HasMeta> struct meta_lookup
		{
			static const TypeInfo* Get()
			{
				return &Type::MetaStaticHolder::s_TypeInfo;
			}
		};

		// no meta? Try stripping const and reference, and try that type.
		template <typename Type> struct meta_lookup<Type, false>
		{
			static const TypeInfo* Get()
			{
				return &internal::MetaHolder<typename std::remove_const<typename std::remove_reference<Type>::type>::type>::s_TypeInfo;
			}
		};

		// still not meta? Give it back void type
		template <> struct meta_lookup<void, false>
		{
			static const TypeInfo* Get()
			{
				return &internal::MetaHolder<void>::s_TypeInfo;
			}
		};

		// Got handed a nullptr, give it back a null type.
		template <> struct meta_lookup<std::nullptr_t, false>
		{
			static const TypeInfo* Get()
			{
				return nullptr;
			}
		};
	}

	//////////////////////////////////////////////////////////////////////
	/////  Get: Retrieve MetaData in TypeInfo objects.

	/// Get the TypeInfo for a specific type.
	template <typename Type> 
	const TypeInfo* Get()
	{
		return internal::meta_lookup<Type, internal::has_meta<Type>::value>::Get();
	}

	/// Get the TypeInfo if object supports GetType()
	template <typename Type> 
	typename std::enable_if< internal::has_get_meta<Type>::value, const TypeInfo*>::type 
		Get(const Type* type)
	{
		return type->GetType();
	}

	/// Get the TypeInfo if it doesn't have GetType on it.
	template <typename Type> 
	typename std::enable_if<!internal::has_get_meta<Type>::value, const TypeInfo*>::type 
		Get(const Type*)
	{
		return Get<Type>();
	}

	/// Get the TypeInfo if it has GetType(), and is not a pointer
	template <typename Type> 
	typename std::enable_if<!std::is_pointer<Type>::value && internal::has_get_meta<Type>::value, const TypeInfo*>::type 
		Get(const Type& type)
	{
		return type.GetType();
	}

	/// Get the TypeInfo if it doesn't have GetType(), and is not a pointer
	template <typename Type> 
	typename std::enable_if<!std::is_pointer<Type>::value && !internal::has_get_meta<Type>::value, const TypeInfo*>::type 
		Get(const Type&)
	{
		return Get<Type>();
	}

	//////////////////////////////////////////////////////////////////////////////
	//  TypeRecord
	//////////////////////////////////////////////////////////////////////////////

	// Represents a type with qualifiers (const, reference, pointer, etc.)
	struct TypeRecord
	{
		enum Qualifier 
		{
			Void,			
			Value,			// The encoded value is not a by-reference type, but is a by-copy type.
			Pointer,		// The encoded value is a pointer and assignments point to a new value.
			ConstPointer,	// The encoded value is a pointer and assignments point to a new value, but its value is const.
		};

		const TypeInfo* type; // The type encoded
		Qualifier qualifier; // Qualifier to the type denoting access pattern

		TypeRecord(const TypeInfo* t, Qualifier q) : 
			type(t), 
			qualifier(q)
		{}

		TypeRecord() : 
			type(nullptr), 
			qualifier(Void)
		{}
	};

	//////////////////////////////////////////////////////////////////////////////
	//  TypeRecord Makers
	//////////////////////////////////////////////////////////////////////////////

	namespace internal
	{
		// Construct a TypeRecord for a specific type by value
		template <typename Type> struct make_type_record
		{
			static const TypeRecord type()
			{
				return TypeRecord(Get<Type>(), TypeRecord::Value);
			}
		};

		// Construct a TypeRecord for a specific type by pointer
		template <typename Type> struct make_type_record<Type*>
		{
			static const TypeRecord type()
			{
				return TypeRecord(Get<Type>(), TypeRecord::Pointer);
			}
		};

		// Construct a TypeRecord for a specific type by const pointer
		template <typename Type> struct make_type_record<const Type*>
		{
			static const TypeRecord type()
			{
				return TypeRecord(Get<Type>(), TypeRecord::ConstPointer);
			}
		};

		// Construct a TypeRecord for a specific type by reference
		template <typename Type> struct make_type_record<Type&>
		{
			static const TypeRecord type()
			{
				return TypeRecord(Get<Type>(), TypeRecord::Pointer);
			}
		};

		// Construct a TypeRecord for a specific type by const reference
		template <typename Type> struct make_type_record<const Type&>
		{
			static const TypeRecord type()
			{
				return TypeRecord(Get<Type>(), TypeRecord::ConstPointer);
			}
		};

		// Construct a TypeRecord for void
		template <> struct make_type_record<void>
		{
			static const TypeRecord type()
			{
				return TypeRecord(Get<void>(), TypeRecord::Void);
			}
		};

		//////////////////////////////////////////////////////////////////////////////
		//  GetTypeRecordByIndex: Expands variadic template Args into array, and 

		template<int size, typename... Args>
		TypeRecord GetTypeRecordByIndex(int i)
		{
			if(i >= 0 && i < sizeof...(Args))
			{
				meta::TypeRecord argRecords[] = {
					internal::make_type_record<Args>::type()...,
				};
				return argRecords[i];
			}
				
			return TypeRecord();
		}

		template<>
		inline TypeRecord GetTypeRecordByIndex<0>(int i) //must be inline (or put in cpp file) because all types are known, so it upgrades to a regular function
		{
			return TypeRecord();
		}

	}
}
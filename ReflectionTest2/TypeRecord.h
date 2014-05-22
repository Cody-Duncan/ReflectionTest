#pragma once

#include <cstddef>

namespace meta
{
	class TypeInfo;

	namespace internal
	{
		template <typename T> struct MetaHolder;

		//! \brief Type trait to convert a possibly const reference or pointer to a non-const pointer
		template <typename T> struct to_pointer
		{
			typedef typename std::remove_const<typename std::remove_reference<typename std::remove_pointer<T>::type>::type>::type* type;
		};

		/*!\ brief Type trait to check if a given type has a GetMeta method. */
		template <typename T> class has_get_meta
		{
			template <typename U> static std::true_type test(decltype(U::GetType())*);
			template <typename U> static std::false_type test(...);

		public:
			/*! \brief True if the type has a GetMeta() method, false otherwise. */
			static const bool value = std::is_same<decltype(test<T>(nullptr)), std::true_type>::value;
		};

		/*! \brief Type trait to check if a class has a MetaStaticHolder::s_TypeInfo field. */
		template <typename T> class has_inline_meta
		{
			template <typename U> static std::true_type test(decltype(U::MetaStaticHolder::s_TypeInfo)*);
			template <typename U> static std::false_type test(...);

		public:
			/*! \brief True if the type has a GetMeta() method, false otherwise. */
			static const bool value = std::is_same<decltype(test<T>(nullptr)), std::true_type>::value;
		};

		/*! \brief Helper to find the TypeInfo for a type which has a MetaStaticHolder::s_TypeInfo. */
		template <typename Type, bool HasInlineMeta> struct meta_lookup
		{
			static const TypeInfo* Get()
			{
				return &Type::MetaStaticHolder::s_TypeInfo;
			}
		};

		/*! \brief Helper to find the TypeInfo for a type which has no MetaStaticHolder::s_TypeInfo. */
		template <typename Type> struct meta_lookup<Type, false>
		{
			static const TypeInfo* Get()
			{
				return &internal::MetaHolder<typename std::remove_const<typename std::remove_reference<Type>::type>::type>::s_TypeInfo;
			}
		};

		/*! \brief Helper to retrieve the null TypeInfo for a void type. */
		template <> struct meta_lookup<void, false>
		{
			static const TypeInfo* Get()
			{
				return nullptr;
			}
		};

		/*! \brief Helper to retrieve the null TypeInfo for a nullptr_t type. */
		template <> struct meta_lookup<std::nullptr_t, false>
		{
			static const TypeInfo* Get()
			{
				return nullptr;
			}
		};

		//! \brief Records information about a base type.
		struct BaseRecord
		{
			BaseRecord(const TypeInfo* t, ptrdiff_t o) : type(t), offset(o)
			{}

			const TypeInfo* type; //!< The type being used
			ptrdiff_t offset; //<! Offset that must be applied to convert a pointer from the deriving type to this base
		};

		//! \brief Knows how to destruct a type.
		template <typename Type> struct destructor
		{
			static void destruct(void* obj)
			{
				static_cast<Type*>(obj)->~Type();
			}
		};

		//! \brief Knows how to move a non-const type.
		template <typename Type> struct mover
		{
			static void move(void* dst, void* src)
			{
				new (dst)Type(std::move(*static_cast<Type*>(src)));
			}
		};

		//! \brief Knows how to copy a type.
		template <typename Type> struct mover<const Type*>
		{
			static void move(void* dst, void* src)
			{
				new (dst)Type(*static_cast<Type*>(src));
			}
		};
	}

	/*! \brief Get the TypeInfo for a specific type. */
	template <typename Type> const TypeInfo* Get()
	{
		return internal::meta_lookup<Type, internal::has_inline_meta<Type>::value>::Get();
	}

	/*! \brief Get the TypeInfo for a specific instance supporting a GetMeta() method, usually used/needed in polymorphic interfaces. */
	template <typename Type> typename std::enable_if< internal::has_get_meta<Type>::value, const TypeInfo*>::type Get(const Type* type)
	{
		return type->getMeta();
	}

	/*! \brief Get the TypeInfo for a specific instance which has no GetMeta() method and hence no support for polymorphism of introspection. */
	template <typename Type> typename std::enable_if<!internal::has_get_meta<Type>::value, const TypeInfo*>::type Get(const Type*)
	{
		return Get<Type>();
	}

	/*! \brief Get the TypeInfo for a specific instance supporting a GetMeta() method, usually used/needed in polymorphic interfaces. */
	template <typename Type> typename std::enable_if<!std::is_pointer<Type>::value && internal::has_get_meta<Type>::value, const TypeInfo*>::type Get(const Type& type)
	{
		return type.getMeta();
	}

	/*! \brief Get the TypeInfo for a specific instance which has no GetMeta() method and hence no support for polymorphism of introspection. */
	template <typename Type> typename std::enable_if<!std::is_pointer<Type>::value && !internal::has_get_meta<Type>::value, const TypeInfo*>::type Get(const Type&)
	{
		return Get<Type>();
	}

	//////////////////////////////////////////////////////////////////////////////
	//  TypeRecord
	//////////////////////////////////////////////////////////////////////////////

	// Represents a specific C++ type with qualifiers (const, reference, pointer, etc.)
	struct TypeRecord
	{
		/// <summary>
		/// Qualifiers (not quite correct C++ terminology) specifying how the type is used
		/// </summary>
		enum Qualifier 
		{
			Void,			// This is a void value and may not be access at all
			Value,			// The encoded value is not a by-reference type but is a by-copy type and may not be reassigned.
			Pointer,		// The encoded value is a pointer and assignments point to a new value, and dereferencing allows changing the value.
			ConstPointer,	// The encoded value is a pointer and assignments point to a new value, but dereferencing does not allow changing the value.
		};

		const TypeInfo* type; // The type encoded
		Qualifier qualifier; // Qualifier to the type denoting access pattern

		TypeRecord(const TypeInfo* t, Qualifier q) : type(t), qualifier(q)
		{}

		TypeRecord() : type(nullptr), qualifier(Void)
		{}
	};


	// *********** TypeRecord makers ***********

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
				return TypeRecord(nullptr, TypeRecord::Void);
			}
		};
	}
}
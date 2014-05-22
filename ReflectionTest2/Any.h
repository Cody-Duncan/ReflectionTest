#pragma once

#include <utility>			//std::move
#include "TypeRecord.h"

namespace meta
{
	class TypeInfo;

	namespace internal
	{
		template <typename Type> struct make_any;
	}

	//////////////////////////////////////////////////////////////////////////////
	//  Any
	//////////////////////////////////////////////////////////////////////////////

	// Holds any type of value that can be handled by the introspection system
	class Any
	{
	private:
		union
		{
			double _align_me;				// force allignment
			char m_Data[4 * sizeof(float)]; // large enough for 4 floats, e.g. a vec4
			void* m_Ptr;					// convenient
		};
		TypeRecord m_TypeRecord;			// Type record information stored in this Any
		typedef void(*Destructor)(void*);	// Type of the destructor function
		typedef void(*Mover)(void*, void*); // Type of the mover function
		Destructor m_Destructor;			// Destructor for the bound type
		Mover m_Mover;						// Mover for the bound type

		// NonCopyable
		Any(const Any&); 
		void operator=(const Any&);

	public:
		// Constructs an Any that holds nothing
		Any() : m_Ptr(nullptr), m_Destructor(nullptr), m_Mover(nullptr)
		{}

		// Moves one Any into another
		Any(Any&& src) : m_Destructor(nullptr)
		{
			*this = std::move(src);
		}

		// Moves one Any into another
		Any& operator=(Any&& src)
		{
			if (this != &src)
			{
				if (m_Destructor != nullptr)
					m_Destructor(m_Data);
				m_Mover = src.m_Mover;
				m_Destructor = src.m_Destructor;
				m_Mover(m_Data, src.m_Data);
				if (m_Destructor != nullptr)
					m_Destructor(src.m_Data);
				src.m_Destructor = nullptr;
				src.m_Mover = nullptr;
			}
			return *this;
		}

		// Constucts an Any that contains an object
		template <typename Type> Any(const Type& obj) : m_TypeRecord(internal::make_type_record<Type>::type()), m_Destructor(&internal::destructor<Type>::destruct), m_Mover(&internal::mover<Type>::move)
		{
			static_assert(sizeof(Type) <= sizeof(m_Data), "Type is too large"); new (m_Data)Type(obj);
		}

		// Constucts an Any that points at a non-const object
		template <typename Type> Any(Type* obj) : m_Ptr(obj), m_TypeRecord(internal::make_type_record<Type*>::type()), m_Destructor(nullptr), m_Mover(&internal::mover<Type*>::move)
		{}

		// Constucts an Any that points at a const object
		template <typename Type> Any(const Type* obj) : m_Ptr(const_cast<Type*>(obj)), m_TypeRecord(internal::make_type_record<const Type*>::type()), m_Destructor(nullptr), m_Mover(&internal::mover<const Type*>::move)
		{}

		// Cleans up the value stored in the Any if necessary
		~Any()
		{
			if (m_Destructor != nullptr)
				m_Destructor(m_Data);
		}

		// Checks if the value should be considered a constant when dereferenced.
		bool IsConst() const
		{
			return m_TypeRecord.qualifier != TypeRecord::Qualifier::Pointer;
		}

		// Gets the TypeInfo for the value stored in the Any, if any
		const TypeInfo* GetType() const
		{
			return m_TypeRecord.type;
		}

		// Gets the TypeRecord information associated with the value stored in this Any
		const TypeRecord& GetTypeRecord() const
		{
			return m_TypeRecord;
		}

		// Retrieves a pointer to the type stored in this Any
		void* GetPointer() const
		{
			switch (m_TypeRecord.qualifier)
			{
				case TypeRecord::Qualifier::Value : return const_cast<char*>(m_Data);
				case TypeRecord::Qualifier::Pointer :
				case TypeRecord::Qualifier::ConstPointer :
					return m_Ptr;
				default: return nullptr;
			}
		}

		// Retrieves a pointer to the type stored in this Any adjusted by base type offset if necessary
		inline void* GetPointer(const TypeInfo* type) const;

		// Retrieves a pointer to the type stored in this Any adjusted by base type offset if necessary
		template <typename Type> Type* GetPointer() const
		{
			return static_cast<Type*>(GetPointer(Get<Type>()));
		}

		// Retrieves a reference to the type stored in this Any adjusted by base type offset if necessary
		template <typename Type> Type& GetReference() const
		{
			return *static_cast<Type*>(GetPointer(Get<Type>()));
		}

		template <typename Type> friend struct internal::make_any;
	};

	// ******* make Any *******

	namespace internal
	{
		template <typename Type> struct make_any
		{
			static Any make(Type value)
			{
				return Any(value);
			}
		};
		template <typename Type> struct make_any<Type&>
		{
			static Any make(Type& value)
			{
				return Any(&value);
			}
		};
	}

	// ******* Any Cast Helpers *******
	// internally casts for any_cast(const Any& any)

	namespace internal
	{
		template <typename Type> struct any_cast_helper
		{
			static const Type& cast(const Any& any)
			{
				return any.GetReference<Type>();
			}
		};

		template <typename Type> struct any_cast_helper<Type*>
		{
			static Type* cast(const Any& any)
			{
				return any.GetPointer<Type>();
			}
		};

		template <typename Type> struct any_cast_helper<const Type*>
		{
			static const Type *cast(const Any& any)
			{
				return any.GetPointer<Type>();
			}
		};

		template <typename Type> struct any_cast_helper<Type&>
		{
			static Type& cast(const Any& any)
			{
				return any.GetReference<Type>();
			}
		};

		template <typename Type> struct any_cast_helper<const Type&>
		{
			static const Type& cast(const Any& any)
			{
				return any.GetReference<Type>();
			}
		};
	}

	/// <summary>
	/// Attempts to convert an any reference into a pointer to the actual C++ type.
	/// </summary>
	/// <param name="any">An Any reference to try to convert.</param>
	/// <returns></returns>
	template <typename Type> Type any_cast(const Any& any)
	{
		return internal::any_cast_helper<Type>::cast(any);
	}

}
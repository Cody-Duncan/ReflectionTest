
#include "Meta.h"

namespace meta
{
	class Any
	{
	private:
		union
		{
			double  _align_me;				  //!< force allignment
			char   m_Data[32];				  //!< large enough for 4 floats, e.g. a vec4
			void*  m_Ptr;					  //!< convenient
		};

		TypeRecord m_TypeRecord;
		typedef void(*Destructor)(void*);
		typedef void(*Mover)(void*, void*);
		Destructor m_Destructor;
		Mover m_Mover;

		// Non-Copyable
		Any(const Any&);		
		void operator=(const Any&); 

	public:

		// Constructs an Any that holds nothing
		Any() : m_Ptr(nullptr), m_Destructor(nullptr), m_Mover(nullptr) {}

		//Moves one Any into another
		Any(Any&& src) : m_Destructor(nullptr) { *this = std::move(src); }

		//Move Constructor
		Any& operator=(Any&& src)
		{
			if (this != &src)
			{
				if (m_Destructor != nullptr)
				{
					m_Destructor(m_Data);			//destruct this object
				}

				m_Mover = src.m_Mover;				//get mover
				m_Destructor = src.m_Destructor;	//get destructor
				m_Mover(m_Data, src.m_Data);		//call mover, move others data to this one

				if (m_Destructor != nullptr)
				{
					m_Destructor(src.m_Data);		//destruct the other object
				}

				src.m_Destructor = nullptr;			//clean up other Destructor
				src.m_Mover = nullptr;				//clean up other Mover
			}
			return *this;
		}

		//Constucts an Any that contains an object
		template <typename Type> Any(const Type& obj) :
			m_TypeRecord(internal::make_type_record<Type>::type()),
			m_Destructor(&internal::destructor<Type>::destruct),
			m_Mover(&internal::mover<Type>::move)
		{
			static_assert(sizeof(Type) <= sizeof(m_Data), "Type is too large");
			new (m_Data)Type(obj);
		}

		//! \brief Constucts an Any that points at a non-const object
		template <typename Type> Any(Type* obj) : 
			m_Ptr(obj), 
			m_TypeRecord(internal::make_type_record<Type*>::type()), 
			m_Destructor(nullptr), m_Mover(&internal::mover<Type*>::move) 
		{}

		//! \brief Constucts an Any that points at a const object
		template <typename Type> Any(const Type* obj) : 
			m_Ptr(const_cast<Type*>(obj)), 
			m_TypeRecord(internal::make_type_record<const Type*>::type()), 
			m_Destructor(nullptr), 
			m_Mover(&internal::mover<const Type*>::move) 
		{}

		~Any()
		{
			if (m_Destructor != nullptr)
				m_Destructor(m_Data);
		}

		bool IsConst() const { return m_TypeRecord.qualifier != TypeRecord::Pointer; }

		const TypeInfo* GetType() const { return m_TypeRecord.type; }

		const TypeRecord& GetTypeRecord() const { return m_TypeRecord; }

		void* GetPointer() const
		{
			switch (m_TypeRecord.qualifier)
			{
				case TypeRecord::Value: return const_cast<char*>(m_Data);
				case TypeRecord::Pointer:
				case TypeRecord::ConstPointer:
					return m_Ptr;
				default: return nullptr;
			}
		}

		inline void* GetPointer(const TypeInfo* type) const;

		template <typename T> T* GetPointer() const 
		{ 
			//return static_cast<T*>( GetPointer( get<T>() ) );
			return static_cast<T*>( GetPointer() ); 
		}

		template <typename T> T& GetReference() const 
		{ 
			//return *static_cast<T*>(GetPointer(get<T>()));
			return *static_cast<T*>( GetPointer() );
		}

		template <typename Type> friend struct internal::make_any;
	};

	inline void* Any::GetPointer(const TypeInfo* type) const
	{
		//return m_TypeRecord.type->Adjust(type, GetPointer());
		assert(false);
		return nullptr;
	}

	namespace internal
	{
		template <typename T> struct make_any { static Any make(T value) { return Any(value); } };
		template <typename T> struct make_any<T&> { static Any make(T& value) { return Any(&value); } };
	}
}
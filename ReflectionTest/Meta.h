#pragma once


#include <string>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <iostream>

#include "MacroHelpers.h"
#include "RemoveQualifiers.h"


namespace meta
{
	class TypeInfo;
	template <typename Metatype>
	class TypeCreator;
	class Member;
	class Meta;

	namespace internal
	{
		//! \brief Knows how to destruct a type.
		template <typename Type> struct destructor
		{
			static void destruct(void* obj) { static_cast<Type*>(obj)->~Type(); }
		};

		//! \brief Knows how to move a non-const type.
		template <typename Type> struct mover
		{
			static void move(void* dst, void* src) { new (dst)Type(std::move(*static_cast<Type*>(src))); }
		};

		//! \brief Knows how to copy a type.
		template <typename Type> struct mover<const Type*>
		{
			static void move(void* dst, void* src) { new (dst)Type(*static_cast<Type*>(src)); }
		};
	}
	
	//////////////////////////////////////////////////////////////////////////////
	//  Type
	//////////////////////////////////////////////////////////////////////////////
	
	class TypeInfo
	{
	public:
		TypeInfo() {}
		~TypeInfo() {};

		const std::string& Name(void) const { return name; }
		unsigned Size(void) const { return size; }

		void AddMember(const Member *member);

		std::vector<const Member *> members;
		std::unordered_map<std::string, const Member *> mamberNames;

		void Copy(void* dest, const void* src) const
		{
			std::memcpy(dest, src, size);
		}

		void Delete(void* data) const
		{
			delete[] reinterpret_cast<char*>(data);
			data = NULL;
		}

		void* NewCopy(const void* src) const
		{
			void* data = new char[size];
			std::memcpy(data, src, size);
			return data;
		}

		void* New(void) const
		{
			return new char[size];
		}

	private:
		friend void InitType(TypeInfo* type, std::string& string, unsigned val);

		std::string name;
		unsigned size;
	};

	

	//////////////////////////////////////////////////////////////////////////////
	//  TypeCreator Singleton
	//////////////////////////////////////////////////////////////////////////////

	template<typename T>
	static void registerType();

	template <typename Metatype>
	class TypeCreator
	{
	public:
		TypeCreator(std::string name, unsigned size)
		{
			assert(instance == NULL);

			allTypesStorage.emplace_back();
			assert(allTypesStorage.size() > 0);
			instance = &(allTypesStorage[allTypesStorage.size()-1]);

			Init(name, size);
		}

		static void Init(std::string name, unsigned size)
		{
			TypeInfo* newType = Get();			//construct
			InitType(newType, name, size);	//set variables
			registerType<Metatype>();		//register

			RegisterMetaData();
		}

		static void RegisterMetaData(void);

		static void AddMember(std::string memberName, unsigned memberOffset, TypeInfo *meta)
		{
			Get()->AddMember(new Member(memberName, memberOffset, meta));
		}

		static Metatype* NullCast(void)
		{
			return reinterpret_cast<Metatype *>(NULL);
		}

		// Ensure a single instance can exist for this class type
		static TypeInfo* Get(void)
		{
			return instance;
		}
	private:
		static TypeInfo* instance;
	};

	template<typename Metatype>
	meta::TypeInfo* meta::TypeCreator<Metatype>::instance = nullptr;

	//////////////////////////////////////////////////////////////////////////////
	//  Member
	//////////////////////////////////////////////////////////////////////////////
	// Purpose: Stores information (name and offset of member) about a data member of a specific class. Multiple
	//          Member objects can be stored in MetaData objects within a std::vector.
	class Member
	{
	public:
		Member::Member(std::string string, unsigned val, TypeInfo *meta) : name(string), offset(val), data(meta) {}
		Member::~Member() {}

		const std::string &Name(void) const { return name; } 	// Gettor for name
		unsigned Offset(void) const { return offset; };			// Gettor for offset
		const TypeInfo *Meta(void) const { return data; };			// Gettor for data
		const std::string TypeName() const { return data->Name(); }
		int Size() const { return data->Size(); }

	private:
		std::string name;
		unsigned offset;
		const TypeInfo *data;
	};


	//////////////////////////////////////////////////////////////////////////////
	// TypeRecord
	//////////////////////////////////////////////////////////////////////////////
	// Purpose: Stores qualifier information about a variable, like const, pointer, void, etc.
	//          
	struct TypeRecord
	{
		enum Qualifier 
		{
			Void, 
			Value, 
			Pointer, 
			ConstPointer, 
		};

		const TypeInfo* type; //!< The type encoded
		Qualifier qualifier; //!< Qualifier to the type denoting access pattern

		TypeRecord(const TypeInfo* t, Qualifier q) : type(t), qualifier(q) { }
		TypeRecord() : type(nullptr), qualifier(Void) { }
	};

	namespace internal
	{
		//Construct a TypeRecord for a specific type by value
		template <typename Type> 
		struct make_type_record 
		{ 
			static const TypeRecord type() { return TypeRecord(get<Type>(), TypeRecord::Value); } 
		};

		//Construct a TypeRecord for a specific type by pointer
		template <typename Type> 
		struct make_type_record<Type*> 
		{ 
			static const TypeRecord type() { return TypeRecord(get<Type>(), TypeRecord::Pointer); } 
		};

		//Construct a TypeRecord for a specific type by const pointer
		template <typename Type> 
		struct make_type_record<const Type*> 
		{ 
			static const TypeRecord type() { return TypeRecord(get<Type>(), TypeRecord::ConstPointer); } 
		};

		//Construct a TypeRecord for a specific type by reference
		template <typename Type> 
		struct make_type_record<Type&>
		{
			static const TypeRecord type() { return TypeRecord(get<Type>(), TypeRecord::Pointer); } 
		};

		//Construct a TypeRecord for a specific type by const reference
		template <typename Type> 
		struct make_type_record<const Type&> 
		{ 
			static const TypeRecord type() { return TypeRecord(get<Type>(), TypeRecord::ConstPointer); } 
		};

		//Construct a TypeRecord for void
		template <> 
		struct make_type_record<void> 
		{ 
			static const TypeRecord type() { return TypeRecord(nullptr, TypeRecord::Void); } 
		};

		template <typename Type> struct make_any;
	}



	//////////////////////////////////////////////////////////////////////////////
	//  Meta
	//////////////////////////////////////////////////////////////////////////////
	//
	// MetaManager
	// Purpose: Just a collection of some functions for management of all the
	//          various MetaData objects.
	//
	class Meta
	{
	public:
		typedef std::unordered_map<std::string, TypeInfo *> MetaMap;

		// Insert a MetaData into the map of objects
		static void RegisterMeta(TypeInfo *instance)
		{
			std::pair<MetaMap::iterator, bool> result = GetMap().insert(std::make_pair(instance->Name(), instance));
			assert(result.second == true); //if false, already existed
		}

		static const bool IsRegistered(const std::string name)
		{
			return GetMap().count(name) > 0;
		}

		// Retrieve a MetaData instance by string name from the map of MetaData objects
		static TypeInfo* Get(std::string name) // NULL if not found
		{
			if (GetMap().count(name) > 0)
			{
				return GetMap()[name];
			}
			return NULL;
		}

		// Safe and easy singleton for map of MetaData objects
		static MetaMap& GetMap(void)
		{
			// Define static map here, so no need for explicit definition
			static MetaMap map;
			return map;
		}
	};

	template<typename T>
	static void registerType()
	{
		Meta::RegisterMeta(TypeCreator<meta::RemoveQualifiers<T>::type>::Get());
	}

	//////////////////////////////////////////////////////////////////////////////
	//  Meta Definition Functions
	//////////////////////////////////////////////////////////////////////////////

	//registers a type.
	//Defines RegisterMetaData for the TypeCreator, so this must be followed by {}.
	#define meta_define(TYPE) \
		namespace namespace_for_meta_types {																									\
			meta::TypeCreator<meta::RemoveQualifiers<TYPE>::type> NAME_GENERATOR()(#TYPE, sizeof(TYPE));										\
		}																																		\
		meta::RemoveQualifiersPtr<TYPE>::type* TYPE::NullCast(void) { return reinterpret_cast<meta::RemoveQualifiers<TYPE>::type *>(NULL); }	\
		void TYPE::AddMember(std::string name, unsigned offset, meta::TypeInfo *data) { return meta::TypeCreator<meta::RemoveQualifiersPtr<TYPE>::type>::AddMember(name, offset, data); } \
		void meta::TypeCreator<meta::RemoveQualifiersPtr<TYPE>::type>::RegisterMetaData(void) { TYPE::RegisterMetaData(); }						\
		void TYPE::RegisterMetaData(void) //define after this


	//Allows RegisterMetaData (in meta_define) to get access to private members.
	#define meta_expose_internal(TYPE) \
		static void AddMember(std::string name, unsigned offset, meta::TypeInfo* data);		\
		static meta::RemoveQualifiers<TYPE>::type* NullCast(void);						\
		static void TYPE::RegisterMetaData(void);

	//registers a Plain Old DataType (POD) type with the meta system.
	#define meta_define_pod(TYPE) \
		namespace namespace_for_meta_POD_types {															\
			meta::TypeCreator<meta::RemoveQualifiers<TYPE>::type> NAME_GENERATOR()(#TYPE, sizeof(TYPE));	\
		}																									\
		void meta::TypeCreator<meta::RemoveQualifiers<TYPE>::type>::RegisterMetaData(void) {}

	//registers a member of a type
	#define meta_add_member( MEMBER ) \
		AddMember(#MEMBER, (unsigned)(&(NullCast()->MEMBER)), meta::get(NullCast()->MEMBER))


	//////////////////////////////////////////////////////////////////////////////
	//  Meta Access Functions
	//////////////////////////////////////////////////////////////////////////////

	//////// Registered? ////////

	//check if a type has been registered
	template<typename T>
	static const bool has()
	{
		return Meta::IsRegistered(meta::TypeCreator<RemoveQualifiers<T>::type>::Get()->Name());
	}

	//check if a type of an object has been registered
	template<typename T>
	static const bool has(const T& object)
	{
		return Meta::IsRegistered(meta::TypeCreator<RemoveQualifiers<T>::type>::Get()->Name());
	}

	//check if a type has been registered, by name
	static const bool has_name(const std::string& str)
	{
		return Meta::IsRegistered(str);
	}

	//check if a type has been registered, by name
	static const bool has_name(const char* str)
	{
		return Meta::IsRegistered(str);
	}

	//////// Get Meta Information ////////

	//Get meta directly by type
	template<typename T>
	static TypeInfo* get()
	{
		return meta::TypeCreator<RemoveQualifiers<T>::type>::Get();
	}

	//get meta about an object's type
	template<typename T>
	static TypeInfo* get(const T& object)
	{
		return meta::TypeCreator<RemoveQualifiers<T>::type>::Get();
	}

	//get meta about an object by name
	static TypeInfo* get_name(const std::string& str)
	{
		return Meta::Get(str);
	}

	//get meta about an object by name
	static TypeInfo* get_name(const char* str)
	{
		return Meta::Get(str);
	}


	//Friend function to initialize Type.
	// A: InitType won't show up in Type as public function.
	// B: Constructor for InitType called in singleton function.
	void InitType(TypeInfo* type, std::string& string, unsigned val);

	extern std::vector<TypeInfo> allTypesStorage;

}

#include "Variant.inl"
#include "Any.inl"
#pragma once


#include <string>
#include <unordered_map>
#include <assert.h>
#include <iostream>

#include "MacroHelpers.h"
#include "RemoveQualifiers.h"

namespace meta
{
	class Type;
	template <typename Metatype>
	class TypeCreator;
	class Member;
	class Meta;

	//////////////////////////////////////////////////////////////////////////////
	//  Type
	//////////////////////////////////////////////////////////////////////////////


	//Friend function to initialize Type.
	// A: InitType won't show up in Type as public function.
	// B: Constructor for InitType called in singleton function.
	void InitType(Type* type, std::string& string, unsigned val);

	class Type
	{
	public:
		Type() {}
		~Type() {};

		const std::string& Name(void) const { return name; }
		unsigned Size(void) const { return size; }

		void AddMember(const Member *member) { members.push_back(member); }

		std::vector<const Member *> members;

	private:
		friend void InitType(Type* type, std::string& string, unsigned val);

		std::string name;
		unsigned size;
	};

	void InitType(Type* type, std::string& string, unsigned val)
	{
		type->name = string;
		type->size = val;
	}

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
			Init(name, size);
		}

		static void Init(std::string name, unsigned size)
		{
			Type* newType = Get();			//construct
			InitType(newType, name, size);	//set variables
			registerType<Metatype>();		//register

			RegisterMetaData();
		}

		static void RegisterMetaData(void);

		static void AddMember(std::string memberName, unsigned memberOffset, Type *meta)
		{
			Get()->AddMember(new Member(memberName, memberOffset, meta));
		}

		static Metatype* NullCast(void)
		{
			return reinterpret_cast<Metatype *>(NULL);
		}

		// Ensure a single instance can exist for this class type
		static Type *Get(void)
		{
			static Type instance;
			return &instance;
		}
	};


	//////////////////////////////////////////////////////////////////////////////
	//  Member
	//////////////////////////////////////////////////////////////////////////////
	// Purpose: Stores information (name and offset of member) about a data member of a specific class. Multiple
	//          Member objects can be stored in MetaData objects within a std::vector.
	class Member
	{
	public:
		Member::Member(std::string string, unsigned val, Type *meta) : name(string), offset(val), data(meta) {}
		Member::~Member() {}

		const std::string &Name(void) const { return name; } 	// Gettor for name
		unsigned Offset(void) const { return offset; };			// Gettor for offset
		const Type *Meta(void) const { return data; };			// Gettor for data
		const std::string TypeName() const { return data->Name(); }
		int Size() const { return data->Size(); }

	private:
		std::string name;
		unsigned offset;
		const Type *data;
	};



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
		typedef std::unordered_map<std::string, Type *> MetaMap;

		// Insert a MetaData into the map of objects
		static void RegisterMeta(Type *instance)
		{
			std::pair<MetaMap::iterator, bool> result = GetMap().insert(std::make_pair(instance->Name(), instance));
			assert(result.second == true); //if false, already existed
		}

		static const bool IsRegistered(const std::string name)
		{
			return GetMap().count(name) > 0;
		}

		// Retrieve a MetaData instance by string name from the map of MetaData objects
		static Type* Get(std::string name) // NULL if not found
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
	//  Variant
	//////////////////////////////////////////////////////////////////////////////
	class Variant
	{
	public:

		template <typename T>
		Variant(const T& value);

		template <typename T>
		T& GetValue();

		template<typename T>
		const T& GetValue() const;

		template <typename TYPE>
		Variant& operator=(const TYPE& rhs);

	private:
		const Type *meta;
		void *data;
	};

	//////////////////////////////////////////////////////////////////////////////
	//  Meta Definition Functions
	//////////////////////////////////////////////////////////////////////////////

	//registers a type.
	//Defines RegisterMetaData for the TypeCreator, so this must be followed by {}.
	#define meta_define(TYPE)\
		namespace namespace_for_meta_types {																									\
			meta::TypeCreator<meta::RemoveQualifiers<TYPE>::type> NAME_GENERATOR()(#TYPE, sizeof(TYPE));										\
		}																																		\
		meta::RemoveQualifiersPtr<TYPE>::type* TYPE::NullCast(void) { return reinterpret_cast<meta::RemoveQualifiers<Test>::type *>(NULL); }	\
		void TYPE::AddMember(std::string name, unsigned offset, meta::Type *data) { return meta::TypeCreator<meta::RemoveQualifiersPtr<TYPE>::type>::AddMember(name, offset, data); } \
		void meta::TypeCreator<meta::RemoveQualifiersPtr<TYPE>::type>::RegisterMetaData(void) { TYPE::RegisterMetaData(); }						\
		void TYPE::RegisterMetaData(void) //define after this


	//Allows RegisterMetaData (in meta_define) to get access to private members.
	#define meta_expose_internal(TYPE) \
		static void AddMember(std::string name, unsigned offset, meta::Type* data);		\
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
	static Type* get()
	{
		return meta::TypeCreator<RemoveQualifiers<T>::type>::Get();
	}

	//get meta about an object's type
	template<typename T>
	static Type* get(const T& object)
	{
		return meta::TypeCreator<RemoveQualifiers<T>::type>::Get();
	}

	//get meta about an object by name
	static Type* get_name(const std::string& str)
	{
		return Meta::Get(str);
	}

	//get meta about an object by name
	static Type* get_name(const char* str)
	{
		return Meta::Get(str);
	}


	
}




//////////////////////////////////////////////////////////////////////////////
//  Primitive Types
//////////////////////////////////////////////////////////////////////////////
meta_define_pod(bool)
meta_define_pod(int)
meta_define_pod(unsigned int)
meta_define_pod(short)
meta_define_pod(unsigned short)
meta_define_pod(long)
meta_define_pod(unsigned long)
meta_define_pod(float)
meta_define_pod(double)
meta_define_pod(char)
meta_define_pod(char*)
meta_define_pod(unsigned char)
meta_define_pod(unsigned char*)

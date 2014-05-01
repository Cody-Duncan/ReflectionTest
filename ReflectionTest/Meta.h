#pragma once

#include "MacroHelpers.h"
#include <string>
#include <unordered_map>
#include <assert.h>
#include <iostream>

namespace meta
{

	

	//////////////////////////////////////////////////////////////////////////////
	//  Type
	//////////////////////////////////////////////////////////////////////////////
	
	class Type;

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
			Type* newType = Get();
			InitType(newType, name, size);
			registerType<Metatype>();
		}

		// Ensure a single instance can exist for this class type
		static Type *Get(void)
		{
			static Type instance;
			return &instance;
		}
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
		typedef std::unordered_map<std::string, const Type *> MetaMap;

		// Insert a MetaData into the map of objects
		static void RegisterMeta(const Type *instance)
		{
			std::pair<MetaMap::iterator, bool> result = GetMap().insert(std::make_pair(instance->Name(), instance));
			assert(result.second == true); //if false, already existed
		}

		static const bool IsRegistered(const std::string name)
		{
			return GetMap().count(name) > 0;
		}

		// Retrieve a MetaData instance by string name from the map of MetaData objects
		static const Type* Get(std::string name) // NULL if not found
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


	//////////////////////////////////////////////////////////////////////////////
	//  Meta Accessing Functions
	//////////////////////////////////////////////////////////////////////////////

	//////// Define ////////

	//registers a type with the meta system.
	#define meta_define(TYPE)											\
		meta::TypeCreator<TYPE> NAME_GENERATOR()(#TYPE, sizeof(TYPE));	

	//////// Registered? ////////

	//check if a type has been registered
	template<typename T>
	static const bool has()
	{
		return Meta::IsRegistered(meta::TypeCreator<T>::Get()->Name());
	}

	//check if a type of an object has been registered
	template<typename T>
	static const bool has(T& object)
	{
		return Meta::IsRegistered(meta::TypeCreator<T>::Get()->Name());
	}

	//check if a type of an object has been registered
	template<typename T>
	static const bool has(const T& object)
	{
		return Meta::IsRegistered(meta::TypeCreator<T>::Get()->Name());
	}

	//check if a type has been registered, by name
	template<>
	static const bool has<std::string>(std::string& str)
	{
		return Meta::IsRegistered(str);
	}

	//check if a type has been registered, by name
	template<>
	static const bool has<const std::string>(const std::string& str)
	{
		return Meta::IsRegistered(str);
	}

	//check if a type has been registered, by name
	static const bool has(const char* str)
	{
		return Meta::IsRegistered(str);
	}

	//////// Get Meta Information ////////

	//Get meta directly by type
	template<typename T>
	static const Type* get()
	{
		return meta::TypeCreator<T>::Get();
	}

	//get meta about an object's type
	template<typename T>
	static const Type* get(T& object)
	{
		return Meta::Get(meta::TypeCreator<T>::Get()->Name());
	}

	//get meta about an object's type
	template<typename T>
	static const Type* get(const T& object)
	{
		return Meta::Get(meta::TypeCreator<T>::Get()->Name());
	}

	//get meta about an object by name
	template<>
	static const Type* get<std::string>(std::string& str)
	{
		return Meta::Get(str);
	}

	//get meta about an object by name
	template<>
	static const Type* get<std::string>(const std::string& str)
	{
		return Meta::Get(str);
	}


	//get meta about an object by name
	static const Type* get(const char* str)
	{
		return Meta::Get(str);
	}

	template<typename T>
	static void registerType()
	{
		Meta::RegisterMeta(TypeCreator<T>::Get());
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

}
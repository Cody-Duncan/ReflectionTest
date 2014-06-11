#pragma once

template <typename Type> struct destructor
{
	static void destruct(void* obj)
	{
		static_cast<Type*>(obj)->~Type();
	}
};

template <typename Type> struct mover
{
	static void move(void* dst, void* src)
	{
		new (dst)Type(std::move(*static_cast<Type*>(src)));
	}
};

template <typename Type> struct mover<const Type*>
{
	static void move(void* dst, void* src)
	{
		new (dst)Type(*static_cast<Type*>(src));
	}
};
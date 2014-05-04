#pragma once

//////////////////////////////////////////////////////////////////////////////
//  RemoveQualifiers overloads
//////////////////////////////////////////////////////////////////////////////

//
// RemoveQualifiers
// Strips down qualified types/references/pointers to a single unqualified type, for passing into
// a templated type as a typename parameter.
//

namespace meta
{

	template <typename T>
	struct RemoveQualifiers
	{
		typedef T type;
	};


	//strip const
	template <typename T>
	struct RemoveQualifiers<const T>
	{
		typedef T type;
	};

	//strip lvalue reference
	template <typename T>
	struct RemoveQualifiers<T&>
	{
		typedef T type;
	};

	//strip const lvalue reference
	template <typename T>
	struct RemoveQualifiers<const T&>
	{
		typedef T type;
	};

	//strip rvalue reference
	template <typename T>
	struct RemoveQualifiers<T&&>
	{
		typedef T type;
	};

	//degrade array to pointer (for c-strings)
	template <typename T, unsigned int size> 
	struct RemoveQualifiers<T[size]>
	{ 
		typedef typename RemoveQualifiers<T *>::type type;
	}; 
	
	//degrade const array to pointer (for c-strings)
	template <typename T, unsigned int size> 
	struct RemoveQualifiers<const T[size]>
	{ 
		typedef typename RemoveQualifiers<T *>::type type;
	};

	//base type for explicitly removing pointers
	template <typename T>
	struct RemoveQualifiersPtr
	{
		typedef T type;
	};

	//strip pointer
	template <typename T>
	struct RemoveQualifiersPtr<T *>
	{
		typedef T type;
	};

	//strip pointer to const object
	template <typename T>
	struct RemoveQualifiersPtr<const T *>
	{
		typedef T type;
	};
}
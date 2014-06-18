#pragma once


//original from http://www.codeproject.com/Articles/11250/High-Performance-Dynamic-Typing-in-C-using-a-Repla

#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>

namespace anyimpl
{
	struct bad_any_cast {};
	struct empty_any {};

	struct base_any_policy
	{
		virtual void static_delete(void** x) = 0;
		virtual void copy_from_value(void const* src, void** dest) = 0;
		virtual void clone(void* const* src, void** dest) = 0;
		virtual void move(void* const* src, void** dest) = 0;
		virtual void* get_value(void** src) = 0;
		virtual size_t get_size() = 0;
	};

	template<typename T>
	struct typed_base_any_policy : base_any_policy
	{
		virtual size_t get_size() { return sizeof(T); }
	};

	template<typename T>
	struct small_any_policy : typed_base_any_policy<T>
	{
		virtual void static_delete(void** x) {}
		virtual void copy_from_value(void const* src, void** dest){ new (dest) T(*reinterpret_cast<T const*>(src));}
		virtual void clone(void* const* src, void** dest) { *dest = *src; }
		virtual void move(void* const* src, void** dest)  { *dest = *src; }
		virtual void* get_value(void** src) { return reinterpret_cast<void*>(src); }
	};

	template<typename T>
	struct big_any_policy : typed_base_any_policy<T>
	{
		virtual void static_delete(void** x) 
		{
			if(*x)
				delete(*reinterpret_cast<T**>(x));
			*x = NULL;
		}
		virtual void copy_from_value(void const* src, void** dest)
		{ 
			*dest = new T(*reinterpret_cast<T const*>(src));
		}
		virtual void clone(void* const* src, void** dest) 
		{ 
			*dest = new T(**reinterpret_cast<T* const*>(src));
		}
		virtual void move(void* const* src, void** dest)  
		{ 
			(*reinterpret_cast<T**>(dest))->~T(); 
			**reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
		}
		virtual void* get_value(void** src) 
		{ 
			return *src;
		}
	};

	template<typename T>
    struct choose_policy 
    {
        typedef big_any_policy<T> type;
    };

	template<typename T> 
    struct choose_policy<T*> 
    { 
        typedef small_any_policy<T*> type; 
    };

	struct any;

    template<>
    struct choose_policy<any>
    {
		/// Choosing the policy for an any type is illegal, but should never happen.
		/// This is designed to throw a compiler error.
		typedef void type;
    };

	 /// Specializations for small types.
    #define SMALL_POLICY(TYPE) template<> struct \
       choose_policy<TYPE> { typedef small_any_policy<TYPE> type; };

	SMALL_POLICY(signed char);
    SMALL_POLICY(unsigned char);
    SMALL_POLICY(signed short);
    SMALL_POLICY(unsigned short);
    SMALL_POLICY(signed int);
    SMALL_POLICY(unsigned int);
    SMALL_POLICY(signed long);
    SMALL_POLICY(unsigned long);
    SMALL_POLICY(float);
    SMALL_POLICY(bool);

    #undef SMALL_POLICY

	/// This function will return a different policy for each type. 
    template<typename T>
    base_any_policy* get_policy()
    {
        static typename choose_policy<T>::type policy;
        return &policy;
    };
}

struct Any
{
private:
	anyimpl::base_any_policy* policy;
	void* object;

public:

	template<typename T>
	Any(const T& x) : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
	{
		assign(x);
	}

	Any() : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
	{}

	Any(const char* x) : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
	{
		assign(x);
	}

	Any(const Any& x) : : policy(anyimpl::get_policy<anyimpl::empty_any>()), object(NULL)
	{
		assign(x);
	}

	/// Destructor. 
    ~Any() 
	{
        policy->static_delete(&object);
    }

	/// Assignment function from another any. 
	Any& assign(const Any& x) 
	{
        reset();
        policy = x.policy;
        policy->clone(&x.object, &object);
        return *this;
    }

	 /// Assignment function. 
    template <typename T>
    Any& assign(const T& x) 
	{
        reset();
        policy = anyimpl::get_policy<T>();
        policy->copy_from_value(&x, &object);
        return *this;
    }

	/// Assignment operator.
    template<typename T>
    Any& operator=(const T& x) 
	{
        return assign(x);
    }

	/// Assignment operator, specialized for c-strings.
    /// They have types like const char [6] which don't work as expected. 
    Any& operator=(const char* x) 
	{
        return assign(x);
    }

	/// Utility functions
    Any& swap(Any& x) 
	{
        std::swap(policy, x.policy);
        std::swap(object, x.object);
        return *this;
    }

	 /// Cast operator. You can only cast to the original type.
    template<typename T>
    T& cast() 
	{
		if(policy != anyimpl::get_policy<T>())
		{
			throw anyimpl::bad_any_cast();
		}
        T* r = reinterpret_cast<T*>(policy->get_value(&object)); 
        return *r;
    }

	/// Returns true if the any contains no value. 
    bool empty() const 
	{
        return policy == anyimpl::get_policy<anyimpl::empty_any>();
    }

	void reset() 
	{
        policy->static_delete(&object);
        policy = anyimpl::get_policy<anyimpl::empty_any>();
    }

	 /// Returns true if the two types are the same. 
    bool compatible(const Any& x) const 
	{
        return policy == x.policy;
    }
};


/*
//original from http://codereview.stackexchange.com/questions/20058/a-c11-any-class

template<class T>
using StorageType = typename decay<T>::type; 

struct Any
{
	
private:
    struct Base
    {
        virtual ~Base() {}
        virtual Base* clone() const = 0;
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U> Derived(U&& value) : value(forward<U>(value)) { }
        T value;
        Base* clone() const { return new Derived<T>(value); }
    };

public:

    template<typename U> 
	Any(U&& value) : ptr(new Derived<StorageType<U>>(std::forward<U>(value)))
    {

    }

    Any() : ptr(nullptr)
    {}

    Any(Any& that): ptr(that.clone())
    {}

    Any(Any&& that) : ptr(that.ptr)
    {
        that.ptr = nullptr;
    }

    Any(const Any& that) : ptr(that.clone())
    {}

    Any(const Any&& that) : ptr(that.clone())
    {}

	Any& operator=(const Any& a)
	{
		if(ptr == a.ptr)
		{
			return *this;
		}

        auto old_ptr = ptr;

        ptr = a.clone();

		if(old_ptr)
		{
			delete old_ptr;
		}

        return *this;
    }

    Any& operator=(Any&& a)
    {
		if(ptr == a.ptr)
		{
			return *this;
		}
            
        std::swap(ptr, a.ptr);
        return *this;
    }

    ~Any()
    {
		if(ptr)
		{
			delete ptr;
		}
    }

	template<class U>
    operator U()
    {
        return as<StorageType<U>>();
    }

	
	bool empty() const { return ptr == nullptr; }

    template<class U> bool is() const
    {
        typedef StorageType<U> T;
        auto derived = dynamic_cast<Derived<T>*> (ptr);
        return derived == nullptr;
    }

    template<class U>
    StorageType<U>& as()
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*> (ptr);
		if(!derived)
		{
			throw bad_cast();
		}

        return derived->value;
    }

private:
	Base* clone() const
	{
		if(ptr)
		{
			return ptr->clone();
		}
		
		return nullptr;
    }

    Base* ptr;
};

*/


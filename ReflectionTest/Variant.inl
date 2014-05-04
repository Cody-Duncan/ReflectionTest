#pragma once

#include "Meta.h"

namespace meta
{
	//////////////////////////////////////////////////////////////////////////////
	//  Variant
	//////////////////////////////////////////////////////////////////////////////

	//a container to a variable.
	class Variant
	{
	public:

		template <typename T>
		Variant(const T& value) : meta(meta::get<T>()), data(NULL)
		{
			data = meta->NewCopy(&value);
		}

		template <typename TYPE>
		Variant& operator=(const TYPE& rhs)
		{
			Type* type = meta::get<TYPE>();

			// We require a new copy if meta does not match!
			if (meta != type)
			{
				assert(type); // Cannot create instance of NULL meta!

				meta->Delete(data);
				meta = type;
				data = meta->NewCopy(&rhs);
			}
			else
			{
				meta->Copy(data, &rhs);
			}
			return *this;
		}

		template <typename T>
		T& GetValue(void)
		{
			return *reinterpret_cast<T *>(data);
		}

		template <typename T>
		const T& GetValue(void) const
		{
			return *reinterpret_cast<T *>(data);
		}

		const std::string& GetTypeName() const
		{
			return meta->Name();
		}

	private:
		const meta::Type* meta;
		void* data;
	};

	//////////////////////////////////////////////////////////////////////////////
	//  RefVariant
	//////////////////////////////////////////////////////////////////////////////

	//a container to a variable.
	class RefVariant
	{
	public:

		template <typename T>
		RefVariant(const T& value) : meta(meta::get<T>()), reference(NULL)
		{
			reference = (void*)&value;
		}

		template <typename TYPE>
		RefVariant& operator=(const TYPE& rhs)
		{
			Type* type = meta::get<TYPE>();

			// We require a new copy if meta does not match!
			if (meta != type)
			{
				assert(type); // Cannot create instance of NULL meta!
				meta = type;
				reference = (void*)&rhs;
			}
			else
			{
				reference = (void*)&rhs;
			}
			return *this;
		}

		template <typename T>
		T& GetValue(void)
		{
			return *reinterpret_cast<T *>(reference);
		}

		template <typename T>
		const T& GetValue(void) const
		{
			return *reinterpret_cast<T *>(reference);
		}

	private:
		const meta::Type* meta;
		void* reference;
	};
}
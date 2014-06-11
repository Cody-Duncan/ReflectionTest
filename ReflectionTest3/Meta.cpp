#include "Meta.h"

//*************************************************//

namespace meta
{
	Member* TypeInfo::FindMember(const char* name) const
	{
		for(auto member : m_Members)
		{
			if(std::strcmp(member->GetName(), name) == 0)
			{
				return member;
			}
		}

		return nullptr;
	}

	Member* TypeInfo::FindMember(std::string& name) const
	{
		return FindMember(name.c_str());
	}

	
	Method* TypeInfo::FindMethod(const char* name) const
	{
		for(auto method : m_Methods)
		{
			if(std::strcmp(method->GetName(), name) == 0)
			{
				return method;
			}
		}

		return nullptr;
	}

	Method* TypeInfo::FindMethod(std::string& name) const
	{
		return FindMethod(name.c_str());
	}
}

std::unordered_map<std::string, meta::TypeInfo*>* meta::TypeInfo::sTypeInfoDictionary = nullptr;

//special definition for void
const meta::TypeInfo meta::internal::MetaHolder<void>::s_TypeInfo = ::meta::internal::TypeInfoBuilder<void, false>("void", 0);

meta_DEFINE_EXTERN(std::string);
meta_DEFINE_EXTERN(bool);
meta_DEFINE_EXTERN(int);
meta_DEFINE_EXTERN(unsigned int);
meta_DEFINE_EXTERN(short);
meta_DEFINE_EXTERN(unsigned short);
meta_DEFINE_EXTERN(long);
meta_DEFINE_EXTERN(unsigned long);
meta_DEFINE_EXTERN(float);
meta_DEFINE_EXTERN(double);
meta_DEFINE_EXTERN(char);
meta_DEFINE_EXTERN(unsigned char);

meta_DEFINE_EXTERN(char*);
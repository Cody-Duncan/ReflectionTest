#include "Meta.h"

namespace meta
{
	void TypeInfo::AddMember(const Member *member)
	{
		members.push_back(member);
		mamberNames.insert(std::make_pair(member->Name(), member));
	}

	std::vector<TypeInfo> allTypesStorage(200);

	void InitType(TypeInfo* type, std::string& string, unsigned val)
	{
		type->name = string;
		type->size = val;
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
meta_define_pod(std::string)

namespace namespace_for_meta_POD_types {
		meta::TypeCreator<void> voidTypeCreator("void", 0);	
}																									
void meta::TypeCreator<void>::RegisterMetaData(void) {}
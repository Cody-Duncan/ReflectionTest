#include "Meta.h"

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
#include "MetaUtil.h"
#include <string>

namespace metaUtil
{
	const char* TrimTypeToName(const char* fullname)
	{
		const char* name;

		//start at the end of the string, get just the name of the type.
		for(name = fullname + std::strlen(fullname) - 1; name != fullname; --name)
		{
			if(*name == ':' || *name == '>')
			{
				++name;
				break;
			}
		}

		return name;
	}
}
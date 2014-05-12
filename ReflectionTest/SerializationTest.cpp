#include "SerializationTest.h"
#include "jansson.h"

meta_define(Vector3)
{
	meta_add_member(x);
	meta_add_member(y);
	meta_add_member(z);
}

meta_define(Thing)
{
	meta_add_member(size);
	meta_add_member(name);
	meta_add_member(radius);
	meta_add_member(height);
	meta_add_member(position);
}

template<typename T>
T* PointerAdd(void* ptr, int val)
{
	return (T*) (static_cast<char*>(ptr) + val);
}

void assignProperty(const meta::Member* member, void* object, json_t* jObject)
{
	int jtype = json_typeof(jObject);

	switch (jtype)
	{
		case JSON_OBJECT:
		{
			//probably shouldn't get here.
			std::cout << "ERROR: Trying to parse " << member->TypeName() << " " << member->Name() << " as a primitive." << std::endl;
		}
			break;
		case JSON_ARRAY:
		{
			//going to require another parse
		}
			break;
		case JSON_STRING:
		{
			if (member->TypeName().compare("std::string") == 0)
			{
				*PointerAdd<std::string>(object, member->Offset()) = json_string_value(jObject);
			}
			else if (member->TypeName().compare("char*") == 0)
			{
				const char* str = json_string_value(jObject);
				char** dest = PointerAdd<char*>(object, member->Offset());
				strcpy_s(*dest, strlen(str), str);
			}
		}
			break;
		case JSON_INTEGER:
		{
			*PointerAdd<int>(object, member->Offset()) = (int)json_integer_value(jObject);
		}
			break;
		case JSON_REAL:
		{
			if (member->TypeName().compare("float") == 0)
			{
				*PointerAdd<float>(object, member->Offset()) = (float)json_real_value(jObject);
			}
			else
			{
				*PointerAdd<double>(object, member->Offset()) = json_real_value(jObject);
			}
		}
			break;
		case JSON_TRUE:
		{
			*PointerAdd<bool>(object, member->Offset()) = true;
		}
			break;
		case JSON_FALSE:
		{
			*PointerAdd<bool>(object, member->Offset()) = false;
		}
			break;
		case JSON_NULL:
		{
			*PointerAdd<void*>(object, member->Offset()) = 0;
		}
			break;
	}
}


void DeSerializeJsonObject(json_t* jThing, void* thingToBuild, const std::string& typeName)
{
	const char *c_key;
	json_t *value;

	meta::Type* thingType = meta::get_name(typeName);

	json_object_foreach(jThing, c_key, value)
	{
		std::string key = c_key;

		if (thingType->mamberNames.count(key) > 0)
		{
			const meta::Member* member = thingType->mamberNames.at(key);

			if (json_is_object(value))	//if an object, recursively parse
			{
				DeSerializeJsonObject(value, PointerAdd<void>(thingToBuild, member->Offset()), member->TypeName());
			}
			else						//assign primitives
			{
				assignProperty(member, thingToBuild, value);
			}
		}
		else
		{
			std::cout << typeName << " doesn't contain member: " << key << std::endl;
		}
	}
}

bool parseFile(std::string filename)
{
	json_t *json;
	json_error_t error;

	json = json_load_file(filename.c_str(), 0, &error);
	if (!json) 
	{
		return false;
	}
	
	Thing thing;

	json_t *obj = json;
	const char *key;
	json_t *value;

	json_object_foreach(obj, key, value) 
	{
		if (json_is_object(obj) && strcmp(key, "Thing") == 0)
		{
			DeSerializeJsonObject(value, &thing, key);
		}
	}

	std::cout <<
		"Thing" << std::endl <<
		"{" << std::endl;

	printf("%18s %8d\n", "int size", thing.size);
	printf("%18s %8s\n", "std::string name", thing.name.c_str());
	printf("%18s %8.2f\n", "float radius", thing.radius);
	printf("%18s %8.2f\n", "double height", thing.height);
	printf("%18s %8.2f, %4.2f, %4.2f\n", "Vector3 position", thing.position.x, thing.position.y, thing.position.z);
	printf("}\n");
	printf("\n");


	return true;
}

void TestDeSerialization()
{
	parseFile("ThingFile.json");

	return;
}
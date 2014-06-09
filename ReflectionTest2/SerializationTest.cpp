#include "SerializationTest.h"
#include "jansson.h"
#include <iostream>

meta_DEFINE(Vector3)
.member("x", &Vector3::x) 
.member("y", &Vector3::y)
.member("z", &Vector3::z);

meta_DEFINE(Thing)
.member("size",		&Thing::size) 
.member("name",		&Thing::name)
.member("radius",	&Thing::radius)
.member("height",	&Thing::height)
.member("position", &Thing::position);


template<typename T>
T* PointerAdd(void* ptr, int val)
{
	return (T*) (static_cast<char*>(ptr) + val);
}

void assignProperty(const meta::Member* member, meta::Any& object, json_t* jObject)
{
	int jtype = json_typeof(jObject);

	switch (jtype)
	{
		case JSON_OBJECT:
			{
				//probably shouldn't get here.
				std::cout << "ERROR: Trying to parse " << member->GetTypeName() << " " << member->GetName() << " as a primitive." << std::endl;
			}
			break;

		case JSON_ARRAY:
			{
				//going to require another parse
			}
			break;

		case JSON_STRING:
			{
				if (member->GetTypeNameStr().compare( meta::Get<std::string>()->GetNameStr() ) == 0)
				{
					member->Set(object, std::string(json_string_value(jObject)));
				}
				else if (member->GetTypeNameStr().compare("char*") == 0)
				{
					const char* str = json_string_value(jObject);
					char* newStr = new char[strlen(str)];
					strcpy_s(newStr, strlen(str), str);
					member->Set(object, json_string_value(jObject));
				}
			}
			break;

		case JSON_INTEGER:
			{
				member->Set(object, (int)json_integer_value(jObject));
			}
			break;

		case JSON_REAL:
			{
				if (member->GetTypeNameStr().compare("float") == 0)
				{
					member->Set(object, (float)json_real_value(jObject));
				}
				else
				{
					member->Set(object, json_real_value(jObject));
				}
			}
			break;

		case JSON_TRUE:
			{
				member->Set(object, true);
			}
			break;

		case JSON_FALSE:
			{
				member->Set(object, false);
			}
			break;

		case JSON_NULL:
			{
				member->Set(object, 0);
			}
			break;
	}
}


void DeSerializeJsonObject(json_t* jThing, meta::Any& thingToBuild, const std::string& typeName)
{
	const char *c_key;
	json_t *value;

	meta::TypeInfo* thingType = meta::Get_Name(typeName);

	json_object_foreach(jThing, c_key, value)
	{
		std::string key = c_key;

		meta::Member* member = thingType->FindMember(key.c_str());

		if ( member != nullptr)
		{
			if (json_is_object(value))	//if an object, recursively parse
			{
				DeSerializeJsonObject(value, member->Get(thingToBuild), member->GetTypeName());
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
			Thing* t = &thing;
			DeSerializeJsonObject(value, meta::Any(t), key);
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
//*********  Any  *********//
inline void* Any::GetPointer(const TypeInfo* type) const
{
	return m_TypeRecord.type->Adjust(type, GetPointer());
}

//*********  Member  *********//

inline const char* Member::GetTypeName() const 
{ 
	return m_Type->GetName(); 
}

inline const std::string Member::GetTypeNameStr() const 
{ 
	return std::string(m_Type->GetName()); 
}

bool Member::CanGet(const Any& obj) const
{
	if (!obj.GetType()->IsSameOrDerivedFrom(m_Owner))
		return false;
	return true;
}

Any Member::Get(const Any& obj) const
{
	return DoGet(obj);
}

bool Member::CanSet(const Any& obj, const Any& in) const
{
	if (!IsMutable())
		return false;
	if (!obj.GetType()->IsSameOrDerivedFrom(m_Owner))
		return false;
	if (m_Type != in.GetType())
		return false;
	if (obj.IsConst())
		return false;
	return true;
}

void Member::Set(const Any& obj, const Any& in) const
{
	DoSet(obj, in);
}

//*********  Method  *********//

Any Method::Call(const Any& obj, const Any* argv) const
{
	return DoCall(obj, argv);
}

template<class T, typename... Args>
Any Method::Call(T& obj, Args... args) const
{
	Any argArray[] =
	{
		args...
	};
	return DoCall(obj, argArray);
}

template<class T> 
Any Method::Call(T& obj, const Any* argv) const
{
	return DoCall(Any(obj), argv);
}

template<class T> 
Any Method::Call(T& obj, Any* argv) const
{
	return DoCall(Any(obj), argv);
}

template<class T>
Any Method::Call(T& obj) //must be inline (or put in cpp file) because all types are known, so it upgrades to a regular function
{
	return DoCall(obj, nullptr);
}

bool Method::CanCall(const Any& obj, int argc, const Any* argv) const
{
	if(!obj.GetType()->IsSameOrDerivedFrom(m_Owner))
	{
		//LogError("Cannot call method. Object checked is not the same or derived from the class type this method is apart of.");
		return false;
	}

	if(argc != GetArity())
	{
		//LogError("Cannot call method. Methods has %d args, asked if method has %d args.", GetArity(), argc);
		return false;
	}

	for (int i = 0; i < argc; ++i)
	{
		auto tr = GetParamType(i);
		if(argv[i].GetType() != tr.type)
		{
			//LogError("Arguement %d type is %s. Expected type of %s.", i, argv[i].GetType()->GetName(), tr.type->GetName());
			return false;
		}
		if(argv[i].IsConst() && tr.qualifier == TypeRecord::Pointer)
		{
			//LogError("Arguement %d type is %s. Expected type of %s.", i, argv[i].GetType()->GetName(), tr.type->GetName());
			return false;
		}
	}
	return true;
}

template<class T, typename... Args> bool Method::CanCall(T& obj, Args... args) const
{
	Any argArray[] =
	{
		args...
	};
	return CanCall(Any(obj), sizeof...(Args), argArray);
}

template<class T> bool Method::CanCall(T& obj)
{
	return CanCall(Any(obj), 0, nullptr);
}

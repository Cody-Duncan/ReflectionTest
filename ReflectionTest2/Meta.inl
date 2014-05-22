//*********  Any  *********//
inline void* Any::GetPointer(const TypeInfo* type) const
{
	return m_TypeRecord.type->Adjust(type, GetPointer());
}

//*********  Member  *********//

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

Any Method::Call(const Any& obj, int argc, const Any* argv) const
{
	return DoCall(obj, argc, argv);
}

bool Method::CanCall(const Any& obj, int argc, const Any* argv) const
{
	if(!obj.GetType()->IsSameOrDerivedFrom(m_Owner))
	{
		return false;
	}

	if(argc != GetArity())
	{
		return false;
	}

	for (int i = 0; i < argc; ++i)
	{
		auto tr = GetParamType(i);
		if(argv[i].GetType() != tr.type)
		{
			return false;
		}
		if(argv[i].IsConst() && tr.qualifier == TypeRecord::Pointer)
		{
			return false;
		}
	}
	return true;
}
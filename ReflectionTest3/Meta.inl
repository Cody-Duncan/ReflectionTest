//*********  Member  *********//

inline const char* Member::GetTypeName() const 
{ 
	return m_Type->GetName(); 
}

inline const std::string Member::GetTypeNameStr() const 
{ 
	return std::string(m_Type->GetName()); 
}


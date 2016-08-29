/********************************************************************
	created:	2016/05/22
	created:	22:5:2016   10:31
	filename: 	D:\OneDrive\Practice\C++\Reflection\Reflection\Reflection.cpp
	file path:	D:\OneDrive\Practice\C++\Reflection\Reflection
	file base:	Reflection
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Reflection
*********************************************************************/
#include "Reflection.h"

namespace reflection
{
	struct NoBaseClass
	{
		static Type* GetStaticType()
		{
			return nullptr;
		}
	};

	DEFINE_TYPE(Object, NoBaseClass)
	{
	}

	bool Object::IsTypeOf(const Type* type)
	{
		const Type* myType = GetType();
		while (myType)
		{
			if (myType == type)
				return true;
			myType = myType->GetBaseType();
		}
		return false;
	}


#define DEFINE_BUILTIN_TYPE(T) \
	Type* PASTE(s_BuiltinType, __LINE__) = nullptr; \
	void PASTE(InitBuiltinType_, __LINE__)() \
	{ \
		if (PASTE(s_BuiltinType, __LINE__)) \
			return; \
		PASTE(s_BuiltinType, __LINE__) = new Type(#T, nullptr); \
		TypeRegistry::Instance().RegisterType(PASTE(s_BuiltinType, __LINE__)); \
	} \
	Type* TypeOf<T>::GetStaticType() \
	{ \
		if (! PASTE(s_BuiltinType, __LINE__)) \
			PASTE(InitBuiltinType_, __LINE__)(); \
		return PASTE(s_BuiltinType, __LINE__); \
	}

	DEFINE_BUILTIN_TYPE(bool);
	DEFINE_BUILTIN_TYPE(char);
	DEFINE_BUILTIN_TYPE(unsigned char);
	DEFINE_BUILTIN_TYPE(int);
	DEFINE_BUILTIN_TYPE(unsigned int);
	DEFINE_BUILTIN_TYPE(long);
	DEFINE_BUILTIN_TYPE(unsigned long);
	DEFINE_BUILTIN_TYPE(float);
	DEFINE_BUILTIN_TYPE(double);
	DEFINE_BUILTIN_TYPE(const char*);
	DEFINE_BUILTIN_TYPE(std::string);

	Reflection::Reflection()
	{
		TypeRegistry::Instance().Init();
	}

	Reflection::~Reflection()
	{
		TypeRegistry::Instance().Release();
	}
}
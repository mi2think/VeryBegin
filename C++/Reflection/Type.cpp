/********************************************************************
	created:	2016/05/21
	created:	21:5:2016   11:45
	filename: 	D:\OneDrive\Practice\C++\Reflection\Reflection\Type.cpp
	file path:	D:\OneDrive\Practice\C++\Reflection\Reflection
	file base:	Type
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Type
*********************************************************************/
#include "Type.h"

namespace reflection
{
	const char* FieldInfo::GetFieldTagAsStr() const
	{
		switch (fieldTag_)
		{
		case FieldTag::Public:
			return "Public";
		case FieldTag::Protected:
			return "Protected";
		case FieldTag::Private:
			return "Private";
		}
		return "";
	}

	Type::Type(const char* name, Type* baseType)
		: name_(name)
		, baseType_(baseType)
	{}

	Type::~Type()
	{
		for (auto& method : methods_)
		{
			delete method;
		}
		methods_.clear();
	}

	void Type::AddField(const FieldInfo& field)
	{
		fields_.push_back(field);
	}

	FieldInfo* Type::GetField(const std::string& name)
	{
		for (auto& field : fields_)
		{
			if (field.GetName() == name)
				return &field;
		}

		if (baseType_)
			return baseType_->GetField(name);
		return nullptr;
	}

	void Type::AddMethod(MethodInfo* method)
	{
		methods_.push_back(method);
	}

	MethodInfo* Type::GetMethod(const std::string& name)
	{
		for (auto& method : methods_)
		{
			if (method->GetName() == name)
				return method;
		}

		if (baseType_)
			return baseType_->GetMethod(name);
		return nullptr;
	}

	const char* MethodInfo::GetMethodTagAsStr() const
	{
		switch (methodTag_)
		{
		case MethodTag::Public:
			return "Public";
		case MethodTag::Protected:
			return "Protected";
		case MethodTag::Private:
			return "Private";
		}
		return "";
	}
}

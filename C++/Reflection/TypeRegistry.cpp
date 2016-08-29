/********************************************************************
	created:	2016/05/21
	created:	21:5:2016   23:25
	filename: 	D:\OneDrive\Practice\C++\Reflection\Reflection\TypeRegistry.cpp
	file path:	D:\OneDrive\Practice\C++\Reflection\Reflection
	file base:	TypeRegistry
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Type Registry
*********************************************************************/
#include "TypeRegistry.h"
#include "Type.h"
#include <cassert>

namespace reflection
{
	void TypeRegistry::Init()
	{

	}

	void TypeRegistry::Release()
	{
		for (auto& e : typeMap_)
		{
			if (e.second)
				delete e.second;
		}
		typeMap_.clear();
	}

	Type* TypeRegistry::GetType(const std::string& name) const
	{
		auto it = typeMap_.find(name);
		if (it == typeMap_.end())
			return nullptr;
		return it->second;
	}

	void TypeRegistry::RegisterType(Type* type)
	{
		assert(type != nullptr);
		assert(GetType(type->GetName()) == nullptr);
		typeMap_[type->GetName()] = type;
	}

	void TypeRegistry::UnregisterType(Type* type)
	{
		assert(type != nullptr);
		assert(GetType(type->GetName()) != nullptr);
		typeMap_.erase(type->GetName());
	}
}

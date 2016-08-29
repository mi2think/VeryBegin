/********************************************************************
	created:	2016/05/21
	created:	21:5:2016   23:23
	filename: 	D:\OneDrive\Practice\C++\Reflection\Reflection\TypeRegistry.h
	file path:	D:\OneDrive\Practice\C++\Reflection\Reflection
	file base:	TypeRegistry
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Type Registry
*********************************************************************/
#ifndef __TYPE_REGISTRY_H__
#define __TYPE_REGISTRY_H__

#include <string>
#include <unordered_map>

namespace reflection
{
	class Type;

	class TypeRegistry
	{
	public:
		static TypeRegistry& Instance() { static TypeRegistry ins; return ins; }

		void Init();
		void Release();

		Type* GetType(const std::string& name) const;

		void RegisterType(Type* type);
		void UnregisterType(Type* type);
	private:
		std::unordered_map<std::string, Type*> typeMap_;
	};
}


#endif

/********************************************************************
	created:	2016/05/21
	created:	21:5:2016   11:42
	filename: 	D:\OneDrive\Practice\C++\Reflection\Reflection\Type.h
	file path:	D:\OneDrive\Practice\C++\Reflection\Reflection
	file base:	Type
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Type
*********************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

#include <vector>
#include <string>
#include <cassert>

namespace reflection
{
#define DECLARE_ENUM(name) \
		namespace name { \
			enum Type {

#define END_DECLARE_ENUM() \
				,Max \
			}; \
		}

	DECLARE_ENUM(FieldTag)
		Public,
		Protected,
		Private
	END_DECLARE_ENUM()

	DECLARE_ENUM(MethodTag)
		Public,
		Protected,
		Private
	END_DECLARE_ENUM()

	class Type;

	class FieldInfo
	{
	public:
		FieldInfo(const char* name, const Type* type, int offset, int fieldTag)
			: name_(name)
			, type_(type)
			, offset_(offset)
			, fieldTag_(fieldTag)
		{}

		const char* GetName() const { return name_; }
		const Type* GetType() const { return type_; }
		int GetOffset() const { return offset_; }
		int GetFieldTag() const { return fieldTag_; }
		void* GetData(void* obj) const { return (char*)obj + offset_; }

		const char* GetFieldTagAsStr() const;
	private:
		const char* name_;
		const Type* type_;
		int offset_;
		int fieldTag_;
	};

	class MethodInfo
	{
	public:
		MethodInfo(const char* name, int methodTag)
			: name_(name)
			, methodTag_(methodTag)
		{}
		virtual ~MethodInfo() {}

		const char* GetName() const { return name_; }
		const char* GetMethodTagAsStr() const;

		template<typename R, typename T, typename...Args>
		R Invoke(T* obj, Args&&...args)
		{
			auto method = static_cast<MethodInfoT<R, T, Args...>*>(this);
			return method->Invoke(obj, std::forward<Args>(args)...);
		}
	private:
		const char* name_;
		int methodTag_;
	};

	template<typename R, typename T, typename...Args>
	class MethodInfoT : public MethodInfo
	{
	public:
		MethodInfoT(const char* name, R(T::* func)(Args...), int methodTag)
			: MethodInfo(name, methodTag)
			, func_(func)
		{}

		R Invoke(T* obj, Args&&...args)
		{
			return (obj->*func_)(std::forward<Args>(args)...);
		}
	private:
		R(T::*func_)(Args...);
	};


	class Type
	{
	public:
		Type(const char* name, Type* baseType);
		~Type();

		const std::string& GetName() const { return name_; }
		const Type* GetBaseType() const { return baseType_; }

		// fields
		void AddField(const FieldInfo& field);
		const std::vector<FieldInfo>& GetFields() const { return fields_; }
		FieldInfo* GetField(const std::string& name);
		
		// methods
		void AddMethod(MethodInfo* method);
		const std::vector<MethodInfo*>& GetMethods() const { return methods_; }
		MethodInfo* GetMethod(const std::string& name);
	private:
		std::string name_;
		Type* baseType_;
		std::vector<FieldInfo> fields_;
		std::vector<MethodInfo*> methods_;
	};
}

#endif

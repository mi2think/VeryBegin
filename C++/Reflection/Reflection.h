/********************************************************************
	created:	2016/05/21
	created:	21:5:2016   21:56
	filename: 	D:\OneDrive\Practice\C++\Reflection\Reflection\Reflection.h
	file path:	D:\OneDrive\Practice\C++\Reflection\Reflection
	file base:	Reflection
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Reflection
*********************************************************************/
#ifndef __REFLECTION_H__
#define __REFLECTION_H__

#include "Ref.h"
#include "Type.h"
#include "TypeRegistry.h"

#include <cassert>

namespace reflection
{
	template<typename R, typename T>
	inline size_t member_offset(R T::* member) { return (size_t) &(((T*)nullptr)->*member); }

	class Type;

	template<typename Tp>
	struct AsTypeTp { AsTypeTp() { Tp::GetStaticType(); } };

#define PASTE_INTERNAL(a, b) a##b
#define PASTE(a, b) PASTE_INTERNAL(a, b)

#define DECLARE_TYPE(T) \
public:\
	static Type* s_Type##T; \
    static Type* GetStaticType(); \
	static void InitType_##T(); \
    static void RegisterFields(Type& type); \
	virtual Type* GetType() const; \
	typedef T this_type;

#define DEFINE_TYPE(T, Base) \
	Type* T::s_Type##T = nullptr; \
	Type* T::GetStaticType() \
	{ \
		if (! s_Type##T) \
			InitType_##T(); \
		return s_Type##T; \
	} \
	Type* T::GetType() const \
	{ \
		return T::GetStaticType(); \
	} \
	void T::InitType_##T() \
	{ \
		if (s_Type##T) \
			return; \
		Type* baseType = Base::GetStaticType(); \
		s_Type##T = new Type(#T, baseType); \
		TypeRegistry::Instance().RegisterType(s_Type##T); \
		T::RegisterFields(*s_Type##T); \
	} \
	AsTypeTp<T> s_AsType##T; \
	void T::RegisterFields(Type& type) // caller define block

#define DEFINE_FIELD(field) \
    type.AddField(MakeFieldInfo(#field, &this_type::field))

#define DEFINE_FIELD_WITH_TAG(field, fieldTag) \
    type.AddField(MakeFieldInfo(#field, &this_type::field, fieldTag))

#define DEFINE_METHOD(method) \
	type.AddMethod(MakeMethodInfo(#method, &this_type::method))

#define DEFINE_METHOD_WITH_TAG(method, methodTag) \
	type.AddMethod(MakeMethodInfo(#method, &this_type::method, methodTag))



	// base class for reflecting objects 
	class Object
	{
	public:
		DECLARE_TYPE(Object);
		virtual ~Object() {}
		bool IsTypeOf(const Type* type);
	};

	// Type of
	template<typename T>
	struct TypeOf
	{
		static Type* GetStaticType()
		{
			return T::GetStaticType();
		}
	};
	template<> struct TypeOf<bool> { static Type* GetStaticType(); };
	template<> struct TypeOf<char> { static Type* GetStaticType(); };
	template<> struct TypeOf<unsigned char> { static Type* GetStaticType(); };
	template<> struct TypeOf<int> { static Type* GetStaticType(); };
	template<> struct TypeOf<unsigned int> { static Type* GetStaticType(); };
	template<> struct TypeOf<long> { static Type* GetStaticType(); };
	template<> struct TypeOf<unsigned long> { static Type* GetStaticType(); };
	template<> struct TypeOf<float> { static Type* GetStaticType(); };
	template<> struct TypeOf<double> { static Type* GetStaticType(); };
	template<> struct TypeOf<const char*> { static Type* GetStaticType(); };
	template<> struct TypeOf<std::string> { static Type* GetStaticType(); };

	template<typename R, typename T>
	inline FieldInfo MakeFieldInfo(const char* name, R T::* member)
	{
		return FieldInfo(name, TypeOf<R>::GetStaticType(), member_offset<R, T>(member), FieldTag::Public);
	}

	template<typename R, typename T>
	inline FieldInfo MakeFieldInfo(const char* name, R T::* member, int fieldTag)
	{
		return FieldInfo(name, TypeOf<R>::GetStaticType(), member_offset<R, T>(member), fieldTag);
	}

	template<typename R, typename T, typename...Args>
	inline MethodInfo* MakeMethodInfo(const char* name, R(T::*func)(Args...))
	{
		return new MethodInfoT<R, T, Args...>(name, func, MethodTag::Public);
	}

	template<typename R, typename T, typename...Args>
	inline MethodInfo* MakeMethodInfo(const char* name, R(T::*func)(Args...), int methodTag)
	{
		return new MethodInfoT<R, T, Args...>(name, func, methodTag);
	}

	struct Reflection
	{
		Reflection();
		~Reflection();
	};
}

#endif
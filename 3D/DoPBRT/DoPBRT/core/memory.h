/********************************************************************
	created:	2016/06/16
	created:	16:6:2016   16:48
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\memory.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	memory
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_MEMORY_H__
#define __CORE_MEMORY_H__

#include "pbrt.h"
#include "parallel.h"

namespace dopbrt
{
	class RefCounted
	{
	public:
		RefCounted() : counts_(0) {}
		RefCounted(const RefCounted&) = delete;
		RefCounted& operator=(const RefCounted&) = delete;

		AtomicInt32 counts_;
	};

	template<typename T>
	class Ref
	{
	public:
		Ref(T* p = nullptr) : ptr_(p)
		{
			if (ptr_)
				AtomicAdd(&ptr_->counts_, 1);
		}
		Ref(const Ref<T>& r) : ptr_(r.ptr_)
		{
			if (ptr_)
				AtomicAdd(&ptr_->counts_, 1);
		}
		~Ref()
		{
			if (ptr_ && AtomicAdd(&ptr_->counts_, -1) == 0)
				delete ptr_;
		}

		Ref& operator=(T* p)
		{
			if (p)
				AtomicAdd(&p->counts_, 1);
			if (ptr_ && AtomicAdd(&ptr_->counts_, -1) == 0)
				delete ptr_;
			ptr_ = p;
			return *this;
		}
		Ref& operator=(const Ref<T>& r)
		{
			if (r.ptr_)
				AtomicAdd(&r.ptr_->counts_, 1);
			if (ptr_ && AtomicAdd(&ptr_->counts_, -1) == 0)
				delete ptr_;
			ptr_ = r.ptr_;
			return *this;
		}

		T* operator->() { return ptr_; }
		const T* operator->() const { return ptr_; }
		operator bool() const { return ptr_ != nullptr; }
		const T* Get() const { return ptr_; }
	private:
		T* ptr_;
	};
}

#endif

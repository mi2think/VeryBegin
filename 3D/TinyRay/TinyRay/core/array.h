/********************************************************************
	created:	2016/07/05
	created:	5:7:2016   22:13
	filename: 	D:\Code\TinyRay\TinyRay\core\array.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	array
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <cassert>

namespace tinyray
{
	template<typename T>
	class Array
	{
	public:
		Array() 
			: dataCount_(0)
			, arraySize_(4)
			, data_(new T[4])
		{}
		Array(int size)
			: dataCount_(size)
			, arraySize_(size)
			, data_(new T[size])
		{}
		~Array() { if (data_) delete[] data_; }

		T& operator[](int index) { assert(index < dataCount_); return data_[index]; }
		const T& operator[](int index) const { assert(index < dataCount_); return data_[index]; }

		int  Length() const { return dataCount_; }
		bool Empty() const { return Length() == 0; }
		void Clear() { dataCount_ = 0; }
		void Reserve(int size)
		{
			if (size > arraySize_)
			{
				arraySize_ = size;
				T* p = data_;
				data_ = new T[arraySize_];
				if (dataCount_ > 0)
				{
					for (int i = 0; i < dataCount_; ++i)
						data_[i] = p[i];
					delete[] p;
				}
			}
		}
		void Resize(int size)
		{
			Reserve(size);
			dataCount_ = size;
		}
		void Append(const T& elem)
		{
			if (dataCount_ == arraySize_)
			{
				arraySize_ *= 2;
				T* p = data_;
				data_ = new T[arraySize_];
				for (int i = 0; i < dataCount_; ++i)
					data_[i] = p[i];
				delete[] p;
			}
			data_[dataCount_++] = elem;
		}
		void Shrink()
		{
			if (dataCount_ != arraySize_)
			{
				T* p = data_;
				arraySize_ = dataCount_;
				data_ = new T[dataCount_];
				for (int i = 0; i < dataCount_; ++i)
					data_[i] = p[i];
				delete[] p;
			}
		}
	private:
		T* data_;
		int dataCount_;
		int arraySize_;
	};
}

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

RefCounter
Version 19.10.21.0000

This file is part of RefCounter

RefCounter is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

// 支持引用计数与垃圾回收的指针
template <typename T> class Ref
{
private:
	T* _Ptr;
	size_t* _Count;

	inline void _Increase()
	{
		if (_Ptr)
		{
			(*_Count)++;
		}
	}

	inline void _Decrease()
	{
		if (_Ptr)
		{
			(*_Count)--;

			if (*_Count == 0)
			{
				delete _Ptr;
				delete _Count;
			}
		}
	}

	Ref()
	{
		_Ptr = nullptr;
		_Count = new size_t(0);
	}

public:
	Ref(const T* ptr = nullptr)
	{
		_Ptr = ptr;
		_Count = new size_t(_Ptr ? 1 : 0);
	}

	Ref(const T& val)
	{
		_Ptr = new T();
		_Count = new size_t(_Ptr ? 1 : 0);

		*_Ptr = val;
	}

	Ref(const Ref& ref)
	{
		_Ptr = ref._Ptr;
		_Count = ref._Count;

		_Increase();
	}

	Ref& operator=(const Ref& ref)
	{
		if (this == &ref)
		{
			return *this;
		}

		if (_Ptr == ref._Ptr)
		{
			return *this;
		}

		_Decrease();

		_Ptr = ref._Ptr;
		_Count = ref._Count;

		_Increase();

		return *this;
	}

	~Ref()
	{
		_Decrease();
	}

	inline bool operator==(const Ref& ref) const
	{
		return (_Ptr == ref._Ptr);
	}

	inline bool operator!=(const Ref& ref) const
	{
		return (_Ptr != ref._Ptr);
	}

	inline T& operator*()
	{
		return *_Ptr;
	}

	inline const T& operator*() const
	{
		return *_Ptr;
	}

	inline T* operator->()
	{
		return _Ptr;
	}

	inline const T* operator->() const
	{
		return _Ptr;
	}

	inline operator T() const
	{
		return *_Ptr;
	}
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

RefCounter
Version 19.10.25.0000

This file is part of RefCounter

RefCounter is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <map>

using namespace std;

// 引用计数器
class RefCounter
{
private:
	static map<void*, size_t> _Obj;

	using pair = map<void*, size_t>::value_type;
	using iterator = map<void*, size_t>::iterator;

protected:
	RefCounter();
	RefCounter(const RefCounter&);
	RefCounter& operator=(const RefCounter&);
	virtual ~RefCounter();

	size_t Increase(void* ptr);
	size_t Decrease(void* ptr);
};

// 支持引用计数与垃圾回收的指针
template <typename T> class Ref : private RefCounter
{
private:
	T* _Ptr;

	_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
		void* __CRTDECL operator new(size_t _Size)
	{
		return nullptr;
	}

	_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
		void* __CRTDECL operator new(size_t _Size, std::nothrow_t const&) noexcept
	{
		return nullptr;
	}

	_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
		void* __CRTDECL operator new[](size_t _Size)
	{
		return nullptr;
	}

		_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
		void* __CRTDECL operator new[](size_t _Size, std::nothrow_t const&) noexcept
	{
		return nullptr;
	}

		void __CRTDECL operator delete(void* _Block) noexcept
	{
	}

	void __CRTDECL operator delete(void* _Block, std::nothrow_t const&) noexcept
	{
	}

	void __CRTDECL operator delete[](void* _Block) noexcept
	{
	}

		void __CRTDECL operator delete[](void* _Block, std::nothrow_t const&) noexcept
	{
	}

		void __CRTDECL operator delete(void* _Block, size_t _Size) noexcept
	{
	}

	void __CRTDECL operator delete[](void* _Block, size_t _Size) noexcept
	{
	}

		inline void _Increase()
	{
		Increase(_Ptr);
	}

	inline void _Decrease()
	{
		if (Decrease(_Ptr) == 0 && _Ptr)
		{
			delete _Ptr;
		}
	}

public:
	Ref() :
		_Ptr(nullptr)
	{
		_Increase();
	}

	Ref(const Ref& ref) :
		_Ptr(ref._Ptr)
	{
		_Increase();
	}

	Ref(nullptr_t) :
		_Ptr(nullptr)
	{
		_Increase();
	}

	Ref(T* ptr) :
		_Ptr(ptr)
	{
		_Increase();
	}

	Ref(const T* ptr) :
		_Ptr(ptr)
	{
		_Increase();
	}

	Ref(const T& val)
	{
		_Ptr = new T();
		*_Ptr = val;

		_Increase();
	}

	Ref(T&& val)
	{
		_Ptr = new T();
		*_Ptr = val;

		_Increase();
	}

	Ref& operator=(const Ref& ref)
	{
		if (this == &ref)
		{
			return *this;
		}

		if (_Ptr != ref._Ptr)
		{
			_Decrease();

			_Ptr = ref._Ptr;
		}

		_Increase();

		return *this;
	}

	Ref& operator=(nullptr_t)
	{
		if (_Ptr != nullptr)
		{
			_Decrease();

			_Ptr = nullptr;
		}

		_Increase();

		return *this;
	}

	Ref& operator=(T* ptr)
	{
		if (_Ptr != ptr)
		{
			_Decrease();

			_Ptr = ptr;
		}

		_Increase();

		return *this;
	}

	Ref& operator=(const T* ptr)
	{
		if (_Ptr != ptr)
		{
			_Decrease();

			_Ptr = const_cast<T*>(ptr);
		}

		_Increase();

		return *this;
	}

	Ref& operator=(const T& val)
	{
		_Decrease();

		_Ptr = new T();
		*_Ptr = val;

		_Increase();

		return *this;
	}

	Ref& operator=(T&& val)
	{
		_Decrease();

		_Ptr = new T();
		*_Ptr = val;

		_Increase();

		return *this;
	}

	virtual ~Ref()
	{
		_Decrease();
	}

	inline bool operator==(const Ref& ref) const
	{
		return (_Ptr == ref._Ptr);
	}

	inline bool operator==(nullptr_t) const
	{
		return (_Ptr == nullptr);
	}

	inline bool operator==(T* ptr) const
	{
		return (_Ptr == ptr);
	}

	inline bool operator==(const T* ptr) const
	{
		return (_Ptr == ptr);
	}

	inline bool operator!=(const Ref& ref) const
	{
		return (_Ptr != ref._Ptr);
	}

	inline bool operator!=(nullptr_t) const
	{
		return (_Ptr != nullptr);
	}

	inline bool operator!=(T* ptr) const
	{
		return (_Ptr != ptr);
	}

	inline bool operator!=(const T* ptr) const
	{
		return (_Ptr != ptr);
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

	Ref RefCopy() const
	{
		return Ref(_Ptr);
	}

	Ref BinaryCopy() const
	{
		Ref ref;

		if (_Ptr)
		{
			ref._Decrease();

			ref._Ptr = new T();
			memcpy(ref._Ptr, _Ptr, sizeof(T));

			ref._Increase();
		}

		return ref;
	}

	Ref ShallowCopy() const
	{
		Ref ref;

		if (_Ptr)
		{
			ref._Decrease();

			ref._Ptr = new T();
			*ref._Ptr = *_Ptr;

			ref._Increase();
		}

		return ref;
	}
};
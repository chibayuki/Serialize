/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

RefCounter
Version 19.10.26.0000

This file is part of RefCounter

RefCounter is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RefCounter.h"

map<void*, size_t> RefCounter::_Obj;

void* RefCounter::operator new(size_t _Size)
{
	return ::operator new(_Size);
}

void* RefCounter::operator new(size_t _Size, const std::nothrow_t& _Tag) noexcept
{
	return ::operator new(_Size, _Tag);
}

void* RefCounter::operator new[](size_t _Size)
{
	return ::operator new[](_Size);
}

void* RefCounter::operator new[](size_t _Size, const std::nothrow_t& _Tag) noexcept
{
	return ::operator new[](_Size, _Tag);
}

void RefCounter::operator delete(void* _Block) noexcept
{
	::operator delete(_Block);
}

void RefCounter::operator delete(void* _Block, const std::nothrow_t& _Tag) noexcept
{
	::operator delete(_Block, _Tag);
}

void RefCounter::operator delete[](void* _Block) noexcept
{
	::operator delete[](_Block);
}

void RefCounter::operator delete[](void* _Block, const std::nothrow_t& _Tag) noexcept
{
	::operator delete[](_Block, _Tag);
}

void RefCounter::operator delete(void* _Block, size_t _Size) noexcept
{
	::operator delete(_Block, _Size);
}

void RefCounter::operator delete[](void* _Block, size_t _Size) noexcept
{
	::operator delete[](_Block, _Size);
}

#ifdef __cpp_aligned_new
void* RefCounter::operator new(size_t _Size, std::align_val_t _Al)
{
	return ::operator new(_Size, _Al);
}

void* RefCounter::operator new(size_t _Size, std::align_val_t _Al, const std::nothrow_t& _Tag) noexcept
{
	return ::operator new(_Size, _Al, _Tag);
}

void* RefCounter::operator new[](size_t _Size, std::align_val_t _Al)
{
	return ::operator new[](_Size, _Al);
}

void* RefCounter::operator new[](size_t _Size, std::align_val_t _Al, const std::nothrow_t& _Tag) noexcept
{
	return ::operator new[](_Size, _Al, _Tag);
}

void RefCounter::operator delete(void* _Block, std::align_val_t _Al) noexcept
{
	::operator delete(_Block, _Al);
}

void RefCounter::operator delete(void* _Block, std::align_val_t _Al, const std::nothrow_t& _Tag) noexcept
{
	::operator delete(_Block, _Al, _Tag);
}

void RefCounter::operator delete[](void* _Block, std::align_val_t _Al) noexcept
{
	::operator delete[](_Block, _Al);
}

void RefCounter::operator delete[](void* _Block, std::align_val_t _Al, const std::nothrow_t& _Tag) noexcept
{
	::operator delete[](_Block, _Al, _Tag);
}

void RefCounter::operator delete(void* _Block, size_t _Size, std::align_val_t _Al) noexcept
{
	::operator delete(_Block, _Size, _Al);
}

void RefCounter::operator delete[](void* _Block, size_t _Size, std::align_val_t _Al) noexcept
{
	::operator delete[](_Block, _Size, _Al);
}
#endif

RefCounter::RefCounter()
{
}

RefCounter::RefCounter(const RefCounter&)
{
}

RefCounter& RefCounter::operator=(const RefCounter&)
{
	return *this;
}

RefCounter::~RefCounter()
{
}

size_t RefCounter::Increase(void* ptr)
{
	if (ptr)
	{
		if (_Obj.empty())
		{
			_Obj.insert(pair(ptr, 1));

			return 1;
		}
		else
		{
			iterator i = _Obj.find(ptr);

			if (i == _Obj.end())
			{
				_Obj.insert(pair(ptr, 1));

				return 1;
			}
			else
			{
				i->second++;

				return i->second;
			}
		}
	}
	else
	{
		return 0;
	}
}

size_t RefCounter::Decrease(void* ptr)
{
	if (ptr)
	{
		if (!_Obj.empty())
		{
			iterator i = _Obj.find(ptr);

			if (i != _Obj.end())
			{
				if (i->second > 0)
				{
					i->second--;
				}

				size_t count = i->second;

				if (count == 0)
				{
					_Obj.erase(i);
				}

				return count;
			}
		}

		return 0;
	}
	else
	{
		return 0;
	}
}

#if DEBUG
size_t RefCounter::GetCount(void* ptr)
{
	if (ptr)
	{
		if (!_Obj.empty())
		{
			iterator i = _Obj.find(ptr);

			if (i != _Obj.end())
			{
				return i->second;
			}
		}

		return 0;
	}
	else
	{
		return 0;
	}
}
#endif

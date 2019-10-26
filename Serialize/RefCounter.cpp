/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

RefCounter
Version 19.10.23.0000

This file is part of RefCounter

RefCounter is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RefCounter.h"

map<void*, size_t> RefCounter::_Obj;

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
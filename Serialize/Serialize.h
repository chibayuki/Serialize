/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

Serialize
Version 19.10.20.0000

This file is part of Serialize

Serialize is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <cstdint>
#include <cstring>
#include <iterator>
#include <map>
#include <set>
#include <list>
#include <vector>

#include "RefCounter.h"

using namespace std;

// 表示对象允许释放其托管资源
class IDisposable
{
public:
	virtual void Dispose() = 0;
};

// 二进制块
static class Block
{
	friend class BlockBuilder;

private:
	size_t _Size;
	char* _Ptr;

	inline void _Dispose()
	{
		_Size = 0;

		if (_Ptr)
		{
			delete _Ptr;
		}
	}

	Block()
	{
		_Size = 0;
		_Ptr = nullptr;
	}

public:
	Block(const size_t size, const char* ptr = nullptr)
	{
		_Size = size;
		_Ptr = (_Size > 0 ? new char[_Size] : nullptr);

		if (ptr)
		{
			memcpy(_Ptr, ptr, _Size);
		}
	}

	Block(const Block& block)
	{
		_Size = block._Size;
		_Ptr = (_Size > 0 ? new char[_Size] : nullptr);

		if (block._Ptr)
		{
			memcpy(_Ptr, block._Ptr, _Size);
		}
	}

	Block& operator=(const Block& block)
	{
		if (this == &block)
		{
			return *this;
		}

		_Dispose();

		_Size = block._Size;
		_Ptr = (_Size > 0 ? new char[_Size] : nullptr);

		if (block._Ptr)
		{
			memcpy(_Ptr, block._Ptr, _Size);
		}

		return *this;
	}

	~Block()
	{
		_Dispose();
	}
};

// 二进制块构建器
class BlockBuilder
{
private:
	list<Ref<Block>> _Blocks;

	inline void _Dispose()
	{
		if (!_Blocks.empty())
		{
			_Blocks.clear();
		}
	}

public:
	BlockBuilder()
	{

	}

	BlockBuilder(const BlockBuilder& blockBuilder)
	{

	}

	BlockBuilder& operator=(const BlockBuilder& blockBuilder)
	{
		if (this == &blockBuilder)
		{
			return *this;
		}

		return *this;
	}

	~BlockBuilder()
	{
		_Dispose();
	}
};

// 序列化器
class Serializer
{
private:
	inline void _Dispose()
	{

	}

public:
	Serializer()
	{

	}

	Serializer(const Serializer& serializer)
	{

	}

	Serializer& operator=(const Serializer& serializer)
	{
		if (this == &serializer)
		{
			return *this;
		}

		return *this;
	}

	~Serializer()
	{
		_Dispose();
	}
};

// 表示对象支持序列化
class ISerializable
{
private:
	inline void _Dispose()
	{

	}

public:
	ISerializable()
	{

	}

	ISerializable(const ISerializable& iSerializable)
	{

	}

	ISerializable& operator=(const ISerializable& iSerializable)
	{
		if (this == &iSerializable)
		{
			return *this;
		}

		return *this;
	}

	~ISerializable()
	{
		_Dispose();
	}
};
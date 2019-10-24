/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

Serialize
Version 19.10.25.0000

This file is part of Serialize

Serialize is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <iterator>
#include <map>
#include <set>
#include <list>
#include <vector>

#include "RefCounter.h"

using namespace std;

using byte = uint8_t;

// 二进制块
class Chunk
{
private:
	size_t _Size;
	byte* _Ptr;

	inline void _Dispose()
	{
		_Size = 0;

		if (_Ptr)
		{
			delete _Ptr;
			_Ptr = nullptr;
		}
	}

public:
	Chunk() :
		_Size(0),
		_Ptr(nullptr)
	{
	}

	Chunk(const Chunk& chunk)
	{
		_Size = chunk._Size;
		_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);

		if (chunk._Ptr)
		{
			memcpy(_Ptr, chunk._Ptr, _Size);
		}
	}

	Chunk(const size_t size, const byte* ptr)
	{
		_Size = size;
		_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);

		if (ptr)
		{
			memcpy(_Ptr, ptr, _Size);
		}
	}

	Chunk(const size_t size)
	{
		_Size = size;
		_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);
	}

	Chunk& operator=(const Chunk& chunk)
	{
		if (this == &chunk)
		{
			return *this;
		}

		_Dispose();

		_Size = chunk._Size;
		_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);

		if (chunk._Ptr)
		{
			memcpy(_Ptr, chunk._Ptr, _Size);
		}

		return *this;
	}

	virtual ~Chunk()
	{
		_Dispose();
	}

	inline size_t Size() const
	{
		return _Size;
	}

	inline const byte* Ptr() const
	{
		return _Ptr;
	}
};

using ChunkRef = Ref<Chunk>;

// 二进制块构建器
class ChunkBuilder
{
private:
	list<ChunkRef> _Chunks;

	inline void _Dispose()
	{
		if (!_Chunks.empty())
		{
			_Chunks.clear();
		}
	}

public:
	ChunkBuilder() :
		_Chunks()
	{
	}

	ChunkBuilder(const ChunkBuilder& chunkBuilder)
	{
		_Chunks = chunkBuilder._Chunks;
	}

	ChunkBuilder& operator=(const ChunkBuilder& chunkBuilder)
	{
		if (this == &chunkBuilder)
		{
			return *this;
		}

		_Dispose();

		_Chunks = chunkBuilder._Chunks;

		return *this;
	}

	virtual ~ChunkBuilder()
	{
		_Dispose();
	}

	inline void Append(const ChunkRef& chunk)
	{
		_Chunks.push_back(chunk);
	}

	inline void Prepend(const ChunkRef& chunk)
	{
		_Chunks.push_front(chunk);
	}

	inline void Clear()
	{
		_Dispose();
	}

	inline bool IsEmpty() const
	{
		return _Chunks.empty();
	}

	ChunkRef Combine()
	{
		size_t size = 0;

		for (list<ChunkRef>::const_iterator i = _Chunks.begin(); i != _Chunks.end(); ++i)
		{
			size += (*i)->Size();
		}

		ChunkRef chunk = new Chunk(size);
		byte* ptr = const_cast<byte*>(chunk->Ptr());

		for (list<ChunkRef>::const_iterator i = _Chunks.begin(); i != _Chunks.end(); ++i)
		{
			size_t s = (*i)->Size();

			if (s > 0)
			{
				memcpy(ptr, (*i)->Ptr(), s);
				ptr += s;
			}
		}

		_Dispose();

		return chunk;
	}
};

// 序列化器
class Serializer
{
	// 数据类型
	enum DataType
	{
		Unknown = -1, // 未知（不支持）

		Void, // void（不支持）
		Null, // std::nullptr_t（不支持）

		Binary, // Ref<Chunk>

		Arithmetic, // 基本数据类型
		Enum, // 枚举
		Struct, // 结构体与类
		Union, // 联合体

		Array, // 数组（不支持）
		Pointer, // 指针（不支持）
		Function, // 函数（不支持）
		Object, // 对象（不支持）

		String, // std::string

		List, // std::list
		Map, // std::map
		Set, // std::set
		Vector // std::vector
	};

	template<typename T> DataType GetDataType()
	{
		if (is_same<T, ChunkRef>::value)
		{
			return Binary;
		}
		else if (is_same<T, string>::value)
		{
			return String;
		}
		else if (is_void<T>::value)
		{
			return Void;
		}
		else if (is_null_pointer<T>::value)
		{
			return Null;
		}
		else if (is_arithmetic<T>::value)
		{
			return Arithmetic;
		}
		else if (is_enum<T>::value)
		{
			return Enum;
		}
		else if (is_class<T>::value)
		{
			return Struct;
		}
		else if (is_union<T>::value)
		{
			return Union;
		}
		else if (is_array<T>::value)
		{
			return Array;
		}
		else if (is_pointer<T>::value || is_member_pointer<T>::value)
		{
			return Pointer;
		}
		else if (is_function<T>::value)
		{
			return Function;
		}
		else if (is_object<T>::value)
		{
			return Object;
		}
		else
		{
			return Unknown;
		}
	}

	// 元数据
	struct Metadata
	{
	public:
		DataType Type;
		size_t Size;

		Metadata() :
			Type(Void),
			Size(0)
		{
		}

		Metadata(DataType type, size_t size) :
			Type(type),
			Size(size)
		{
		}
	};

private:
	ChunkBuilder _ChunkBuilder;
	list<Metadata> _Metadata;
	ChunkRef _Chunk;
	size_t _Index;
	byte* _Ptr;

	inline void _Dispose()
	{
		if (!_ChunkBuilder.IsEmpty())
		{
			_ChunkBuilder.Clear();
		}

		if (!_Metadata.empty())
		{
			_Metadata.clear();
		}

		if (_Chunk != nullptr)
		{
			_Chunk = nullptr;
		}

		_Index = 0;
		_Ptr = nullptr;
	}

	Serializer(const Serializer&)
	{
	}

	Serializer& operator=(const Serializer&)
	{
		return *this;
	}

	void PackStart()
	{
		_Dispose();
	}

	Serializer& PackFinish(ChunkRef& chunk)
	{
		PackMetadata();

		chunk = _ChunkBuilder.Combine();

		_Dispose();

		return *this;
	}

	Serializer& UnpackStart(const ChunkRef& chunk)
	{
		_Dispose();

		_Chunk = chunk;
		_Index = 0;
		_Ptr = (chunk != nullptr ? const_cast<byte*>(_Chunk->Ptr()) : nullptr);

		UnpackMetadata();

		return *this;
	}

	void UnpackFinish()
	{
		_Dispose();
	}

	void PackMetadata()
	{
		size_t sT = sizeof(Metadata);

		for (list<Metadata>::const_reverse_iterator i = _Metadata.rbegin(); i != _Metadata.rend(); ++i)
		{
			_ChunkBuilder.Prepend(new Chunk(sT, (byte*)(&(*i))));
		}

		size_t n = _Metadata.size();
		size_t sN = sizeof(size_t);

		_ChunkBuilder.Prepend(new Chunk(sN, (byte*)(&n)));
	}

	void UnpackMetadata()
	{
		if (_Ptr)
		{
			size_t n = 0;
			size_t sN = sizeof(size_t);

			memcpy(&n, _Ptr, sN);
			_Index += sN;
			_Ptr += sN;

			if (_Chunk->Size() >= sN + n * sizeof(Metadata))
			{
				Metadata md;
				size_t sT = sizeof(Metadata);

				for (size_t i = 0; i < n; i++)
				{
					memcpy(&md, _Ptr, sT);
					_Index += sT;
					_Ptr += sT;

					_Metadata.push_back(md);
				}
			}
		}
	}

	Serializer& PackBinary(const ChunkRef& data)
	{
		DataType dt = GetDataType<ChunkRef>();

		if (_ChunkBuilder.IsEmpty())
		{
			PackStart();
		}

		size_t sT = data->Size();

		_ChunkBuilder.Append(data.ShallowCopy());

		Metadata md(dt, sT);

		_Metadata.push_back(md);

		return *this;
	}

	Serializer& UnpackBinary(ChunkRef& data)
	{
		if (_Ptr)
		{
			DataType dt = GetDataType<ChunkRef>();

			Metadata md = _Metadata.front();

			if (dt == md.Type)
			{
				size_t sT = md.Size;

				if (_Index + sT <= _Chunk->Size())
				{
					data = new Chunk(sT, _Ptr);
				}

				_Index += sT;
				_Ptr += sT;

				_Metadata.pop_front();

				if (_Metadata.empty() || _Index >= _Chunk->Size())
				{
					UnpackFinish();
				}
			}
		}

		return *this;
	}

	template<typename T> Serializer& PackStruct(const T& data)
	{
		DataType dt = GetDataType<T>();

		if (dt == Arithmetic || dt == Enum || dt == Struct || dt == Union)
		{
			if (_ChunkBuilder.IsEmpty())
			{
				PackStart();
			}

			size_t sT = sizeof(T);

			_ChunkBuilder.Append(new Chunk(sT, (byte*)&data));

			Metadata md(dt, sT);

			_Metadata.push_back(md);
		}

		return *this;
	}

	template<typename T> Serializer& UnpackStruct(T& data)
	{
		if (_Ptr)
		{
			DataType dt = GetDataType<T>();

			if (dt == Arithmetic || dt == Enum || dt == Struct || dt == Union)
			{
				Metadata md = _Metadata.front();

				size_t sT = sizeof(T);

				if (dt == md.Type && sT == md.Size)
				{
					if (_Index + sT <= _Chunk->Size())
					{
						memcpy((byte*)&data, _Ptr, sT);
					}

					_Index += sT;
					_Ptr += sT;

					_Metadata.pop_front();

					if (_Metadata.empty() || _Index >= _Chunk->Size())
					{
						UnpackFinish();
					}
				}
			}
		}

		return *this;
	}

	Serializer& PackString(const string& data)
	{
		DataType dt = GetDataType<string>();

		if (_ChunkBuilder.IsEmpty())
		{
			PackStart();
		}

		size_t sT = data.size() + 1;

		_ChunkBuilder.Append(new Chunk(sT, (byte*)data.c_str()));

		Metadata md(dt, sT);

		_Metadata.push_back(md);

		return *this;
	}

	Serializer& UnpackString(string& data)
	{
		if (_Ptr)
		{
			DataType dt = GetDataType<string>();

			Metadata md = _Metadata.front();

			if (dt == md.Type && md.Size > 0)
			{
				size_t sT = md.Size;

				if (_Index + sT <= _Chunk->Size())
				{
					data = string((char*)_Ptr, sT - 1);
				}

				_Index += sT;
				_Ptr += sT;

				_Metadata.pop_front();

				if (_Metadata.empty() || _Index >= _Chunk->Size())
				{
					UnpackFinish();
				}
			}
		}

		return *this;
	}

public:
	Serializer() :
		_ChunkBuilder(ChunkBuilder()),
		_Metadata(),
		_Chunk(nullptr),
		_Index(0),
		_Ptr(nullptr)
	{
	}

	virtual ~Serializer()
	{
		_Dispose();
	}

	inline Serializer& ToBinary(ChunkRef& chunk)
	{
		return PackFinish(chunk);
	}

	inline Serializer& ToBinary(size_t& size, byte*& ptr)
	{
		ChunkRef chunk;

		Serializer& ser = PackFinish(chunk);

		size = chunk->Size();
		ptr = new byte[size];

		memcpy(ptr, chunk->Ptr(), size);

		return ser;
	}

	inline Serializer& FromBinary(const ChunkRef& chunk)
	{
		return UnpackStart(chunk);
	}

	inline Serializer& FromBinary(const size_t size, const byte* ptr)
	{
		return UnpackStart(new Chunk(size, ptr));
	}

	inline Serializer& Pack(const ChunkRef& chunk)
	{
		return PackBinary(chunk);
	}

	inline Serializer& operator<<(const ChunkRef& chunk)
	{
		return PackBinary(chunk);
	}

	inline Serializer& Unpack(ChunkRef& chunk)
	{
		return UnpackBinary(chunk);
	}

	inline Serializer& operator>>(ChunkRef& chunk)
	{
		return UnpackBinary(chunk);
	}

	template<typename T> inline Serializer& Pack(const T& data)
	{
		return PackStruct(data);
	}

	template<typename T> inline Serializer& operator<<(const T& data)
	{
		return PackStruct(data);
	}

	template<typename T> inline Serializer& Unpack(T& data)
	{
		return UnpackStruct(data);
	}

	template<typename T> inline Serializer& operator>>(T& data)
	{
		return UnpackStruct(data);
	}

	inline Serializer& Pack(const string& data)
	{
		return PackString(data);
	}

	inline Serializer& operator<<(const string& data)
	{
		return PackString(data);
	}

	inline Serializer& Unpack(string& data)
	{
		return UnpackString(data);
	}

	inline Serializer& operator>>(string& data)
	{
		return UnpackString(data);
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

	virtual ~ISerializable()
	{
		_Dispose();
	}

protected:
	virtual void Serialize(ChunkRef& chunk) = 0;
	virtual void Serialize(size_t& size, byte*& ptr) = 0;
	virtual void Deserialize(const ChunkRef& chunk) = 0;
	virtual void Deserialize(const size_t size, const byte* ptr) = 0;
};
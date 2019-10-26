/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

Serialize
Version 19.10.26.0000

This file is part of Serialize

Serialize is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>

#include "RefCounter.h"

using std::string;
using std::map;
using std::set;
using std::list;
using std::vector;

using byte = uint8_t;

// 二进制块
class Chunk
{
private:
	size_t _Size;
	byte* _Ptr;

	void _Dispose();

public:
	Chunk();
	Chunk(const Chunk& chunk);
	Chunk(Chunk&& chunk);
	explicit Chunk(const size_t size);
	Chunk(const size_t size, const byte* ptr);
	Chunk& operator=(const Chunk& chunk);
	Chunk& operator=(Chunk&& chunk);
	virtual ~Chunk();

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

	void _Dispose();

public:
	ChunkBuilder();
	ChunkBuilder(const ChunkBuilder& chunkBuilder);
	ChunkBuilder& operator=(const ChunkBuilder& chunkBuilder);
	virtual ~ChunkBuilder();

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

	ChunkRef Combine();
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
		if (std::is_same<T, ChunkRef>::value)
		{
			return Binary;
		}
		else if (std::is_same<T, string>::value)
		{
			return String;
		}
		else if (std::is_void<T>::value)
		{
			return Void;
		}
		else if (std::is_null_pointer<T>::value)
		{
			return Null;
		}
		else if (std::is_arithmetic<T>::value)
		{
			return Arithmetic;
		}
		else if (std::is_enum<T>::value)
		{
			return Enum;
		}
		else if (std::is_class<T>::value)
		{
			return Struct;
		}
		else if (std::is_union<T>::value)
		{
			return Union;
		}
		else if (std::is_array<T>::value)
		{
			return Array;
		}
		else if (std::is_pointer<T>::value || std::is_member_pointer<T>::value)
		{
			return Pointer;
		}
		else if (std::is_function<T>::value)
		{
			return Function;
		}
		else if (std::is_object<T>::value)
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

	void _Dispose();

	Serializer(const Serializer&);
	Serializer& operator=(const Serializer&);

	void PackStart();
	Serializer& PackFinish(ChunkRef& chunk);
	Serializer& UnpackStart(const ChunkRef& chunk);
	void UnpackFinish();

	void PackMetadata();
	void UnpackMetadata();

	Serializer& PackBinary(const ChunkRef& data);
	Serializer& UnpackBinary(ChunkRef& data);

	template<typename T> Serializer& PackStruct(const T& data);
	template<typename T> Serializer& UnpackStruct(T& data);

	Serializer& PackString(const string& data);
	Serializer& UnpackString(string& data);

public:
	Serializer();
	virtual ~Serializer();

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

template<typename T> Serializer& Serializer::PackStruct(const T& data)
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

template<typename T> Serializer& Serializer::UnpackStruct(T& data)
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

// 表示对象支持序列化
class ISerializable
{
private:
	void _Dispose();

public:
	ISerializable();
	ISerializable(const ISerializable& iSerializable);
	ISerializable& operator=(const ISerializable& iSerializable);
	virtual ~ISerializable();

protected:
	virtual void Serialize(ChunkRef& chunk) = 0;
	virtual void Serialize(size_t& size, byte*& ptr) = 0;
	virtual void Deserialize(const ChunkRef& chunk) = 0;
	virtual void Deserialize(const size_t size, const byte* ptr) = 0;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2019 chibayuki@foxmail.com

Serialize
Version 19.10.26.0000

This file is part of Serialize

Serialize is released under the GPLv3 license
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Serialize.h"

void Chunk::_Dispose()
{
	_Size = 0;

	if (_Ptr)
	{
		delete _Ptr;
		_Ptr = nullptr;
	}
}

Chunk::Chunk() :
	_Size(0),
	_Ptr(nullptr)
{
}

Chunk::Chunk(const Chunk& chunk) :
	_Size(chunk._Size)
{
	_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);

	if (chunk._Ptr)
	{
		memcpy(_Ptr, chunk._Ptr, _Size);
	}
}

Chunk::Chunk(Chunk&& chunk) :
	_Size(chunk._Size),
	_Ptr(chunk._Ptr)
{
	chunk._Size = 0;
	chunk._Ptr = nullptr;
}

Chunk::Chunk(const size_t size) :
	_Size(size)
{
	_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);
}

Chunk::Chunk(const size_t size, const byte* ptr) :
	_Size(size)
{
	_Ptr = (_Size > 0 ? new byte[_Size] : nullptr);

	if (ptr)
	{
		memcpy(_Ptr, ptr, _Size);
	}
}

Chunk& Chunk::operator=(const Chunk& chunk)
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

Chunk& Chunk::operator=(Chunk&& chunk)
{
	if (this == &chunk)
	{
		return *this;
	}

	_Size = chunk._Size;
	_Ptr = chunk._Ptr;

	chunk._Size = 0;
	chunk._Ptr = nullptr;

	return *this;
}

Chunk::~Chunk()
{
	_Dispose();
}

void ChunkBuilder::_Dispose()
{
	if (!_Chunks.empty())
	{
		_Chunks.clear();
	}
}

ChunkBuilder::ChunkBuilder() :
	_Chunks()
{
}

ChunkBuilder::ChunkBuilder(const ChunkBuilder& chunkBuilder)
{
	_Chunks = chunkBuilder._Chunks;
}

ChunkBuilder& ChunkBuilder::operator=(const ChunkBuilder& chunkBuilder)
{
	if (this == &chunkBuilder)
	{
		return *this;
	}

	_Dispose();

	_Chunks = chunkBuilder._Chunks;

	return *this;
}

ChunkBuilder::~ChunkBuilder()
{
	_Dispose();
}

ChunkRef ChunkBuilder::Combine()
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

void Serializer::_Dispose()
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

Serializer::Serializer(const Serializer&)
{
}

Serializer& Serializer::operator=(const Serializer&)
{
	return *this;
}

void Serializer::PackStart()
{
	_Dispose();
}

Serializer& Serializer::PackFinish(ChunkRef& chunk)
{
	PackMetadata();

	chunk = _ChunkBuilder.Combine();

	_Dispose();

	return *this;
}

Serializer& Serializer::UnpackStart(const ChunkRef& chunk)
{
	_Dispose();

	_Chunk = chunk;
	_Index = 0;
	_Ptr = (chunk != nullptr ? const_cast<byte*>(_Chunk->Ptr()) : nullptr);

	UnpackMetadata();

	return *this;
}

void Serializer::UnpackFinish()
{
	_Dispose();
}

void Serializer::PackMetadata()
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

void Serializer::UnpackMetadata()
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

Serializer& Serializer::PackBinary(const ChunkRef& data)
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

Serializer& Serializer::UnpackBinary(ChunkRef& data)
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

Serializer& Serializer::PackString(const string& data)
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

Serializer& Serializer::UnpackString(string& data)
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

Serializer::Serializer() :
	_ChunkBuilder(ChunkBuilder()),
	_Metadata(),
	_Chunk(nullptr),
	_Index(0),
	_Ptr(nullptr)
{
}

Serializer::~Serializer()
{
	_Dispose();
}

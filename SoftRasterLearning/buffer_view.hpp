#pragma once

template<typename T>
struct BufferView
{
	void resize(UINT w, UINT h)
	{
		this->w = w;
		this->h = h;
	}

	void set(UINT x, UINT y, T v)
	{
		size_t index = (size_t)y * w + x;
		if (index<0 || index>w * h - 1) return;
		buffer[index] = v;
	}
	T get(UINT x, UINT y)
	{
		size_t index = (size_t)y * w + x;
		if (index<0 || index>w * h - 1) return;
		return buffer[index];
	}

	T* buffer;
	UINT w;
	UINT h;
};

struct BufferColor
{
	union
	{
		struct
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
		};
		UINT32 color;
		UINT32 bgra;
	};
};
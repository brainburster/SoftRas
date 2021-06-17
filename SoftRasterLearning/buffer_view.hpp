#pragma once

using uint32 = unsigned int;

template<typename T>
struct Buffer2DView
{
	void ReSize(uint32 w, uint32 h)
	{
		this->w = w;
		this->h = h;
	}

	void Set(uint32 x, uint32 y, T v)
	{
		size_t index = (size_t)y * w + x;
		if (index<0 || index>w * h - 1) return ;
		buffer[index] = v;
	}
	T Get(uint32 x, uint32 y)
	{
		size_t index = (size_t)y * w + x;
		if (index<0 || index>w * h - 1) return {0};
		return buffer[index];
	}

	struct Iter
	{
		T* location;
		bool operator!= (const Iter& other) const noexcept
		{
			return this->location != other.location;
		}
		T& operator* () const
		{
			return *location;
		}
		const Iter& operator++() noexcept
		{
			++location;
			return *this;
		}
	};

	Iter begin() const
	{
		return Iter{ buffer };
	}

	Iter end() const
	{
		return Iter{ buffer + w * h };
	}

	T* buffer;
	uint32 w;
	uint32 h;
};

template<typename T>
struct Buffer1DView
{
	T* buffer;
	uint32 size;

	struct Iter
	{
		T* location;
		bool operator!= (const Iter& other) const noexcept
		{
			return this->location != other.location;
		}
		T& operator* () const
		{
			return *location;
		}
		const Iter& operator++() noexcept
		{
			++location;
			return *this;
		}
	};

	Iter begin() const
	{
		return Iter{ buffer };
	}

	Iter end() const
	{
		return Iter{ buffer + size };
	}
};

struct Color32
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
		uint32 color;
		uint32 bgra;
	};
};

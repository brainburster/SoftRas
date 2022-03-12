#pragma once

#include "types_and_defs.hpp"

namespace core
{
	template<typename T>
	struct Buffer2DView
	{
		void ReSize(size_t w, size_t h)
		{
			this->w = w;
			this->h = h;
		}

		void Set(size_t x, size_t y, T v)
		{
			if (x < 0 || x >= w || y < 0 || y >= h) return;
			size_t index = (size_t)y * w + x;
			buffer[index] = v;
		}
		T Get(size_t x, size_t y)
		{
			if (x < 0 || x >= w || y < 0 || y >= h) return { 0 };
			size_t index = (size_t)y * w + x;
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
		size_t w;
		size_t h;
	};

	template<typename T>
	struct Buffer1DView
	{
		T* buffer;
		size_t size;

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
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			};
			uint32 color;
		};
	};
};

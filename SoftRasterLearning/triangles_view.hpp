#pragma once

#pragma pack(1)
template<typename T>
struct Trangle
{
	union
	{
		T data[3];
		struct
		{
			T* a;
			T* b;
			T* c;
		};
	};
};

template<typename T>
struct TrianglesView
{
	T* data;
	size_t len;
	Trangle<T>& Get(unsigned int i)
	{
		if (i>len/3-1)
		{
			return nullptr;
		}
		return *reinterpret_cast<Trangle<T>*>(&data[i * 3]);
	}
	struct Iter
	{
		T* location;
		bool operator!=(const Iter& other) const noexcept
		{
			return this->location != other.location;
		}
		const Iter& operator++() noexcept
		{
			++location;
			return *this;
		}
		Trangle<T>& operator*() const
		{
			return *reinterpret_cast<Trangle<T>*>(&data[i * 3]);
		}
	};

	Iter begin() 
	{
		return Iter{ data };
	}

	Iter end()
	{
		return Iter{ data+len };
	}
};

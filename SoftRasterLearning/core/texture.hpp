#pragma once

#include "types_and_defs.hpp"
//#include <vector>

namespace core
{
	class Texture
	{
	public:
		Texture() :_w{ 0 }, _h{ 0 }, _order{ 0 } {}
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) noexcept : _data{ std::move(other._data) }, _w{ other._w }, _h{ other._h }, _order{ other._order } {}
		Texture& operator=(Texture&& other) noexcept
		{
			if (this == &other) { return *this; }
			memcpy(this, &other, sizeof(Texture));
			memset(&other, 0, sizeof(Texture));
		}
		Texture(size_t w, size_t h) :_w{ w }, _h{ h }, _order{ 0 }
		{
			size_t x_order = (size_t)ceil(log2(w));
			size_t y_order = (size_t)ceil(log2(h));
			_order = max(x_order, y_order);
			_data.resize((size_t)pow(2, x_order)* (size_t)pow(2, y_order));
		}
		template<typename T>
		Texture(size_t w, size_t h, T* buffer) :_w{ w }, _h{ h }, _order{ 0 }
		{
			size_t x_order = ceil(log2(w));
			size_t y_order = ceil(log2(h));
			_order = max(x_order, y_order);
			_data.resize(pow(2, x_order)* pow(2, y_order));
			const size_t size = w * h;
#pragma omp parallel for num_threads(4)
			for (int i = 0; i < size; ++i)
			{
				const int x = i % w;
				const int y = i / w;
				GetRef(x, y) = buffer[i];
			};
		}

		Vec4 Get(size_t x, size_t y) const noexcept
		{
			using gmath::utility::Clamp;
			x = Clamp(x, 0ULL, _w - 1);
			y = Clamp(y, 0ULL, _h - 1);

			//const size_t i = x + y * _w;
			const size_t i = GetMortonCode(x, y);
			return _data[i];
		}

		Vec4& GetRef(size_t x, size_t y)
		{
			using gmath::utility::Clamp;
			x = Clamp(x, 0, _w - 1);
			y = Clamp(y, 0, _h - 1);

			const size_t i = GetMortonCode(x, y);
			return _data[i];
		}

		static Vec4 Sample(Texture* tex, Vec2 uv) noexcept
		{
			if (!tex) return { 0,0,0,1.f };
			float x = uv.x * tex->_w;
			float y = uv.y * tex->_h;
			auto _x = (size_t)x;
			auto _y = (size_t)y;
			x = x - _x;
			y = y - _y;

			//双线性插值
			auto color0 = tex->Get(_x, _y);
			auto color1 = tex->Get(_x + 1, _y);
			auto color2 = tex->Get(_x, _y + 1);
			auto color3 = tex->Get(_x + 1, _y + 1);
			auto color01 = color0 * (1.f - x) + color1 * x;
			auto color23 = color2 * (1.f - x) + color3 * x;
			auto color = color01 * (1.f - y) + color23 * y;
			return color;
		}

		size_t GetWidth() const noexcept
		{
			return _w;
		}

		size_t GetHeight() const noexcept
		{
			return _h;
		}

		size_t GetOrder() const noexcept
		{
			return _order;
		}

		size_t GetSize() const noexcept
		{
			return _data.size();
		}

		std::vector<Vec4>& GetData()
		{
			return _data;
		}

		const std::vector<Vec4>& GetCData() const noexcept
		{
			return _data;
		}

		void Resize(size_t w, size_t h)
		{
			this->_w = w;
			this->_h = h;
			size_t x_order = (size_t)ceil(log2(w));
			size_t y_order = (size_t)ceil(log2(h));
			_order = max(x_order, y_order);
			_data.resize((size_t)pow(2, x_order) * (size_t)pow(2, y_order));
		}

	protected:
		//计算二维z型曲线的morton code
		size_t GetMortonCode(size_t x, size_t y) const noexcept
		{
			size_t z = 0;
			for (size_t j = 0; j < _order; ++j) {
				size_t mask = 1ULL << j;
				if (x & mask)
					z |= 1ULL << (0 + j * 2);
				if (y & mask)
					z |= 1ULL << (1 + j * 2);
			}
			return z;
		}
		//计算二维z型曲线的x,y值
		void GetXYformMortonCode(size_t value, size_t& x, size_t& y) const noexcept
		{
			x = y = 0;
			for (size_t j = 0; j < _order; ++j) {
				size_t mask = 1ULL << j * 2;
				if (value & mask)
					y |= 1ULL << j;
				mask <<= 1;
				if (value & mask)
					x |= 1ULL << j;
			}
		}

	protected:
		std::vector<Vec4> _data;
		size_t _w;
		size_t _h;
		size_t _order;
	};
}

#pragma once

#include "types_and_defs.hpp"

namespace core
{
	class Texture
	{
	public:
		Texture() :_w(0), _h(0) {}
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) noexcept : _data{ std::move(other._data) }, _w{ other._w }, _h{ other._h } {}
		Texture& operator=(Texture&& other) noexcept
		{
			if (this == &other) { return *this; }
			memcpy(this, &other, sizeof(Texture));
			memset(&other, 0, sizeof(Texture));
		}
		Texture(size_t w, size_t h)
		{
			this->_w = w;
			this->_h = h;
			_data.resize(w * h);
		}

		Vec4 Get(size_t x, size_t y)
		{
			using gmath::utility::Clamp;
			x = Clamp(x, 0, _w - 1);
			y = Clamp(y, 0, _h - 1);

			size_t i = x + y * _w;

			return _data[i];
		}

		Vec4& GetRef(size_t x, size_t y)
		{
			using gmath::utility::Clamp;
			x = Clamp(x, 0, _w - 1);
			y = Clamp(y, 0, _h - 1);

			size_t i = x + y * _w;

			return _data[i];
		}

		static Vec4 Sample(Texture* tex, Vec2 uv)
		{
			if (!tex) return { 0,0,0,1.f };
			float x = uv.x * tex->_w;
			float y = uv.y * tex->_h;
			auto _x = (size_t)x;
			auto _y = (size_t)y;
			x = x - _x;
			y = y - _y;

			//Ë«ÏßÐÔ²åÖµ
			auto color0 = tex->Get(_x, _y);
			auto color1 = tex->Get(_x + 1, _y);
			auto color2 = tex->Get(_x, _y + 1);
			auto color3 = tex->Get(_x + 1, _y + 1);
			auto color01 = color0 * (1.f - x) + color1 * x;
			auto color23 = color2 * (1.f - x) + color3 * x;
			auto color = color01 * (1.f - y) + color23 * y;
			return color;
		}

		size_t GetWidth() const
		{
			return _w;
		}

		size_t GetHeight() const
		{
			return _h;
		}

		std::vector<Vec4>& GetData()
		{
			return _data;
		}

		void SetSize(size_t w, size_t h)
		{
			this->_w = w;
			this->_h = h;
			_data.resize(w * h);
		}

	private:
		std::vector<Vec4> _data;
		size_t _w;
		size_t _h;
	};
}

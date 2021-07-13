#pragma once

#include "types_and_defs.hpp"

namespace core
{
	class Texture
	{
	public:
		Vec4 Get(size_t x, size_t y)
		{
			using gmath::utility::Clamp;
			x = Clamp(x, 0, w - 1);
			y = Clamp(y, 0, h - 1);

			//size_t i = x + (h - y - 1) * w;
			size_t i = x + y * w;

			return data[i];
		}

		static Vec4 Sample(Texture* tex, Vec2 uv)
		{
			if (!tex) return { 0,0,0,1.f };
			float x = uv.x * tex->w;
			float y = uv.y * tex->h;
			auto _x = (int)x;
			auto _y = (int)y;

			auto color = tex->Get(_x, _y);

			return color;
		}

		size_t GetWidth() const
		{
			return w;
		}

		size_t GetHeight() const
		{
			return h;
		}

		Vec4* GetData()
		{
			return &data[0];
		}
		Texture() : data{}, w{ 0 }, h{ 0 }{}
		Texture(Texture&& other) noexcept : data{ std::move(other.data) }, w{ other.w }, h{ other.h }
		{
		}
		Texture& operator=(Texture&& other) noexcept
		{
			memcpy(this, &other, sizeof(Texture));
			memset(&other, 0, sizeof(Texture));
		}
		void SetSize(size_t w, size_t h)
		{
			this->w = w;
			this->h = h;
		}
		//private:
	public:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture& other) = delete;
		std::vector<Vec4> data;
		size_t w;
		size_t h;
	};
}

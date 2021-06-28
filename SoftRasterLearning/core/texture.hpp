#pragma once

#include "types_and_defs.hpp"

namespace core
{
	class Texture
	{
	public:
		Vec4 Get(size_t x, size_t y)
		{
			using gmath::Utility::Clamp;
			x = Clamp(x, 0, w - 1);
			y = Clamp(y, 0, h - 1);

			size_t i = x * channel + y * w * channel;
			if (channel == 4)
			{
				return gmath::Vec4<float>{
					(float)data[i], (float)data[i + 1], (float)data[i + 2], (float)data[i + 3]
				};
			}
			else
			{
				return gmath::Vec4<float>{
					(float)data[i], (float)data[i + 1], (float)data[i + 2], 255
				};
			}
		}

		gmath::Vec4<float> Sampler(gmath::Vec2<float> uv, bool loop = true)
		{
			float x = uv.x * w;
			float y = uv.y * h;
			auto _x = (int)x;
			auto _y = (int)y;

			auto color = Get(_x, _y);

			return gmath::Vec4<float> {
				color.b / 255.f, color.g / 255.f, color.r / 255.f, color.a / 255.f
			};
		}

		size_t GetWidth() const
		{
			return w;
		}

		size_t GetHeight() const
		{
			return h;
		}

		uint8* GetData()
		{
			return &data[0];
		}
		Texture() : data{}, w{ 0 }, h{ 0 }, channel{ 0 }{}
		Texture(Texture&& other) noexcept : data{ std::move(other.data) }, w{ other.w }, h{ other.h }, channel{ other.channel }
		{
		}
		Texture& operator=(Texture&& other) noexcept
		{
			memcpy(this, &other, sizeof(Texture));
			memset(&other, 0, sizeof(Texture));
		}
		void SetSize(size_t w, size_t h, size_t channel)
		{
			this->w = w;
			this->h = h;
			this->channel = channel;
		}
		//private:
	public:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture& other) = delete;
		std::vector<core::uint8> data;
		size_t w;
		size_t h;
		size_t channel;
	};
}

#pragma once

#include <fstream>
#include <vector>
#include "game_math.hpp"

namespace loader
{
	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned long;

#pragma pack(2) 
	struct BmpHeader
	{
		uint16 type;
		uint32 size_of_file;
		uint16 reserved[2];
		uint32 offset;
	};
#pragma pack() 

	//static_assert(sizeof(BmpHeader) == 14, "the size of bmp header must be 14 bytes");

#pragma pack(2) 
	struct BmpInfo
	{
		uint32 size_of_info;
		uint32 width;
		uint32 height;
		uint16 planes; //1
		uint16 bitCount; //1,4,8,16,24,32
		uint32 compression; //压缩类型 
		uint32 size_image;
		uint32 xppm; // x分辨率 x pixel per meter
		uint32 yppm; // y分辨率
		uint32 clrUsed;
		uint32 clrImportant;
	};
#pragma pack() 

	//static_assert(sizeof(BmpInfo) == 40, "the size of bmp info must be 40 bytes");

	class BmpLoader;

	class Texture
	{
	public:
		gmath::Vec4<float> Get(int x, int y)
		{
			using gmath::Utility::Clamp;
			x = Clamp(x, 0, w);
			y = Clamp(y, 0, h);

			size_t i = x * channel + y * w * channel;
			if (channel==4)
			{
				return gmath::Vec4<float>{
					(float)data[i], (float)data[i + 1], (float)data[i + 2], (float)data[i + 3]
				};
			} 
			else
			{
				return gmath::Vec4<float>{
					(float)data[i + 2], (float)data[i + 1], (float)data[i], 255
				};
			}
		}

		gmath::Vec4<float> Sampler(gmath::Vec2<float> uv, bool loop = true)
		{
			float x = uv.x * w;
			float y = (1-uv.y) * h;
			auto _x = (int)x;
			auto _y = (int)y;
			//float wx = _x - x;
			//float wy = _y - y;
			//
			//if (loop)
			//{
			//	_x = (_x + w) % w;
			//	_y = (_y + h) % h;
			//}

			//auto color_1 = Get(_x, _y);
			//auto color_2 = Get(_x+ 1, _y);
			//auto color_3 = Get(_x, _y + 1);
			//auto color_4 = Get(_x + 1, _y + 1);

			//auto color_5 = color_1 * wx + color_2 * (1 - wx);
			//auto color_6 = color_3 * wx + color_4 * (1 - wx);
			//auto color_7 = color_5 * wy + color_6 * (1 - wy);

			auto color_7 = Get(_x, _y);
			
			return gmath::Vec4<float> {
					color_7.b / 255.f,
					color_7.g / 255.f,
					color_7.r / 255.f,
					color_7.a / 255.f
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
	private:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture& other) = delete;
		std::vector<uint8> data;
		size_t w;
		size_t h;
		size_t channel;
		friend class BmpLoader;
	};

	class BmpLoader
	{
	public:
		static Texture LoadBmp(const wchar_t* file_path)
		{
			std::ifstream bmp_file;
			bmp_file.open(file_path, std::ios::binary | std::ios::in);
			if (!bmp_file)
			{
				throw "can't open bmp file";
			}

			BmpHeader bmp_header;
			BmpInfo bmp_info;


			bmp_file.read((char*)&bmp_header, sizeof(BmpHeader));
			bmp_file.read((char*)&bmp_info, sizeof(BmpInfo));

			if (bmp_info.bitCount != 32 && bmp_info.bitCount != 24)
			{
				throw "bitCount less than 24 bits are not supported";
			}

			Texture bitmap;

			bitmap.w = bmp_info.width;
			bitmap.h = bmp_info.height;
			bitmap.channel = bmp_info.bitCount == 32 ? 4 : 3;

			bitmap.data.resize(bitmap.w * bitmap.h * bitmap.channel);

			bmp_file.read((char*)&bitmap.data[0], bitmap.data.size());

			return bitmap;
		}
	};

}

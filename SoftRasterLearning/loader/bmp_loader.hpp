#pragma once

#include <fstream>
#include "core/texture.hpp"
#include <algorithm>

namespace loader::bmp
{
#pragma pack(2)
	struct BmpHeader
	{
		core::uint16 type;
		core::uint32 size_of_file;
		core::uint16 reserved[2];
		core::uint32 offset;
	};
#pragma pack()

	static_assert(sizeof(BmpHeader) == 14, "the size of bmp header must be 14 bytes");

#pragma pack(2)
	struct BmpInfo
	{
		core::uint32 size_of_info;
		core::uint32 width;
		core::uint32 height;
		core::uint16 planes; //1
		core::uint16 bitCount; //1,4,8,16,24,32
		core::uint32 compression; //压缩类型
		core::uint32 size_image;
		core::uint32 xppm; // x 分辨率 x pixel per meter
		core::uint32 yppm; // y 分辨率
		core::uint32 clrUsed;
		core::uint32 clrImportant;
	};
#pragma pack()

	static_assert(sizeof(BmpInfo) == 40, "the size of bmp info must be 40 bytes");

	std::shared_ptr<core::Texture> LoadFromFile(const wchar_t* file_path, bool b_gamma_conrrection = true)
	{
		std::ifstream bmp_file;
		bmp_file.open(file_path, std::ios::binary | std::ios::in);
		if (!bmp_file)
		{
			return nullptr;
		}

		BmpHeader bmp_header{};
		BmpInfo bmp_info{};

		bmp_file.read((char*)&bmp_header, sizeof(BmpHeader));
		bmp_file.read((char*)&bmp_info, sizeof(BmpInfo));

		if (bmp_info.bitCount != 32 && bmp_info.bitCount != 24)
		{
			return nullptr;
		}

		core::Texture texture;
		const size_t width = bmp_info.width;
		const size_t height = bmp_info.height;
		texture.SetSize(width, height);

		const size_t channel = bmp_info.bitCount == 32 ? 4 : 3;
		const size_t size = width * height;
		const size_t size_of_byte = size * channel;

		std::vector<core::uint8> buffer{};
		buffer.resize(size_of_byte, 0);
		bmp_file.read((char*)&buffer[0], buffer.size());

		for (size_t i = 0; i < size; ++i)
		{
			size_t x = i % width;
			size_t y = i / width;
			texture.GetRef(x, y) = core::Vec4{
					(float)buffer[i * channel + 2] / 255,
					(float)buffer[i * channel + 1] / 255,
					(float)buffer[i * channel] / 255,
					(channel < 4) ? (1.f) : ((float)buffer[i * channel + 3] / 255)
			};
		}

		std::vector<core::Vec4>& tex_data = texture.GetData();
		if (b_gamma_conrrection) {
			std::transform(tex_data.begin(), tex_data.end(), tex_data.begin(),
				[](core::Vec4 color) {
					return core::Vec4{
					   pow(color.r, core::gamma),  //从伽马空间映射到线性空间
					   pow(color.g, core::gamma),
					   pow(color.b, core::gamma),
					   color.a
					};
				});
		}

		return std::make_shared<core::Texture>(std::move(texture));
	}
}

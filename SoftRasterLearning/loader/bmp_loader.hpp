#pragma once

#include <fstream>
#include <optional>
#include "core/texture.hpp"

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

	std::optional<core::Texture> LoadFromFile(const wchar_t* file_path)
	{
		std::ifstream bmp_file;
		bmp_file.open(file_path, std::ios::binary | std::ios::in);
		if (!bmp_file)
		{
			return std::nullopt;
		}

		BmpHeader bmp_header{};
		BmpInfo bmp_info{};

		bmp_file.read((char*)&bmp_header, sizeof(BmpHeader));
		bmp_file.read((char*)&bmp_info, sizeof(BmpInfo));

		if (bmp_info.bitCount != 32 && bmp_info.bitCount != 24)
		{
			return std::nullopt;
		}

		core::Texture bitmap;

		bitmap.w = bmp_info.width;
		bitmap.h = bmp_info.height;
		bitmap.channel = bmp_info.bitCount == 32 ? 4 : 3;

		bitmap.data.resize(bitmap.w * bitmap.h * bitmap.channel);

		bmp_file.read((char*)&bitmap.data[0], bitmap.data.size());

		return bitmap;
	}
}

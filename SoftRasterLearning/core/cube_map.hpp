#pragma once

#include "texture.hpp"
#include <memory>

namespace core
{
	class CubeMap
	{
	private:
		std::shared_ptr<core::Texture> front;
		std::shared_ptr<core::Texture> back;
		std::shared_ptr<core::Texture> top;
		std::shared_ptr<core::Texture> bottom;
		std::shared_ptr<core::Texture> left;
		std::shared_ptr<core::Texture> right;

	public:
		CubeMap(std::shared_ptr<core::Texture> front, std::shared_ptr<core::Texture> back, std::shared_ptr<core::Texture> top,
			std::shared_ptr<core::Texture> bottom, std::shared_ptr<core::Texture> left, std::shared_ptr<core::Texture> right) :
			front{ front }, back{ back }, top{ top }, bottom{ bottom }, left{ left }, right{ right }
		{
		}

		CubeMap(size_t w, size_t h)
		{
			front = std::make_shared<core::Texture>(w, h);
			back = std::make_shared<core::Texture>(w, h);
			top = std::make_shared<core::Texture>(w, h);
			bottom = std::make_shared<core::Texture>(w, h);
			left = std::make_shared<core::Texture>(w, h);
			right = std::make_shared<core::Texture>(w, h);
		}

		core::Vec4 Sample(core::Vec3 dir) const
		{
			//3个分量中绝对值最大的,决定采样哪个面
			//对剩下的两个分量"归一化"，决定uv
			float abs_x = fabs(dir.x) + core::epsilon;
			float abs_y = fabs(dir.y) + core::epsilon;
			float abs_z = fabs(dir.z) + core::epsilon;
			float u = 0;
			float v = 0;
			core::Texture* tex = front.get();

			if (abs_x > abs_y && abs_x > abs_z)
			{
				if (dir.x > 0)
				{
					//采样右面
					tex = right.get();
					u = (-dir.z / abs_x + 1) / 2;
					v = (dir.y / abs_x + 1) / 2;
				}
				else
				{
					//采样左面
					tex = left.get();
					u = (dir.z / abs_x + 1) / 2;
					v = (dir.y / abs_x + 1) / 2;
				}
			}
			else if (abs_y > abs_x && abs_y > abs_z)
			{
				if (dir.y > 0)
				{
					//采样顶面
					tex = top.get();
					u = (dir.x / abs_y + 1) / 2;
					v = (-dir.z / abs_y + 1) / 2;
				}
				else
				{
					//采样底面
					tex = bottom.get();
					u = (dir.x / abs_y + 1) / 2;
					v = (dir.z / abs_y + 1) / 2;
				}
			}
			else if (abs_z > abs_x && abs_z > abs_y)
			{
				if (dir.z > 0)
				{
					//采样前面
					tex = front.get();
					u = (dir.x / abs_z + 1) / 2;
					v = (dir.y / abs_z + 1) / 2;
				}
				else
				{
					//采样后面
					tex = back.get();
					u = (-dir.x / abs_z + 1) / 2;
					v = (dir.y / abs_z + 1) / 2;
				}
			}

			return core::Texture::Sample(tex, { u, v });
		}
	};
}

#pragma once

#include "game_math.hpp"
#include <vector>

namespace core
{
	using Position = gmath::Vec4<float>;
	using Color = gmath::Vec4<float>;
	using Vec4 = gmath::Vec4<float>;
	using Vec3 = gmath::Vec3<float>;
	using Vec2 = gmath::Vec2<float>;
	using Mat = gmath::Mat4x4<float>;
	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	static constexpr float epsilon = 1e-20f;
	static constexpr float pi = 3.14159265358979f;
}

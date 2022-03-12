#pragma once

#include "game_math.hpp"
#include "shader_varying.hpp"
#include <vector>

namespace core
{
	using Position = gmath::Vec4<float>;
	using Color = gmath::Vec4<float>;
	using Vec4 = gmath::Vec4<float>;
	using Vec3 = gmath::Vec3<float>;
	using Vec2 = gmath::Vec2<float>;
	using Mat = gmath::Mat4x4<float>;
	using Mat3 = gmath::Mat3x3<float>;
	using Quat = gmath::Quaternions<float>;

	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	static constexpr float epsilon = 1e-20f;
	static constexpr float pi = 3.14159265358979f;
	static constexpr float gamma = 2.2f;//2.2f;
	static constexpr float inf = 1e20f;

	//默认的顶点类,只有位置和颜色2个属性, 继承 core::shader_varying_float<T> 可作为PS的输入
	struct Vertex_Default : core::shader_varying_float<Vertex_Default>
	{
		Position position;
		Color color;
	};

	//抄的gsl的实现https://github.com/microsoft/GSL/blob/main/include/gsl/util#L98
	template <class T, class U>
	//[[gsl::suppress(type.1)]]
	constexpr T narrow_cast(U&& u) noexcept
	{
		return static_cast<T>(std::forward<U>(u));
	}
}

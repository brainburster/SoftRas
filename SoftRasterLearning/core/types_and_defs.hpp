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

	template<typename T>
	struct shader_varying_float
	{
		T operator+(const T& rhs) {
			static_assert(!std::is_pod_v<T>);

			const float* buffer_lhs = reinterpret_cast<const float*>(this);
			const float* buffer_rhs = reinterpret_cast<const float*>(&rhs);
			T ret{};
			float* buffer_ret = reinterpret_cast<float*>(&ret);
			constexpr size_t size = sizeof(T) / sizeof(float);
			for (size_t i = 0; i < size; ++i)
			{
				buffer_ret[i] = buffer_lhs[i] + buffer_rhs[i];
			}
			return ret;
		}

		T operator*(float rhs) {
			static_assert(!std::is_pod_v<T>);

			const float* buffer_lhs = reinterpret_cast<const float*>(this);
			T ret{};
			float* buffer_ret = reinterpret_cast<float*>(&ret);
			constexpr size_t size = sizeof(T) / sizeof(float);
			for (size_t i = 0; i < size; ++i)
			{
				buffer_ret[i] = buffer_lhs[i] * rhs;
			}

			return ret;
		}
	};

	template<typename T>
	constexpr bool is_shader_varing_type = std::is_pod_v<T> && std::is_base_of_v<shader_varying_float<T>, T>;

	template<typename T, typename... Args>
	auto CreateVarying(Args&&... args)  -> decltype(T{ {}, std::forward<Args>(args)... })
	{
		static_assert(!is_shader_varing_type<T>);
		return T{ {}, std::forward<Args>(args)... };
	}
}

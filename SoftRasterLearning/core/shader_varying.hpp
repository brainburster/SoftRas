#pragma once

#include<type_traits>
#include<immintrin.h>

namespace core
{
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

			if constexpr (alignof(T) == 16)
			{
				for (size_t i = 0; i < size; i += 4)
				{
					//手动向量化
					__m128 _lhs = _mm_load_ps(&buffer_lhs[i]);
					__m128 _rhs = _mm_load_ps(&buffer_rhs[i]);
					__m128 _ret = _mm_add_ps(_lhs, _rhs);
					_mm_store_ps(&buffer_ret[i], _ret);
				}
			}
			else
			{
				for (size_t i = 0; i < size; ++i)
				{
					buffer_ret[i] = buffer_lhs[i] + buffer_rhs[i];
				}
			}

			return ret;
		}

		T operator*(float rhs) {
			static_assert(!std::is_pod_v<T>);

			const float* buffer_lhs = reinterpret_cast<const float*>(this);
			T ret{};
			float* buffer_ret = reinterpret_cast<float*>(&ret);
			constexpr size_t size = sizeof(T) / sizeof(float);

			if constexpr (alignof(T) == 16)
			{
				for (size_t i = 0; i < size; i += 4)
				{
					__m128 _lhs = _mm_load_ps(&buffer_lhs[i]);
					__m128 _rhs = _mm_set_ps1(rhs);
					__m128 _ret = _mm_mul_ps(_lhs, _rhs);
					_mm_store_ps(&buffer_ret[i], _ret);
				}
			}
			else
			{
				for (size_t i = 0; i < size; ++i)
				{
					buffer_ret[i] = buffer_lhs[i] * rhs;
				}
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

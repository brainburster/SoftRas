#pragma once

namespace gmath
{
	inline Vec4<float>::Vec4(float v) : x{ v }, y{ v }, z{ v }, w{ v }
	{
	}

	inline Vec4<float>::Vec4(float x, float y, float z, float w) : x{ x }, y{ y }, z{ z }, w{ w }
	{
	}

	inline Vec4<float>::Vec4(const Vec2<float>& vec2, float z, float w) : x{ vec2.x }, y{ vec2.y }, z{ z }, w{ w }
	{
	}

	inline Vec4<float>::Vec4(const Vec3<float>& vec3, float w) : x{ vec3.x }, y{ vec3.y }, z{ vec3.z }, w{ w }
	{
	}

	inline Vec4<float>::Vec4(__m128 data)
	{
		data_m128 = data;
	}

	inline Vec4<float>::operator __m128()
	{
		return data_m128;
	}

	__forceinline Vec4<float> _vectorcall Vec4<float>::Pow(float rhs) const noexcept
	{
		__m128 rhs4 = _mm_set_ps1(rhs);
		return _mm_pow_ps(data_m128, rhs4); //¼ÙµÄintrinsic,
		//return _mm_exp10_ps(_mm_mul_ps(_mm_log10_ps(data_m128), rhs4));
		//return _mm_sqrt_ps(data_m128);
	}

	__forceinline Vec4<float> _vectorcall Vec4<float>::Sqrt() const noexcept
	{
		return _mm_sqrt_ps(data_m128);
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator+=(Vec4 rhs) noexcept
	{
		data_m128 = _mm_add_ps(data_m128, rhs.data_m128);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator+=(float rhs) noexcept
	{
		__m128 rhs4 = _mm_set_ps1(rhs);
		data_m128 = _mm_add_ps(data_m128, rhs4);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator-=(const Vec4& rhs) noexcept
	{
		data_m128 = _mm_sub_ps(data_m128, rhs.data_m128);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator-=(float rhs) noexcept
	{
		__m128 rhs4 = _mm_set_ps1(rhs);
		data_m128 = _mm_sub_ps(data_m128, rhs4);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator*=(const Vec4& rhs) noexcept
	{
		data_m128 = _mm_mul_ps(data_m128, rhs.data_m128);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator*=(float rhs) noexcept
	{
		__m128 rhs4 = _mm_set_ps1(rhs);
		data_m128 = _mm_mul_ps(data_m128, rhs4);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator/=(const Vec4& rhs) noexcept
	{
		data_m128 = _mm_div_ps(data_m128, rhs.data_m128);
		return *this;
	}

	__forceinline Vec4<float>& _vectorcall Vec4<float>::operator/=(float rhs) noexcept
	{
		__m128 rhs4 = _mm_set_ps1(rhs);
		data_m128 = _mm_div_ps(data_m128, rhs4);
		return *this;
	}

	__forceinline float _vectorcall Vec4<float>::Dot(Vec4 rhs) const noexcept
	{
		__m128 ret = _mm_mul_ps(data_m128, rhs.data_m128);
		ret = _mm_hadd_ps(ret, ret);
		ret = _mm_hadd_ps(ret, ret);
		return ret.m128_f32[0];
	}

	__forceinline Vec4<float> _vectorcall operator+(Vec4<float> lhs, Vec4<float> rhs) noexcept
	{
		return _mm_add_ps(lhs, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator+(float lhs, Vec4<float> rhs) noexcept
	{
		__m128 lhs4 = _mm_set1_ps(lhs);
		return _mm_add_ps(lhs4, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator+(Vec4<float> lhs, float rhs) noexcept
	{
		__m128 rhs4 = _mm_set1_ps(rhs);
		return _mm_add_ps(lhs, rhs4);
	}

	__forceinline Vec4<float> _vectorcall operator-(Vec4<float> lhs, Vec4<float> rhs) noexcept
	{
		return _mm_sub_ps(lhs, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator-(float lhs, Vec4<float> rhs) noexcept
	{
		__m128 lhs4 = _mm_set1_ps(lhs);
		return _mm_sub_ps(lhs4, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator-(Vec4<float> lhs, float rhs) noexcept
	{
		__m128 rhs4 = _mm_set1_ps(rhs);
		return _mm_sub_ps(lhs, rhs4);
	}

	__forceinline Vec4<float> _vectorcall operator*(Vec4<float> lhs, Vec4<float> rhs) noexcept
	{
		return _mm_mul_ps(lhs, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator*(float lhs, Vec4<float> rhs) noexcept
	{
		__m128 lhs4 = _mm_set1_ps(lhs);
		return _mm_mul_ps(lhs4, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator*(Vec4<float> lhs, float rhs) noexcept
	{
		__m128 rhs4 = _mm_set1_ps(rhs);
		return _mm_mul_ps(lhs, rhs4);
	}

	__forceinline Vec4<float> _vectorcall operator/(Vec4<float> lhs, Vec4<float> rhs) noexcept
	{
		return _mm_div_ps(lhs, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator/(float lhs, Vec4<float> rhs) noexcept
	{
		__m128 lhs4 = _mm_set1_ps(lhs);
		return _mm_div_ps(lhs4, rhs);
	}

	__forceinline Vec4<float> _vectorcall operator/(Vec4<float> lhs, float rhs) noexcept
	{
		__m128 rhs4 = _mm_set1_ps(rhs);
		return _mm_div_ps(lhs, rhs4);
	}
};

namespace gmath
{
	template<typename T> inline Vec4<T>::Vec4(T v) : x{ v }, y{ v }, z{ v }, w{ v }{}
	template<typename T> inline Vec4<T>::Vec4(T x, T y, T z, T w) : x{ x }, y{ y }, z{ z }, w{ w }{}
	template<typename T> inline Vec4<T>::Vec4(const Vec2<T>& vec2, T z, T w) : x{ vec2.x }, y{ vec2.y }, z{ z }, w{ w }{}
	template<typename T> inline Vec4<T>::Vec4(const Vec3<T>& vec3, T w) : x{ vec3.x }, y{ vec3.y }, z{ vec3.z }, w{ w }{}

	template<typename T>inline  T  Vec4<T>::Dot(const Vec4<T>& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	template<typename T> inline Vec4<T> Vec4<T>::operator+(const Vec4& rhs) const
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z, w + rhs.w };
	}

	template<typename T> inline Vec4<T> Vec4<T>::operator-(const Vec4& rhs) const
	{
		return { x - rhs.x,y - rhs.y,z - rhs.z, w - rhs.w };
	}

	template<typename T> inline Vec4<T>& Vec4<T>::operator+=(const Vec4& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}
	template<typename T> inline Vec4<T>& Vec4<T>::operator-=(const Vec4& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}
	template<typename T> inline Vec4<T>& Vec4<T>::operator*=(const Vec4& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}
	template<typename T> inline Vec4<T>& Vec4<T>::operator/=(const Vec4& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		w /= rhs.w;
		return *this;
	}

	template<typename T> inline Vec4<T> operator*(const Vec4<T>& lhs, T rhs)
	{
		return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs,lhs.w * rhs };
	}

	template<typename T> inline Vec4<T> operator*(T lhs, const Vec4<T>& rhs)
	{
		return { rhs.x * lhs,rhs.y * lhs,rhs.z * lhs, rhs.w * lhs };
	}

	template<typename T> inline Vec4<T> operator/(const Vec4<T>& lhs, T rhs)
	{
		return { lhs.x / rhs,lhs.y / rhs,lhs.z / rhs, lhs.w / rhs };
	}

	template<typename T> inline Vec4<T> operator*(const Vec4<T>& lhs, const Vec4<T>& rhs)
	{
		return { lhs.x * rhs.x,lhs.y * rhs.y,lhs.z * rhs.z, lhs.w * rhs.w };
	}
};

namespace gmath
{
	template<typename T> inline Vec3<T>::Vec3(T v) : x{ v }, y{ v }, z{ v }{}

	template<typename T> inline Vec3<T>::Vec3(T x, T y, T z) : x{ x }, y{ y }, z{ z }{}

	template<typename T> inline Vec3<T>::Vec3(const Vec4<T>& vec4) :
		x{ vec4.x },
		y{ vec4.y },
		z{ vec4.z }
	{}

	template<typename T> inline Vec3<T>::Vec3(const Vec2<T>& vec2) :x{ vec2.x }, y{ vec2.y }{}

	template<typename T> inline Vec4<T> Vec3<T>::ToHomoCoord() const
	{
		return Vec4<T>{ x, y, z, 1.f };
	}

	template<typename T> inline Vec3<T> Vec3<T>::normalize() const
	{
		T len = pow(x * x + y * y + z * z, 0.5f);
		return {
			x / len,
			y / len,
			z / len
		};
	}

	template<typename T> inline T Vec3<T>::Dot(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	template<typename T> inline Vec3<T> Vec3<T>::cross(const Vec3& b) const
	{
		return Vec3{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x };
	}

	template<typename T> inline Vec3<T> Vec3<T>::operator+(const Vec3& rhs) const
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z };
	}

	template<typename T> inline Vec3<T> Vec3<T>::operator-(const Vec3& rhs) const
	{
		return { x - rhs.x,y - rhs.y,z - rhs.z };
	}

	template<typename T> inline Vec3<T> operator*(const Vec3<T>& lhs, T rhs)
	{
		return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs };
	}
	template<typename T> inline Vec3<T>operator*(T lhs, const Vec3<T>& rhs)
	{
		return { rhs.x * lhs,rhs.y * lhs,rhs.z * lhs };
	}
	template<typename T> inline Vec3<T> operator/(const Vec3<T>& lhs, T rhs)
	{
		return { lhs.x / rhs,lhs.y / rhs,lhs.z / rhs };
	}

	template<typename T> inline Vec3<T> operator*(const Vec3<T>& lhs, const Vec3<T>& rhs)
	{
		return { lhs.x * rhs.x,lhs.y * rhs.y,lhs.z * rhs.z };
	}

	template<typename T> inline Vec3<T>& Vec3<T>::operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	template<typename T> inline Vec3<T>& Vec3<T>::operator-=(const Vec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	template<typename T> inline Vec3<T>& Vec3<T>::operator*=(const Vec3& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}
	template<typename T> inline Vec3<T>& Vec3<T>::operator/=(const Vec3& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}
};

namespace gmath
{
	template<typename T> inline Vec2<T>::Vec2(T v) : x{ v }, y{ v } {}
	template<typename T> Vec2<T>::Vec2(T x, T y) : x{ x }, y{ y } {}
	template<typename T> Vec2<T>::Vec2(const Vec4<T>& vec4) : x{ vec4.x }, y{ vec4.y }{}
	template<typename T> Vec2<T>::Vec2(const Vec3<T>& vec3) : x{ vec3.x }, y{ vec3.y }{}

	template<typename T> Vec2<T> Vec2<T>::operator+(const Vec2& rhs) const
	{
		return { x + rhs.x,y + rhs.y };
	}

	template<typename T> Vec2<T> Vec2<T>::operator-(const Vec2& rhs) const
	{
		return { x - rhs.x,y - rhs.y };
	}

	template<typename T> T Vec2<T>::Dot(const Vec2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	template<typename T> T Vec2<T>::cross(const Vec2 b) const
	{
		return x * b.y - y * b.x;
	}

	template<typename T> Vec2<T> Vec2<T>::normalize() const
	{
		T len = pow(x * x + y * y, 0.5f);
		return {
			x / len,
			y / len
		};
	}

	template<typename T> Vec2<T> operator*(const  Vec2<T>& lhs, T rhs)
	{
		return { lhs.x * rhs, lhs.y * rhs };
	}
	template<typename T>  Vec2<T> operator*(T lhs, const  Vec2<T>& rhs)
	{
		return { rhs.x * lhs, rhs.y * lhs };
	}
	template<typename T>  Vec2<T> operator/(const  Vec2<T>& lhs, T rhs)
	{
		return { lhs.x / rhs, lhs.y / rhs };
	}

	template<typename T>  Vec2<T> operator*(const  Vec2<T>& lhs, const  Vec2<T>& rhs)
	{
		return { lhs.x * rhs.x,lhs.y * rhs.y };
	}

	template<typename T> Vec2<T>& Vec2<T>::operator+=(const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	template<typename T> Vec2<T>& Vec2<T>::operator-=(const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	template<typename T> Vec2<T>& Vec2<T>::operator*=(const Vec2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}
	template<typename T> Vec2<T>& Vec2<T>::operator/=(const Vec2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}
};

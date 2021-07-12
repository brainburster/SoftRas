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
		return _mm_pow_ps(data_m128, rhs4); //假的intrinsic,
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
	template<typename T> inline Vec2<T>::Vec2(T x, T y) : x{ x }, y{ y } {}
	template<typename T> inline Vec2<T>::Vec2(const Vec4<T>& vec4) : x{ vec4.x }, y{ vec4.y }{}
	template<typename T> inline Vec2<T>::Vec2(const Vec3<T>& vec3) : x{ vec3.x }, y{ vec3.y }{}

	template<typename T> inline Vec2<T> Vec2<T>::operator+(const Vec2& rhs) const
	{
		return { x + rhs.x,y + rhs.y };
	}

	template<typename T> inline Vec2<T> Vec2<T>::operator-(const Vec2& rhs) const
	{
		return { x - rhs.x,y - rhs.y };
	}

	template<typename T> inline T Vec2<T>::Dot(const Vec2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	template<typename T> inline T Vec2<T>::cross(const Vec2 b) const
	{
		return x * b.y - y * b.x;
	}

	template<typename T> inline Vec2<T> Vec2<T>::normalize() const
	{
		T len = pow(x * x + y * y, 0.5f);
		return {
			x / len,
			y / len
		};
	}

	template<typename T> inline Vec2<T> operator*(const  Vec2<T>& lhs, T rhs)
	{
		return { lhs.x * rhs, lhs.y * rhs };
	}
	template<typename T> inline Vec2<T> operator*(T lhs, const  Vec2<T>& rhs)
	{
		return { rhs.x * lhs, rhs.y * lhs };
	}
	template<typename T> inline Vec2<T> operator/(const  Vec2<T>& lhs, T rhs)
	{
		return { lhs.x / rhs, lhs.y / rhs };
	}

	template<typename T>  Vec2<T> operator*(const  Vec2<T>& lhs, const  Vec2<T>& rhs)
	{
		return { lhs.x * rhs.x,lhs.y * rhs.y };
	}

	template<typename T> inline Vec2<T>& Vec2<T>::operator+=(const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	template<typename T> inline Vec2<T>& Vec2<T>::operator-=(const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	template<typename T> inline Vec2<T>& Vec2<T>::operator*=(const Vec2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}
	template<typename T> inline Vec2<T>& Vec2<T>::operator/=(const Vec2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}
};

namespace gmath
{
	template<typename T> inline  Mat3x3<T>::Mat3x3(Vec3<T> a, Vec3<T> b, Vec3<T> c)
	{
		data[0] = a.x;
		data[1] = b.x;
		data[2] = c.x;
		data[3] = a.y;
		data[4] = b.y;
		data[5] = c.y;
		data[6] = a.z;
		data[7] = b.z;
		data[8] = c.z;
	}

	template<typename T> inline Mat3x3<T>::Mat3x3(T a, T b, T c, T d, T e, T f, T g, T h, T i)
	{
		data[0] = a;
		data[1] = b;
		data[2] = c;
		data[3] = d;
		data[4] = e;
		data[5] = f;
		data[6] = g;
		data[7] = h;
		data[8] = i;
	}

	//乘矩阵
	template<typename T> inline Mat3x3<T> Mat3x3<T>::operator*(const Mat3x3& rhs) const
	{
		return Mat3x3{
			//第一行
			data[0] * rhs.data[0] + data[1] * rhs.data[3] + data[2] * rhs.data[6],
			data[0] * rhs.data[1] + data[1] * rhs.data[4] + data[2] * rhs.data[7],
			data[0] * rhs.data[2] + data[1] * rhs.data[5] + data[2] * rhs.data[8],
			//第二行
			data[3] * rhs.data[0] + data[4] * rhs.data[3] + data[5] * rhs.data[6],
			data[3] * rhs.data[1] + data[4] * rhs.data[4] + data[5] * rhs.data[7],
			data[3] * rhs.data[2] + data[4] * rhs.data[5] + data[5] * rhs.data[8],
			//第三行
			data[6] * rhs.data[0] + data[7] * rhs.data[3] + data[8] * rhs.data[6],
			data[6] * rhs.data[1] + data[7] * rhs.data[4] + data[8] * rhs.data[7],
			data[6] * rhs.data[2] + data[7] * rhs.data[5] + data[8] * rhs.data[8],
		};
	}

	//乘向量
	template<typename T> inline Vec3<T> Mat3x3<T>::operator*(const Vec3<T>& rhs) const
	{
		return Vec3<T>{
			data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z,
				data[3] * rhs.x + data[4] * rhs.y + data[5] * rhs.z,
				data[6] * rhs.x + data[7] * rhs.y + data[8] * rhs.z
		};
	}

	//乘向量
	template<typename T> inline Vec4<T> Mat3x3<T>::operator*(const Vec4<T>& rhs) const
	{
		return Vec4<T>{
			data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z,
				data[3] * rhs.x + data[4] * rhs.y + data[5] * rhs.z,
				data[6] * rhs.x + data[7] * rhs.y + data[8] * rhs.z,
				1.f
		};
	}

	//构造单位矩阵
	template<typename T> inline Mat3x3<T> Mat3x3<T>::Unit()
	{
		return Mat3x3{
			1.f,0,0,
			0,1.f,0,
			0,0,1.f,
		};
	}

	//转置
	template<typename T> inline Mat3x3<T> Mat3x3<T>::transpose() const
	{
		return Mat3x3{
			data[0],data[3],data[6],
			data[1],data[4],data[7],
			data[2],data[5],data[8]
		};
	}

	//求逆
	template<typename T> inline Mat3x3<T> Mat3x3<T>::inverse() const
	{
		// A^-1 =  A*/|A|
		//A* : A的伴随矩阵
		//|A| : A的行列式

		//手算 |A|
		float det = data[0] * data[4] * data[8] + data[1] * data[5] * data[6] + data[2] * data[3] * data[7] -
			data[6] * data[4] * data[2] - data[7] * data[5] * data[0] - data[8] * data[3] * data[1];

		if (fabs(det) <= 1e-20)
		{
			//不可求逆,返回自身
			return *this;
		}

		//手算 A* / |A|（的转置）
		Mat3x3<T> _inverse = Mat3x3<T>{
			//第一行
			 (data[4] * data[8] - data[5] * data[7]) / det,
			-(data[3] * data[8] - data[5] * data[6]) / det,
			(data[3] * data[7] - data[4] * data[6]) / det,
			//第二行
			-(data[1] * data[8] - data[2] * data[7]) / det,
			(data[0] * data[8] - data[2] * data[6]) / det,
			-(data[0] * data[7] - data[1] * data[6]) / det,
			//第三行
			(data[1] * data[5] - data[2] * data[4]) / det,
			-(data[0] * data[5] - data[2] * data[3]) / det,
			(data[0] * data[4] - data[1] * data[3]) / det,
		};

		//转置
		_inverse = _inverse.transpose();

		//矩阵中可能出现-0.0f，但不重要, 因为矩阵中的元素不会被除
		return _inverse;
	}
};

namespace gmath
{
	//矩阵乘法
	template<typename T> Mat4x4<T> Mat4x4<T>::operator*(const Mat4x4& rhs) const
	{
		return Mat4x4{
			//第一行
			data[0] * rhs.data[0] + data[1] * rhs.data[4] + data[2] * rhs.data[8] + data[3] * rhs.data[12],
			data[0] * rhs.data[1] + data[1] * rhs.data[5] + data[2] * rhs.data[9] + data[3] * rhs.data[13],
			data[0] * rhs.data[2] + data[1] * rhs.data[6] + data[2] * rhs.data[10] + data[3] * rhs.data[14],
			data[0] * rhs.data[3] + data[1] * rhs.data[7] + data[2] * rhs.data[11] + data[3] * rhs.data[15],
			//第二行
			data[4] * rhs.data[0] + data[5] * rhs.data[4] + data[6] * rhs.data[8] + data[7] * rhs.data[12],
			data[4] * rhs.data[1] + data[5] * rhs.data[5] + data[6] * rhs.data[9] + data[7] * rhs.data[13],
			data[4] * rhs.data[2] + data[5] * rhs.data[6] + data[6] * rhs.data[10] + data[7] * rhs.data[14],
			data[4] * rhs.data[3] + data[5] * rhs.data[7] + data[6] * rhs.data[11] + data[7] * rhs.data[15],
			//第三行
			data[8] * rhs.data[0] + data[9] * rhs.data[4] + data[10] * rhs.data[8] + data[11] * rhs.data[12],
			data[8] * rhs.data[1] + data[9] * rhs.data[5] + data[10] * rhs.data[9] + data[11] * rhs.data[13],
			data[8] * rhs.data[2] + data[9] * rhs.data[6] + data[10] * rhs.data[10] + data[11] * rhs.data[14],
			data[8] * rhs.data[3] + data[9] * rhs.data[7] + data[10] * rhs.data[11] + data[11] * rhs.data[15],
			//第四行
			data[12] * rhs.data[0] + data[13] * rhs.data[4] + data[14] * rhs.data[8] + data[15] * rhs.data[12],
			data[12] * rhs.data[1] + data[13] * rhs.data[5] + data[14] * rhs.data[9] + data[15] * rhs.data[13],
			data[12] * rhs.data[2] + data[13] * rhs.data[6] + data[14] * rhs.data[10] + data[15] * rhs.data[14],
			data[12] * rhs.data[3] + data[13] * rhs.data[7] + data[14] * rhs.data[11] + data[15] * rhs.data[15]
		};
	}

	//乘向量 4分量
	template<typename T> Vec4<T> Mat4x4<T>::operator*(const Vec4<T>& rhs) const
	{
		return Vec4<T>{
			data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z + data[3] * rhs.w,
				data[4] * rhs.x + data[5] * rhs.y + data[6] * rhs.z + data[7] * rhs.w,
				data[8] * rhs.x + data[9] * rhs.y + data[10] * rhs.z + data[11] * rhs.w,
				data[12] * rhs.x + data[13] * rhs.y + data[14] * rhs.z + data[15] * rhs.w,
		};
	}

	template<typename T> Mat3x3<T> Mat4x4<T>::ToMat3x3() const
	{
		return Mat3x3 <T>{
			data[0], data[1], data[2],
				data[4], data[5], data[6],
				data[8], data[9], data[10],
		};
	}

	//unit
	template<typename T> Mat4x4<T> Mat4x4<T>::Unit()
	{
		return Mat4x4{
			1.f,0,0,0,
			0,1.f,0,0,
			0,0,1.f,0,
			0,0,0,1.f
		};
	}

	//平移
	template<typename T> Mat4x4<T> Mat4x4<T>::Translate(T x, T y, T z)
	{
		return Mat4x4{
			1.f,0,0,x,
			0,1.f,0,y,
			0,0,1.f,z,
			0,0,0,1.f
		};
	}

	template<typename T> Mat4x4<T> Mat4x4<T>::Translate(const Vec3<T>& v)
	{
		return Mat4x4{
			1.f,0,0,v.x,
			0,1.f,0,v.y,
			0,0,1.f,v.z,
			0,0,0,1.f
		};
	}

	//旋转
	template<typename T> Mat4x4<T> Mat4x4<T>::Rotate(T x, T y, T z)
	{
		return Mat4x4{
		   1.f,0,0,0,
		   0, cos(x), -sin(x), 0,
		   0, sin(x), cos(x),  0,
		   0, 0,   0, 1.f
		} *Mat4x4{
		   cos(y),0,sin(y),0,
		   0,1.f,0,0,
		   -sin(y),0,cos(y),0,
		   0,0,0,1.f
		} *Mat4x4{
		   cos(z),-sin(z),0,0,
		   sin(z),cos(z),0,0,
		   0,0,1.f,0,
		   0,0,0,1.f
		};
	}

	template<typename T> Mat4x4<T> Mat4x4<T>::Rotate(const Vec3<T>& v)
	{
		return Mat4x4{
		   1.f,0,0,0,
		   0, cos(v.x), -sin(v.x), 0,
		   0, sin(v.x), cos(v.x),  0,
		   0, 0,   0, 1.f
		} *Mat4x4{
		   cos(v.y),0,sin(v.y),0,
		   0,1.f,0,0,
		   -sin(v.y),0,cos(v.y),0,
		   0,0,0,1.f
		} *Mat4x4{
		   cos(v.z),-sin(v.z),0,0,
		   sin(v.z),cos(v.z),0,0,
		   0,0,1.f,0,
		   0,0,0,1.f
		};
	}

	//缩放
	template<typename T> Mat4x4<T> Mat4x4<T>::Scale(T x, T y, T z)
	{
		return Mat4x4{
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		};
	}

	//缩放
	template<typename T> Mat4x4<T> Mat4x4<T>::Scale(const Vec3<T>& v)
	{
		return Mat4x4{
			v.x, 0, 0, 0,
			0, v.y, 0, 0,
			0, 0, v.z, 0,
			0, 0, 0, 1
		};
	}

	//view 矩阵
	template<typename T> Mat4x4<T> Mat4x4<T>::View(const Vec3<T>& position, const Vec3<T>& front, const Vec3<T>& up)
	{
		Vec3<T> f = front.normalize();
		Vec3<T> u = up.normalize();
		Vec3<T> right = f.cross(u);

		return Mat4x4{
			right.x,right.y,right.z,0,
			u.x,u.y,u.z,0,
			-f.x,-f.y,-f.z,0,
			0,0,0,1
		} *Mat4x4{
			1,0,0,-position.x,
			0,1,0,-position.y,
			0,0,1,-position.z,
			0,0,0,1
		};
	}

	//相机矩阵
	template<typename T> Mat4x4<T> Mat4x4<T>::LookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up)
	{
		Vec3<T> f = (target - position).normalize();
		Vec3<T> u = up.normalize();
		Vec3<T> right = f.cross(u);

		return Mat4x4{
			right.x,right.y,right.z,0,
			u.x,u.y,u.z,0,
			-f.x,-f.y,-f.z,0,
			0,0,0,1
		} *Mat4x4{
			1,0,0,-position.x,
			0,1,0,-position.y,
			0,0,1,-position.z,
			0,0,0,1
		};
	}

	//正交
	template<typename T> Mat4x4<T> Mat4x4<T>::Ortho(T left, T right, T bottom, T top, T _near, T _far)
	{
		T dx = right - left;
		T dy = top - bottom;
		T dz = _far - _near;
		T a = 2 / dx;
		T b = -(right + left) / dx;
		T c = 2 / dy;
		T d = -(top + bottom) / dy;
		T e = 2 / dz;
		T f = -(_near + _far) / dy;
		return Mat4x4{
			a,0,0,b,
			0,c,0,d,
			0,0,e,f,
			0,0,0,1
		};
	}

	//透视矩阵-定义平截头体
	template<typename T> Mat4x4<T> Mat4x4<T>::Frustum(T left, T right, T bottom, T top, T _near, T _far)
	{
		T dx = right - left;
		T dy = top - bottom;
		T dz = _far - _near;
		T a = 2 * _near / dx;
		T b = (right + left) / dx;
		T c = 2 * _near / dy;
		T d = (top + bottom) / dy;
		T e = -(_far + _near) / dz;
		T f = -2 * _far * _near / dz;
		return Mat4x4{
			a,0,b,0,
			0,c,d,0,
			0,0,e,f,
			0,0,-1,0
		};
	}

	//透视矩阵
	template<typename T> Mat4x4<T> Mat4x4<T>::Projection(T fovy, T aspect, T _near, T _far)
	{
		T dz = _far - _near;
		T b = 1 / tan(fovy / 2);
		T a = b / aspect;
		T c = -(_near + _far) / dz;
		T d = -2 * _near * _far / dz;
		return Mat4x4{
			a,0,0,0,
			0,b,0,0,
			0,0,c,d,
			0,0,-1,0
		};
	}
};

namespace gmath::Utility
{
	template<typename T, typename U, typename V>
	inline T Clamp(T v, U a, V b)
	{
		return ((v < a ? a : v) < b ? (v < a ? a : v) : b);
	}

	template<typename T>
	inline T Clamp(T v)
	{
		return ((v < 0 ? 0 : v) < 1 ? (v < 0 ? 0 : v) : 1);
	}

	template<typename T, typename U>
	inline T Lerp(T a, T b, U n)
	{
		return a * n + b * (1.0f - n);
	}

	template<typename T>
	inline T BlendColor(T color0, T color1)
	{
		float a = 1.0f - (1.0f - color1.a) * (1.0f - color0.a);
		T color = 1.0f / a * (color1 * color1.a + (1.0f - color1.a) * color0.a * color0);
		color.a = a;
		return color;
	}

	inline float radians(float degree)
	{
		return degree / 180.f * pi;
	}

	inline float degrees(float radian)
	{
		return radian * 180.f / pi;
	}
};

#pragma once

#include <cmath>
#include <immintrin.h>
#include <DirectXMath.h>

namespace gmath
{
	template<typename T = float> struct Vec2;
	template<typename T = float> struct Vec3;
	template<typename T = float> struct Vec4;
	template<typename T = float> struct Mat3x3;
	template<typename T = float> struct Mat4x4;

	template<typename T>
	struct Vec4
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};
		};

		Vec4(T v = 0);
		Vec4(T x, T y, T z, T w = 0);
		Vec4(const Vec2<T>& vec2, T z = 0, T w = 0);
		Vec4(const Vec3<T>& vec3, T w = 0);
		Vec4(const Vec4& vec4) = default;

		T  Dot(const Vec4& rhs) const;

		Vec4 operator+(const Vec4& rhs) const;
		Vec4 operator-(const Vec4& rhs) const;

		Vec4& operator+=(const Vec4& rhs);
		Vec4& operator-=(const Vec4& rhs);
		Vec4& operator*=(const Vec4& rhs);
		Vec4& operator/=(const Vec4& rhs);

		template<typename T>
		friend Vec4<T> operator*(const Vec4<T>& lhs, T rhs);
		template<typename T>
		friend Vec4<T> operator*(T lhs, const Vec4<T>& rhs);
		template<typename T>
		friend Vec4<T> operator/(const Vec4<T>& lhs, T rhs);
		template<typename T>
		friend Vec4<T> operator*(const Vec4<T>& lhs, const Vec4<T>& rhs);
	};

	template<typename T>
	struct Vec3
	{
		T x;
		T y;
		T z;

		Vec3(T v = 0);
		Vec3(T x, T y, T z);
		Vec3(const Vec4<T>& vec4);
		Vec3(const Vec2<T>& vec2);
		Vec3(const Vec3& vec3) = default;

		Vec4<T> ToHomoCoord() const;
		T Dot(const Vec3& rhs) const;
		Vec3 cross(const Vec3& b) const;
		Vec3 normalize() const;

		Vec3 operator+(const Vec3& rhs) const;
		Vec3 operator-(const Vec3& rhs) const;

		template<typename T>
		friend Vec3<T> operator*(const Vec3<T>& lhs, T rhs);
		template<typename T>
		friend Vec3<T> operator*(T lhs, const Vec3<T>& rhs);
		template<typename T>
		friend Vec3<T> operator/(const Vec3<T>& lhs, T rhs);
		template<typename T>
		friend Vec3<T> operator*(const Vec3<T>& lhs, const Vec3<T>& rhs);

		Vec3& operator+=(const Vec3& rhs);
		Vec3& operator-=(const Vec3& rhs);
		Vec3& operator*=(const Vec3& rhs);
		Vec3& operator/=(const Vec3& rhs);
	};

	template<typename T>
	struct Vec2
	{
		T x;
		T y;

		Vec2(T v = 0);
		Vec2(T x, T y);
		Vec2(const Vec4<T>& vec4);
		Vec2(const Vec3<T>& vec3);
		Vec2(const Vec2& vec2) = default;

		Vec2 operator+(const Vec2& rhs) const;
		Vec2 operator-(const Vec2& rhs) const;

		T Dot(const Vec2& rhs) const;
		T cross(const Vec2 b) const;
		Vec2 normalize() const;

		template<typename T>
		friend Vec2<T> operator*(const Vec2<T>& lhs, T rhs);
		template<typename T>
		friend Vec2<T> operator*(T lhs, const Vec2<T>& rhs);
		template<typename T>
		friend Vec2<T> operator/(const Vec2<T>& lhs, T rhs);
		template<typename T>
		friend Vec2<T> operator*(const Vec2<T>& lhs, const Vec2<T>& rhs);

		Vec2& operator+=(const Vec2& rhs);
		Vec2& operator-=(const Vec2& rhs);
		Vec2& operator*=(const Vec2& rhs);
		Vec2& operator/=(const Vec2& rhs);
	};

	template<typename T>
	struct Mat3x3
	{
		T data[9];

		Mat3x3() = default;

		static Mat3x3 Unit()
		{
			return Mat3x3{
				1.f,0,0,
				0,1.f,0,
				0,0,1.f,
			};
		}

		Mat3x3(Vec3<T> a, Vec3<T> b, Vec3<T> c)
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

		//Mat3x3(Vec3<T> a, Vec3<T> b, Vec3<T> c)
		//{
		//	data[0] = a.x;
		//	data[1] = a.y;
		//	data[2] = a.z;
		//	data[3] = b.x;
		//	data[4] = b.y;
		//	data[5] = b.z;
		//	data[6] = c.x;
		//	data[7] = c.y;
		//	data[8] = c.z;
		//}

		Mat3x3(T a, T b, T c, T d, T e, T f, T g, T h, T i)
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
		Mat3x3 operator*(const Mat3x3& rhs) const
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
		Vec3<T> operator*(const Vec3<T>& rhs) const
		{
			return Vec3<T>{
				data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z,
					data[3] * rhs.x + data[4] * rhs.y + data[5] * rhs.z,
					data[6] * rhs.x + data[7] * rhs.y + data[8] * rhs.z
			};
		}

		//乘向量
		Vec4<T> operator*(const Vec4<T>& rhs) const
		{
			return Vec4<T>{
				data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z,
					data[3] * rhs.x + data[4] * rhs.y + data[5] * rhs.z,
					data[6] * rhs.x + data[7] * rhs.y + data[8] * rhs.z,
					1.f
			};
		}

		//转置
		Mat3x3 transpose() const
		{
			return Mat3x3{
				data[0],data[3],data[6],
				data[1],data[4],data[7],
				data[2],data[5],data[8]
			};
		}

		//求逆
		Mat3x3 inverse() const
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

	template<typename T>
	struct Mat4x4
	{
		T data[16];

		//矩阵乘法
		Mat4x4 operator*(const Mat4x4& rhs) const
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
		Vec4<T> operator*(const Vec4<T>& rhs) const
		{
			return Vec4<T>{
				data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z + data[3] * rhs.w,
					data[4] * rhs.x + data[5] * rhs.y + data[6] * rhs.z + data[7] * rhs.w,
					data[8] * rhs.x + data[9] * rhs.y + data[10] * rhs.z + data[11] * rhs.w,
					data[12] * rhs.x + data[13] * rhs.y + data[14] * rhs.z + data[15] * rhs.w,
			};
		}

		Mat3x3<T> ToMat3x3() const
		{
			return Mat3x3 <T>{
				data[0], data[1], data[2],
					data[4], data[5], data[6],
					data[8], data[9], data[10],
			};
		}

		//unit
		static Mat4x4 Unit()
		{
			return Mat4x4{
				1.f,0,0,0,
				0,1.f,0,0,
				0,0,1.f,0,
				0,0,0,1.f
			};
		}

		//平移
		static Mat4x4 Translate(T x, T y, T z)
		{
			return Mat4x4{
				1.f,0,0,x,
				0,1.f,0,y,
				0,0,1.f,z,
				0,0,0,1.f
			};
		}

		static Mat4x4 Translate(const Vec3<T>& v)
		{
			return Mat4x4{
				1.f,0,0,v.x,
				0,1.f,0,v.y,
				0,0,1.f,v.z,
				0,0,0,1.f
			};
		}

		//旋转
		static Mat4x4 Rotate(T x, T y, T z)
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

		static Mat4x4 Rotate(const Vec3<T>& v)
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
		static Mat4x4 Scale(T x, T y, T z)
		{
			return Mat4x4{
				x, 0, 0, 0,
				0, y, 0, 0,
				0, 0, z, 0,
				0, 0, 0, 1
			};
		}

		//缩放
		static Mat4x4 Scale(const Vec3<T>& v)
		{
			return Mat4x4{
				v.x, 0, 0, 0,
				0, v.y, 0, 0,
				0, 0, v.z, 0,
				0, 0, 0, 1
			};
		}

		//view 矩阵
		static Mat4x4 View(const Vec3<T>& position, const Vec3<T>& front, const Vec3<T>& up)
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
		static Mat4x4 LookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up)
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
		static Mat4x4 Ortho(T left, T right, T bottom, T top, T _near, T _far)
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
		static Mat4x4 Frustum(T left, T right, T bottom, T top, T _near, T _far)
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
		static Mat4x4 Projection(T fovy, T aspect, T _near, T _far)
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

	namespace Utility
	{
		static constexpr float pi = 3.1415926f;

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

	template<>
	struct alignas(16) Vec4<float>
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};
			__m128 data_m128;
		};

		Vec4(float v = 0);
		Vec4(float x, float y, float z, float w = 1);
		Vec4(const Vec2<float>& vec2, float z = 0, float w = 0);
		Vec4(const Vec3<float>& vec3, float w = 0);
		Vec4(const Vec4& vec4) = default;

		Vec4(__m128 data);
		operator __m128();

		__forceinline Vec4 _vectorcall Pow(float rhs) const noexcept;
		__forceinline Vec4 _vectorcall Sqrt() const noexcept;
		__forceinline float _vectorcall Dot(Vec4 rhs) const noexcept;

		__forceinline Vec4& _vectorcall operator+=(Vec4 rhs) noexcept;
		__forceinline Vec4& _vectorcall operator+=(float rhs) noexcept;
		__forceinline Vec4& _vectorcall operator-=(const Vec4& rhs) noexcept;
		__forceinline Vec4& _vectorcall operator-=(float rhs) noexcept;
		__forceinline Vec4& _vectorcall operator*=(const Vec4& rhs) noexcept;
		__forceinline Vec4& _vectorcall operator*=(float rhs) noexcept;
		__forceinline Vec4& _vectorcall operator/=(const Vec4& rhs) noexcept;
		__forceinline Vec4& _vectorcall operator/=(float rhs) noexcept;

		friend __forceinline Vec4 _vectorcall operator+(Vec4 lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator+(float lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator+(Vec4 lhs, float rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator-(Vec4 lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator-(float lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator-(Vec4 lhs, float rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator*(Vec4 lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator*(float lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator*(Vec4 lhs, float rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator/(Vec4 lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator/(float lhs, Vec4 rhs) noexcept;
		friend __forceinline Vec4 _vectorcall operator/(Vec4 lhs, float rhs) noexcept;
	};
};

#include "impl/game_math.inl"

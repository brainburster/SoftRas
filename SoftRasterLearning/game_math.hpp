#pragma once
#include <cmath>

namespace gmath
{

	template<typename T = double> struct Vec2;
	template<typename T = double> struct Vec3;


	template<typename T = double>
	struct Vec4
	{
		union {
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
			struct
			{
				T r;
				T g;
				T b;
				T a;
			};
		};

		Vec4(T v = 0) : x{ v }, y{ v }, z{ v }, w{ v }
		{
		}

		Vec4(T x, T y, T z, T w = 1) : x{ x }, y{ y }, z{ z }, w{ w }
		{
		}

		Vec4(const Vec2<T>& vec2, T z = 0, T w = 0) : x{ vec2.x }, y{ vec2.y }, z{ z }, w{ w }
		{
		}

		Vec4(const Vec3<T>& vec3, T w = 0) : x{ vec3.x }, y{ vec3.y }, z{ vec3.z }, w{ w }
		{
		}

		Vec4(const Vec4& vec4_hc) = default;

		Vec4 operator+(const Vec4& rhs) const
		{
			return { x + rhs.x,y + rhs.y,z + rhs.z, w + rhs.w };
		}

		Vec4& operator+=(const Vec4& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}
		Vec4& operator-=(const Vec4& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}
		Vec4& operator*=(const Vec4& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}
		Vec4& operator/=(const Vec4& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		Vec4 operator-(const Vec4& rhs) const
		{
			return { x - rhs.x,y - rhs.y,z - rhs.z, w - rhs.w };
		}

		T operator*(const Vec4& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
		}

		Vec4 normalize()
		{
			return { x / w,y / w,z / w, 1 };
		}

		friend Vec4 operator*(const Vec4& lhs, T rhs)
		{
			return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs,lhs.w * rhs };
		}

		friend Vec4 operator*(T lhs, const Vec4& rhs)
		{
			return { rhs.x * lhs,rhs.y * lhs,rhs.z * lhs, rhs.w * lhs };
		}

		friend Vec4 operator/(const Vec4& lhs, T rhs)
		{
			return { lhs.x / rhs,lhs.y / rhs,lhs.z / rhs, lhs.w / rhs };
		}
	};

	template<typename T>
	struct Vec3
	{
		T x;
		T y;
		T z;

		Vec3(T v = 0) : x{ v }, y{ v }, z{ v }
		{
		}

		Vec3(T x, T y, T z) :x{ x }, y{ y }, z{ z }
		{
		}

		Vec3(const Vec4<T>& vec4) :
			x{ vec4.x },
			y{ vec4.y },
			z{ vec4.z }
		{
		}

		Vec3(const Vec2<T>& vec2) :x{ vec2.x }, y{ vec2.y }
		{
		}

		Vec3(const Vec3& vec3) = default;
		Vec3 operator+(const Vec3& rhs) const
		{
			return { x + rhs.x,y + rhs.y,z + rhs.z };
		}

		Vec3 operator-(const Vec3& rhs) const
		{
			return { x - rhs.x,y - rhs.y,z - rhs.z };
		}

		T operator*(const Vec3& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}

		Vec3 cross(const Vec3& b) const
		{
			return Vec3{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x };
		}

		friend Vec3 operator*(const Vec3& lhs, T rhs)
		{
			return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs };
		}
		friend Vec3 operator*(T lhs, const Vec3& rhs)
		{
			return { rhs.x * lhs,rhs.y * lhs,rhs.z * lhs };
		}
		friend Vec3 operator/(const Vec3& lhs, T rhs)
		{
			return { lhs.x / rhs,lhs.y / rhs,lhs.z / rhs };
		}

		Vec3& operator+=(const Vec3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		Vec3& operator-=(const Vec3& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		Vec3& operator*=(const Vec3& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}
		Vec3& operator/=(const Vec3& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		Vec3 normalize() const
		{
			T len = pow(x*x+y*y+z*z,0.5);
			return {
				x / len,
				y / len,
				z / len
			};
		}
	};

	template<typename T>
	struct Vec2
	{
		T x;
		T y;

		Vec2(T v = 0) : x{ v }, y{ v } {}
		Vec2(T x, T y) : x{ x }, y{ y } {}
		Vec2() = default;
		Vec2(const Vec3<T>& vec3) : x{ vec3.x }, y{ vec3.y }{}
		Vec2(const Vec4<T>& vec4) : x{ vec4.x }, y{ vec4.y }{}


		Vec2 operator+(const Vec2& rhs) const
		{
			return { x + rhs.x,y + rhs.y };
		}

		Vec2 operator-(const Vec2& rhs) const
		{
			return { x - rhs.x,y - rhs.y };
		}

		T operator*(const Vec2& rhs) const
		{
			return x * rhs.x + y * rhs.y;
		}

		T cross(const Vec2 b) const
		{
			return x * b.y - y * b.x;
		}

		friend Vec2 operator*(const Vec2& lhs, T rhs)
		{
			return { lhs.x * rhs, lhs.y * rhs };
		}
		friend Vec2 operator*(T lhs, const Vec2& rhs)
		{
			return { rhs.x * lhs, rhs.y * lhs };
		}
		friend Vec2 operator/(const Vec2& lhs, T rhs)
		{
			return { lhs.x / rhs, lhs.y / rhs };
		}

		Vec2& operator+=(const Vec2& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		Vec2& operator-=(const Vec2& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		Vec2& operator*=(const Vec2& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}
		Vec2& operator/=(const Vec2& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}


		Vec2 normalize() const
		{
			T len = pow(x * x + y * y, 0.5);
			return {
				x / len,
				y / len
			};
		}
	};

	template<typename T = double>
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

		//乘向量
		Vec4<T> operator*(const Vec4<T>& rhs) const
		{
			return Vec4<T>{
				data[0] * rhs.x + data[1] * rhs.y + data[2] * rhs.z + data[3] * rhs.w,
				data[4] * rhs.x + data[5] * rhs.y + data[6] * rhs.z + data[7] * rhs.w,
				data[8] * rhs.x + data[9] * rhs.y + data[10] * rhs.z + data[11] * rhs.w,
				data[12] * rhs.x + data[13] * rhs.y + data[14] * rhs.z + data[15] * rhs.w,
			};
		}

		//unit
		static Mat4x4 Unit()
		{
			return Mat4x4{
				1.,0,0,0,
				0,1.,0,0,
				0,0,1.,0,
				0,0,0,1.
			};
		}

		//平移
		static Mat4x4 Translate(T x, T y, T z)
		{
			return Mat4x4{
				1.,0,0,x,
				0,1.,0,y,
				0,0,1.,z,
				0,0,0,1.
			};
		}

		//旋转
		static Mat4x4 Rotate(T x, T y, T z)
		{
			return Mat4x4{
			   1.,0,0,0,
			   0, cos(x), -sin(x), 0,
			   0, sin(x), cos(x),  0,
			   0, 0,   0, 1.
			} * Mat4x4{
			   cos(y),0,sin(y),0,
			   0,1.,0,0,
			   -sin(y),0,cos(y),0,
			   0,0,0,1.
			} * Mat4x4{
			   cos(z),-sin(z),0,0,
			   sin(z),cos(z),0,0,
			   0,0,1.,0,
			   0,0,0,1.
			};
		}

		//缩放
		static Mat4x4 Scale(T x, T y, T z)
		{
			return Mat4x4{
				x, 0, 0, 0,
				0, y, 0, 0,
				0, 0, z, 0,
				0, 0, 0,1.
			};
		}

		//相机矩阵
		static Mat4x4 Camera(const Vec3<T>& position, const Vec3<T>& front, const Vec3<T>& up)
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

		//正交
		static Mat4x4 Ortho(T left,T right, T bottom, T top, T _near, T _far)
		{
			return Mat4x4{
				2 / (right - left),0,0,0,
				0,2 / (top - bottom),0,0,
				0,0,2 / (_near - _far),0,
				0,0,0,1
			}*Mat4x4{
				1.,0,0,-(right + left) / 2,
				0,1.,0,-(top + bottom) / 2,
				0,0,1,-(_near + _far) / 2,
				0,0,0,1
			};
		}

		//透视


	};

};

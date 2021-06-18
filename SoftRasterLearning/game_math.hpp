#pragma once
#include <cmath>

namespace gmath
{

	template<typename T = float> struct Vec2;
	template<typename T = float> struct Vec3;
	template<typename T = float> struct Vec4_hc;


	template<typename T = float>
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
		explicit Vec4(const Vec4_hc<T>& vec4_hc) : x{ vec4_hc.x }, y{ vec4_hc.y }, z{ vec4_hc.z }, w{ vec4_hc.w } {};


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
	struct Vec4_hc
	{
		T x;
		T y;
		T z;
		T w;

		Vec4_hc(T v = 0) : x{ v }, y{ v }, z{ v }, w{ 1 }
		{
		}

		Vec4_hc(T x, T y, T z, T w = 1) : x{ x }, y{ y }, z{ z }, w{ w }
		{
		}

		Vec4_hc(const Vec2<T>& vec2, T z = 0) : x{ vec2.x }, y{ vec2.y }, z{ z }, w{ 1 }
		{
		}

		Vec4_hc(const Vec3<T>& vec3) : x{ vec3.x }, y{ vec3.y }, z{ vec3.z }, w{ 1 }
		{
		}

		Vec4_hc(const Vec4_hc& vec4_hc) = default;

		explicit Vec4_hc(const Vec4<T>& vec4) : x{ vec4.x }, y{ vec4.y }, z{ vec4.z }, w{ vec4.w }{};


		Vec4_hc operator+(const Vec4_hc& rhs) const
		{
			return { x + rhs.x,y + rhs.y,z + rhs.z, 1 };
		}

		Vec4_hc operator-(const Vec4_hc& rhs) const
		{
			return { x - rhs.x,y - rhs.y,z - rhs.z, 1 };
		}

		T operator*(const Vec4_hc& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z + 1;
		}

		Vec4_hc cross(const Vec4_hc& b) const
		{
			return Vec4_hc{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x,1 };
		}

		Vec4_hc normalize()
		{
			return { x / w,y / w,z / w,1 };
		}

		friend Vec4_hc operator*(const Vec4_hc& lhs, T rhs)
		{
			return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs,1 };
		}

		friend Vec4_hc operator*(T lhs, const Vec4_hc& rhs)
		{
			return { rhs.x * lhs,rhs.y * lhs,rhs.z * lhs, 1 };
		}

		friend Vec4_hc operator/(const Vec4_hc& lhs, T rhs)
		{
			return { lhs.x / rhs,lhs.y / rhs,lhs.z / rhs, 1 };
		}


		Vec4_hc& operator+=(const Vec4_hc& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w = 1;
			return *this;
		}
		Vec4_hc& operator-=(const Vec4_hc& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w = 1;
			return *this;
		}
		Vec4_hc& operator*=(const Vec4_hc& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w = 1;
			return *this;
		}
		Vec4_hc& operator/=(const Vec4_hc& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w = 1;
			return *this;
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

		Vec3(const Vec4_hc<T>& vec4_hc) :
			x{ vec4_hc.x / vec4_hc.w },
			y{ vec4_hc.y / vec4_hc.w },
			z{ vec4_hc.z / vec4_hc.w }
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
		Vec2(const Vec4_hc<T>& vec4_hc) : x{ vec4_hc.x / vec4_hc.w }, y{ vec4_hc.y / vec4_hc.w }{}
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
	};


	struct Mat4x4
	{

	};

};

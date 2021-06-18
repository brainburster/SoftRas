#pragma once
#include <cmath>

template<typename T = float> struct Vec2;
template<typename T = float> struct Vec3;

//暂时这么定义
template<typename T = float>
struct Vec4_hc
{
	T x;
	T y;
	T z;
	T w;

	Vec4_hc(T v) : x{ v }, y{ v }, z{ v }, w{1}
	{
	}

	Vec4_hc(T x, T y, T z, T w = 1) : x{ x }, y{ y }, z{ z }, w{w}
	{
	}

	Vec4_hc(const Vec2<T>& vec2, T z = 0) : x{ vec2.x }, y{ vec2.y }, z{ z }, w{1}
	{
	}

	Vec4_hc(const Vec3<T>& vec3) : x{ vec3.x }, y{ vec3.y }, z{ vec3.z }, w{1}
	{
	}

	Vec4_hc(const Vec4_hc& vec4_hc) = default;

	Vec4_hc operator+(const Vec4_hc& rhs) const
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z,w + rhs.w };
	}

	Vec4_hc operator-(const Vec4_hc& rhs) const
	{
		return { x - rhs.x,y - rhs.y,z - rhs.z,w - rhs.w };
	}

	T operator*(const Vec4_hc& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	Vec4_hc cross(const Vec4_hc& b) const
	{
		return Vec4_hc{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x,1 };
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
};

template<typename T>
struct Vec3
{
	T x;
	T y;
	T z;

	Vec3(T v) : x{ v }, y{ v }, z{v}
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

	Vec3(const Vec2<T>& vec2) :x{ vec2.x }, y{vec2.y}
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
		return Vec3{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x};
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
};

template<typename T>
struct Vec2
{
	T x;
	T y;

	Vec2(T v) : x{ v }, y{ v } {}
	Vec2(T x, T y) : x{ x }, y{ y } {}
	Vec2() = default;
	Vec2(const Vec3<T>& vec3) : x{ vec3.x }, y{vec3.y}{}
	Vec2(const Vec4_hc<T>& vec4) : x{ vec4.x/ vec4.w }, y{ vec4.y/ vec4.w }{}

	Vec2 operator+(const Vec2& rhs) const
	{
		return { x + rhs.x,y + rhs.y};
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
};

using Vec4 = Vec4_hc<float>;
#pragma once
#include <cmath>

//暂时这么定义
struct Vec4
{
	float x;
	float y;
	float z;
	float w;
	Vec4 operator+(const Vec4& rhs) const
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z,w + rhs.w };
	}

	float operator*(const Vec4& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	Vec4 cross(const Vec4& b) const
	{
		return Vec4{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x,1 };
	}

	friend Vec4 operator*(const Vec4& lhs, float rhs)
	{
		return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs,lhs.w * rhs };
	}

};

struct Vec3
{
	float x;
	float y;
	float z;

	Vec3 operator+(const Vec3& rhs) const
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z };
	}

	float operator*(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vec3 cross(const Vec3& b) const
	{
		return Vec3{ y * b.z - z * b.y,z * b.x - x * b.z,x * b.y - y * b.x};
	}

	friend Vec3 operator*(const Vec3& lhs, float rhs)
	{
		return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs };
	}

};

struct Vec2
{
	float x;
	float y;

	Vec2 operator+(const Vec2& rhs) const
	{
		return { x + rhs.x,y + rhs.y};
	}

	float operator*(const Vec2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	float cross(const Vec2 b) const
	{
		return x * b.y - y * b.x;
	}

	friend Vec2 operator*(const Vec2& lhs, float rhs)
	{
		return { lhs.x * rhs,lhs.y * rhs };
	}

	float length()const
	{
		return pow(x*x+y*y ,0.5);
	}
};
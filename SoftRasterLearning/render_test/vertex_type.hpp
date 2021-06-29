#pragma once

#include "../core/types_and_defs.hpp"

struct Vertex
{
	core::Position position;
	core::Color color;
	core::Vec2 uv;
	core::Vec3 normal;

	Vertex operator+(const Vertex& rhs) const
	{
		return {
			position + rhs.position,
			color + rhs.color,
			uv + rhs.uv,
			normal + rhs.normal
		};
	}

	friend Vertex operator*(const Vertex& lhs, float rhs)
	{
		return {
			lhs.position * rhs,
			lhs.color * rhs,
			lhs.uv * rhs,
			lhs.normal * rhs
		};
	}
};

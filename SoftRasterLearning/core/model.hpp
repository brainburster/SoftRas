#pragma once

#include "types_and_defs.hpp"

namespace core
{
	struct Model_Vertex
	{
		Vec3 position;
		Vec2 uv;
		Vec3 normal;
		Vec3 tangent;

		Model_Vertex() = default;
		Model_Vertex(Vec3 p, Vec2 t, Vec3 n) :
			position(p),
			uv(t),
			normal(n)
		{
		}
	};

	struct Model
	{
		std::vector<Model_Vertex> mesh;

		Model() = default;
		Model(Model&& other) noexcept :
			mesh{ std::move(other.mesh) }
		{}
		Model& operator=(Model&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			this->mesh = std::move(other.mesh);
		}
		//...
	};
}

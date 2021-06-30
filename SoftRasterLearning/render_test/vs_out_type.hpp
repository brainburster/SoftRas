#pragma once

#include "../core/types_and_defs.hpp"

struct VS_OUT_Unlit : core::shader_varying_float<VS_OUT_Unlit>
{
	core::Position position;
	core::Vec2 uv;
	core::Vec3 normal;
};

struct VS_OUT_Light : core::shader_varying_float<VS_OUT_Light>
{
	core::Position position;
	core::Position world_position;
	//core::Color color;
	core::Vec2 uv;
	core::Vec3 normal;
};

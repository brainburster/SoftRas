#pragma once

#include "../core/types_and_defs.hpp"

//无光照
struct Varying_Unlit : core::shader_varying_float<Varying_Unlit>
{
	core::Position position;
	core::Vec2 uv;
	core::Vec3 normal;
};

//世界空间光照
struct Varying_Light_ws : core::shader_varying_float<Varying_Light_ws>
{
	core::Position position; //clip space
	core::Vec3 position_ws;
	core::Vec2 uv;
	core::Vec3 normal_ws; //转换到 world space 要乘模型(法线)矩阵
	core::Mat3 TBN = {};
};

//切线空间光照
struct Varying_Light_ts : core::shader_varying_float<Varying_Light_ts>
{
	core::Position position; //clip space
	core::Vec3 light_dir_ts; // tangent space
	core::Vec3 view_dir_ts; // tangent space
	core::Vec3 half_dir_ts;
	core::Vec2 uv;
};

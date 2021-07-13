#pragma once

#include "../core/types_and_defs.hpp"

//�޹���
struct Varying_Unlit : core::shader_varying_float<Varying_Unlit>
{
	core::Position position;
	core::Vec2 uv;
	core::Vec3 normal;
};

//����ռ����
struct Varying_Light_ws : core::shader_varying_float<Varying_Light_ws>
{
	core::Position position; //clip space
	core::Vec3 position_ws;
	core::Vec2 uv;
	core::Vec3 normal_ws; //ת���� world space Ҫ��ģ��(����)����
	core::Mat3 TBN = {};
};

//���߿ռ����
struct Varying_Light_ts : core::shader_varying_float<Varying_Light_ts>
{
	core::Position position; //clip space
	core::Vec3 light_dir_ts; // tangent space
	core::Vec3 view_dir_ts; // tangent space
	core::Vec3 half_dir_ts;
	core::Vec2 uv;
};

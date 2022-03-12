#pragma once

#include "../core/types_and_defs.hpp"

//��VsOut
struct VsOut_Simple : core::vs_out_base<VsOut_Simple>
{
	core::Position position;
	core::Vec2 uv;
	core::Vec3 normal;
};

//����ռ����
struct VsOut_Light_ws : core::vs_out_base<VsOut_Light_ws>
{
	core::Position position; //clip space
	core::Vec3 position_ws;
	core::Vec2 uv;
	core::Vec3 normal_ws; //ת���� world space Ҫ��ģ��(����)����
	core::Mat3 TBN = {};
};

//���߿ռ����
struct VsOut_Light_ts : core::vs_out_base<VsOut_Light_ts>
{
	core::Position position; //clip space
	core::Vec3 light_dir_ts; // tangent space
	core::Vec3 view_dir_ts; // tangent space
	core::Vec3 half_dir_ts;
	core::Vec2 uv;
};

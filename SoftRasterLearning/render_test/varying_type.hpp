#pragma once

#include "../core/types_and_defs.hpp"

struct Varying_Unlit : core::shader_varying_float<Varying_Unlit>
{
	core::Position position;
	core::Vec2 uv;
	core::Vec3 normal;
};

struct Varying_Light : core::shader_varying_float<Varying_Light>
{
	core::Position position; //clip space
	core::Position position_ls;
	core::Position position_ws;
	core::Vec2 uv;
	core::Vec3 normal; //local space
	core::Vec3 normal_ws; //world space Ҫ��ģ�;�����Ϊ����w����Ϊ0, ����ֻ����ת������������
};

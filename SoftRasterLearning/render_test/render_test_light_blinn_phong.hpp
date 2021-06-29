#pragma once

#include "../framework/fps_renderer_app.hpp"
#include "../core/texture.hpp"
#include "../core/types_and_defs.hpp"
#include "vertex_type.hpp"

class Material_Blinn_Phong
{
public:
	core::Mat mat = core::Mat::Unit();
	core::Texture* tex0 = nullptr;
	core::Vec3 light = { 0,10,0 };

	Vertex VS(const core::Model_Vertex& v) const
	{
		return {
			mat * core::Vec4{v.position,1.0},
			core::Vec4(v.normal,1),
			v.uv
		};
	}

	gmath::Vec4<float> FS(const Vertex& v) const
	{
		return /*v.color +*/ tex0->Sampler(tex0, v.uv);
	}
};

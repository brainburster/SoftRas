#pragma once

#include "object.hpp"
#include "../core/texture.hpp"
#include "camera.hpp"
#include "render_engine.hpp"

namespace framework
{
	class BillBord : public Object
	{
	public:
		std::shared_ptr <core::Texture> tex0;

		virtual void Render(IRenderEngine& engine) const override
		{
			//
			static core::Vec4 rect[6] = {
				{-1,-1,0,0},
				{1,-1,1,0},
				{-1,1,0,1},
				{-1,1,0,1},
				{1,-1,1,0},
				{1,1,1,1}
			};

			struct varying_t : core::shader_varying_float<varying_t>
			{
				core::Vec4 position;
				core::Vec2 uv;
			};

			struct Shader
			{
				std::shared_ptr <core::Texture> tex0;
				core::Mat mvp = {};
				varying_t VS(const core::Vec4& point) const
				{
					varying_t v{};
					v.position = mvp * core::Vec4(point.x, point.y, 0, 1);
					v.uv = core::Vec2(point.z, point.w);
					return v;
				}
				core::Color FS(const varying_t& v) const
				{
					//本地空间
					core::Color color = core::Texture::Sample(tex0.get(), v.uv);
					return color;
				}
			} shader{ tex0 };

			if (const ICamera* camera = engine.GetMainCamera())
			{
				using namespace gmath::utility;
				//使公告板朝向摄像机
				core::Vec3 c_f = camera->GetFront();
				core::Vec3 c_r = c_f.cross(core::Vec3{ 0,1.f,0 }).normalize();
				core::Vec3 c_u = c_r.cross(c_f).normalize();
				core::Vec3 front = (camera->GetPosition() - transform.position).normalize() * -1;
				core::Vec3 right = front.cross(c_u).normalize();
				core::Vec3 up = right.cross(front).normalize();
				core::Mat look_at_camera = { right,up,front,{0,0,0,1.f} }; //View({ 0,0,0 }, front, up).Transpose()
				core::Mat model = Translate(transform.position) * look_at_camera * Rotate(transform.rotation) * Scale(transform.scale);
				shader.mvp = camera->GetProjectionViewMatrix() * model;
			}

			core::Renderer<Shader> renderer = { engine.GetCtx(), shader };
			renderer.DrawTriangles(rect, sizeof(rect) / sizeof(rect[0]));
		}
	};
};

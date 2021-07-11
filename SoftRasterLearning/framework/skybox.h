#pragma once

#include "object.hpp"
#include "render_engine.hpp"
#include "../core/cube_map.hpp"

namespace framework
{
	struct Skybox : public IRenderAble
	{
	public:
		std::shared_ptr<core::CubeMap> cube_map;
	public:
		virtual void Render(IRenderEngine& engine) const override
		{
			if (!cube_map)
			{
				return;
			}

			static float box[] =
			{
				   -1.f, 1.f, -1.f,
					-1.f, -1.f, -1.f,
					1.f, -1.f, -1.f,
					1.f, -1.f, -1.f,
					1.f, 1.f, -1.f,
					-1.f, 1.f, -1.f,

					-1.f, -1.f, 1.f,
					-1.f, -1.f, -1.f,
					-1.f, 1.f, -1.f,
					-1.f, 1.f, -1.f,
					-1.f, 1.f, 1.f,
					-1.f, -1.f, 1.f,

					1.f, -1.f, -1.f,
					1.f, -1.f, 1.f,
					1.f, 1.f, 1.f,
					1.f, 1.f, 1.f,
					1.f, 1.f, -1.f,
					1.f, -1.f, -1.f,

					-1.f, -1.f, 1.f,
					-1.f, 1.f, 1.f,
					1.f, 1.f, 1.f,
					1.f, 1.f, 1.f,
					1.f, -1.f, 1.f,
					-1.f, -1.f, 1.f,

					-1.f, 1.f, -1.f,
					1.f, 1.f, -1.f,
					1.f, 1.f, 1.f,
					1.f, 1.f, 1.f,
					-1.f, 1.f, 1.f,
					-1.f, 1.f, -1.f,

					-1.f, -1.f, -1.f,
					-1.f, -1.f, 1.f,
					1.f, -1.f, -1.f,
					1.f, -1.f, -1.f,
					-1.f, -1.f, 1.f,
					1.f, -1.f, 1.f
			};

			static auto cube = reinterpret_cast<core::Vec3*>(box);

			struct varying_t : core::shader_varying_float<varying_t>
			{
				core::Vec4 position;
				core::Vec3 position_ls;
			};

			static struct Shader
			{
				std::shared_ptr<core::CubeMap> cube_map;
				core::Mat mvp = core::Mat::Unit();
				varying_t VS(const core::Vec3& point) const
				{
					varying_t v{};
					v.position = mvp * point.ToHomoCoord();
					v.position.z = v.position.w - 0.0001f;
					v.position_ls = point;
					return v;
				}
				core::Color FS(const varying_t& v) const
				{
					//±¾µØ¿Õ¼ä
					core::Color color = cube_map->Sample(v.position_ls);
					return color;
				}
			} shader;

			shader.cube_map = cube_map;
			if (const ICamera* camera = engine.GetMainCamera())
			{
				shader.mvp = camera->GetProjectionViewMatrix() * core::Mat::Translate(camera->GetPosition());
			}

			core::Renderer<Shader> renderer = { engine.GetCtx(), shader };
			renderer.DrawTriangles(cube, sizeof(box) / sizeof(core::Vec3));
		}
	};
}

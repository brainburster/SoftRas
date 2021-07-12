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

			static core::Vec4 box[] =
			{
				{-1.f, 1.f, -1.f},
				{-1.f, -1.f, -1.f},
				{1.f, -1.f, -1.f},
				{1.f, -1.f, -1.f},
				{1.f, 1.f, -1.f},
				{-1.f, 1.f, -1.f},

				{-1.f, -1.f, 1.f},
				{-1.f, -1.f, -1.f},
				{-1.f, 1.f, -1.f},
				{-1.f, 1.f, -1.f},
				{-1.f, 1.f, 1.f},
				{-1.f, -1.f, 1.f},

				{1.f, -1.f, -1.f},
				{1.f, -1.f, 1.f},
				{1.f, 1.f, 1.f},
				{1.f, 1.f, 1.f},
				{1.f, 1.f, -1.f},
				{1.f, -1.f, -1.f},

				{-1.f, -1.f, 1.f},
				{-1.f, 1.f, 1.f},
				{1.f, 1.f, 1.f},
				{1.f, 1.f, 1.f},
				{1.f, -1.f, 1.f},
				{-1.f, -1.f, 1.f},

				{-1.f, 1.f, -1.f},
				{1.f, 1.f, -1.f},
				{1.f, 1.f, 1.f},
				{1.f, 1.f, 1.f},
				{-1.f, 1.f, 1.f},
				{ -1.f, 1.f, -1.f},

				{-1.f, -1.f, -1.f},
				{-1.f, -1.f, 1.f},
				{1.f, -1.f, -1.f},
				{1.f, -1.f, -1.f},
				{-1.f, -1.f, 1.f},
				{1.f, -1.f, 1.f}
			};

			struct varying_t : core::shader_varying_float<varying_t>
			{
				core::Vec4 position;
				core::Vec4 position_ls;
			};

			static struct Shader
			{
				std::shared_ptr<core::CubeMap> cube_map;
				core::Mat mvp = core::Mat::Unit();
				varying_t VS(const core::Vec4& point) const
				{
					varying_t v{};
					v.position = mvp * point;
					v.position.z = v.position.w - 0.0001f;
					v.position_ls = point;
					return v;
				}
				core::Color FS(const varying_t& v) const
				{
					//���ؿռ�
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
			renderer.DrawTriangles(box, sizeof(box));
		}
	};
}

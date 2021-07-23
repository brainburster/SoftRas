#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "varying_type.hpp"

struct Shader_Shadow_Mapping
{
	core::Mat model = {};
	core::Mat mvp = {};
	core::Mat light_mat = {};
	core::Texture* tex0 = nullptr;
	core::Buffer2DView<float>* shadow_map;
	core::Vec3 light_dir;
	core::Vec3 light_color;

	Varying_Light_ws VS(const core::Model_Vertex& v) const
	{
		Varying_Light_ws varying;
		varying.position = mvp * core::Vec4{ v.position, 1.0f };
		varying.position_ws = model * core::Vec4{ v.position, 1.0f };
		varying.uv = v.uv;
		varying.normal_ws = core::Vec3(model * v.normal).Normalize();
		return varying;
	}

	core::Vec4 FS(const Varying_Light_ws& v) const
	{
		core::Vec3 base_color = { 0.8f,0.8f,0.8f };/*core::Texture::Sample(tex0, v.uv);*/
		float ndl = max(v.normal_ws.Dot(light_dir.Normalize() * -1), 0.f);
		core::Vec3 final_color = base_color * light_color * ndl;
		//计算shadow mapping
		core::Vec4 farg_pos_light_space = light_mat * core::Vec4(v.position_ws, 1.f);
		farg_pos_light_space /= farg_pos_light_space.w;
		core::Vec2 shadow_uv = farg_pos_light_space;
		shadow_uv *= 0.5f;
		shadow_uv += 0.5f;
		shadow_uv *= 512.f;
		int s_u = gmath::utility::Clamp((int)shadow_uv.x, 0, 511);
		int s_v = gmath::utility::Clamp((int)shadow_uv.y, 0, 511);
		float depth0 = -shadow_map->Get(s_u, s_v);
		float bias = ndl * 0.005f;
		float depth = farg_pos_light_space.z - bias;
		float shadow = depth < depth0 ? 1.0f : 0.03f;
		return core::Vec4(final_color * shadow, 1.f);
	}
};

class Material_Shadow_Mapping : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	core::Buffer2DView<float>* shadow_map = nullptr;
	std::shared_ptr<framework::DirectionalLight> light;

	void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override
	{
		Shader_Shadow_Mapping shader{};
		core::Renderer<Shader_Shadow_Mapping> renderer = { engine.GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.shadow_map = shadow_map;
		shader.model = entity.transform.GetModelMatrix();
		shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
		shader.light_dir = light->GetDirection();
		shader.light_color = light->GetColor();
		shader.light_mat = light->GetLightMartrix();
		renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
	}
};

class Scene_Render_Shadow_Mapping : public framework::Scene
{
private:
	std::shared_ptr<framework::MaterialEntity> cube;
	std::shared_ptr<framework::TargetCamera> camera;
	std::shared_ptr<framework::DirectionalLight> light;
	core::Context shadow_ctx;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		auto material = std::make_shared<Material_Shadow_Mapping>();
		material->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		cube = Spawn<framework::MaterialEntity>();
		cube->model = framework::GetResource<core::Model>(L"box").value();
		auto cube2 = Spawn<framework::MaterialEntity>();
		cube2->model = framework::GetResource<core::Model>(L"box").value();
		cube2->transform.position = core::Vec4{ 0.f,-5.f,0.f };
		cube2->transform.scale = core::Vec3(10.f, 0.1f, 10.f);
		auto cube3 = Spawn<framework::MaterialEntity>();
		cube3->model = framework::GetResource<core::Model>(L"box").value();
		cube3->transform.position = core::Vec4{ 1.f,2.5f,-1.f };
		cube3->transform.scale = core::Vec3(1.f, 5.f, 1.f);
		cube->material = material;
		cube2->material = material;
		cube3->material = material;
		//...
		camera = std::make_shared<framework::TargetCamera>(cube, 20.f, 90.f, 30.f);
		light = std::make_shared<framework::DirectionalLight>();
		light->color = { 1.f,1.f,1.f };
		light->transform.position = { -2.f,7.f,2.f };
		light->dirction = core::Vec3(0, 0, 0) - light->transform.position;

		shadow_ctx.Viewport(512, 512);

		material->shadow_map = &shadow_ctx.depth_buffer_view;
		material->light = light;
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		camera->HandleInput(engine);

		if (framework::IsKeyPressed<VK_CONTROL, 'R'>())
		{
			cube->transform.rotation += core::Vec3{ 1, 1, 1 }*0.01f;
		}
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return camera.get();
	}

	virtual void OnMouseMove(const framework::IRenderEngine& engine) override
	{
		return camera->OnMouseMove(engine);
	}

	virtual void OnMouseWheel(const framework::IRenderEngine& engine) override
	{
		return camera->OnMouseWheel(engine);
	}

	virtual void Update(const framework::IRenderEngine& engine) override
	{
		Scene::Update(engine);
		size_t time = engine.GetEngineState().total_time;
		cube->transform.position = core::Vec3{ (sin(time / 500.f)) * 4, 0.f, (-cos(time / 500.f)) * 4 };
		cube->transform.rotation = core::Vec3{ time / 600.f,time / 300.f,0.f };
	}

	virtual void RenderFrame(framework::IRenderEngine& engine)override
	{
		struct Shader_Shadow_Gen
		{
			core::Mat mvp = {};
			struct alignas(16) varying_t : core::shader_varying_float<varying_t>
			{
				core::Vec4 position;
			};
			varying_t VS(const core::Model_Vertex& v) const
			{
				return { {},mvp * core::Vec4{ v.position, 1.0f } };
			}

			core::Vec4 FS(const varying_t& v) const
			{
				//最后的颜色不重要只需要深度贴图
				return { 0,0,0,0 };
			}
		};
		shadow_ctx.Clear();
		for (auto& object : objects)
		{
			if (const auto* entity = dynamic_cast<framework::Entity*>(object.get()))
			{
				Shader_Shadow_Gen shader{};
				core::Renderer<Shader_Shadow_Gen> renderer = { shadow_ctx, shader };
				shader.mvp = light->GetLightMartrix() * entity->transform.GetModelMatrix();
				renderer.DrawTriangles(&entity->model->mesh[0], entity->model->mesh.size());
			}
		}

		Scene::RenderFrame(engine);
		//light->Render(engine);
	}
};

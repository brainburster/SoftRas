#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "vs_out_type.hpp"

struct ShaderShadowMapping
{
	core::Mat model = {};
	core::Mat mvp = {};
	core::Mat light_mat = {};
	core::Texture* tex0 = nullptr;
	core::Buffer2DView<float>* shadow_map;
	core::Vec4 light_vec;
	core::Vec3 light_color;

	VsOut_Light_ws VS(const core::Model_Vertex& v) const
	{
		VsOut_Light_ws vs_out;
		vs_out.position = mvp * core::Vec4{ v.position, 1.0f };
		vs_out.position_ws = model * core::Vec4{ v.position, 1.0f };
		vs_out.uv = v.uv;
		vs_out.normal_ws = core::Vec3(model * v.normal).Normalize();
		return vs_out;
	}

	core::Vec4 FS(const VsOut_Light_ws& v) const
	{
		//计算颜色
		core::Vec3 base_color = { 0.8f,0.8f,0.8f };
		core::Vec3 light_dir = 0;
		if (light_vec.w > 0.1f)
		{
			light_dir = v.position_ws - core::Vec3(light_vec);
		}
		else
		{
			light_dir = core::Vec3(light_vec);
		}
		float ndl = max(v.normal_ws.Dot(-light_dir.Normalize()), 0.f);
		core::Vec3 final_color = base_color * light_color * ndl;

		//计算shadow
		core::Vec4 farg_pos_light_space = light_mat * core::Vec4(v.position_ws, 1.f);
		float w = farg_pos_light_space.w;
		farg_pos_light_space /= w;
		core::Vec2 shadow_uv = farg_pos_light_space;
		shadow_uv *= 0.5f;
		shadow_uv += 0.5f;
		shadow_uv *= 1024.f;
		float shadow = 0.f;

		int s_u = gmath::utility::Clamp((int)shadow_uv.x, 0, 1023);
		int s_v = gmath::utility::Clamp((int)shadow_uv.y, 0, 1023);
		const float w_light = 20.f;

		float d_blocker = shadow_map->Get(s_u, s_v);
		float d_receiver = farg_pos_light_space.z;
		if (light_vec.w > 0.1f)
		{
			//如果是点光源，透视投影导致深度被压缩，需要从NDC转换回世界坐标
			d_blocker = 200.f / (1000.1f - d_blocker * (999.9f)); //这里提前知道了近远平面是0.1,1000
			d_receiver = 200.f / (1000.1f - d_receiver * (999.9f));
		}

		float w_penumbra = (d_receiver - d_blocker) * w_light / d_blocker;
		int kernal_size = gmath::utility::Clamp((int)w_penumbra, 0, 20);

		for (int j = -kernal_size / 2; j <= kernal_size / 2; ++j)
		{
			for (int i = -kernal_size / 2; i <= kernal_size / 2; ++i)
			{
				int s_u = gmath::utility::Clamp((int)shadow_uv.x + i, 0, 1023);
				int s_v = gmath::utility::Clamp((int)shadow_uv.y + j, 0, 1023);
				float depth0 = shadow_map->Get(s_u, s_v);
				float depth = farg_pos_light_space.z;
				shadow += depth < depth0 ? 1.0f : 0.f;
			}
		}

		if (kernal_size)
		{
			shadow /= kernal_size * kernal_size;
			shadow = min(shadow * 2.1f, 0.95f);
		}
		shadow += 0.03f;
		//shadow = d_blocker > d_receiver ? 1.0f : 0.0f;
		return core::Vec4(final_color * shadow, 1.f);
	}
};

class MaterialShadowMapping : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	core::Buffer2DView<float>* shadow_map = nullptr;
	framework::ILight* light = nullptr;

	void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override
	{
		ShaderShadowMapping shader{};
		core::Renderer<ShaderShadowMapping, core::RF_DEFAULT_AA> renderer = { engine.GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.shadow_map = shadow_map;
		shader.model = entity.transform.GetModelMatrix();
		shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
		if (light->GetLightCategory() == framework::ELightCategory::DirectionalLight)
		{
			shader.light_vec = light->GetDirection();
		}
		else
		{
			shader.light_vec = light->GetPosition().ToHomoCoord();
		}
		shader.light_color = light->GetColor();
		shader.light_mat = light->GetLightMartrix();
		renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
	}
};

class SceneRenderTestShadowMapping : public framework::Scene
{
private:
	std::shared_ptr<framework::MaterialEntity> cube;
	std::shared_ptr<framework::MaterialEntity> wall;
	std::shared_ptr<framework::TargetCamera> camera;
	std::shared_ptr<framework::DirectionalLight> light_d;
	std::shared_ptr<framework::PointLight> light_p;
	std::shared_ptr<framework::ILight> light;
	std::shared_ptr<MaterialShadowMapping> material;
	core::Context<core::Color> shadow_ctx;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		material = std::make_shared<MaterialShadowMapping>();
		material->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		cube = Spawn<framework::MaterialEntity>();
		cube->model = framework::GetResource<core::Model>(L"box").value();
		auto cube2 = Spawn<framework::MaterialEntity>();
		cube2->model = framework::GetResource<core::Model>(L"box").value();
		cube2->transform.position = core::Vec4{ 0.f,-5.f,0.f };
		cube2->transform.scale = core::Vec3(15.f, 0.1f, 15.f);
		auto cube3 = Spawn<framework::MaterialEntity>();
		cube3->model = framework::GetResource<core::Model>(L"box").value();
		cube3->transform.position = core::Vec4{ -1.f,1.f,-1.f };
		cube3->transform.scale = core::Vec3(1.f, 6.f, 1.f);
		auto bunny = Spawn<framework::MaterialEntity>();
		bunny->model = framework::GetResource<core::Model>(L"bunny").value();
		bunny->transform.position = core::Vec4{ 5.f,-6.f,-5.f };
		bunny->transform.scale = core::Vec3(30.f, 30.f, 30.f);
		wall = std::make_shared<framework::MaterialEntity>();
		wall->model = framework::GetResource<core::Model>(L"box").value();
		wall->transform.position = core::Vec4{ 0.f,3.f,-15.f };
		wall->transform.scale = core::Vec3(15.f, 8.f, 0.1f);

		cube->material = material;
		cube2->material = material;
		cube3->material = material;
		bunny->material = material;
		wall->material = material;

		//...
		camera = std::make_shared<framework::TargetCamera>(cube3, 50.f, 30.f, -45.f);
		//
		light_d = std::make_shared<framework::DirectionalLight>();
		light_d->color = { 1.f,1.f,1.f };
		light_d->transform.position = { -5.f,8.f,5.f };
		light_d->dirction = core::Vec3(0, 0, 0) - light_d->transform.position;
		//
		light_p = std::make_shared<framework::PointLight>();
		light_p->color = { 1.f,1.f,1.f };
		light_p->transform.rotation = core::Quat{ {1,0,0}, -45.f }.ToEulerAngles();
		light_p->transform.position = { 0.f,8.f,4.f };

		shadow_ctx.Viewport(1024, 1024);
		light = light_p;
		material->shadow_map = &shadow_ctx.depth_buffer_view;
		material->light = light.get();
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		camera->HandleInput(engine);
		if (engine.GetInputState().key_pressed['L'])
		{
			if (light.get() == light_p.get())
			{
				light = light_d;
				material->light = light.get();
			}
			else
			{
				light = light_p;
				material->light = light.get();
			}
		}
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return camera.get();
	}

	virtual void Update(const framework::IRenderEngine& engine) override
	{
		Scene::Update(engine);
		size_t time = engine.GetEngineState().total_time;
		cube->transform.position = core::Vec3{ (sin(time / 500.f)) * 4 - 1, 0.f, (-cos(time / 500.f)) * 4 - 1 };
		cube->transform.rotation = core::Vec3{ time / 600.f,time / 300.f,0.f };
	}

	virtual void RenderFrame(framework::IRenderEngine& engine)override
	{
		struct Shader_Shadow_Gen
		{
			core::Mat mvp = {};
			struct alignas(16) vs_out_t : core::vs_out_base<vs_out_t>
			{
				core::Vec4 position;
			};
			vs_out_t VS(const core::Model_Vertex& v) const
			{
				return { {},mvp * core::Vec4{ v.position, 1.0f } };
			}

			core::Vec4 FS(const vs_out_t& v) const
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
				constexpr size_t flag = core::RF_DEFAULT & ~core::RF_CULL_BACK | core::RF_CULL_FRONT;
				core::Renderer<Shader_Shadow_Gen, flag> renderer = { shadow_ctx, shader };
				shader.mvp = light->GetLightMartrix() * entity->transform.GetModelMatrix();
				renderer.DrawTriangles(&entity->model->mesh[0], entity->model->mesh.size());
			}
		}

		Scene::RenderFrame(engine);

		if (light.get() == light_p.get())
		{
			wall->Render(engine);
			light_p->Render(engine);
		}
		else
		{
			light_d->Render(engine);
		}
	}
};

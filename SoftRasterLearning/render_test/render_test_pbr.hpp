#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "varying_type.hpp"

class Material_PBR : public framework::IMaterial
{
public:
	std::vector<std::shared_ptr<framework::ILight>> lights;
	core::Vec3 albedo;
	float metallic = 0;
	float roughness = 0;

	virtual void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override;
};

//世界空间 blinn_phong 着色器
struct Shader_PBR
{
	Material_PBR* material = nullptr;
	core::Mat mvp = {};
	core::Mat model = {};

	Varying_Light_ws VS(const core::Model_Vertex& v) const
	{
		Varying_Light_ws varying{};
		varying.position = mvp * core::Vec4{ v.position, 1.f };
		varying.position_ws = model * core::Vec4{ v.position, 1.f };
		varying.normal_ws = model * core::Vec4{ v.normal, 0.f };
		varying.uv = v.uv;
		//...
		return varying;
	}

	core::Vec4 FS(const Varying_Light_ws& v) const
	{
		using namespace core;
		return Vec4{ material->metallic,material->roughness,1.f, 1.f };
	}
};

inline void Material_PBR::Render(const framework::Entity& entity, framework::IRenderEngine& engine)
{
	//准备shader数据
	Shader_PBR shader{ this };
	shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
	shader.model = entity.transform.GetModelMatrix();
	//渲染
	core::Renderer<Shader_PBR> renderer = { engine.GetCtx(), shader };
	renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
}

class Scene_Render_PBR : public framework::Scene
{
private:
	std::vector<std::shared_ptr<framework::MaterialEntity>> spheres;
	std::vector<std::shared_ptr<framework::Object>> lights;
	std::shared_ptr<framework::TargetCamera> camera;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		//创建光源
		auto light0 = std::make_shared<framework::PointLight>();
		light0->transform.position = { -1.f,16.f,3.f };
		light0->color = { 2.4f,2.4f,1.6f };
		auto light1 = std::make_shared<framework::PointLight>();
		light1->transform.position = { 8.f,16.f,8.f };
		light1->color = { 1.4f,2.0f,2.6f };
		auto light2 = std::make_shared<framework::PointLight>();
		light2->transform.position = { 0.f,-5.f,3.f };
		light2->color = { 2.0f,2.6f,1.2f };
		auto light3 = std::make_shared<framework::DirectionalLight>();
		light3->transform.position = { 0.f,16.f,16.f };
		light3->dirction = { 0,0.5f,0.5f };
		light3->color = { 2.0f,2.6f,1.2f };

		//创建8x8个球体，x轴roughness增大,y轴metallic增大
		objects.reserve(32);
		spheres.reserve(25);
		for (size_t j = 0; j < 5; j++)
		{
			for (size_t i = 0; i < 5; i++)
			{
				auto material = std::make_shared<Material_PBR>();
				material->albedo = { 0.8f,0.8f,0.8f };
				material->metallic = j / 4.f;
				material->roughness = i / 4.f;

				auto sphere = Spawn<framework::MaterialEntity>();
				sphere->transform.position = { i * 2.4f,j * 2.4f,0 };
				sphere->model = framework::GetResource<core::Model>(L"sphere").value();
				sphere->material = material;

				material->lights.reserve(4);
				material->lights.push_back(light0);
				material->lights.push_back(light1);
				material->lights.push_back(light2);
				material->lights.push_back(light3);
				spheres.push_back(sphere);
			}
		}

		//创建摄像机
		camera = std::make_shared<framework::TargetCamera>(spheres[2 + 2 * 5], 30.f);
		//..
		lights.push_back(light0);
		lights.push_back(light1);
		lights.push_back(light2);
		lights.push_back(light3);
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		camera->HandleInput(engine);
		//if (framework::IsKeyPressed<VK_CONTROL, 'R'>())
		//{
		//	sphere->transform.rotation += core::Vec3{ 0, 0, 1 }*0.05f;
		//}
		//if (framework::IsKeyPressed<VK_CONTROL, 'F'>())
		//{
		//	static size_t count = 0;
		//	sphere->transform.position = core::Vec3{ (cos(count / 500.f) - 1) * 2,0,  (sin(count / 500.f) - 1) * 2 };
		//	count += engine.GetEngineState().delta_count;
		//}
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return camera.get();
	}

	//virtual void Update(const framework::IRenderEngine& engine) override
	//{
	//	size_t count = engine.GetEngineState().frame_count;
	//	light->transform.position = core::Vec3{ (sin(count / 50.f)) * 2, 0.f, (-cos(count / 50.f)) * 2 };
	//}

	virtual void OnMouseMove(const framework::IRenderEngine& engine) override
	{
		return camera->OnMouseMove(engine);
	}

	virtual void OnMouseWheel(const framework::IRenderEngine& engine) override
	{
		return camera->OnMouseWheel(engine);
	}

	virtual void RenderFrame(framework::IRenderEngine& engine)override
	{
		Scene::RenderFrame(engine);

		//画家算法对光源（透明物体进行排序）
		std::sort(lights.begin(), lights.end(), [&](auto l1, auto l2) {
			//简单排序，通过摄像机的距离
			//auto c_p = camera->GetPosition();
			//auto d1 = (l1->transform.position - c_p).Length();
			//auto d2 = (l2->transform.position - c_p).Length();
			//return d1 > d2;

			//稍微复杂一点，转换到NDC空间
			auto vp = camera->GetProjectionViewMatrix();
			auto p1 = vp * l1->transform.position.ToHomoCoord();
			auto p2 = vp * l2->transform.position.ToHomoCoord();
			return p1.z / p1.w > p2.z / p2.w;
			});

		for (auto& light : lights)
		{
			light->Render(engine);
		}
	}
};

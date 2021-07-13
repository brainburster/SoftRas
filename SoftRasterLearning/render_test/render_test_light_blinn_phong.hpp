#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "varying_type.hpp"

//世界空间 blinn_phong 着色器
class Shader_Blinn_Phong
{
public:
	core::Mat mvp = core::Mat::Unit();
	core::Mat m = core::Mat::Unit();
	core::Texture* tex0 = nullptr;
	core::Vec3 light_position_ws = { 0,10,0 };
	core::Vec3 light_color = { 2,2,2 };
	core::Vec3 camera_position_ws = { 0,0,5 };

	Varying_Light_ws VS(const core::Model_Vertex& v) const
	{
		Varying_Light_ws varying{};
		varying.position = mvp * v.position.ToHomoCoord();
		varying.position_ws = m * v.position.ToHomoCoord();
		varying.uv = v.uv;
		varying.normal_ws = m.ToMat3x3() * v.normal;
		return varying;
	}

	core::Vec4 FS(const Varying_Light_ws& v) const
	{
		using namespace core;
		Vec3 L = (light_position_ws - v.position_ws).normalize();
		Vec3 V = (camera_position_ws - v.position_ws).normalize();
		Vec3 H = (L + V).normalize();
		Vec3 N = v.normal_ws.normalize();
		Vec3 base_color = Texture::Sample(tex0, v.uv);
		Vec3 Ks = Vec3(0.3f, 0.3f, 0.3f);
		Vec3 ambient = Vec3(0.01f, 0.012f, 0.01f);

		Vec3 diffuse = base_color * light_color * max(N.Dot(L), 0);
		Vec3 specular = Ks * light_color * pow(max(N.Dot(H), 0), 32.f);
		Vec3 color = ambient + diffuse / pi + ((32 + 8) / (8 * pi)) * specular;

		return Vec4{ color, 1.f };
	}
};

class Material_Blinn_Phong : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;

	void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override
	{
		Shader_Blinn_Phong shader{};
		core::Renderer<Shader_Blinn_Phong> renderer = { engine.GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
		shader.m = entity.transform.GetModelMatrix();
		shader.light_position_ws = core::Vec3{ -1.f,2.f,3.f };//engine->GetCamera().GetPosition();
		shader.camera_position_ws = engine.GetMainCamera()->GetPosition();

		renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
	}
};

class Scene_Render_Test_Test_Blinn_Phong : public framework::Scene
{
private:
	std::shared_ptr<framework::MaterialEntity> sphere;
	std::shared_ptr<framework::TargetCamera> camera;

public:
	void Init(framework::IRenderEngine& engine) override
	{
		auto material_blinn_phong = std::make_shared<Material_Blinn_Phong>();
		material_blinn_phong->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		//auto skybox = Spawn<framework::Skybox>();
		//skybox->cube_map = framework::GetResource<core::CubeMap>(L"cube_map").value();
		sphere = Spawn<framework::MaterialEntity>();
		sphere->model = framework::GetResource<core::Model>(L"sphere").value();
		sphere->material = material_blinn_phong;
		camera = std::make_shared<framework::TargetCamera>(sphere);
		//..
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		camera->HandleInput(engine);
		if (framework::IsKeyPressed<VK_CONTROL, 'R'>())
		{
			sphere->transform.rotation += core::Vec3{ 0, 0, 1 }*0.05f;
		}
		if (framework::IsKeyPressed<VK_CONTROL, 'F'>())
		{
			static size_t count = 0;
			sphere->transform.position = core::Vec3{ (cos(count / 500.f) - 1) * 2,0,  (sin(count / 500.f) - 1) * 2 };
			count += engine.GetEngineState().delta_count;
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
};

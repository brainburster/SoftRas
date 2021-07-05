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

	Varying_Light VS(const core::Model_Vertex& v) const
	{
		return core::CreateVarying<Varying_Light>(
			mvp * v.position,
			v.position,
			m * v.position,
			v.uv,
			v.normal,
			m * core::Vec4(v.normal, 0.0f)
			);
	}

	core::Vec4 FS(const Varying_Light& v) const
	{
		using namespace core;
		Vec3 L = (light_position_ws - v.position_ws).normalize();
		Vec3 V = (camera_position_ws - v.position_ws).normalize();
		Vec3 H = (L + V).normalize();
		Vec3 N = v.normal_ws.normalize();
		//Vec3 Ka = Vec3(1, 1, 1);
		Vec3 Kd = Texture::Sampler(tex0, v.uv);
		Vec3 Ks = Vec3(0.3f, 0.3f, 0.3f);
		Vec3 ambient = Vec3(0.01f, 0.01f, 0.01f);
		Vec3 diffuse = Kd * light_color * max(N.Dot(L), 0);
		Vec3 specular = Ks * light_color * (float)pow(max(N.Dot(H), 0), 32);
		Vec3 color = ambient + diffuse / core::pi + ((32 + 8) / (8 * core::pi)) * specular;
		return Vec4{ color, 1.f };
	}
};

class Material_Blinn_Phong : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	void Render(const framework::Entity* entity, framework::IRenderEngine* engine) override
	{
		Shader_Blinn_Phong shader{};
		core::Renderer<Shader_Blinn_Phong> renderer = { engine->GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.mvp = engine->GetCamera().GetProjectionViewMatrix() * entity->transform.GetModelMatrix();
		shader.m = entity->transform.GetModelMatrix();
		shader.light_position_ws = core::Vec3{ -1.f,2.f,3.f };//engine->GetCamera().GetPosition();
		shader.camera_position_ws = engine->GetCamera().GetPosition();

		renderer.DrawTriangles(&entity->model->mesh[0], entity->model->mesh.size());
	}
};

class RenderTest_Blinn_Phong final : public framework::FPSRenderAPP
{
private:
	std::shared_ptr<framework::MaterialEntity> sphere;

public:
	RenderTest_Blinn_Phong(HINSTANCE hinst) : FPSRenderAPP{ hinst } {}

protected:

	void Init() override
	{
		SoftRasterApp::Init();
		auto _tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto _sphere = loader::obj::LoadFromFile(L".\\resource\\models\\sphere.obj");

		framework::SetResource(L"sphere", std::make_shared<core::Model>(std::move(_sphere.value())));
		framework::SetResource(L"tex0", std::make_shared<core::Texture>(std::move(_tex.value())));

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5.f }, -90.f);
		sphere = world.Spawn<framework::MaterialEntity>();
		sphere->model = framework::GetResource<core::Model>(L"sphere").value();
		auto material_blinn_phong = std::make_shared<Material_Blinn_Phong>();
		material_blinn_phong->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		sphere->material = material_blinn_phong;
		//...
	}

	void HandleInput() override
	{
		FPSRenderAPP::HandleInput();

		if (IsKeyPressed<VK_CONTROL, 'R'>())
		{
			sphere->transform.rotation += core::Vec3{ 0, 0, 1 }*0.05f;
		}
		if (IsKeyPressed<VK_CONTROL, 'F'>())
		{
			static size_t count = 0;
			sphere->transform.position = core::Vec3{ (cos(count / 500.f) - 1) * 2,0,  (sin(count / 500.f) - 1) * 2 };
			count += app_state.delta_count;
		}
	}
};

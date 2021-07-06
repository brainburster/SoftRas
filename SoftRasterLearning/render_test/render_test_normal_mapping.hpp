#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "varying_type.hpp"

class Shader_Normal
{
public:
	core::Mat mvp = core::Mat::Unit();
	core::Mat m = core::Mat::Unit();
	core::Texture* tex0 = nullptr;
	core::Texture* normal_map = nullptr;
	core::Vec3 light_position_ws = { 0,10,0 };
	core::Vec3 light_color = { 1.5f,2.f,2.f };
	core::Vec3 camera_position_ws = { 0,0,5 };

	Varying_Light_ts VS(const core::Model_Vertex& v) const
	{
		using namespace core;

		//暂时就不求法线矩阵了(因为要求逆)，用model矩阵代替
		Vec3 tangent = (m.ToMat3x3() * v.tangent).normalize();
		Vec3 normal = (m.ToMat3x3() * v.normal).normalize();
		Vec3 bitangent = (m.ToMat3x3() * normal.cross(tangent).normalize()).normalize();

		gmath::Mat3x3 TBN = { tangent, bitangent, normal };
		gmath::Mat3x3 TBN_inverse = TBN.transpose();

		Varying_Light_ts varying{};
		varying.position = mvp * v.position.ToHomoCoord();
		Vec3 position_ws = m * v.position.ToHomoCoord();
		varying.view_dir_ts = (TBN_inverse * (camera_position_ws - position_ws)).normalize();
		varying.light_dir_ts = (TBN_inverse * (light_position_ws - position_ws)).normalize();
		varying.uv = v.uv;

		return varying;
	}

	core::Vec4 FS(const Varying_Light_ts& v) const
	{
		using namespace core;

		Vec3 N = (Texture::Sampler(normal_map, v.uv) * 2 - 1.f).normalize(); //切线空间法线
		Vec3 base_color = Texture::Sampler(tex0, v.uv);

		Vec3 L = v.view_dir_ts;
		Vec3 V = v.light_dir_ts;
		Vec3 H = (L + V).normalize();

		float k_diffuse = 1 / pi;
		float k_specular = (128 + 8) / (8 * pi);

		Vec3 ambient = Vec3(0.01f, 0.01f, 0.01f) * base_color;
		Vec3 diffuse = base_color * light_color * max(N.Dot(L), 0);
		Vec3 specular = 0.1f * light_color * pow(max(N.Dot(H), 0), 128.f);
		Vec3 final_color = ambient + k_diffuse * diffuse + k_specular * specular;

		return Vec4{ final_color, 1.f };
	}
};

class Material_Normal : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	std::shared_ptr<core::Texture> normal_map;

	void Render(const framework::Entity* entity, framework::IRenderEngine* engine) override
	{
		Shader_Normal shader{};
		core::Renderer<Shader_Normal> renderer = { engine->GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.normal_map = normal_map.get();
		shader.mvp = engine->GetCamera().GetProjectionViewMatrix() * entity->transform.GetModelMatrix();
		shader.m = entity->transform.GetModelMatrix();
		shader.light_position_ws = core::Vec3{ -1.f,2.f,3.f };//engine->GetCamera().GetPosition();
		shader.camera_position_ws = engine->GetCamera().GetPosition();

		renderer.DrawTriangles(&entity->model->mesh[0], entity->model->mesh.size());
	}
};

class RenderTest_Normal final : public framework::FPSRenderAPP
{
private:
	std::shared_ptr<framework::MaterialEntity> sphere;

public:
	RenderTest_Normal(HINSTANCE hinst) : FPSRenderAPP{ hinst } {}

protected:

	void Init() override
	{
		SoftRasterApp::Init();
		auto _tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto _normal_map = loader::bmp::LoadFromFile(L".\\resource\\pictures\\normal.bmp", false);
		auto _sphere = loader::obj::LoadFromFile(L".\\resource\\models\\sphere.obj");

		framework::SetResource(L"sphere", std::make_shared<core::Model>(std::move(_sphere.value())));
		framework::SetResource(L"tex0", std::make_shared<core::Texture>(std::move(_tex.value())));
		framework::SetResource(L"normal_map", std::make_shared<core::Texture>(std::move(_normal_map.value())));

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5.f }, -90.f);
		sphere = world.Spawn<framework::MaterialEntity>();
		sphere->model = framework::GetResource<core::Model>(L"sphere").value();
		auto material_blinn_phong = std::make_shared<Material_Normal>();
		material_blinn_phong->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		material_blinn_phong->normal_map = framework::GetResource<core::Texture>(L"normal_map").value();
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

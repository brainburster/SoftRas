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
	core::Vec3 light_position_ws = { 0,10.f,0 };
	core::Vec3 light_color = { 2.f,2.f,2.f };
	core::Vec3 camera_position_ws = { 0,0,5.f };

	Varying_Light_ts VS(const core::Model_Vertex& v) const
	{
		using namespace core;

		//求法线矩阵
		Mat3 normal_mat = m.ToMat3x3();//m.ToMat3x3().inverse().transpose();
		//求tbn
		Vec3 tangent = (normal_mat * v.tangent).normalize();
		Vec3 normal = (normal_mat * v.normal).normalize();
		Vec3 bitangent = v.normal.cross(v.tangent).normalize();
		bitangent = (normal_mat * bitangent).normalize();

		gmath::Mat3x3 TBN = { tangent, bitangent, normal }; //由于我的矩阵是行主序，所以不用转置
		//TBN = TBN.transpose();

		Varying_Light_ts varying{};
		varying.position = mvp * v.position.ToHomoCoord();
		Vec3 position_ws = m * v.position.ToHomoCoord();

		Vec3 view_dir = (camera_position_ws - position_ws).normalize();
		Vec3 light_dir = (light_position_ws - position_ws).normalize();
		Vec3 half_dir = (view_dir + light_dir).normalize();

		varying.view_dir_ts = (TBN * view_dir).normalize();
		varying.light_dir_ts = (TBN * light_dir).normalize();
		varying.half_dir_ts = (TBN * half_dir).normalize();

		varying.uv = v.uv;

		return varying;
	}

	core::Vec4 FS(const Varying_Light_ts& v) const
	{
		using namespace core;

		Vec3 N = (Texture::Sampler(normal_map, v.uv) * 2 - 1.f).normalize(); //切线空间法线
		Vec3 base_color = Texture::Sampler(tex0, v.uv);

		Vec3 L = v.light_dir_ts;
		Vec3 V = v.view_dir_ts;
		Vec3 H = v.half_dir_ts;

		float k_diffuse = 1 / pi;
		float k_specular = (128 + 8) / (8 * pi);

		Vec3 ambient = Vec3(0.01f, 0.01f, 0.01f) * base_color;
		Vec3 diffuse = base_color * light_color * max(N.Dot(L), 0);
		Vec3 specular = 0.3f * light_color * pow(max(N.Dot(H), 0), 128.f);
		Vec3 final_color = ambient + k_diffuse * diffuse + k_specular * specular;

		return Vec4{ final_color, 1.f };
	}
};

class Material_Normal : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	std::shared_ptr<core::Texture> normal_map;
	std::shared_ptr<framework::ICamera> camera;

	void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override
	{
		Shader_Normal shader{};
		core::Renderer<Shader_Normal> renderer = { engine.GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.normal_map = normal_map.get();
		shader.mvp = camera->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
		shader.m = entity.transform.GetModelMatrix();
		shader.light_position_ws = core::Vec3{ -1.f,2.f,3.f };//engine->GetCamera().GetPosition();
		shader.camera_position_ws = camera->GetPosition();

		renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
	}
};

class Scene_Render_Test_Normal : public framework::Scene
{
private:
	std::shared_ptr<framework::MaterialEntity> sphere;
	std::shared_ptr<framework::FPSCamera> fps_camera;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		fps_camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5.f }, -90.f);
		auto material_normal = std::make_shared<Material_Normal>();
		material_normal->tex0 = framework::GetResource<core::Texture>(L"normal_map").value();
		material_normal->camera = fps_camera;
		material_normal->normal_map = framework::GetResource<core::Texture>(L"normal_map").value();
		sphere = Spawn<framework::MaterialEntity>();
		sphere->model = framework::GetResource<core::Model>(L"sphere").value();
		sphere->material = material_normal;
		//...
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		fps_camera->HandleInput(engine);
		if (framework::IsKeyPressed<VK_CONTROL, 'R'>())
		{
			sphere->transform.rotation += core::Vec3{ 0, 1, 0 }*0.05f;
		}
		if (framework::IsKeyPressed<VK_CONTROL, 'F'>())
		{
			static size_t count = 0;
			sphere->transform.position = core::Vec3{ (cos(count / 500.f) - 1) * 2,0,  (sin(count / 500.f) - 1) * 2 };
			count += engine.GetEngineState().delta_count;
		}
	}
};

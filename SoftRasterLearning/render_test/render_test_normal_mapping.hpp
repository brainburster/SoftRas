#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "vs_out_type.hpp"


class Shader_Normal
{
public:
	core::Mat mvp = {};
	core::Mat model = {};
	core::Texture* tex0 = nullptr;
	core::Texture* normal_map = nullptr;
	core::Vec3 light_position_ws = { 0,10.f,0 };
	core::Vec3 light_color = { 2.f,2.f,2.f };
	core::Vec3 camera_position_ws = { 0,0,5.f };

	VsOut_Light_ts VS(const core::Model_Vertex& v) const
	{
		using namespace core;

		//求法线矩阵
		Mat3 normal_mat = model.ToMat3x3().Inverse().Transpose();
		//求tbn
		Vec3 tangent = (normal_mat * v.tangent).Normalize();
		Vec3 normal = (normal_mat * v.normal).Normalize();
		//tangent = (tangent - tangent.Dot(normal) * normal).normalize();
		Vec3 bitangent = tangent.Cross(normal).Normalize();

		Mat3 TBN = { tangent, bitangent, normal };
		TBN = TBN.Transpose();

		VsOut_Light_ts vs_out{};
		vs_out.position = mvp * v.position.ToHomoCoord();
		Vec3 position_ws = model * v.position.ToHomoCoord();

		Vec3 view_dir = (camera_position_ws - position_ws).Normalize();
		Vec3 light_dir = (light_position_ws - position_ws).Normalize();
		Vec3 half_dir = (view_dir + light_dir).Normalize();

		vs_out.view_dir_ts = (TBN * view_dir).Normalize();
		vs_out.light_dir_ts = (TBN * light_dir).Normalize();
		vs_out.half_dir_ts = (TBN * half_dir).Normalize();

		vs_out.uv = v.uv;

		return vs_out;
	}

	core::Vec4 FS(const VsOut_Light_ts& v) const
	{
		using namespace core;

		Vec3 N = Vec3(Texture::Sample(normal_map, v.uv) * 2 - 1.f).Normalize(); //切线空间法线
		Vec3 albedo = Vec3{ 0.91f };//Texture::Sample(tex0, v.uv);

		//得到切线空间中的L、V、H
		//经过线性插值之后原先是单位向量的，不一定还是
		Vec3 L = v.light_dir_ts.Normalize();
		Vec3 V = v.view_dir_ts.Normalize();
		Vec3 H = v.half_dir_ts.Normalize();

		//float k_diffuse = 1 / pi;
		//float k_specular = (32 + 8) / (8 * pi);

		//Vec3 ambient = Vec3{ 0.01f, 0.01f, 0.01f } *base_color;
		//Vec3 diffuse = base_color * light_color * max(N.Dot(L), 0);
		//Vec3 specular = 0.3f * light_color * pow(max(N.Dot(H), 0), 32.f);
		//Vec3 final_color = ambient + k_diffuse * diffuse + k_specular * specular;

		float NdotV = max(N.Dot(V), 0.0f);
		float NdotL = max(N.Dot(L), 0.0f);
		float NdotH = max(N.Dot(H), 0.0f);
		float metalness = 0.0f;
		float roughness = 0.35f;

		Vec3 F0 = pbr::GetF0(albedo, metalness);
		Vec3 F = pbr::FresnelSchlickRoughness(F0, NdotV, roughness);
		float D = pbr::DistributionGGX(NdotH, roughness);
		float G = pbr::GeometrySmith(NdotV, NdotL, roughness);
		Vec3 specular = pbr::SpecularCooKTorrance(D, F, G, NdotV, NdotL);
		auto Ks = pbr::FresnelSchlick(F0, NdotV);
		Vec3 Kd = (Vec3(1.f) - Ks) * (1.f - metalness);
		Vec3 final_color = ((Kd * albedo) / core::pi + specular) * light_color * NdotL;

		return Vec4{ final_color, 1.f };
	}
};

class Material_Normal : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	std::shared_ptr<core::Texture> normal_map;

	void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override
	{
		Shader_Normal shader{};
		core::Renderer<Shader_Normal> renderer = { engine.GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.normal_map = normal_map.get();
		shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
		shader.model = entity.transform.GetModelMatrix();
		shader.light_position_ws = core::Vec3{ 0.f,2.f,3.f };//engine->GetCamera().GetPosition();
		shader.camera_position_ws = engine.GetMainCamera()->GetPosition();

		renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
	}
};

class Scene_Render_Test_Test_Normal : public framework::Scene
{
private:
	std::shared_ptr<framework::MaterialEntity> sphere;
	std::shared_ptr<framework::TargetCamera> camera;

public:
	void Init(framework::IRenderEngine& engine) override
	{
		auto material_normal = std::make_shared<Material_Normal>();
		material_normal->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		material_normal->normal_map = framework::GetResource<core::Texture>(L"normal_map").value();
		sphere = Spawn<framework::MaterialEntity>();
		sphere->model = framework::GetResource<core::Model>(L"sphere").value();
		//sphere->model = framework::GetResource<core::Model>(L"box").value();
		sphere->material = material_normal;

		camera = std::make_shared<framework::TargetCamera>(sphere);
		//...
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		camera->HandleInput(engine);
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

	virtual void Update(const framework::IRenderEngine& engine) override
	{
		size_t delta = engine.GetEngineState().delta_count;
		sphere->transform.rotation += core::Vec3{ 0.f, 1.f, 0.f }*0.001f * (float)delta;
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return camera.get();
	}
};

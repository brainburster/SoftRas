#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "vs_out_type.hpp"


//
class Material_PBR : public framework::IMaterial
{
public:
	std::shared_ptr<core::pbr::IBL> ibl;
	std::vector<std::shared_ptr<framework::ILight>> lights;
	core::Vec3 albedo;
	float metalness = 0;
	float roughness = 0;
	bool b_enable_light = false;
	bool b_enable_ibl = true;
	virtual void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override;
};

struct Shader_PBR
{
	Material_PBR* material = nullptr;
	core::Vec3 cam_pos_ws = {};
	core::Mat mvp = {};
	core::Mat model = {};

	VsOut_Light_ws VS(const core::Model_Vertex& v) const
	{
		VsOut_Light_ws vs_out{};
		vs_out.position = mvp * v.position.ToHomoCoord();
		vs_out.position_ws = model * v.position.ToHomoCoord();
		vs_out.normal_ws = core::Vec3(model * v.normal).Normalize();
		vs_out.uv = v.uv;
		//...
		return vs_out;
	}

	core::Vec4 FS(const VsOut_Light_ws& v) const
	{
		using namespace core;
		Vec3 albedo = material->albedo;
		float metalness = material->metalness;
		float roughness = material->roughness;
		auto& IBL = material->ibl;
		Vec3 N = v.normal_ws.Normalize(); //插值之后不一定是归一化的
		Vec3 V = cam_pos_ws - v.position_ws;
		float d_cam = V.Length();
		V = V.Normalize();
		float NdotV = max(N.Dot(V), 0.0f);
		Vec3 Lo = 0;
		if (material->b_enable_light)
		{
			for (const auto& light : material->lights)
			{
				Vec3 L = 0;
				if (light->GetLightCategory() == framework::ELightCategory::DirectionalLight)
				{
					L = light->GetDirection();
				}
				else
				{
					L = light->GetPosition() - v.position_ws;
				}

				L = L.Normalize();
				Vec3 H = V + L;
				H = H.Normalize();

				float distance = (light->GetPosition() - v.position_ws).Length();
				float attenuation = (light->GetLightCategory() == framework::ELightCategory::DirectionalLight) ? (1.0f) : (1.0f / (distance * distance));
				Vec3 radiance = light->GetColor() * attenuation; //入射的radiance

				float NdotL = max(N.Dot(L), 0.0f);
				float NdotH = max(N.Dot(H), 0.0f);

				Vec3 F0 = pbr::GetF0(albedo, metalness);
				Vec3 F = pbr::FresnelSchlickRoughness(F0, NdotV, roughness);
				float D = pbr::DistributionGGX(NdotH, roughness);
				float G = pbr::GeometrySmith(NdotV, NdotL, roughness);
				Vec3 specular = pbr::SpecularCooKTorrance(D, F, G, NdotV, NdotL);
				auto Ks = pbr::FresnelSchlick(F0, NdotV);
				Vec3 Kd = (Vec3(1.f) - Ks) * (1.f - metalness);
				Lo += ((Kd * albedo) / core::pi + specular) * radiance * NdotL;
			}
		}

		core::Vec3 ambient = { 0.01f };
		//计算环境光
		if (IBL && material->b_enable_ibl)
		{
			Vec3 F0 = pbr::GetF0(albedo, metalness);
			Vec3 F = pbr::FresnelSchlickRoughness(F0, NdotV, roughness);
			Vec3 Ks = pbr::FresnelSchlick(F0, NdotV);
			Vec3 Kd = 1.0f - Ks;
			Kd *= 1.0f - metalness;
			Vec3 irradiance = IBL->diffuse_map->Sample(N);
			Vec3 diffuse = irradiance * albedo;
			Vec3 R = (-V).Reflect(N).Normalize();
			using gmath::utility::Clamp;
			float u = roughness * 4.f;
			size_t lod = size_t(u);
			u = u - lod;
			Vec3 prefilteredColor = IBL->specular_maps[lod]->Sample(R);
			if (u > 0.05f)
			{
				size_t lod_1 = Clamp(lod + 1, 0, 4);
				Vec3 prefilteredColor1 = IBL->specular_maps[lod_1]->Sample(R);
				prefilteredColor = prefilteredColor * (1 - u) + prefilteredColor1 * u;
			}

			Vec3 envBRDF = core::Texture::Sample(IBL->brdf_map.get(), { NdotV,roughness });
			Vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
			ambient = Kd * diffuse + specular;
		}

		return Vec4{ Lo + ambient, 1.f };
	}
};

inline void Material_PBR::Render(const framework::Entity& entity, framework::IRenderEngine& engine)
{
	//准备shader数据
	Shader_PBR shader{ this };
	shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
	shader.model = entity.transform.GetModelMatrix();
	shader.cam_pos_ws = engine.GetMainCamera()->GetPosition();
	//渲染
	core::Renderer<Shader_PBR> renderer = { engine.GetCtx(), shader };
	renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
}

class SceneRenderTestPBR : public framework::Scene
{
private:
	std::vector<std::shared_ptr<framework::MaterialEntity>> spheres;
	std::vector<std::shared_ptr<framework::Object>> lights;
	std::shared_ptr<framework::ICamera> camera;
	std::shared_ptr<framework::FPSCamera> fps_camera;
	std::shared_ptr<framework::TargetCamera> target_camera;
	std::shared_ptr<framework::Skybox> skybox;
	std::shared_ptr<core::pbr::IBL> ibl;
	bool b_show_light_icon = false;
	bool b_show_skybox = true;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		//创建光源
		auto light0 = std::make_shared<framework::PointLight>();
		light0->transform.position = { -8.f,16.f,-8.f };
		light0->color = 16.0f;
		auto light1 = std::make_shared<framework::PointLight>();
		light1->transform.position = { 16.f,16.f,8.f };
		light1->color = 16.0f;
		auto light2 = std::make_shared<framework::PointLight>();
		light2->transform.position = { 16.f,-10.f,8.f };
		light2->color = 16.0f;
		auto light3 = std::make_shared<framework::DirectionalLight>();
		light3->transform.position = { 8.f,16.f,16.f };
		light3->dirction = { 0,0.5f,0.5f };
		light3->color = 0.4f;

		//创建3x7个球体，x轴roughness增大,y轴metallic增大
		objects.reserve(32);
		spheres.reserve(25);
		for (size_t j = 0; j < 3; j++)
		{
			for (size_t i = 0; i < 7; i++)
			{
				auto material = std::make_shared<Material_PBR>();
				material->albedo = { 0.91f,0.92f,0.92f };
				material->metalness = j / 2.f;
				material->roughness = i / 6.f;
				material->ibl = framework::GetResource<core::pbr::IBL>(L"env_map").value();
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
		target_camera = std::make_shared<framework::TargetCamera>(spheres[3ULL + 1ULL * 7], 30.f, 0.f, 0.1f);
		fps_camera = std::make_shared <framework::FPSCamera>();
		camera = target_camera;
		skybox = std::make_shared<framework::Skybox>();
		skybox->cube_map = framework::GetResource<core::pbr::IBL>(L"env_map").value()->diffuse_map;
		//..
		lights.reserve(4);
		lights.push_back(light0);
		lights.push_back(light1);
		lights.push_back(light2);
		lights.push_back(light3);
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		camera->HandleInput(engine);
		if (engine.GetInputState().key_pressed['L'])
		{
			for (auto& sphere : spheres)
			{
				auto* material = static_cast<Material_PBR*>(sphere->material.get());
				material->b_enable_light = !material->b_enable_light;
				b_show_light_icon = !b_show_light_icon;
			}
		}
		if (engine.GetInputState().key_pressed['I'] || engine.GetInputState().key_pressed['B'])
		{
			for (auto& sphere : spheres)
			{
				auto* material = static_cast<Material_PBR*>(sphere->material.get());
				material->b_enable_ibl = !material->b_enable_ibl;
				b_show_skybox = !b_show_skybox;
			}
		}
		if (engine.GetInputState().key_pressed['1'])
		{
			skybox->cube_map = framework::GetResource<core::pbr::IBL>(L"env_map").value()->diffuse_map;
		}
		else if (engine.GetInputState().key_pressed['2'])
		{
			skybox->cube_map = framework::GetResource<core::pbr::IBL>(L"env_map").value()->specular_maps[0];
		}
		else if (engine.GetInputState().key_pressed['3'])
		{
			skybox->cube_map = framework::GetResource<core::pbr::IBL>(L"env_map").value()->specular_maps[1];
		}
		else if (engine.GetInputState().key_pressed['4'])
		{
			skybox->cube_map = framework::GetResource<core::pbr::IBL>(L"env_map").value()->specular_maps[2];
		}
		else if (engine.GetInputState().key_pressed['5'])
		{
			skybox->cube_map = framework::GetResource<core::pbr::IBL>(L"env_map").value()->specular_maps[3];
		}
		else if (engine.GetInputState().key_pressed[VK_OEM_3] || engine.GetInputState().key_pressed['6'])
		{
			skybox->cube_map = framework::GetResource<core::CubeMap>(L"cube_map").value();
		}
		if (engine.GetInputState().key_pressed['P'] || engine.GetInputState().key_pressed['F'])
		{
			if (camera == target_camera)
			{
				fps_camera->SetPosition(target_camera->GetPosition());
				fps_camera->SetYaw(target_camera->GetYaw());
				fps_camera->SetPitch(target_camera->GetPitch());

				camera = fps_camera;
			}
			else
			{
				camera = target_camera;
			}
		}
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return camera.get();
	}

	virtual void Update(const framework::IRenderEngine& engine) override
	{
		size_t count = engine.GetEngineState().frame_count;
	}

	virtual void RenderFrame(framework::IRenderEngine& engine)override
	{
		Scene::RenderFrame(engine);

		if (b_show_skybox)
		{
			skybox->Render(engine);
		}

		if (b_show_light_icon)
		{
			//画家算法对光源（透明物体进行排序）
			std::sort(lights.begin(), lights.end(), [&](auto l1, auto l2) {
				//转换到NDC空间
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
	}
};

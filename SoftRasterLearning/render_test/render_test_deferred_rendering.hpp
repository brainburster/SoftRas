#pragma once

#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "vs_out_type.hpp"

class MaterialDrPBR : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> normal_map = nullptr;
	std::shared_ptr<core::pbr::IBL> ibl;
	core::Vec3 albedo;
	float metalness = 0;
	float roughness = 0;
	bool b_enable_light = false;
	bool b_enable_ibl = true;
	virtual void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override;
};

//输出到GBuffer
struct ShaderDrPBR
{
	MaterialDrPBR* material = nullptr;
	core::Vec3 cam_pos_ws = {};
	core::Mat mvp = {};
	core::Mat model = {};

	VsOut_Light_ws VS(const core::Model_Vertex& v) const
	{
		using namespace core;
		VsOut_Light_ws vs_out{};
		vs_out.position = mvp * v.position.ToHomoCoord();
		vs_out.position_ws = model * v.position.ToHomoCoord();
		vs_out.normal_ws = Vec3(model * v.normal).Normalize();
		vs_out.uv = v.uv;
		Mat3 normal_mat = model.ToMat3x3().Inverse().Transpose(); //保证法线矩阵是正交的
		Vec3 tangent = (normal_mat * v.tangent).Normalize();
		Vec3 normal = (normal_mat * v.normal).Normalize();
		Vec3 bitangent = normal.Cross(tangent).Normalize();
		vs_out.TBN = { tangent, bitangent,normal };
		//...
		return vs_out;
	}

	framework::GbufferType FS(const VsOut_Light_ws& v) const
	{
		using namespace core;
		Vec3 albedo = material->albedo;
		float metalness = material->metalness;
		float roughness = material->roughness;
		Vec3 N = Vec3(Texture::Sample(material->normal_map.get(), v.uv) * 2 - 1.f);
		N = (v.TBN * N).Normalize();
		//Vec3 N = v.normal_ws.Normalize();
		Vec3 V = cam_pos_ws - v.position_ws;
		float d_cam = V.Length();
		V = V.Normalize();
		float NdotV = max(N.Dot(V), 0.0f);
		auto& IBL = material->ibl;
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

		framework::GbufferType g{};
		g.base_color = Vec4(material->albedo, 1);
		g.ambient = ambient;
		g.metallic = metalness;
		g.roughness = roughness;
		g.normal = N;
		return g;
	}
};

inline void MaterialDrPBR::Render(const framework::Entity& entity, framework::IRenderEngine& engine)
{
	//准备shader数据
	ShaderDrPBR shader{ this };
	shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
	shader.model = entity.transform.GetModelMatrix();
	shader.cam_pos_ws = engine.GetMainCamera()->GetPosition();
	//渲染
	core::Renderer<ShaderDrPBR> renderer = { engine.GetGBuffer(), shader };
	renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
}

class SceneRenderTestDrPBR : public framework::Scene
{
private:
	std::vector<std::shared_ptr<framework::MaterialEntity>> bunnys;
	std::vector<std::shared_ptr<framework::Object>> lights;
	std::shared_ptr<framework::ICamera> camera;
	std::shared_ptr<framework::FPSCamera> fps_camera;
	std::shared_ptr<framework::TargetCamera> target_camera;
	std::shared_ptr<framework::Skybox> skybox;
	std::shared_ptr<core::pbr::IBL> ibl;
	bool b_show_light_icon = true;
	bool b_show_skybox = true;
	int displaymodel = 0;
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

		//创建3x3个球体，x轴roughness增大,y轴metallic增大
		for (size_t j = 0; j < 2; j++)
		{
			for (size_t i = 0; i < 4; i++)
			{
				auto material = std::make_shared<MaterialDrPBR>();
				material->albedo = { 0.77f,0.78f,0.78f };
				material->metalness = j / 1.f;
				material->roughness = i / 3.f;
				material->normal_map = framework::GetResource<core::Texture>(L"bunny_normal_map").value();
				material->ibl = framework::GetResource<core::pbr::IBL>(L"env_map").value();
				auto bunny = Spawn<framework::MaterialEntity>();
				bunny->transform.position = { i * 2.f,j * 2.f,0 };
				bunny->model = framework::GetResource<core::Model>(L"bunny").value();
				bunny->transform.scale *= 10.f;
				bunny->material = material;
				bunnys.push_back(bunny);
			}
		}

		//创建摄像机
		target_camera = std::make_shared<framework::TargetCamera>(bunnys[4], 10.f, 0.f, 0.1f);
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
			b_show_light_icon = !b_show_light_icon;
		}
		if (engine.GetInputState().key_pressed['I'] || engine.GetInputState().key_pressed['B'])
		{
			for (auto& bunny : bunnys)
			{
				auto* material = static_cast<MaterialDrPBR*>(bunny->material.get());
				material->b_enable_ibl = !material->b_enable_ibl;
				b_show_skybox = !b_show_skybox;
			}
		}
		if (engine.GetInputState().key_pressed['1'])
		{
			displaymodel = 0;
		}
		else if (engine.GetInputState().key_pressed['2'])
		{
			displaymodel = 1;
		}
		else if (engine.GetInputState().key_pressed['3'])
		{
			displaymodel = 2;
		}
		else if (engine.GetInputState().key_pressed['4'])
		{
			displaymodel = 3;
		}
		else if (engine.GetInputState().key_pressed['5'])
		{
			displaymodel = 4;
		}
		else if (engine.GetInputState().key_pressed[VK_OEM_3] || engine.GetInputState().key_pressed['6'])
		{
			displaymodel = 5;
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

		LightingPass(engine);

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

protected:
	void LightingPass(framework::IRenderEngine& engine) {
		using PixelInfo = framework::GbufferType;
		using namespace core;
		//遍历GBuffer
		//计算color
		auto& gbuffer = engine.GetGBuffer();
		auto& ctx = engine.GetCtx();
		const int size = narrow_cast<int>(gbuffer.back_buffer.size());
		auto cam_pos_ws = engine.GetMainCamera()->GetPosition();

#pragma omp parallel for num_threads(4)
		for (int i = 0; i < size; i++)
		{
			PixelInfo p_info = gbuffer.back_buffer[i];
			if (p_info.base_color.a < 1e-8f) { continue; }
			Color color = {};
			Vec3 N = p_info.normal.Normalize(); //插值之后不一定是归一化的
			Vec3 V = cam_pos_ws - p_info.position;
			float d_cam = V.Length();
			V = V.Normalize();
			float NdotV = max(N.Dot(V), 0.0f);
			Vec3 Lo = 0;
			switch (displaymodel)
			{
			case 0:
			{

				if (b_show_light_icon)
				{
					//遍历灯光
					//半球积分
					for (const auto& o : lights)
					{
						Vec3 L = 0;
						const framework::ILight* light = dynamic_cast<const framework::ILight*>(o.get());
						if (light->GetLightCategory() == framework::ELightCategory::DirectionalLight)
						{
							L = light->GetDirection();
						}
						else
						{
							L = light->GetPosition() - p_info.position;
						}

						L = L.Normalize();
						Vec3 H = V + L;
						H = H.Normalize();

						float distance = (light->GetPosition() - p_info.position).Length();
						float attenuation = (light->GetLightCategory() == framework::ELightCategory::DirectionalLight) ? (1.0f) : (1.0f / (distance * distance));
						Vec3 radiance = light->GetColor() * attenuation; //入射的radiance

						float NdotL = max(N.Dot(L), 0.0f);
						float NdotH = max(N.Dot(H), 0.0f);

						Vec3 F0 = pbr::GetF0(p_info.base_color, p_info.metallic);
						Vec3 F = pbr::FresnelSchlickRoughness(F0, NdotV, p_info.roughness);
						float D = pbr::DistributionGGX(NdotH, p_info.roughness);
						float G = pbr::GeometrySmith(NdotV, NdotL, p_info.roughness);
						Vec3 specular = pbr::SpecularCooKTorrance(D, F, G, NdotV, NdotL);
						auto Ks = pbr::FresnelSchlick(F0, NdotV);
						Vec3 Kd = (Vec3(1.f) - Ks) * (1.f - p_info.metallic);
						Lo += ((Kd * Vec3(p_info.base_color)) / core::pi + specular) * radiance * NdotL;
					}
				}
				color = Vec4(Lo + p_info.ambient + p_info.emissive, 1.f);
				break;
			}
			case 1:
			{
				color = Vec4(p_info.normal, 1.f);
				break;
			}
			case 2:
			{
				color = Vec4(Vec3(p_info.metallic), 1.f);
				break;
			}
			case 3:
			{
				color = Vec4(Vec3(p_info.roughness), 1.f);
				break;
			}
			case 4:
			{
				color = Vec4(Vec3(p_info.base_color), 1.f);
				break;
			}
			case 5:
			{
				color = Vec4(p_info.ambient, 1.f);
				break;
			}
			default:
				break;
			}
			//
			//加上环境光和自发光
			//计算ssao..暂时不做，太卡了
			//
			ctx.back_buffer[i] = color;
		}

		//复制深度，这是个设计缺陷
		std::copy(gbuffer.depth_buffer.begin(), gbuffer.depth_buffer.end(), ctx.depth_buffer.begin());
		gbuffer.Clear();
	}
};

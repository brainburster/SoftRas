#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "varying_type.hpp"

//
struct IBL
{
	std::shared_ptr<core::CubeMap> diffuse_map; //�����䲿�־��
	std::vector<core::CubeMap> specular_maps; //���淴�䲿�־��
	std::shared_ptr<core::Texture> lut; //BRDF����ͼ
	IBL()
	{
		//size_t w = evn_map.front->GetWidth();
		//size_t h = evn_map.front->GetHeight();
		diffuse_map = std::make_shared<core::CubeMap>(16, 16);
		specular_maps.reserve(5);
		specular_maps.emplace_back(128, 128);
		specular_maps.emplace_back(64, 64);
		specular_maps.emplace_back(32, 32);
		specular_maps.emplace_back(16, 16);
		specular_maps.emplace_back(8, 8);
	}

	void init(const core::CubeMap& env)
	{
		auto* diffuse_arrary = reinterpret_cast<std::shared_ptr<core::Texture>*>(diffuse_map.get());
		const core::Vec3 r = { 1,0,0 };
		const core::Vec3 u = { 0,1,0 };
		const core::Vec3 f = { 0,0,1 };

		core::Mat3 rotate_mat[6] = {
			//front, ����Ҫ��ת(����ϵ�任),r,u,f
			{r,u,f},
			//back, -r,u,-f
			{-r,u,-f},
			//top, r,-f,u,
			{ r,-f,u },
			//bottom, r,f,-u
			{r,f,-u},
			//left, f,u,-r
			{f,u,-r},
			//right, -f,u,r
			{-f,u,r}
		};
		for (size_t k = 0; k < 6; k++)
		{
			auto& tex = diffuse_arrary[k];
			size_t w = tex->GetWidth();
			size_t h = tex->GetHeight();
			for (size_t j = 0; j < h; j++)
			{
				for (size_t i = 0; i < w; i++)
				{
					//��i,j,kӳ�䵽�߳�Ϊ1�������巽����
					//i,j => {i/w-0.5f,j/w-0.5f,0.5f}
					//k ������ת����
					//���env��������
					core::Vec3 normal = core::Vec3{ (float)i / w - 0.5f, (float)j / h - 0.5f, 0.5f };
					normal = rotate_mat[k] * normal;
					normal = normal.Normalize();

					//������
					core::Vec3 up = { 0.0, 1.0f, 0.0 };
					core::Vec3 right = up.Cross(normal);
					up = normal.Cross(right);

					float sampleDelta = 0.05f;
					float nrSamples = 0.0f;

					core::Vec3 irradiance = {};
					for (float phi = 0.0f; phi < 2.0f * core::pi; phi += sampleDelta)
					{
						for (float theta = 0.0f; theta < 0.5f * core::pi; theta += sampleDelta)
						{
							// spherical to cartesian (in tangent space), ��������������߿ռ�Ĳ�������
							core::Vec3  tangentSample = core::Vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
							// tangent space to world �����߿ռ�ת��������ռ�
							core::Vec3  sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
							irradiance += core::Vec3{ env.Sample(sampleVec) } *cos(theta) * sin(theta);
							nrSamples++;
						}
					}
					irradiance = core::pi * irradiance * (1.0f / float(nrSamples));
					tex->GetRef(i, j) = core::Vec4{ irradiance,1.0f };
				}
			}
		}
	}
};

class Material_PBR : public framework::IMaterial
{
public:
	std::vector<std::shared_ptr<framework::ILight>> lights;
	core::Vec3 albedo;
	float metalness = 0;
	float roughness = 0;

	virtual void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override;
};

//����ռ� blinn_phong ��ɫ��
struct Shader_PBR
{
	Material_PBR* material = nullptr;
	core::Vec3 cam_pos_ws = {};
	core::Mat mvp = {};
	core::Mat model = {};
	core::Vec3 ambient = { 0.01f };

	Varying_Light_ws VS(const core::Model_Vertex& v) const
	{
		Varying_Light_ws varying{};
		varying.position = mvp * v.position.ToHomoCoord();
		varying.position_ws = model * v.position.ToHomoCoord();
		varying.normal_ws = core::Vec3(model * v.normal).Normalize();
		varying.uv = v.uv;
		//...
		return varying;
	}

	core::Vec4 FS(const Varying_Light_ws& v) const
	{
		using namespace core;
		Vec3 albedo = material->albedo;
		float metalness = material->metalness;
		float roughness = material->roughness;
		Vec3 N = v.normal_ws.Normalize(); //��ֵ֮��һ���ǹ�һ����
		Vec3 V = cam_pos_ws - v.position_ws;
		V = V.Normalize();
		Vec3 Lo = 0;

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

			float distance = (light->GetPosition() - v.position_ws).Length() * 0.01f;
			float attenuation = (light->GetLightCategory() == framework::ELightCategory::DirectionalLight) ? (1.0f) : (1.0f / (distance * distance));
			Vec3 radiance = light->GetColor() * attenuation; //�����radiance

			float NdotV = max(N.Dot(V), 0.0f);
			float NdotL = max(N.Dot(L), 0.0f);
			float NdotH = max(N.Dot(H), 0.0f);

			Vec3 F = pbr::GetF0(albedo, metalness);
			F = pbr::FresnelSchlick(F, N.Dot(V)); //learnopengl.com��pbr�̳������ﴫ�����dot(H,V), �Ҿ��ú��޵���, Ӧ����д����
			float D = pbr::DistributionGGX(NdotH, roughness);
			float G = pbr::GeometrySmith(NdotV, NdotL, roughness);
			Vec3 specular = pbr::SpecularCooKTorrance(D, F, G, NdotV, NdotL);
			auto Ks = F;
			Vec3 Kd = (Vec3(1.f) - Ks) * (1.f - metalness);
			Lo += ((Kd * albedo) / core::pi + specular) * radiance * NdotL;
		}
		return Vec4{ Lo + ambient * albedo, 1.f };
	}
};

inline void Material_PBR::Render(const framework::Entity& entity, framework::IRenderEngine& engine)
{
	//׼��shader����
	Shader_PBR shader{ this };
	shader.mvp = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
	shader.model = entity.transform.GetModelMatrix();
	shader.cam_pos_ws = engine.GetMainCamera()->GetPosition();
	//��Ⱦ
	core::Renderer<Shader_PBR> renderer = { engine.GetCtx(), shader };
	renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
}

class Scene_Render_PBR : public framework::Scene
{
private:
	std::vector<std::shared_ptr<framework::MaterialEntity>> spheres;
	std::vector<std::shared_ptr<framework::Object>> lights;
	std::shared_ptr<framework::TargetCamera> camera;
	std::mutex mutex_0;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		//������Դ
		auto light0 = std::make_shared<framework::PointLight>();
		light0->transform.position = { -8.f,16.f,-8.f };
		light0->color = 3.f;
		auto light1 = std::make_shared<framework::PointLight>();
		light1->transform.position = { 16.f,16.f,8.f };
		light1->color = 3.f;
		auto light2 = std::make_shared<framework::PointLight>();
		light2->transform.position = { 16.f,-10.f,8.f };
		light2->color = 3.f;
		auto light3 = std::make_shared<framework::DirectionalLight>();
		light3->transform.position = { 8.f,16.f,16.f };
		light3->dirction = { 0,0.5f,0.5f };
		light3->color = 3.f;

		//����8x8�����壬x��roughness����,y��metallic����
		objects.reserve(32);
		spheres.reserve(25);
		for (size_t j = 0; j < 5; j++)
		{
			for (size_t i = 0; i < 5; i++)
			{
				auto material = std::make_shared<Material_PBR>();
				material->albedo = { 0.91f,0.92f,0.92f };
				material->metalness = i * 0.25f;
				material->roughness = j * 0.25f;

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

		//���������
		camera = std::make_shared<framework::TargetCamera>(spheres[2 + 2 * 5], 30.f);
		//
		static IBL ibl{};
		std::thread t{ [&]() {
			ibl.init(*framework::GetResource<core::CubeMap>(L"cube_map").value().get());
			std::lock_guard lock{ mutex_0 };
			auto skybox = Spawn<framework::Skybox>();
			skybox->cube_map = ibl.diffuse_map;
		} };
		t.detach();
		//ibl.init(*framework::GetResource<core::CubeMap>(L"cube_map").value().get());
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

	virtual void Update(const framework::IRenderEngine& engine) override
	{
		size_t count = engine.GetEngineState().frame_count;
	}

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
		{
			std::lock_guard lock{ mutex_0 };
			Scene::RenderFrame(engine);
		}

		//�����㷨�Թ�Դ��͸�������������
		std::sort(lights.begin(), lights.end(), [&](auto l1, auto l2) {
			//������ͨ��������ľ���
			//auto c_p = camera->GetPosition();
			//auto d1 = (l1->transform.position - c_p).Length();
			//auto d2 = (l2->transform.position - c_p).Length();
			//return d1 > d2;

			//��΢����һ�㣬ת����NDC�ռ�
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

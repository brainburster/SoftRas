#pragma once

#include "../framework/fps_renderer_app.hpp"
#include "../core/texture.hpp"
#include "../core/types_and_defs.hpp"
#include "../framework/resource_manager.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "vertex_type.hpp"

class Shader_Blinn_Phong
{
public:
	core::Mat mat = core::Mat::Unit();
	core::Texture* tex0 = nullptr;
	core::Vec3 light_position = { 0,10,0 };
	core::Vec3 light_color = { 1,1,1 };
	core::Vec3 camera_position = { 0,0,5 };

	Vertex VS(const core::Model_Vertex& v) const
	{
		return Vertex{
			mat * core::Vec4{v.position,1.0f},
			core::Vec4(v.normal,1),
			v.uv,
			v.normal
		};
	}

	core::Vec4 FS(const Vertex& v) const
	{
		using namespace core;
		Vec3 L = (light_position - v.position).normalize();
		Vec3 V = (camera_position - v.position).normalize();
		Vec3 H = (L + V).normalize();
		Vec3 N = v.normal.normalize();
		//Vec3 Ka = Vec3(1, 1, 1);
		Vec3 Kd = Texture::Sampler(tex0, v.uv);
		Vec3 Ks = Vec3(0.5f, 0.6f, 0.6f);
		Vec3 ambient = Vec3(0.1f, 0.1f, 0.1f);
		Vec3 diffuse = Kd * light_color * max(N.Dot(L), 0);
		Vec3 specular = Ks * light_color * (float)pow(max(H.Dot(N), 0), 128);
		Vec3 color = ambient + diffuse + specular;
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
		shader.mat = engine->GetCamera().GetProjectionViewMatrix() * entity->transform.GetModelMatrix();
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

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5 }, -90.f);
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

		if (IsKeyPressed<VK_CONTROL, 'F'>())
		{
			sphere->transform.rotation += core::Vec3{ 1, 1, 1 }*0.01f;
		}
	}
};

#pragma once

#include "../framework/fps_renderer_app.hpp"
#include "../core/texture.hpp"
#include "../core/types_and_defs.hpp"
#include "../framework/resource_manager.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "vertex_type.hpp"
#include "models.hpp"

class Material_Blinn_Phong
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
		Vec3 Ks = Vec3(1, 1, 1);
		Vec3 ambient = Vec3(0.1f, 0.1f, 0.1f);
		Vec3 diffuse = Kd * light_color * max(N.Dot(L), 0);
		Vec3 specular = Ks * light_color * (float)pow(max(H.Dot(N), 0), 128);
		Vec3 color = ambient + diffuse + specular;
		return Vec4{ color, 1.f };
	}
};

class RenderTest_Blinn_Phong final : public framework::FPSRenderAPP
{
private:
	std::shared_ptr<framework::Object> cube;
public:
	RenderTest_Blinn_Phong(HINSTANCE hinst) : FPSRenderAPP{ hinst } {}

protected:

	void Init() override
	{
		SoftRasterApp::Init();

		auto tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto box = loader::obj::LoadFromFile(L".\\resource\\models\\sphere.obj");
		framework::Resource<core::Model>::Set(L"sphere", std::make_shared<core::Model>(std::move(box.value())));
		framework::Resource<core::Texture>::Set(L"tex0", std::make_shared<core::Texture>(std::move(tex.value())));

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5 }, -90.f);
		cube = world.Spawn<Sphere<Material_Blinn_Phong>>();
		//...
	}

	void HandleInput() override
	{
		FPSRenderAPP::HandleInput();

		if (IsKeyPressed<VK_CONTROL, 'F'>())
		{
			cube->transform.rotation += core::Vec3{ 1, 1, 1 }*0.01f;
		}
	}
};

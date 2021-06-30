#pragma once

#include "../core/texture.hpp"
#include "../core/model.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "../core/buffer_view.hpp"
#include "../core/software_renderer.hpp"
#include "../framework/fps_renderer_app.hpp"
#include "../framework/resource_manager.hpp"
#include "vs_out_type.hpp"

struct Shader_Unlit
{
	core::Mat mat = core::Mat::Unit();
	core::Texture* tex0 = nullptr;

	VS_OUT_Unlit VS(const core::Model_Vertex& v) const
	{
		//return VS_OUT_Unlit{ {},
		//   mat * core::Vec4{v.position,1.0f},
		//   v.uv,
		//   v.normal
		//};
		return core::CreateVarying<VS_OUT_Unlit>(mat * core::Vec4{ v.position,1.0f }, v.uv, v.normal);
	}

	core::Vec4 FS(const VS_OUT_Unlit& v) const
	{
		return core::Texture::Sampler(tex0, v.uv);
	}
};

class Material_Unlit : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;
	void Render(const framework::Entity* entity, framework::IRenderEngine* engine) override
	{
		Shader_Unlit shader{};
		core::Renderer<Shader_Unlit> renderer = { engine->GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.mat = engine->GetCamera().GetProjectionViewMatrix() * entity->transform.GetModelMatrix();
		renderer.DrawTriangles(&entity->model->mesh[0], entity->model->mesh.size());
	}
};

class RenderTest_Unlit final : public framework::FPSRenderAPP
{
private:
	std::shared_ptr<framework::MaterialEntity> cube;
public:
	RenderTest_Unlit(HINSTANCE hinst) : FPSRenderAPP{ hinst } {}

protected:

	void Init() override
	{
		SoftRasterApp::Init();

		auto tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto box = loader::obj::LoadFromFile(L".\\resource\\models\\box.obj");
		framework::Resource<core::Model>::Set(L"cube", std::make_shared<core::Model>(std::move(box.value())));
		framework::Resource<core::Texture>::Set(L"tex0", std::make_shared<core::Texture>(std::move(tex.value())));

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5 }, -90.f);
		cube = world.Spawn<framework::MaterialEntity>();
		auto material = std::make_shared <Material_Unlit>();
		cube->model = framework::GetResource<core::Model>(L"cube").value();
		cube->material = material;
		material->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		//...
	}

	void HandleInput() override
	{
		FPSRenderAPP::HandleInput();

		if (IsKeyPressed<VK_CONTROL, ' '>())
		{
			cube->transform.rotation += core::Vec3{ 1, 1, 1 }*0.01f;
		}
	}
};

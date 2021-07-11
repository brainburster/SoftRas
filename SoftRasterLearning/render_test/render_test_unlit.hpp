#pragma once

#include "../core/core_api.hpp"
#include "../framework/framework.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "varying_type.hpp"

struct Shader_Unlit
{
	core::Mat mat = core::Mat::Unit();
	core::Texture* tex0 = nullptr;

	Varying_Unlit VS(const core::Model_Vertex& v) const
	{
		return core::CreateVarying<Varying_Unlit>(mat * core::Vec4{ v.position,1.0f }, v.uv, v.normal);
	}

	core::Vec4 FS(const Varying_Unlit& v) const
	{
		return core::Texture::Sample(tex0, v.uv);
	}
};

class Material_Unlit : public framework::IMaterial
{
public:
	std::shared_ptr<core::Texture> tex0;

	void Render(const framework::Entity& entity, framework::IRenderEngine& engine) override
	{
		Shader_Unlit shader{};
		core::Renderer<Shader_Unlit> renderer = { engine.GetCtx(), shader };
		shader.tex0 = tex0.get();
		shader.mat = engine.GetMainCamera()->GetProjectionViewMatrix() * entity.transform.GetModelMatrix();
		renderer.DrawTriangles(&entity.model->mesh[0], entity.model->mesh.size());
	}
};

class Scene_Render_Test_Unlit : public framework::Scene
{
private:
	std::shared_ptr<framework::MaterialEntity> cube;
	std::shared_ptr<framework::FPSCamera> fps_camera;

public:
	void Init(framework::IRenderEngine& engine) override
	{
		fps_camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5.f }, -90.f);
		auto material_normal = std::make_shared<Material_Unlit>();
		material_normal->tex0 = framework::GetResource<core::Texture>(L"tex0").value();
		cube = Spawn<framework::MaterialEntity>();
		cube->model = framework::GetResource<core::Model>(L"box").value();
		auto cube2 = Spawn<framework::MaterialEntity>();
		cube2->model = framework::GetResource<core::Model>(L"box").value();
		cube2->transform.position = core::Vec4{ -0.5f,0.5f,-2.f };
		cube->material = material_normal;
		cube2->material = material_normal;
		//...
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		fps_camera->HandleInput(engine);

		if (framework::IsKeyPressed<VK_CONTROL, 'R'>())
		{
			cube->transform.rotation += core::Vec3{ 1, 1, 1 }*0.01f;
		}
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return fps_camera.get();
	}
};

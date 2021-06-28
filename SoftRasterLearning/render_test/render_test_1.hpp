#pragma once

#include "../core/texture.hpp"
#include "../core/model.hpp"
#include "../loader/bmp_loader.hpp"
#include "../loader/obj_loader.hpp"
#include "../core/buffer_view.hpp"
#include "../core/software_renderer.hpp"
#include "../framework/fps_renderer_app.hpp"
#include "../framework/resource_manager.hpp"

struct Vertex
{
	core::Position position;
	core::Color color;
	core::Vec2 uv;

	Vertex operator+(const Vertex& rhs) const
	{
		return {
			position + rhs.position,
			color + rhs.color,
			uv + rhs.uv
		};
	}

	friend Vertex operator*(const Vertex& lhs, float rhs)
	{
		return {
			lhs.position * rhs,
			lhs.color * rhs,
			lhs.uv * rhs
		};
	}
};

struct Material_Model
{
	core::Mat mat = core::Mat::Unit();
	core::Texture* tex0 = nullptr;

	Vertex VS(const core::Model_Vertex& v) const
	{
		return {
			mat * core::Vec4{v.position,1.0},
			core::Vec4(v.normal,1),
			v.uv
		};
	}

	gmath::Vec4<float> FS(const Vertex& v) const
	{
		return /*v.color +*/ tex0->Sampler(v.uv);
	}
};

class Cube : public framework::Entity
{
public:
	std::shared_ptr<core::Texture> tex0;
	Cube()
	{
		model = framework::Resource<core::Model>::Get(L"cube").value();
		tex0 = framework::Resource<core::Texture>::Get(L"tex0").value();
	}

	void Render(framework::IRenderEngine& engine) override
	{
		Material_Model material{};
		core::Renderer<Material_Model> renderer = { engine.GetCtx(), material };
		material.tex0 = tex0.get();
		material.mat = engine.GetCamera().GetProjectionViewMatrix() * transform.GetModelMatrix();
		renderer.DrawTriangles(&model->mesh[0], model->mesh.size());
	}
};

class RenderApp final : public framework::FPSRenderAPP
{
private:
	std::shared_ptr<framework::Object> cube;
public:
	RenderApp(HINSTANCE hinst) : FPSRenderAPP{ hinst } {}

protected:

	void Init() override
	{
		SoftRasterApp::Init();

		auto tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto box = loader::obj::LoadFromFile(L".\\resource\\models\\box.obj");
		framework::Resource<core::Model>::Set(L"cube", std::make_shared<core::Model>(std::move(box.value())));
		framework::Resource<core::Texture>::Set(L"tex0", std::make_shared<core::Texture>(std::move(tex.value())));

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5 }, -90.f);
		cube = world.Spawn<Cube>();
		//...
	}

	void HandleInput() override
	{
		FPSRenderAPP::HandleInput();

		if (input_state.key[' '])
		{
			cube->transform.rotation += core::Vec3{ 1, 1, 1 }*0.01f;
		}
	}
};

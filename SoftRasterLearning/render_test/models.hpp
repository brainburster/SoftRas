#pragma once

#include "vertex_type.hpp"
#include "../framework/object.hpp"
#include "../framework/resource_manager.hpp"
#include "../core/texture.hpp"

template<typename Shader>
class RenderTestModelBase : public framework::Entity
{
public:
	std::shared_ptr<core::Texture> tex0;

	void Render(framework::IRenderEngine& engine) override
	{
		Shader material{};
		core::Renderer<Shader> renderer = { engine.GetCtx(), material };
		material.tex0 = tex0.get();
		material.mat = engine.GetCamera().GetProjectionViewMatrix() * transform.GetModelMatrix();
		renderer.DrawTriangles(&model->mesh[0], model->mesh.size());
	}
};

template<typename Shader>
class Cube : public RenderTestModelBase<Shader>
{
public:
	Cube()
	{
		this->model = framework::Resource<core::Model>::Get(L"cube").value();
		this->tex0 = framework::Resource<core::Texture>::Get(L"tex0").value();
	}
};

template<typename Shader>
class  Sphere : public RenderTestModelBase<Shader>
{
public:
	Sphere()
	{
		this->model = framework::Resource<core::Model>::Get(L"sphere").value();
		this->tex0 = framework::Resource<core::Texture>::Get(L"tex0").value();
	}
};

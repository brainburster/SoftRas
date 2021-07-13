#pragma once

#include "../core/software_renderer.hpp"
#include "../core/model.hpp"
#include "material.hpp"
#include <memory>

namespace framework
{
	class IRenderEngine;

	//渲染接口
	class IRenderAble
	{
	public:
		virtual void Render(IRenderEngine& engine) const = 0;
	};

	//...

	//位置旋转缩放信息
	struct Transform
	{
		core::Vec3 position = { 0,0,0 };
		core::Vec3 rotation = { 0,0,0 };
		core::Vec3 scale = { 1,1,1 };
		core::Mat GetModelMatrix() const
		{
			using namespace gmath::utility;
			return Translate(position) * Rotate(rotation) * Scale(scale);
		}
	};

	//可以被渲染的物体
	class Object : public IRenderAble //,...
	{
	public:
		Transform transform;
		virtual ~Object() = default;
	};

	//拥有mesh的物体
	class Entity : public Object
	{
	public:
		std::shared_ptr<core::Model> model;
	};

	//拥有材质的物体
	class MaterialEntity : public Entity
	{
	public:
		std::shared_ptr<framework::IMaterial> material;
		void Render(framework::IRenderEngine& engine) const override
		{
			material->Render(*this, engine);
		}
	};
};

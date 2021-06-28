#pragma once

#include "../core/software_renderer.hpp"
#include "../core/model.hpp"
#include <memory>

namespace framework
{
	class IRenderEngine;

	//渲染接口
	class IRenderAble
	{
	public:
		virtual void Render(IRenderEngine& engine) = 0;
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
			return core::Mat::Translate(position) * core::Mat::Rotate(rotation) * core::Mat::Scale(scale);
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
};

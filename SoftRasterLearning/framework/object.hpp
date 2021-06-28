#pragma once
#include "../core/software_renderer.hpp"

namespace framework
{
	class IRenderEngine;

	//渲染接口
	class IRenderAble
	{
	public:
		virtual void Render(const IRenderEngine& ctx) = 0;
	};

	//...

	//位置旋转缩放信息
	struct Transform
	{
		core::Vec3 position;
		core::Vec3 rotation;
		core::Vec3 scale;
		core::Mat GetModelMatrix() const
		{
			return core::Mat::Translate(position) * core::Mat::Rotate(rotation) * core::Mat::Scale(scale);
		}
	};

	//可以被渲染的物体
	class Object : public IRenderAble //,...
	{
	public:
		Transform transfom;
		virtual ~Object() = default;
	};

	//拥有mesh的物体
	class Entity : public Object
	{
	public:
		std::vector<core::Vec3> mesh;
	};
};

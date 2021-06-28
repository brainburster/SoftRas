#pragma once
#include "core/software_renderer.hpp"

namespace framework
{
	//渲染接口
	class IRenderAble
	{
	public:
		virtual void Render(const sr::Context& ctx) = 0;
	};

	//...
	struct Transform
	{
		sr::Vec3 position;
		sr::Vec3 rotation;
		sr::Vec3 scale;
		sr::Mat GetModelMatrix()
		{
			return sr::Mat::Translate(position) * sr::Mat::Rotate(rotation) * sr::Mat::Scale(scale);
		}
	};

	//可以被渲染的物体
	class Object : public IRenderAble
	{
	public:
		Transform transfom;
		virtual ~Object() {}
	};

	//拥有mesh的物体
	class Entity : public Object
	{
	public:
		std::vector<sr::Vec3> mesh;
	};
};

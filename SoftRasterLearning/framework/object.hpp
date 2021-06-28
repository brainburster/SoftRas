#pragma once
#include "core/software_renderer.hpp"

namespace framework
{
	//��Ⱦ�ӿ�
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

	//���Ա���Ⱦ������
	class Object : public IRenderAble
	{
	public:
		Transform transfom;
		virtual ~Object() {}
	};

	//ӵ��mesh������
	class Entity : public Object
	{
	public:
		std::vector<sr::Vec3> mesh;
	};
};

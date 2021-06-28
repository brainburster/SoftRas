#pragma once
#include "../core/software_renderer.hpp"

namespace framework
{
	class IRenderEngine;

	//��Ⱦ�ӿ�
	class IRenderAble
	{
	public:
		virtual void Render(const IRenderEngine& ctx) = 0;
	};

	//...

	//λ����ת������Ϣ
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

	//���Ա���Ⱦ������
	class Object : public IRenderAble //,...
	{
	public:
		Transform transfom;
		virtual ~Object() = default;
	};

	//ӵ��mesh������
	class Entity : public Object
	{
	public:
		std::vector<core::Vec3> mesh;
	};
};

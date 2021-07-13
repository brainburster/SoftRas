#pragma once

#include "../core/software_renderer.hpp"
#include "../core/model.hpp"
#include "material.hpp"
#include <memory>

namespace framework
{
	class IRenderEngine;

	//��Ⱦ�ӿ�
	class IRenderAble
	{
	public:
		virtual void Render(IRenderEngine& engine) const = 0;
	};

	//...

	//λ����ת������Ϣ
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

	//���Ա���Ⱦ������
	class Object : public IRenderAble //,...
	{
	public:
		Transform transform;
		virtual ~Object() = default;
	};

	//ӵ��mesh������
	class Entity : public Object
	{
	public:
		std::shared_ptr<core::Model> model;
	};

	//ӵ�в��ʵ�����
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

#pragma once

#include "light.hpp"
#include "object.hpp"

namespace framework
{
	class PointLight : public Object, public ILight
	{
	public:
		Vec4 color = { 1.f };

	public:
		virtual ELightCategory GetLightType()
		{
			return ELightCategory::PointLight;
		}

		virtual Vec3 GetColor()
		{
			return color;
		}

		virtual Vec3 GetPosition()
		{
			return transform.position;
		}

		virtual Vec3 GetDirection()
		{
			return { 0.f };
		}

		virtual float GetCutOff()
		{
			return 0;
		}

		virtual Mat4 GetLightMartrix()
		{
			//���Դ�Ĺ��վ�����Ҫ����6����ģ���̫�������դ�������Ժ���˵��
			return {};
		}

		virtual void Render(framework::IRenderEngine& engine) const override
		{
			BillBoard bb;
			bb.transform.position = transform.position;
			bb.transform.rotation = { 0 };
			bb.transform.scale = { 0.5f,0.5f,1.f };
			auto tex = Resource<core::Texture>::Get(L"bulblight");
			if (tex.has_value())
			{
				bb.tex0 = tex.value();
			}

			bb.Render(engine);
		}
	};
}

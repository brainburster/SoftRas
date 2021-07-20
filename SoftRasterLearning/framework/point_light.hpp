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
		virtual ELightCategory GetLightCategory()  const noexcept
		{
			return ELightCategory::PointLight;
		}

		virtual Vec3 GetColor() const
		{
			return color;
		}

		virtual Vec3 GetPosition() const
		{
			return transform.position;
		}

		virtual Vec3 GetDirection() const
		{
			return { 0.f };
		}

		virtual float GetCutOff() const
		{
			return 0;
		}

		virtual Mat4 GetLightMartrix() const
		{
			//点光源的光照矩阵需要计算6个面的，不太想在软光栅里做，以后再说吧
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

#pragma once

#include "light.hpp"
#include "billboard.hpp"

namespace framework
{
	class DirectionalLight : public Object, public ILight
	{
	public:
		Vec3 dirction = { 0,0,-1 };
		Vec3 color = { 1,1,1 };

	public:
		virtual ELightCategory GetLightCategory() const noexcept override
		{
			return ELightCategory::DirectionalLight;
		}
		virtual Vec3 GetColor() const override
		{
			return color;
		}
		virtual Vec3 GetPosition() const override
		{
			return transform.position;
		}
		virtual Vec3 GetDirection() const override
		{
			return dirction;
		}
		virtual float GetCutOff() const override
		{
			return 0.f;
		}
		virtual Mat4 GetLightMartrix() const override
		{
			using namespace gmath::utility;
			Vec3 front = GetDirection();
			Vec3 right = front.Cross({ 0,1,0 });
			Vec3 up = right.Cross(front);
			//Ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 1000.f)
			//Frustum<float>(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.f)
			return Ortho(-20.f, 20.f, -20.f, 20.f, 0.1f, 1000.f) * View(transform.position, front, up);
		}

		virtual void Render(IRenderEngine& engine) const override
		{
			//...
			//公告板
			//
			BillBoard bb;
			bb.transform.position = transform.position;
			bb.transform.rotation = { 0 };
			bb.transform.scale = { 0.5f,0.5f,1.f };
			auto tex = Resource<core::Texture>::Get(L"sunlight");
			if (tex.has_value())
			{
				bb.tex0 = tex.value();
			}

			bb.Render(engine);
		}
	};
};

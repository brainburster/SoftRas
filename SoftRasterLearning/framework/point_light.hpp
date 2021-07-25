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
			return Vec3(transform.rotation.EularAngleToQuaternions() * Vec4 { 0.f, 0.f, -1.f, 0.f }).Normalize();
		}

		virtual float GetCutOff() const
		{
			return 0;
		}

		virtual Mat4 GetLightMartrix() const
		{
			using namespace gmath::utility;
			Vec3 front = GetDirection();
			Vec3 right = front.Cross({ 0,1,0 });
			Vec3 up = right.Cross(front);
			return Projection(90.f, 1.f, 0.1f, 1000.f) * View(transform.position, front, up);
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

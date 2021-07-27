#pragma once

#include "../core/types_and_defs.hpp"

namespace framework
{
	class ICamera
	{
	public:
		using Vec3 = core::Vec3;
		using Vec2 = core::Vec2;
		using Mat4x4 = core::Mat;

		virtual Mat4x4 GetProjectionViewMatrix() const = 0;
		virtual Mat4x4 GetProjectionwMatrix() const = 0;
		virtual Mat4x4 GetViewMatrix() const = 0;

		virtual Vec3 GetFront() const = 0;
		virtual Vec3 GetRight() const = 0;

		virtual Vec3 GetPosition() const = 0;

		virtual void HandleInput(const class IRenderEngine& engine) = 0;
		virtual void OnMouseMotion(const struct MouseMotion& motion) = 0;
	};
}

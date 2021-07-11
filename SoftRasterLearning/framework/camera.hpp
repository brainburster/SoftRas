#pragma once

namespace framework
{
	class ICamera
	{
	public:
		using Vec3 = gmath::Vec3<float>;
		using Vec2 = gmath::Vec3<float>;
		using Mat4x4 = gmath::Mat4x4<float>;

		virtual Mat4x4 GetProjectionViewMatrix() const = 0;
		virtual Mat4x4 GetProjectionwMatrix() const = 0;
		virtual Mat4x4 GetViewMatrix() const = 0;

		virtual Vec3 GetFront() const = 0;
		virtual void SetPosition(Vec3) = 0;
		virtual void AddPosition(Vec3) = 0;
		virtual Vec3 GetPosition() const = 0;
		virtual void SetYawPitch(float, float) = 0;
		virtual void AddYaw(float) = 0;
		virtual void AddPitch(float) = 0;
		virtual void AddFovy(float) = 0;

		virtual void HandleInput(const class IRenderEngine& engine) = 0;
	};
}

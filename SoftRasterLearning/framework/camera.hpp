#pragma once

#include "core/game_math.hpp"

namespace framework
{
	class ICamera
	{
	public:
		using Vec3 = gmath::Vec3<float>;
		using Vec2 = gmath::Vec3<float>;
		using Mat4x4 = gmath::Mat4x4<float>;
		virtual Mat4x4  GetProjectionViewMatrix() const = 0;
		virtual Vec3 GetFront() const = 0;
		virtual void SetPosition(Vec3) = 0;
		virtual void AddPosition(Vec3) = 0;
		virtual void SetYawPitch(float, float) = 0;
		virtual void AddYaw(float) = 0;
		virtual void AddPitch(float) = 0;
		virtual void AddFovy(float) = 0;
	};

	class FPSCamera : public ICamera
	{
	public:

		FPSCamera(Vec3 position = Vec3{ 0,0,0 }, float yaw = -90, float pitch = 0, float aspect = 4.f / 3.f, float fovy = 60, float _near = 0.1, float _far = 1e10) :
			position{ position },
			yaw(yaw),
			pitch(pitch),
			_far{ _far },
			_near{ _near },
			fovy{ fovy },
			aspect{ aspect }
		{
		}

		Mat4x4 GetProjectionViewMatrix()const override
		{
			using gmath::Utility::radians;
			Vec3 front = GetFront();
			Vec3 right = front.cross({ 0,1,0 });
			Vec3 up = right.cross(front);
			return Mat4x4::Projection(radians(fovy), aspect, _near, _far) * Mat4x4::View(position, front, up);
		}

		Vec3 GetFront() const override
		{
			using gmath::Utility::radians;
			Vec3 front = {};
			front.x = cos(radians(yaw)) * cos(radians(pitch));
			front.y = sin(radians(pitch));
			front.z = sin(radians(yaw)) * cos(radians(pitch));
			return front;
		}

		void SetPosition(Vec3 position) override
		{
			this->position = position;
		}

		void AddPosition(Vec3 position) override
		{
			this->position += position;
		}

		void SetYawPitch(float yaw, float pitch) override
		{
			this->yaw = yaw;
			this->pitch = pitch;
		}

		void AddYaw(float yaw) override
		{
			this->yaw += yaw;
		}

		void AddPitch(float pitch) override
		{
			this->pitch += pitch;
			this->pitch = gmath::Utility::Clamp(this->pitch, -89.f, 89.f);
		}

		void AddFovy(float fovy) override
		{
			this->fovy += fovy;
			this->fovy = gmath::Utility::Clamp(this->fovy, 1.f, 179.f);
		}

	private:
		//view
		Vec3 position;
		float yaw;
		float pitch;

		//projection
		float aspect;
		float fovy;
		float _near;
		float _far;
	};
}

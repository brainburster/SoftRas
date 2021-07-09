#pragma once

#include "../core/game_math.hpp"
#include "render_engine.hpp"
#include <commctrl.h>

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

		virtual void HandleInput(const IRenderEngine& engine) = 0;
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

		Mat4x4 GetProjectionwMatrix() const override
		{
			using gmath::Utility::radians;
			return Mat4x4::Projection(radians(fovy), aspect, _near, _far);
		}

		Mat4x4 GetViewMatrix() const override
		{
			Vec3 front = GetFront();
			Vec3 right = front.cross({ 0,1,0 });
			Vec3 up = right.cross(front);
			return Mat4x4::View(position, front, up);
		}

		Vec3 GetPosition() const override
		{
			return position;
		}

		void HandleInput(const IRenderEngine& engine) override
		{
			const auto& _input_state = engine.GetInputState();
			const auto& _mouse_state = _input_state.mouse_state;
			float delta = (float)engine.GetEngineState().delta_count;

			using gmath::Utility::Clamp;

			if (_mouse_state.button[0] && abs(_mouse_state.dx) < 100 && abs(_mouse_state.dy) < 100)
			{
				using gmath::Utility::Clamp;
				AddYaw(_mouse_state.dx * delta * camera_speed);
				AddPitch(Clamp(-_mouse_state.dy * delta * camera_speed, -89.f, 89.f));
			}

			AddFovy((float)_input_state.mouse_state.scroll * delta * scroll_speed);

			core::Vec3 front = GetFront();
			core::Vec3 right = front.cross({ 0,1,0 }).normalize();
			core::Vec3 up = right.cross(front).normalize();

			if (_input_state.key['W'])
			{
				AddPosition(move_speed * front * delta);
			}
			if (_input_state.key['S'])
			{
				AddPosition(-move_speed * front * delta);
			}
			if (_input_state.key['A'])
			{
				AddPosition(-move_speed * right * delta);
			}
			if (_input_state.key['D'])
			{
				AddPosition(move_speed * right * delta);
			}
			if (_input_state.key['Q'])
			{
				AddPosition(-move_speed * up * delta);
			}
			if (_input_state.key['E'])
			{
				AddPosition(move_speed * up * delta);
			}
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

		//speed
		float camera_speed = 0.015f;
		float move_speed = 0.02f;
		float scroll_speed = 0.005f;
	};
}

#pragma once

#include "camera.hpp"
#include "../core/game_math.hpp"
#include "render_engine.hpp"
#include <commctrl.h>

namespace framework
{
	class FPSCamera : public ICamera
	{
	public:
		FPSCamera(Vec3 position = Vec3{ 0,0,0 }, float yaw = 0, float pitch = 0, float aspect = 4.f / 3.f, float fovy = 60, float _near = 0.1, float _far = 1e10) :
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
			using namespace gmath::utility;
			core::Quat quat = { {0,1,0}, radians(-yaw) };
			quat = quat * core::Quat{ {1,0,0}, radians(pitch) };
			quat.Normalize();
			Vec3 front = quat * core::Vec4{ 0,0,-1,0 };
			Vec3 right = quat * core::Vec4{ 1,0,0,0 };
			Vec3 up = right.Cross(front);
			return Projection(radians(fovy), aspect, _near, _far) * View(position, front, up);
		}

		Vec3 GetFront() const override
		{
			using gmath::utility::radians;
			core::Quat quat = { {0,1,0}, radians(-yaw) };
			quat = quat * core::Quat{ {1,0,0}, radians(pitch) };
			quat.Normalize();
			return quat * core::Vec4{ 0,0,-1,0 };
		}

		Vec3 GetRight() const override
		{
			using gmath::utility::radians;
			core::Quat quat = { {0,1,0}, radians(-yaw) };
			quat = quat * core::Quat{ {1,0,0}, radians(pitch) };
			quat.Normalize();
			return quat * core::Vec4{ 1,0,0,0 };
		}

		void SetPosition(Vec3 position)
		{
			this->position = position;
		}

		void AddPosition(Vec3 position)
		{
			this->position += position;
		}

		void AddYaw(float yaw)
		{
			this->yaw += yaw;
		}

		void AddPitch(float pitch)
		{
			this->pitch += pitch;
			//this->pitch = gmath::utility::Clamp(this->pitch, -89.f, 89.f);
		}

		void AddFovy(float fovy)
		{
			this->fovy += fovy;
			this->fovy = gmath::utility::Clamp(this->fovy, 1.f, 179.f);
		}

		Mat4x4 GetProjectionwMatrix() const override
		{
			using namespace gmath::utility;
			return Projection(radians(fovy), aspect, _near, _far);
		}

		Mat4x4 GetViewMatrix() const override
		{
			using namespace gmath::utility;
			Vec3 front = GetFront();
			Vec3 right = GetRight();
			Vec3 up = right.Cross(front);
			return View(position, front, up);
		}

		Vec3 GetPosition() const override
		{
			return position;
		}

		virtual void OnMouseMotion(const typename framework::MouseMotion& motion) override
		{
			if (motion.MotionType == WM_MOUSEMOVE)
			{
				if (motion.data.button[0] && abs(motion.data.dx) < 100 && abs(motion.data.dy) < 100)
				{
					using gmath::utility::Clamp;
					AddYaw(motion.data.dx * camera_speed);
					AddPitch(-motion.data.dy * camera_speed);
				}
			}
			else if (motion.MotionType == WM_MOUSEWHEEL)
			{
				AddFovy((float)motion.data.scroll * scroll_speed);
			}
		}

		void HandleInput(const IRenderEngine& engine) override
		{
			const auto& _input_state = engine.GetInputState();
			const auto& _mouse_state = _input_state.mouse_state;
			float delta = (float)engine.GetEngineState().delta_count;

			Vec3 front = GetFront();
			Vec3 right = GetRight();
			Vec3 up = right.Cross(front).Normalize();

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

		void SetYaw(float yaw) { this->yaw = yaw; }
		void SetPitch(float pitch) { this->pitch = pitch; };
		float GetYaw() { return yaw; };
		float GetPitch() { return pitch; };
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
		float camera_speed = 0.5f;
		float move_speed = 0.02f;
		float scroll_speed = 0.05f;
	};
}

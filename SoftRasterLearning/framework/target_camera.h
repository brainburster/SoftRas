#pragma once

#pragma once

#include "camera.hpp"
#include "../core/game_math.hpp"
#include "render_engine.hpp"
#include <commctrl.h>
#include "object.hpp"

namespace framework
{
	class TargetCamera : public ICamera
	{
	public:
		TargetCamera(std::shared_ptr<Object> target, float distance = 5, float yaw = 90, float pitch = 0, float aspect = 4.f / 3.f, float fovy = 60, float _near = 0.1, float _far = 1e10) :
			target{ target },
			distance{ distance },
			yaw(yaw),
			pitch(pitch),
			_far{ _far },
			_near{ _near },
			fovy{ fovy },
			aspect{ aspect }
		{
		}

		void SetTarget(std::shared_ptr<Object> target)
		{
			this->target = target;
		}

		Mat4x4 GetProjectionViewMatrix()const override
		{
			using gmath::Utility::radians;
			Vec3 front = GetFront();
			Vec3 right = front.cross({ 0,1,0 });
			Vec3 up = right.cross(front);
			return Mat4x4::Projection(radians(fovy), aspect, _near, _far) * Mat4x4::View(GetPosition(), front, up);
		}

		Vec3 GetFront() const override
		{
			using gmath::Utility::radians;
			Vec3 front = {};
			front.x = -cos(radians(yaw)) * cos(radians(pitch));
			front.y = -sin(radians(pitch));
			front.z = -sin(radians(yaw)) * cos(radians(pitch));
			return front;
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

		void AddDistance(float d)
		{
			this->distance += d;
			this->distance = gmath::Utility::Clamp(this->distance, 2.f, 100.f);
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
			return Mat4x4::View(GetPosition(), front, up);
		}

		Vec3 GetPosition() const override
		{
			//¼ÆËãÎ»ÖÃ
			Vec3 front = GetFront();
			Vec3 pos = target->transform.position - front * distance;
			return pos;
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
				AddPitch(_mouse_state.dy * delta * camera_speed);
			}

			AddDistance((float)_input_state.mouse_state.scroll * delta * scroll_speed);

			if (_input_state.key['W'])
			{
				AddDistance(-move_speed * delta);
			}
			if (_input_state.key['S'])
			{
				AddDistance(move_speed * delta);
			}
			if (_input_state.key['A'])
			{
				AddYaw(delta * move_speed * 10);
			}
			if (_input_state.key['D'])
			{
				AddYaw(-delta * move_speed * 10);
			}
		}

	private:
		//view
		//Vec3 position;
		std::shared_ptr<Object> target;
		float yaw;
		float pitch;
		float distance;

		//projection
		float aspect;
		float fovy;
		float _near;
		float _far;

		//speed
		float camera_speed = 0.05f;
		float move_speed = 0.02f;
		float scroll_speed = 0.005f;
	};
}

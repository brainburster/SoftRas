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
		TargetCamera(std::shared_ptr<Object> target, float distance = 5, float yaw = 0, float pitch = 0, float aspect = 4.f / 3.f, float fovy = 60, float _near = 0.1, float _far = 1e10) :
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
			using namespace gmath::utility;
			core::Quat quat = { {0,1,0}, radians(-yaw) };
			quat = quat * core::Quat{ {1,0,0}, radians(pitch) };
			quat.Normalize();
			Vec3 front = quat * core::Vec4{ 0,0,-1,0 };
			Vec3 right = quat * core::Vec4{ 1,0,0,0 };
			Vec3 up = right.Cross(front);
			return Projection(radians(fovy), aspect, _near, _far) * View(GetPosition(), front, up);
		}

		Vec3 GetFront() const override
		{
			using gmath::utility::radians;
			//Vec3 front = {};
			//front.x = -cos(radians(yaw)) * cos(radians(pitch));
			//front.y = -sin(radians(pitch));
			//front.z = -sin(radians(yaw)) * cos(radians(pitch));
			//return front;
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

		void AddDistance(float d)
		{
			this->distance += d;
			this->distance = gmath::utility::Clamp(this->distance, 2.f, 100.f);
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
			Vec3 right = front.Cross({ 0,1,0 });
			Vec3 up = right.Cross(front);

			return View(GetPosition(), front, up);
		}

		Vec3 GetPosition() const override
		{
			//¼ÆËãÎ»ÖÃ
			Vec3 front = GetFront();
			Vec3 pos = target->transform.position - front * distance + offset;
			return pos;
		}

		void OnMouseMove(const IRenderEngine& engine)
		{
			auto& _input_state = const_cast<framework::InputState&>(engine.GetInputState());
			auto& _mouse_state = _input_state.mouse_state;
			float delta_time = (float)engine.GetEngineState().delta_count;

			if (_mouse_state.button[0] && abs(_mouse_state.dx) < 100 && abs(_mouse_state.dy) < 100)
			{
				using gmath::utility::Clamp;
				AddYaw(_mouse_state.dx * camera_speed * delta_time);
				AddPitch(-_mouse_state.dy * camera_speed * delta_time);
				_input_state.mouse_state.dx = 0;
				_input_state.mouse_state.dy = 0;
			}

			Vec3 front = GetFront();
			Vec3 right = front.Cross({ 0,1,0 }).Normalize();
			Vec3 up = right.Cross(front).Normalize();

			if ((_mouse_state.button[1] || _mouse_state.button[2]) && abs(_mouse_state.dx) < 100 && abs(_mouse_state.dy) < 100)
			{
				offset += camera_speed * right * (float)-_mouse_state.dx * delta_time * distance * drag_speed;
				offset += camera_speed * up * (float)_mouse_state.dy * delta_time * distance * drag_speed;
				_input_state.mouse_state.dx = 0;
				_input_state.mouse_state.dy = 0;
			}
		}

		void OnMouseWheel(const IRenderEngine& engine)
		{
			auto& _input_state = const_cast<framework::InputState&>(engine.GetInputState());
			auto& _mouse_state = _input_state.mouse_state;
			float delta_time = (float)engine.GetEngineState().delta_count;
			if (_input_state.mouse_state.scroll)
			{
				AddDistance((float)_input_state.mouse_state.scroll * delta_time * distance * scroll_speed);
			}
			_input_state.mouse_state.scroll = 0;
		}

		void HandleInput(const IRenderEngine& engine) override
		{
			const auto& _input_state = engine.GetInputState();
			const auto& _mouse_state = _input_state.mouse_state;
			float delta = (float)engine.GetEngineState().delta_count;

			OnMouseMove(engine);
			OnMouseWheel(engine);

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
				AddYaw(-delta * move_speed);
			}
			if (_input_state.key['D'])
			{
				AddYaw(delta * move_speed);
			}
			if (_input_state.key['Q'])
			{
				AddPitch(-delta * move_speed);
			}
			if (_input_state.key['E'])
			{
				AddPitch(delta * move_speed);
			}
			if (_input_state.key_pressed['F'])
			{
				offset = Vec3{ 0,0,0 };
			}
		}

	private:
		//view
		//Vec3 position;
		std::shared_ptr<Object> target;
		core::Vec3 offset = { 0,0,0 };
		float yaw;
		float pitch;
		float distance;

		//projection
		float aspect;
		float fovy;
		float _near;
		float _far;

		//speed
		float camera_speed = 0.035f;
		float move_speed = 0.2f;
		float scroll_speed = 0.00005f;
		float drag_speed = 0.006f;
	};
}

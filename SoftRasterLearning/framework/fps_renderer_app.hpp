#pragma once

#include "../framework/softraster_app.hpp"

namespace framework
{
	class FPSRenderAPP : public framework::SoftRasterApp
	{
	private:
		float move_speed = 0.02f;
		float camera_speed = 0.4f;
		float scroll_speed = 0.1f;
	public:
		FPSRenderAPP(HINSTANCE hinst) : SoftRasterApp{ hinst } {}

	protected:
		void HookInput() override
		{
			SoftRasterApp::HookInput();
			//...
			dc_wnd.RegisterWndProc(WM_MOUSEMOVE, [&](auto wParam, auto lParam) {
				const auto& _mouse_state = input_state.mouse_state;
				if (!_mouse_state.button[0]) return true;
				if (fabs(_mouse_state.dx < 20) && fabs(_mouse_state.dy) < 20)
				{
					using gmath::Utility::Clamp;
					camera->AddYaw(_mouse_state.dx * camera_speed);
					camera->AddPitch(Clamp(-_mouse_state.dy * camera_speed, -89.f, 89.f));
				}
				return true;
				});
			dc_wnd.RegisterWndProc(WM_MOUSEWHEEL, [&](auto wParam, auto lParam) {
				camera->AddFovy((float)input_state.mouse_state.scroll * scroll_speed);
				return true;
				});
		}

		void Init() override
		{
			SoftRasterApp::Init();
			camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5 }, -90.f);
		}

		//void Update() override
		//{
		//	SoftRasterApp::Update();
		//}

		void HandleInput() override
		{
			SoftRasterApp::HandleInput();

			core::Vec3 front = camera->GetFront();
			core::Vec3 right = front.cross({ 0,1,0 }).normalize();
			core::Vec3 up = right.cross(front).normalize();
			float delta = (float)app_state.delta.count();
			if (input_state.key['W'])
			{
				camera->AddPosition(move_speed * front * delta);
			}
			if (input_state.key['S'])
			{
				camera->AddPosition(-move_speed * front * delta);
			}
			if (input_state.key['A'])
			{
				camera->AddPosition(-move_speed * right * delta);
			}
			if (input_state.key['D'])
			{
				camera->AddPosition(move_speed * right * delta);
			}
			if (input_state.key['Q'])
			{
				camera->AddPosition(-move_speed * up * delta);
			}
			if (input_state.key['E'])
			{
				camera->AddPosition(move_speed * up * delta);
			}
		}

		void RenderFrame() override
		{
			SoftRasterApp::RenderFrame();
		}
	};
}

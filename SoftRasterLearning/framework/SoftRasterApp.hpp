#pragma once
#include "../core/dc_wnd.hpp"
#include "../core/software_renderer.hpp"
#include "world.hpp"
#include "camera.hpp"
#include <thread>

namespace framework
{
	class SoftRasterApp
	{
	private:
		struct InputState
		{
			bool key[256];
			struct MouseState
			{
				bool button[3];
				int x;
				int y;
				int dx;
				int dy;
				int scroll;
			} mouse_state;
		} input_state;
		struct APPState {
			long long time;
		} app_state;
		wnd::DC_WND dc_wnd;
		sr::Context ctx;
		std::shared_ptr<ICamera> camera;
		SoftRasterApp(const SoftRasterApp& other) = delete;
		SoftRasterApp& operator=(const SoftRasterApp& other) = delete;

	public:
		SoftRasterApp(HINSTANCE hinst) : dc_wnd{ hinst }, input_state{}
		{
		}
		SoftRasterApp(SoftRasterApp&& other) noexcept : dc_wnd(std::move(other.dc_wnd)), input_state{ other.input_state }
		{
		}
		SoftRasterApp& operator=(SoftRasterApp&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}
			memcpy(this, &other, sizeof(SoftRasterApp));
			memset(&other, 0, sizeof(SoftRasterApp));
			return *this;
		}

		const InputState& GetInputeState() const
		{
			return input_state;
		}

		void run()
		{
			Init();
			HookInput();

			std::thread render_thread{ [&]() {
				while (!dc_wnd.app_should_close())
				{
					RenderFrame();
					ctx.CopyToBuffer(dc_wnd.getFrameBufferView());
					dc_wnd.drawBuffer();
				}
			} };

			while (!dc_wnd.app_should_close())
			{
				dc_wnd.PeekMsg();
				//dc_wnd.GetMsg();
				//...
				HandleInput();
				Update();
			}

			render_thread.join();
		}

	protected:
		virtual void HookInput()
		{
			//...
		}

		virtual void Init()
		{
			dc_wnd.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init();
		}

		virtual void Update()
		{
		}

		virtual void HandleInput()
		{
		}

		virtual void RenderFrame()
		{
		}
	};
}

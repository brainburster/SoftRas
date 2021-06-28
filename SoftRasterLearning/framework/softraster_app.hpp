#pragma once
#include "../core/dc_wnd.hpp"
#include "../core/software_renderer.hpp"
#include "world.hpp"
#include "camera.hpp"
#include <thread>

namespace framework
{
	class IRenderEngine
	{
	public:
		virtual void Run() = 0;
	protected:
		//为windwos消息添加回调函数，调用一次
		virtual void HookInput() = 0;
		//初始化
		virtual void Init() = 0;
		//更新
		virtual void Update() = 0;
		//每帧调用, 通过获取输入信息处理逻辑
		virtual void HandleInput() = 0;
		//渲染每帧
		virtual void RenderFrame() = 0;
		virtual ~IRenderEngine() = default;
	};

	class SoftRasterApp : public IRenderEngine
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
		World world;

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

		void Run() override
		{
			Init();
			HookInput();

			std::thread render_thread{ [&]() {
				while (!dc_wnd.app_should_close())
				{
					RenderFrame();
					ctx.CopyToBuffer(dc_wnd.GetFrameBufferView());
					dc_wnd.BitBltBuffer();
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
		//为windwos消息添加回调函数，调用一次
		void HookInput() override
		{
			dc_wnd.RegisterWndProc(WM_KEYDOWN, [&](auto wParam, auto lParam) {
				input_state.key[wParam] = true;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_KEYUP, [&](auto wParam, auto lParam) {
				input_state.key[wParam] = false;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_MOUSEMOVE, [&](auto wParam, auto lParam) {
				float dx = input_state.mouse_state.x - LOWORD(lParam);
				float dy = input_state.mouse_state.y - HIWORD(lParam);
				input_state.mouse_state.x = LOWORD(lParam);
				input_state.mouse_state.y = HIWORD(lParam);
				return true;
				});

			dc_wnd.RegisterWndProc(WM_LBUTTONDOWN, [&](auto wParam, auto lParam) {
				input_state.mouse_state.button[0] = true;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_LBUTTONUP, [&](auto wParam, auto lParam) {
				input_state.mouse_state.button[0] = false;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_RBUTTONDOWN, [&](auto wParam, auto lParam) {
				input_state.mouse_state.button[2] = true;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_RBUTTONUP, [&](auto wParam, auto lParam) {
				input_state.mouse_state.button[2] = false;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_MBUTTONDOWN, [&](auto wParam, auto lParam) {
				input_state.mouse_state.button[1] = true;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_MBUTTONUP, [&](auto wParam, auto lParam) {
				input_state.mouse_state.button[1] = false;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_MOUSEWHEEL, [&](auto wParam, auto lParam) {
				input_state.mouse_state.scroll = HIWORD(wParam);
				return true;
				});
		}

		//初始化
		void Init() override
		{
			dc_wnd.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init();
		}

		//更新
		void Update() override
		{
		}

		//每帧调用
		void HandleInput() override
		{
		}

		//渲染每帧
		void RenderFrame() override
		{
			ctx.Clear({ 0.4f, 0.6f, 0.2f, 1.f });
			for (auto& object : world)
			{
				object->Render(*this);
			}
		}
	};
}

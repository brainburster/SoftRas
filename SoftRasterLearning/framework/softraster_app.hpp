#pragma once

#include "../core/dc_wnd.hpp"
#include "../core/software_renderer.hpp"
#include "world.hpp"
#include "camera.hpp"
#include <thread>
#include <chrono>

namespace framework
{
	class IRenderEngine
	{
	public:
		virtual void Run() = 0;
		//获取ctx
		virtual core::Context& GetCtx() = 0;
		virtual const ICamera& GetCamera() const = 0;
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
		virtual std::chrono::milliseconds GetDeltaTime() const = 0;
		virtual ~IRenderEngine() = default;
	};

	class SoftRasterApp : public IRenderEngine
	{
	protected:
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
			std::chrono::system_clock::time_point time;
			std::chrono::milliseconds delta;
			size_t delta_count;
			size_t frame_count;
		} app_state;
		core::DC_WND dc_wnd;
		core::Context ctx;
		std::shared_ptr<ICamera> camera;
		World world;

		SoftRasterApp(const SoftRasterApp& other) = delete;
		SoftRasterApp& operator=(const SoftRasterApp& other) = delete;

	public:
		SoftRasterApp(HINSTANCE hinst) : dc_wnd{ hinst }, input_state{}, app_state{}, ctx{}, camera{}, world{}
		{
		}

		SoftRasterApp(SoftRasterApp&& other) noexcept :
			input_state{ std::move(other.input_state) },
			app_state{ std::move(other.app_state) },
			dc_wnd(std::move(other.dc_wnd)),
			ctx{ std::move(other.ctx) },
			camera{ std::move(other.camera) },
			world{ std::move(other.world) }
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

		const ICamera& GetCamera() const override
		{
			return *camera.get();
		}

		core::Context& GetCtx() override
		{
			return ctx;
		}

		std::chrono::milliseconds GetDeltaTime() const override
		{
			return app_state.delta;
		}

		const InputState& GetInputeState() const
		{
			return input_state;
		}

		void Run() override
		{
			Init();
			HookInput();

			//std::thread render_thread{ [&]() {
			//	while (!dc_wnd.app_should_close())
			//	{
			//		RenderFrame();
			//		ctx.CopyToBuffer(dc_wnd.GetFrameBufferView());
			//		dc_wnd.BitBltBuffer();
			//	}
			//} };

			while (!dc_wnd.app_should_close())
			{
				dc_wnd.PeekMsg();
				//dc_wnd.GetMsg();
				//...
				auto last = app_state.time;
				app_state.time = std::chrono::system_clock::now();
				app_state.delta = std::chrono::duration_cast<std::chrono::milliseconds>(app_state.time - last);
				app_state.delta_count = app_state.delta.count();
				app_state.frame_count++;

				HandleInput();
				Update();

				RenderFrame();
				ctx.CopyToBuffer(dc_wnd.GetFrameBufferView());
				dc_wnd.BitBltBuffer();
			}

			//render_thread.join();
		}

		//获取组合键信息
		template<char... VK>
		static bool IsKeyPressed()
		{
			return ((GetKeyState(VK) & 0xF0000000)&&...);
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
				input_state.mouse_state.dx = (short)LOWORD(lParam) - input_state.mouse_state.x;
				input_state.mouse_state.dy = (short)HIWORD(lParam) - input_state.mouse_state.y;
				input_state.mouse_state.x = (short)LOWORD(lParam);
				input_state.mouse_state.y = (short)HIWORD(lParam);
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
				input_state.mouse_state.scroll = (short)HIWORD(wParam);
				return true;
				});
		}

		//初始化
		void Init() override
		{
			dc_wnd.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init();
			ctx.Viewport(800, 600);
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
				object->Render(this);
			}
		}
	};
}

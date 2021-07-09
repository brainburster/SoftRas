#pragma once

#include "../core/dc_wnd.hpp"
#include "../core/software_renderer.hpp"
#include "scene.hpp"
#include "render_engine.hpp"

namespace framework
{
	class SoftRasterApp : public IRenderEngine
	{
	protected:
		InputState input_state;
		EngineState engine_state;

		core::DC_WND dc_wnd;
		core::Context ctx;
		std::shared_ptr<IScene> scene;

		SoftRasterApp(const SoftRasterApp& other) = delete;
		SoftRasterApp& operator=(const SoftRasterApp& other) = delete;

	public:
		SoftRasterApp(HINSTANCE hinst) : dc_wnd{ hinst }, input_state{}, engine_state{}, ctx{}, scene{}
		{
		}

		SoftRasterApp(SoftRasterApp&& other) noexcept :
			input_state{ std::move(other.input_state) },
			engine_state{ std::move(other.engine_state) },
			dc_wnd(std::move(other.dc_wnd)),
			ctx{ std::move(other.ctx) },
			scene{ std::move(other.scene) }
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

		virtual core::Context& GetCtx() noexcept override
		{
			return ctx;
		}

		virtual const InputState& GetInputState() const noexcept override
		{
			return input_state;
		}

		virtual const EngineState& GetEngineState() const noexcept override
		{
			return engine_state;
		}

		void Run() override
		{
			Init();
			AfterInit();
			HookInput();

			while (!dc_wnd.app_should_close())
			{
				auto last = engine_state.time;
				engine_state.time = std::chrono::system_clock::now();
				engine_state.delta = std::chrono::duration_cast<std::chrono::milliseconds>(engine_state.time - last);
				engine_state.delta_count = engine_state.delta.count();
				engine_state.frame_count++;

				dc_wnd.PeekMsg();
				//dc_wnd.GetMsg();
				//...

				HandleInput();
				Update();

				RenderFrame();
				ctx.CopyToBuffer(dc_wnd.GetFrameBufferView());
				dc_wnd.BitBltBuffer();
				EndFrame();
			}
		}

	protected:
		//为windwos消息添加回调函数，调用一次
		virtual void HookInput() override
		{
			dc_wnd.RegisterWndProc(WM_KEYDOWN, [&](auto wParam, auto lParam) {
				input_state.key[wParam] = true;
				input_state.keydown[wParam] = true;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_KEYUP, [&](auto wParam, auto lParam) {
				input_state.key[wParam] = false;
				input_state.keyup[wParam] = true;
				return true;
				});

			dc_wnd.RegisterWndProc(WM_MOUSEMOVE, [&](auto wParam, auto lParam) {
				input_state.mouse_state.dx = (short)LOWORD(lParam) - input_state.mouse_state.x;
				input_state.mouse_state.dy = (short)HIWORD(lParam) - input_state.mouse_state.y;
				input_state.mouse_state.x = (short)LOWORD(lParam);
				input_state.mouse_state.y = (short)HIWORD(lParam);

				TRACKMOUSEEVENT track_mouse_event{};
				track_mouse_event.cbSize = sizeof(TRACKMOUSEEVENT);
				track_mouse_event.dwFlags = TME_LEAVE;
				track_mouse_event.dwHoverTime = HOVER_DEFAULT;
				track_mouse_event.hwndTrack = dc_wnd.Hwnd();
				TrackMouseEvent(&track_mouse_event);
				return true;
				});

			dc_wnd.RegisterWndProc(WM_MOUSELEAVE, [&](auto wParam, auto lParam) {
				input_state.mouse_state = { 0 };
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
		virtual void Init() override
		{
			dc_wnd.WndClassName(L"softraster_wnd_cls").WndName(L"软光栅测试").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init();
			ctx.Viewport(800, 600);
		}
		//
		virtual void AfterInit()
		{
			scene->Init(*this);
		}

		//更新
		virtual void Update() override
		{
			scene->Update(*this);
		}

		//每帧调用
		virtual void HandleInput() override
		{
			scene->HandleInput(*this);
		}

		//渲染每帧
		virtual void RenderFrame() override
		{
			ctx.Clear({ 0.2f, 0.6f, 0.4f, 1.f });
			scene->RenderFrame(*this);
		}

		//每帧结束后的清理工作
		virtual void EndFrame() override
		{
			input_state.mouse_state.dx = 0;
			input_state.mouse_state.dy = 0;
			input_state.mouse_state.scroll = 0;
			memset(&input_state.keydown, 0, sizeof(input_state.keydown));
			memset(&input_state.keyup, 0, sizeof(input_state.keyup));
		}
	};
}

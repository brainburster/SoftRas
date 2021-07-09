#pragma once

#include <chrono>

namespace framework
{
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
	};

	struct EngineState
	{
		std::chrono::system_clock::time_point time;
		std::chrono::milliseconds delta;
		size_t delta_count;
		size_t frame_count;
		size_t cur_scene_id;
	};

	class IRenderEngine
	{
	public:
		virtual void Run() = 0;

		virtual core::Context& GetCtx() noexcept = 0;
		virtual const InputState& GetInputState() const noexcept = 0;
		virtual const EngineState& GetEngineState() const noexcept = 0;

		virtual ~IRenderEngine() = default;
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
		//每帧结束后的清理工作
		virtual void EndFrame() = 0;
	};

	//获取组合键信息
	template<char... VK>
	inline bool IsKeyPressed()
	{
		return ((GetKeyState(VK) & 0xF0000000)&&...);
	}
}

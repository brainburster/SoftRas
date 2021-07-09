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
		//Ϊwindwos��Ϣ��ӻص�����������һ��
		virtual void HookInput() = 0;
		//��ʼ��
		virtual void Init() = 0;
		//����
		virtual void Update() = 0;
		//ÿ֡����, ͨ����ȡ������Ϣ�����߼�
		virtual void HandleInput() = 0;
		//��Ⱦÿ֡
		virtual void RenderFrame() = 0;
		//ÿ֡�������������
		virtual void EndFrame() = 0;
	};

	//��ȡ��ϼ���Ϣ
	template<char... VK>
	inline bool IsKeyPressed()
	{
		return ((GetKeyState(VK) & 0xF0000000)&&...);
	}
}

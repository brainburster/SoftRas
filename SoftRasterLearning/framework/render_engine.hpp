#pragma once

#include <chrono>
#include <functional>
#include "camera.hpp"

namespace framework
{
	struct MouseState
	{
		bool button[3];
		bool button_old[3];
		bool button_pressed[3];
		bool button_released[3];
		int x;
		int y;
		int dx;
		int dy;
		int scroll;
	};

	struct MouseMotion
	{
		UINT MotionType;
		MouseState data;
	};

	struct InputState
	{
		bool key[256];
		bool key_old[256];
		bool key_pressed[256];
		bool key_released[256];
		MouseState mouse_state;
	};

	struct EngineState
	{
		std::chrono::system_clock::time_point time;
		std::chrono::milliseconds delta;
		size_t delta_count;
		size_t frame_count;
		size_t total_time;
		size_t cur_scene_id;
	};

	//定义Guffer
	struct GbufferType
	{
		core::Vec3 ambient_and_emissive; //环境光与自发光
		core::Vec4 base_color; //固有色
		core::Vec3 normal; //切线空间法线
		core::Vec3 emissive; //自发光
		float metallic; //金属性
		float roughness;//粗糙度
		//float ao; //环境光遮蔽
		//core::Vec4 tangent_and_anisotropy; //切线与各向异性
		//float diffuse;  //漫反射
		//float specular; //镜面反射
	};

	class IRenderEngine
	{
	public:
		virtual void Run() = 0;
		virtual core::Context<core::Color>& GetCtx() noexcept = 0;
		virtual core::Context<GbufferType>& GetGBuffer() noexcept = 0;
		virtual const InputState& GetInputState() const noexcept = 0;
		virtual const EngineState& GetEngineState() const noexcept = 0;
		virtual const ICamera* GetMainCamera() const = 0;

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
		//每帧结束前
		virtual void EndFrame() = 0;
	};

	//获取组合键信息
	template<char... VK>
	inline bool IsKeyPressed()
	{
		return ((GetKeyState(VK) & 0xF0000000)&&...);
	}
}

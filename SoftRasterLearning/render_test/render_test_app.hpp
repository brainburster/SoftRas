#pragma once

#include "render_test_light_blinn_phong.hpp"
#include "render_test_normal_mapping.hpp"
#include "render_test_skybox.hpp"
#include "render_test_shadow_mapping.hpp"
#include "render_test_pbr.hpp"

class RenderTestScene final : public framework::IScene
{
private:
	std::vector<std::shared_ptr<framework::IScene>> sub_scenes;
	size_t sub_scene_id = 0;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		sub_scenes.push_back(std::make_shared<Scene_Render_Test_Test_Normal>());
		sub_scenes.push_back(std::make_shared<Scene_Render_Test_Blinn_Phong>());
		sub_scenes.push_back(std::make_shared<Scene_Render_Shadow_Mapping>());
		sub_scenes.push_back(std::make_shared<Scene_Render_Test_Skybox>());
		sub_scenes.push_back(std::make_shared<Scene_Render_PBR>());

		for (auto& sub_scene : sub_scenes)
		{
			sub_scene->Init(engine);
		}
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		sub_scenes[sub_scene_id]->HandleInput(engine);
		if (engine.GetInputState().key_pressed[VK_SPACE])
		{
			sub_scene_id = ++sub_scene_id % sub_scenes.size();
		}
	}
	virtual void Update(const framework::IRenderEngine& engine) override
	{
		sub_scenes[sub_scene_id]->Update(engine);
	}
	virtual void RenderFrame(framework::IRenderEngine& engine) override
	{
		sub_scenes[sub_scene_id]->RenderFrame(engine);
	}

	virtual const framework::ICamera* GetMainCamera() const override
	{
		return sub_scenes[sub_scene_id]->GetMainCamera();
	}

	virtual void OnMouseMove(const framework::IRenderEngine& engine) override
	{
		sub_scenes[sub_scene_id]->OnMouseMove(engine);
	}

	virtual void OnMouseWheel(const framework::IRenderEngine& engine) override
	{
		sub_scenes[sub_scene_id]->OnMouseWheel(engine);
	}
};

class RenderTestApp final : public framework::SoftRasterApp
{
private:
	std::shared_ptr<framework::MaterialEntity> sphere;

public:
	RenderTestApp(HINSTANCE hinst) : SoftRasterApp{ hinst } {}

protected:

	void Init() override
	{
		auto _sphere = loader::obj::LoadFromFile(L".\\resource\\models\\sphere.obj");
		auto _box = loader::obj::LoadFromFile(L".\\resource\\models\\box.obj");

		auto _tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto _sunlight_icon = loader::bmp::LoadFromFile(L".\\resource\\pictures\\icon\\sunlight.bmp");
		auto _bulblight_icon = loader::bmp::LoadFromFile(L".\\resource\\pictures\\icon\\bulblight.bmp");

		auto _normal_map = loader::bmp::LoadFromFile(L".\\resource\\pictures\\normal.bmp", false);

		auto _front = loader::bmp::LoadFromFile(L".\\resource\\pictures\\cubemap\\front.bmp");
		auto _back = loader::bmp::LoadFromFile(L".\\resource\\pictures\\cubemap\\back.bmp");
		auto _left = loader::bmp::LoadFromFile(L".\\resource\\pictures\\cubemap\\left.bmp");
		auto _right = loader::bmp::LoadFromFile(L".\\resource\\pictures\\cubemap\\right.bmp");
		auto _top = loader::bmp::LoadFromFile(L".\\resource\\pictures\\cubemap\\top.bmp");
		auto _bottom = loader::bmp::LoadFromFile(L".\\resource\\pictures\\cubemap\\bottom.bmp");

		framework::SetResource(L"cube_map", std::make_shared<core::CubeMap>(_front, _back, _top, _bottom, _left, _right));
		framework::SetResource(L"sphere", _sphere);
		framework::SetResource(L"box", _box);
		framework::SetResource(L"tex0", _tex);
		framework::SetResource(L"normal_map", _normal_map);
		framework::SetResource(L"sunlight", _sunlight_icon);
		framework::SetResource(L"bulblight", _bulblight_icon);

		//预设了栈空间是连续的，debug模式下无法正常工作
		//auto* env_tex = reinterpret_cast<decltype(_front)*>(&_front);
		decltype(_front) env_tex[6] = { _front, _back, _left, _right, _top, _bottom };
		for (size_t i = 0; i < 6; i++)
		{
			auto& data = env_tex[i]->GetData();
			std::transform(data.begin(), data.end(), data.begin(), [](core::Vec4 color) {
				//提高动态范围，使ldr环境贴图能够作为ibl使用，不过原来的高亮度信息已经损失了，没办法复原了
				//return (color.b > 0.92f) ? (color * 2.f).Pow(2.f) : color;
				//使用sinh函数提亮
				return core::Vec4{ _mm_sinh_ps(color * 2.2f) } / 2.2f; //应该可以把原来接近1的亮度提高到2, 而低亮度信息几乎不变
				});
		}
		SoftRasterApp::Init();
		//...
		scene = std::make_shared<RenderTestScene>();
	}
};

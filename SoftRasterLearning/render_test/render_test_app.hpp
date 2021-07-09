#pragma once

#include "render_test_unlit.hpp"
#include "render_test_light_blinn_phong.hpp"
#include "render_test_normal_mapping.hpp"

class RenderTestScene final : public framework::IScene
{
private:
	std::vector<std::shared_ptr<framework::IScene>> sub_scenes;
	size_t sub_scene_id = 0;
public:
	void Init(framework::IRenderEngine& engine) override
	{
		sub_scenes.push_back(std::make_shared<Scene_Render_Test_Normal>());
		sub_scenes.push_back(std::make_shared<Scene_Render_Test_Blinn_Phong>());
		sub_scenes.push_back(std::make_shared<Scene_Render_Unlit>());

		for (auto& sub_scene : sub_scenes)
		{
			sub_scene->Init(engine);
		}
	}

	void HandleInput(const framework::IRenderEngine& engine) override
	{
		sub_scenes[sub_scene_id]->HandleInput(engine);
		if (engine.GetInputState().keydown[VK_SPACE])
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
		SoftRasterApp::Init();
		auto _tex = loader::bmp::LoadFromFile(L".\\resource\\pictures\\tex0.bmp");
		auto _normal_map = loader::bmp::LoadFromFile(L".\\resource\\pictures\\normal.bmp", false);
		auto _sphere = loader::obj::LoadFromFile(L".\\resource\\models\\sphere.obj");
		auto _box = loader::obj::LoadFromFile(L".\\resource\\models\\box.obj");

		framework::SetResource(L"sphere", std::make_shared<core::Model>(std::move(_sphere.value())));
		framework::SetResource(L"box", std::make_shared<core::Model>(std::move(_box.value())));
		framework::SetResource(L"tex0", std::make_shared<core::Texture>(std::move(_tex.value())));
		framework::SetResource(L"normal_map", std::make_shared<core::Texture>(std::move(_normal_map.value())));

		//...
		scene = std::make_shared<RenderTestScene>();
	}
};

#pragma once

#include "render_test_unlit.hpp"
#include "render_test_light_blinn_phong.hpp"
#include "render_test_normal_mapping.hpp"

//class RenderTestScene final : public framework::IScene
//{
//};

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
		scene = std::make_shared<Scene_Render_Test_Normal>();
	}
};

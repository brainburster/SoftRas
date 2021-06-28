#include <iostream>
#include <thread>
#include "loader/bmp_loader.hpp"
#include "core/buffer_view.hpp"
#include "core/dc_wnd.hpp"
#include "core/software_renderer.hpp"
#include "loader/obj_loader.hpp"
#include "framework/camera.hpp"
#include "framework/softraster_app.hpp"
#include "framework/resource_manager.hpp"

struct Vertex
{
	core::Position position;
	core::Color color;
	core::Vec2 uv;

	Vertex operator+(const Vertex& rhs) const
	{
		return {
			position + rhs.position,
			color + rhs.color,
			uv + rhs.uv
		};
	}

	friend Vertex operator*(const Vertex& lhs, float rhs)
	{
		return {
			lhs.position * rhs,
			lhs.color * rhs,
			lhs.uv * rhs
		};
	}
};

struct Material_Model
{
	core::Mat mat = core::Mat::Unit();
	core::Texture* tex0 = nullptr;

	Vertex VS(const core::Model_Vertex& v) const
	{
		return {
			mat * core::Vec4{v.position,1.0},
			core::Vec4(v.normal,1),
			v.uv
		};
	}

	gmath::Vec4<float> FS(const Vertex& v) const
	{
		return v.color + tex0->Sampler(v.uv);
	}
};

class Cube : public framework::Entity
{
public:
	std::shared_ptr<core::Texture> tex0;
	Cube()
	{
		model = framework::Resource<core::Model>::Get(L"cube").value();
		tex0 = framework::Resource<core::Texture>::Get(L"tex0").value();
	}

	void Render(framework::IRenderEngine& engine) override
	{
		Material_Model material{};
		core::Renderer<Material_Model> renderer = { engine.GetCtx(), material };
		material.tex0 = tex0.get();
		material.mat = engine.GetCamera().GetProjectionViewMatrix() * transform.GetModelMatrix();
		renderer.DrawTriangles(&model->mesh[0], model->mesh.size());
	}
};

class RenderApp final : public framework::SoftRasterApp
{
private:
	//暂时不考虑帧率和时间
	float move_speed = 0.00001f;
	float camera_speed = 1.f;
	float scroll_speed = 0.1f;
public:
	RenderApp(HINSTANCE hinst) : SoftRasterApp{ hinst } {}

protected:
	void HookInput() override
	{
		SoftRasterApp::HookInput();
		//...
		dc_wnd.RegisterWndProc(WM_MOUSEMOVE, [&](auto wParam, auto lParam) {
			const auto& _mouse_state = input_state.mouse_state;
			if (!_mouse_state.button[0]) return true;
			if (fabs(_mouse_state.dx < 20) && fabs(_mouse_state.dy) < 20)
			{
				using gmath::Utility::Clamp;
				camera->AddYaw(_mouse_state.dx * camera_speed);
				camera->AddPitch(Clamp(-_mouse_state.dy * camera_speed, -89.f, 89.f));
			}
			return true;
			});
		dc_wnd.RegisterWndProc(WM_MOUSEWHEEL, [&](auto wParam, auto lParam) {
			camera->AddFovy((float)input_state.mouse_state.scroll * scroll_speed);
			return true;
			});
	}

	void Init() override
	{
		SoftRasterApp::Init();

		auto tex = loader::bmp::LoadFromFile(L"..\\resource\\pictures\\test.bmp");
		auto box = loader::obj::LoadFromFile(L"..\\resource\\models\\box.obj");
		framework::Resource<core::Model>::Set(L"cube", std::make_shared<core::Model>(std::move(box.value())));
		framework::Resource<core::Texture>::Set(L"tex0", std::make_shared<core::Texture>(std::move(tex.value())));

		camera = std::make_shared<framework::FPSCamera>(core::Vec3{ 0,0,5 }, -90.f);
		auto cube = world.Spawn<Cube>();
		//...
	}

	//void Update() override
	//{
	//	SoftRasterApp::Update();
	//}

	void HandleInput() override
	{
		SoftRasterApp::HandleInput();

		core::Vec3 front = camera->GetFront();
		core::Vec3 right = front.cross({ 0,1,0 }).normalize();
		core::Vec3 up = right.cross(front).normalize();
		if (input_state.key['W'])
		{
			camera->AddPosition(move_speed * front);
		}
		if (input_state.key['S'])
		{
			camera->AddPosition(-move_speed * front);
		}
		if (input_state.key['A'])
		{
			camera->AddPosition(-move_speed * right);
		}
		if (input_state.key['D'])
		{
			camera->AddPosition(move_speed * right);
		}
		if (input_state.key['Q'])
		{
			camera->AddPosition(-move_speed * up);
		}
		if (input_state.key['E'])
		{
			camera->AddPosition(move_speed * up);
		}
	}

	void RenderFrame() override
	{
		SoftRasterApp::RenderFrame();
	}
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	RenderApp sr_app{ hInstance };

	sr_app.Run();
}

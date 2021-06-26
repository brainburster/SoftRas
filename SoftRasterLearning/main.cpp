#include <iostream>
#include <thread>
#include "loader/bmp_loader.hpp"
#include "core/buffer_view.hpp"
#include "core/dc_wnd.hpp"
#include "software_renderer.hpp"
#include "loader/obj_loader.hpp"
#include "camera.hpp"

struct Vertex
{
	sr::Position position;
	sr::Color color;
	sr::Vec2 uv;

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
	sr::Mat mat = sr::Mat::Unit();
	bmp_loader::Texture* tex0 = nullptr;

	Vertex VS(const obj_loader::Model_Vertex& v) const
	{
		return {
			mat * sr::Vec4{v.position,1.0},
			sr::Vec4(v.normal,1),
			v.uv
		};
	}

	gmath::Vec4<float> FS(const Vertex& v) const
	{
		return v.color + tex0->Sampler(v.uv);
	}
};

const float pi = 3.14159265358979f;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	wnd::DC_WND wnd = wnd::DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init().Move();

	sr::Context ctx;
	ctx.Viewport(800, 600, { 0.4f, 0.6f, 0.2f, 1.f });

	Material_Model m;

	sr::Renderer<Material_Model> renderer = { ctx,m };

	bmp_loader::Texture tex = bmp_loader::BmpLoader::LoadBmp(L"..\\resource\\pictures\\test.bmp");
	auto ret = obj_loader::obj::LoadFromFile(L"..\\resource\\models\\box.obj");

	m.tex0 = &tex;

	game::Camera camera = game::Camera{ {0,0,10},-90.f };

	const float move_speed = 0.2f;

	wnd.RegisterWndProc(WM_KEYDOWN, [&](auto wParam, auto lParam) {
		//sr::Vec3 right = camera.
		sr::Vec3 front = camera.GetFront();
		sr::Vec3 right = front.cross({ 0,1,0 }).normalize();
		sr::Vec3 up = right.cross(front).normalize();

		switch (wParam)
		{
		case 'W':
			camera.position += move_speed * front;
			break;
		case 'S':
			camera.position -= move_speed * front;
			break;
		case 'A':
			camera.position -= move_speed * right;
			break;
		case 'D':
			camera.position += move_speed * right;
			break;
		case 'Q':
			camera.position += move_speed * up;
			break;
		case 'E':
			camera.position -= move_speed * up;
			break;
		}

		return true;
		});

	int x = 0;
	int y = 0;
	bool flag = false;

	wnd.RegisterWndProc(WM_MOUSEMOVE, [&](auto wParam, auto lParam) {
		float dx = (float)x - LOWORD(lParam);
		float dy = (float)y - HIWORD(lParam);
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x < 5 || x>795 || y < 5 || y>595)
		{
			flag = false;
			return true;
		}
		if (!flag) return true;
		if (dx < 20 && dy < 20)
		{
			camera.yaw -= dx * move_speed;
			camera.pitch += dy * move_speed;
			camera.pitch = gmath::Utility::Clamp(camera.pitch, -89.f, 89.f);
		}
		return true;
		});

	wnd.RegisterWndProc(WM_LBUTTONDOWN, [&](auto wParam, auto lParam) {
		flag = true;
		return true;
		});

	wnd.RegisterWndProc(WM_LBUTTONUP, [&](auto wParam, auto lParam) {
		flag = false;
		return true;
		});

	wnd.RegisterWndProc(WM_MOUSEWHEEL, [&](auto wParam, auto lParam) {
		short d = HIWORD(wParam);
		camera.fovy += d * 0.1f;
		camera.fovy = gmath::Utility::Clamp(camera.fovy, 1.f, 179.f);
		return true;
		});

	std::thread render_thread{ [&]() {
		float time = 0;
		while (!wnd.app_should_close())
		{
			ctx.Clear({ 0.4f, 0.6f, 0.2f, 1.f });

			m.mat = camera.GetProjectionViewMatrix() * sr::Mat::Translate(0.0f, 0.0f, 0.0f) * /*sr::Mat::Rotate(time/3 , time/2 , time ) **/ sr::Mat::Scale(1.f, 1.f, 1.f);// *m.mat;
			renderer.DrawTriangles(&ret->mesh[0], ret->mesh.size());

			m.mat = camera.GetProjectionViewMatrix() * sr::Mat::Translate(0.0f, 0.0f, -2.0f) * sr::Mat::Rotate(time / 3 , time / 2 , time) * sr::Mat::Scale(1.f, 1.f, 1.f);
			renderer.DrawTriangles(&ret->mesh[0], ret->mesh.size());
			time += 0.1f;
			ctx.CopyToScreen(wnd.getFrameBufferView());
			wnd.drawBuffer();
		}
	} };

	while (!wnd.app_should_close())
	{
		//wnd.PeekMsg();
		wnd.GetMsg();
		//...
	}

	render_thread.join();

	return 0;
}

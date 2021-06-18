//#include "Wnd.hpp"
#include "buffer_view.hpp"
#include "dc_wnd.hpp"
#include "software_renderer.hpp"
#include <iostream>

struct Material_Model
{
	using VS_IN = sr::Vertex;
	using VS_OUT = sr::Vertex;

	sr::Mat mat = sr::Mat::Unit();

	VS_OUT VS(const VS_IN& v) const 
	{
		return { mat * v.position,v.color };
	}

	sr::Color FS(const VS_OUT& v) const
	{
		return v.color;
	}
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//Wnd<> wnd = Wnd<>{ hInstance }.WndClassName(L"test_cls").WndName(L"test_wnd").Size(800, 600).Init().Move();

	wnd::DC_WND wnd = wnd::DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init().Move();

	sr::Context ctx;
	ctx.Viewport(800, 600, { 0.4f, 0.6f, 0.2f, 1.f });

	Material_Model m;

	sr::Renderer<Material_Model> renderer = { ctx,m };

	sr::Vertex rect[6] = {
		{{-0.5,0.5,0,1},{1,0,0,1}},
		{{ 0.5,0.5,0,1},{0,1,0,1}},
		{{ 0.5,-0.5,0,1},{0,0,1,1}},
		{{-0.5,0.5,0,1},{1,0,0,1}},
		{{ 0.5,-0.5,0,1},{0,0,1,1}},
		{{-0.5,-0.5,0,1},{1,0,0,1}}
	};

	float pi = 3.14159265358979f;

	m.mat = sr::Mat::Translate(0.3f, 0, 0) * sr::Mat::Rotate(0, 0, 60.f / 180 * pi) * sr::Mat::Scale(0.4f, 1.2f, 1) * m.mat;
	
	renderer.DrawTriangles(rect,6);

	ctx.CopyToScreen(wnd.getFrameBufferView());
	wnd.drawBuffer();


	while (!wnd.app_should_close())
	{
		wnd.PeekMsg();
		//...
	}

	return 0;
}
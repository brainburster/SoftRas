//#include "Wnd.hpp"
#include "buffer_view.hpp"
#include "dc_wnd.hpp"
#include "software_renderer.hpp"
#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//Wnd<> wnd = Wnd<>{ hInstance }.WndClassName(L"test_cls").WndName(L"test_wnd").Size(800, 600).Init().Move();

	wnd::DC_WND wnd = wnd::DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init().Move();

	srr::Context ctx;
	ctx.Viewport(800, 600, { 0.4f, 0.6f, 0.2f, 1.f });
	
	srr::Renderer<> renderer = {ctx};

	srr::Vertex rect[6] = {
		{{-0.5,0.5,0,1},{1,0,0,1}},
		{{ 0.5,0.5,0,1},{0,1,0,1}},
		{{ 0.5,-0.5,0,1},{0,0,1,1}},
		{{-0.5,0.5,0,1},{1,0,0,1}},
		{{ 0.5,-0.5,0,1},{0,0,1,1}},
		{{-0.5,-0.5,0,1},{1,0,0,1}}
	};

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
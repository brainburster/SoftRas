//#include "Wnd.hpp"
#include "buffer_view.hpp"
#include "DC_WND.hpp"
#include "SoftRasterRenderer.hpp"
#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//Wnd<> wnd = Wnd<>{ hInstance }.WndClassName(L"test_cls").WndName(L"test_wnd").Size(800, 600).Init().Move();

	DC_WND wnd = DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init().Move();

	srr::Context ctx;
	ctx.Viewport(800, 600, { 0.4f, 0.6f, 0.2f, 1.f });
	
	srr::Renderer<> renderer = {ctx};

	srr::Vertex triangles[6] = {
		{
			{200.f,400.f,1.f,1.f},
			{1.f,0.f,0.f,0.f}
		},
		{
			{400.f,500.f,1.f,1.f},
			{0.f,1.f,0.f,1.f}
		},
		{
			{500.f,300.f,1.f,1.f},
			{0.f,0.f,1.f,1.f}
		},
		{
			{300.f,100.f,1.f,1.f},
			{0.f,0.f,1.f,0.5f}
		},
		{
			{200.f,400.f,1.f,1.f},
			{1.f,0.f,0.f,0.f}
		},
		{
			{500.f,300.f,1.f,1.f},
			{0.f,0.f,1.f,1.f}
		}
	};


	while (!wnd.app_should_close())
	{
		wnd.PeekMsg();
		//...
		renderer.DrawTriangles(triangles, 6);

		ctx.CopyToScreen(wnd.getFrameBufferView());
		wnd.drawBuffer();
	}

	return 0;
}
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
	ctx.Viewport(800, 600, { 233, 233, 128, 255 });
	
	srr::Renderer<> renderer = {ctx};

	srr::Vertex triangle[3] = {
		{
			{300,400,1,1},
			{1,0,0,1}
		},
		{
			{400,500,1,1},
			{0,1,0,1}
		},
		{
			{500,300,1,1},
			{0,0,1,1}
		}
	};

	renderer.DrawTriangles(triangle,3);

	ctx.CopyToScreen(wnd.getFrameBufferView());
	wnd.drawBuffer();

	while (!wnd.app_should_close())
	{
		wnd.PeekMsg();
		//...
	}

	return 0;
}
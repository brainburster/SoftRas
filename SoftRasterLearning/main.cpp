//#include "Wnd.hpp"
#include "buffer_view.hpp"
#include "DC_WND.hpp"
#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//Wnd<> wnd = Wnd<>{ hInstance }.WndClassName(L"test_cls").WndName(L"test_wnd").Size(800, 600).Init().Move();

	DC_WND wnd = DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Move();
	wnd.Init();
	wnd.FillBuffer(Color32{ 0,233,233,255 }.bgra);

	for (int i = 50; i < 100; ++i)
	{
		wnd.setPixel(i, 200, Color32{ 0,0,233,255 }.bgra);
	}

	for (int i = 100; i < 200; ++i)
	{
		wnd.setPixel(200, i, Color32{ 233,0,0,255 }.bgra);
	}

	wnd.drawBuffer();

	while (!wnd.app_should_close())
	{
		wnd.PeekMsg();
		//...
	}

	return 0;
}
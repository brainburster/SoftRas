#include "head.hpp"
//#include "Wnd.hpp"
#include "DC_WND.hpp"
#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//Wnd wnd = Wnd::Create(hInstance).WndClassName(L"test_cls").WndName(L"test_wnd").Size(800, 600).Init().Move();
	DC_WND wnd = DC_WND{ hInstance };

	wnd.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init();
	wnd.FillBuffer(BufferColor{ 0,233,233,255 }.bgra);

	for (int i=50;i<100;++i)
	{
		wnd.setPixel(i, 200, BufferColor{ 0,0,233,255 }.bgra);
	}

	wnd.drawBuffer();

	while (!Wnd::app_should_close())
	{
		wnd.PeekMsg();
		//...
	}

	return 0;
}
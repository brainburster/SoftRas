#include "head.h"
#include "Wnd.h"
#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Wnd wnd = Wnd::Create(hInstance).WndClassName(L"test_cls").WndName(L"test_wnd").Size(800, 600).init();

	while (!Wnd::app_should_close())
	{
		wnd.PeekMsg();
		//...
	}

	return 0;
}
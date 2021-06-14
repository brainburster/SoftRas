#pragma once
#include <windows.h>
#include <string>
#include <functional>
#include <map>
#include <tuple>

class Wnd final
{
public:
	using MSG_ID = std::tuple<HWND, UINT>;
	using MSG_Handler = std::function<bool(WPARAM, LPARAM)>;
	using MSG_MAP = std::map<MSG_ID, MSG_Handler>;

	HWND GetHwnd() const noexcept;
	HINSTANCE GetHinst() const noexcept;
	Wnd& Size(UINT w, UINT h) noexcept;
	Wnd& WndName(const std::wstring& wnd_name);
	Wnd& WndClassName(const std::wstring& wnd_cls_name);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Wnd Create(HINSTANCE hinst);

	Wnd& RegisterWndProc(UINT message, const MSG_Handler& wndProc);
	Wnd& RegisterLoopBody(const std::function<void()>& loopbody);
	Wnd& init();
	void ShowLastError();
	static void peekMessage();
	static MSG getMsg();
	static bool app_should_close();
	static void abort();
	~Wnd();
private:
	Wnd(HINSTANCE hinst);
	HWND m_hwnd;
	HINSTANCE m_hinst;
	UINT m_width;
	UINT m_height;
	std::wstring m_wnd_name;
	std::wstring m_wnd_class_name;
	std::function<void()> m_loopbody;
	static MSG msg;
	static MSG_MAP msg_map;
	static bool bClose;
	void AdjustWnd();
};

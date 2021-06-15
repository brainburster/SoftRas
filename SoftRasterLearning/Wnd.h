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
	using string = std::wstring;

	HWND GetHwnd() const noexcept;
	HINSTANCE GetHinst() const noexcept;
	Wnd& Size(UINT w, UINT h) noexcept;
	Wnd& WndName(const std::wstring& wnd_name);
	Wnd& WndClassName(const std::wstring& wnd_cls_name);
	Wnd& WndStyle(DWORD wnd_style) noexcept;;
	const std::wstring& WndName() const noexcept;
	const std::wstring& WndClassName() const noexcept;
	DWORD WndStyle() const noexcept;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Wnd Create(HINSTANCE hinst);

	Wnd& RegisterWndProc(UINT message, const MSG_Handler& wndProc);
	Wnd& init();

	static void ShowLastError();
	static void PeekMsg();
	static BOOL GetMsg();
	static MSG Msg() noexcept;
	static bool app_should_close() noexcept;
	static void abort() noexcept;
	~Wnd();
private:
	Wnd(HINSTANCE hinst);
	HWND m_hwnd;
	HINSTANCE m_hinst;
	UINT m_width;
	UINT m_height;
	DWORD m_wnd_style;
	std::wstring m_wnd_name;
	std::wstring m_wnd_class_name;
	static MSG msg;
	static MSG_MAP msg_map;
	static bool bClose;
	void AdjustWnd();
};

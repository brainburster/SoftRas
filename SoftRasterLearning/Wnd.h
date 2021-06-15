#pragma once
#include <windows.h>
#include <string>
#include <functional>
#include <map>
#include <tuple>

class Wnd
{
public:
	using MSG_ID = std::tuple<HWND, UINT>;
	using MSG_Handler = std::function<bool(WPARAM, LPARAM)>;
	using MSG_MAP = std::map<MSG_ID, MSG_Handler>;
	using string = std::wstring;

	HWND Hwnd() const noexcept;
	HINSTANCE Hinst() const noexcept;
	Wnd& Size(UINT w, UINT h) noexcept;
	Wnd& WndName(const string& wnd_name);
	Wnd& WndClassName(const  string& wnd_cls_name);
	Wnd& WndStyle(DWORD wnd_style) noexcept;;
	const string& WndName() const noexcept;
	const string& WndClassName() const noexcept;
	DWORD WndStyle() const noexcept;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Wnd Create(HINSTANCE hinst);

	Wnd& RegisterWndProc(UINT message, const MSG_Handler& wndProc);
	Wnd& Init();
	Wnd Move();

	static void ShowLastError();
	static void PeekMsg();
	static BOOL GetMsg();
	static MSG Msg() noexcept;
	static bool app_should_close() noexcept;
	static void abort() noexcept;
	~Wnd();
	virtual void clear();
	Wnd& operator=(Wnd&& other) noexcept;
	Wnd(Wnd&& other) noexcept;
protected:
	Wnd(HINSTANCE hinst);
	Wnd(const Wnd&) = delete;
	Wnd& operator=(const Wnd&) = delete;

	HWND m_hwnd;
	HINSTANCE m_hinst;
	UINT m_width;
	UINT m_height;
	DWORD m_wnd_style;
	string m_wnd_name;
	string m_wnd_class_name;
	static MSG msg;
	static MSG_MAP msg_map;
	static bool bClose;
	void AdjustWnd();
};

#include "Wnd.h"

#define $_str(x) #x
#define $str(x) $_str(x)
#define $get_location_str() TEXT("\nfile:"$str(__FILE__)"\nfunction:" $str(__FUNCTION__)"\nline:"$str(__LINE__))

MSG Wnd::msg;
Wnd::MSG_MAP Wnd::msg_map;
bool Wnd::bClose;

Wnd::Wnd(HINSTANCE hinst) :
	m_hinst{ hinst },
	m_hwnd{ 0 },
	m_width{ 0 },
	m_height{ 0 },
	m_wnd_style{ WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME },
	m_wnd_name{ L"default_wnd_name" },
	m_wnd_class_name{ L"default_wnd_class_name" }
{
}

Wnd Wnd::Create(HINSTANCE hinst)
{
	return Wnd{ hinst };
}

HWND Wnd::Hwnd() const noexcept
{
	return m_hwnd;
}

HINSTANCE Wnd::Hinst() const noexcept
{
	return m_hinst;
}

Wnd& Wnd::Size(UINT w, UINT h) noexcept
{
	m_width = w;
	m_height = h;
	return *this;
}

Wnd& Wnd::WndName(const string& wnd_name)
{
	m_wnd_name = wnd_name;
	return *this;
}

const Wnd::string& Wnd::WndName() const noexcept
{
	return m_wnd_name;
}

Wnd& Wnd::WndClassName(const string& wnd_class_name)
{
	m_wnd_class_name = wnd_class_name;
	return *this;
}

const Wnd::string& Wnd::WndClassName() const noexcept
{
	return m_wnd_class_name;
}

Wnd& Wnd::WndStyle(DWORD wnd_style) noexcept
{
	m_wnd_style = wnd_style;
	return *this;
}

DWORD Wnd::WndStyle() const noexcept
{
	return m_wnd_style;
}

LRESULT Wnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message== WM_DESTROY)
	{
		Wnd::abort();
	}
	const MSG_ID msg_id = { hWnd, message };
	if (msg_map.find(msg_id) != msg_map.end() && msg_map.at(msg_id)(wParam, lParam))
	{
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

Wnd& Wnd::RegisterWndProc(UINT message, const MSG_Handler& wndProc)
{
	const MSG_ID msg_id = { m_hwnd, message };
	if (msg_map.find(msg_id) == msg_map.end()) {
		msg_map[msg_id] = wndProc;
	}
	else {
		auto previous = std::move(msg_map[msg_id]);
		msg_map[msg_id] = [previous, wndProc](auto a, auto b) {return previous(a, b) && wndProc(a, b); };
	}
	return *this;
}

Wnd& Wnd::Init()
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc = Wnd::WndProc;
	wc.hInstance = m_hinst;
	wc.lpszClassName = m_wnd_class_name.c_str();
	RegisterClass(&wc);

	m_hwnd = CreateWindowEx(NULL,m_wnd_class_name.c_str(), m_wnd_name.c_str(), m_wnd_style,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, m_hinst, NULL);

	if (!m_hwnd)
	{
		ShowLastError();
		Wnd::abort();
	}

	AdjustWnd();
	return *this;
}

Wnd&& Wnd::Move()
{
	return std::move(*this);
}

void Wnd::ShowLastError()
{

	TCHAR szBuf[128];
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	wsprintf((LPWSTR)szBuf, TEXT("location:%s\nerror code: %d\ninfo: %s"), 
		$get_location_str(),
		dw, (LPWSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	OutputDebugString(szBuf);
	MessageBox(NULL, szBuf, TEXT("last error:"), MB_OK);
}

void Wnd::AdjustWnd()
{
	const int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	const int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	RECT rect = { 0 };
	rect.left = (cxScreen - m_width) / 2;
	rect.top = (cyScreen - m_height) / 2;
	rect.right = (cxScreen + m_width) / 2;
	rect.bottom = (cyScreen + m_height) / 2;

	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, 0, 0);
	MoveWindow(m_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
	ShowWindow(m_hwnd, SW_SHOW);
}

void Wnd::PeekMsg()
{
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

BOOL Wnd::GetMsg()
{
	if (BOOL bRet = GetMessage(&msg, NULL, 0, 0)) 
	{
		if(bRet==-1)
		{
			return -1;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		return 1;
	}
	return 0;
}

MSG Wnd::Msg() noexcept
{
	return msg;
}

bool Wnd::app_should_close() noexcept
{
	return bClose;
}

void Wnd::abort() noexcept
{
	bClose = true;
}

Wnd::~Wnd()
{
	::SendMessage(m_hwnd, WM_QUIT, 0, 0);
	UnregisterClass(m_wnd_class_name.c_str(), m_hinst);
}

Wnd& Wnd::operator=(Wnd&& other) noexcept {
	if (this == &other)
	{
		return *this;
	}

	memcpy(this, &other, sizeof(Wnd));
	memset(&other, 0, sizeof(Wnd));
	return *this;
};

Wnd::Wnd(Wnd&& other) noexcept :
	m_hwnd{ other.m_hwnd },
	m_hinst{ other.m_hinst },
	m_width{ other.m_width },
	m_height{ other.m_height },
	m_wnd_style{ other.m_wnd_style },
	m_wnd_class_name{ std::move(other.m_wnd_class_name) },
	m_wnd_name{ std::move(other.m_wnd_name) }
{	
	//memcpy(this, &other, sizeof(Wnd));
	memset(&other, 0, sizeof(Wnd));
};

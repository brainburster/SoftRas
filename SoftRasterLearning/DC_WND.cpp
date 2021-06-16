#include "DC_WND.hpp"

DC_WND::DC_WND(HINSTANCE hinst) : 
	Wnd{ hinst },
	m_hdc{ NULL },
	m_hcdc{ NULL },
	m_bm{ NULL },
	m_buffer_view{ 0 }
{
}

DC_WND::DC_WND(DC_WND&& other) noexcept :
	Wnd{ other.Move() },
	m_hdc{ other.m_hdc },
	m_hcdc{ other.m_hcdc },
	m_bm{ other.m_bm },
	m_buffer_view{ 0 }
{
	other.m_hdc = 0;
}

void DC_WND::FillBuffer(UINT32 color)
{
	//for (UINT y = 0; y < m_buffer_view.h; ++y)
	//{
	//	for (UINT x = 0; x < m_buffer_view.w; ++x)
	//	{
	//		m_buffer_view.Set(x, y, color);
	//	}
	//}
	for (auto& pixel_color : m_buffer_view)
	{
		pixel_color = color;
	}
}

void DC_WND::drawBuffer()
{

	::BitBlt(m_hdc, 0, 0, m_width, m_height, m_hcdc, 0, 0, SRCCOPY);
}

void DC_WND::setPixel(UINT32 x, UINT32 y, UINT32 color)
{
	m_buffer_view.Set(x, y, color);
}

Buffer2DView<UINT32>& DC_WND::getFrameBufferView()
{
	return m_buffer_view;
}

DC_WND& DC_WND::operator=(DC_WND&& other) noexcept
 {
	if (this == &other)
	{
		return *this;
	}

	memcpy(this, &other, sizeof(DC_WND));
	memset(&other, 0, sizeof(DC_WND));
	return *this;
 }

 DC_WND::~DC_WND()
{
	 if (m_hdc)
	 {
		 ::ReleaseDC(m_hwnd, m_hdc);
		 m_hdc = NULL;
	 }
	 if (m_hcdc) 
	 {
		 ::DeleteDC(m_hcdc);
		 m_hcdc = NULL;
	 }
	 if (m_bm)
	 {
		 ::DeleteObject(m_bm);
		 m_bm = NULL;
	 }
}

 Wnd& DC_WND::Init()
 {
	 Wnd::Init();
	 m_hdc = ::GetDC(m_hwnd);
	 m_hcdc = ::CreateCompatibleDC(m_hdc);

	 BITMAPINFO _bitmap_info = { 0 };
	 _bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	 _bitmap_info.bmiHeader.biWidth = m_width;
	 _bitmap_info.bmiHeader.biHeight = m_height;
	 _bitmap_info.bmiHeader.biPlanes = 1;
	 _bitmap_info.bmiHeader.biBitCount = 32;
	 _bitmap_info.bmiHeader.biCompression = BI_RGB;
	 _bitmap_info.bmiHeader.biSizeImage = (DWORD)m_width * m_height * 4;

	 m_buffer_view.ReSize(m_width, m_height);

	 m_bm = CreateDIBSection(m_hcdc, &_bitmap_info, DIB_RGB_COLORS, (void**)&m_buffer_view.buffer, 0, 0);
	 if (!m_bm)
	 {
		 ShowLastError();
		 return *this;
	 }

	 HBITMAP temp = (HBITMAP)SelectObject(m_hcdc, m_bm);
	 DeleteObject(temp);
	 
	 return *this;
 }

#pragma once

#include "buffer_view.hpp"
#include "raw_wnd.hpp"

namespace wnd
{
	using namespace bview;
	class DC_WND : public Wnd<DC_WND>
	{
	public:
		DC_WND(HINSTANCE hinst);
		~DC_WND();
		DC_WND& Init();

		DC_WND(DC_WND&& other) noexcept;
		DC_WND& operator=(DC_WND&& other) noexcept;
		void FillBuffer(uint32 color);
		void drawBuffer();
		void setPixel(uint32 x, uint32 y, uint32 color);
		Buffer2DView<uint32>& getFrameBufferView();
	protected:
		HDC m_hdc;
		HDC m_hcdc;
		HBITMAP m_bm;
		Buffer2DView<uint32> m_buffer_view;
	private:
		DC_WND(const DC_WND&) = delete;
		DC_WND& operator=(const DC_WND& other) = delete;
	};

	inline DC_WND::DC_WND(HINSTANCE hinst) :
		Wnd{ hinst },
		m_hdc{ NULL },
		m_hcdc{ NULL },
		m_bm{ NULL },
		m_buffer_view{ 0 }
	{
	}

#pragma warning(disable:26495)
	inline DC_WND::DC_WND(DC_WND&& other) noexcept
	{
		memcpy(this, &other, sizeof(DC_WND));
		memset(&other, 0, sizeof(DC_WND));
	}
#pragma warning(default:26495)

	inline DC_WND& DC_WND::operator=(DC_WND&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		memcpy(this, &other, sizeof(DC_WND));
		memset(&other, 0, sizeof(DC_WND));
		return *this;
	}

	inline void DC_WND::FillBuffer(uint32 color)
	{
		for (UINT y = 0; y < m_buffer_view.h; ++y)
		{
			for (UINT x = 0; x < m_buffer_view.w; ++x)
			{
				m_buffer_view.Set(x, y, color);
			}
		}
		for (auto& pixel_color : m_buffer_view)
		{
			pixel_color = color;
		}
	}

	inline void DC_WND::drawBuffer()
	{
		::BitBlt(m_hdc, 0, 0, m_width, m_height, m_hcdc, 0, 0, SRCCOPY);
	}

	inline void DC_WND::setPixel(uint32 x, uint32 y, uint32 color)
	{
		m_buffer_view.Set(x, y, color);
	}

	inline Buffer2DView<uint32>& DC_WND::getFrameBufferView()
	{
		return m_buffer_view;
	}

	inline DC_WND::~DC_WND()
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

	inline DC_WND& DC_WND::Init()
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
};

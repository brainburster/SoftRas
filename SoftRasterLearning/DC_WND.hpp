#pragma once

#include "buffer_view.hpp"
#include "Wnd.hpp"
#include <vector>


class DC_WND : public Wnd<DC_WND>
{
public:
	DC_WND(HINSTANCE hinst);
	~DC_WND();
	DC_WND& Init();

	DC_WND(DC_WND&& other) noexcept;
	DC_WND& operator=(DC_WND&& other) noexcept;
	void FillBuffer(UINT32 color);
	void drawBuffer();
	void setPixel(UINT32 x, UINT32 y, UINT32 color);
	Buffer2DView<UINT32>& getFrameBufferView();
protected:
	HDC m_hdc;
	HDC m_hcdc;
	HBITMAP m_bm;
	Buffer2DView<UINT32> m_buffer_view;
private:
	DC_WND(const DC_WND&) = delete;
	DC_WND& operator=(const DC_WND& other) = delete;
};

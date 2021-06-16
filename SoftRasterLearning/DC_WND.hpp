#pragma once

#include "buffer_view.hpp"
#include "Wnd.hpp"
#include <vector>


class DC_WND : public Wnd
{
public:
	DC_WND(HINSTANCE hinst);
	~DC_WND();
	virtual Wnd& Init() override;;

	DC_WND(DC_WND&& other) noexcept;
	DC_WND& operator=(DC_WND&& other) noexcept;
	void FillBuffer(UINT32 color);
	void drawBuffer();
	void setPixel(UINT32 x, UINT32 y, UINT32 color);
	Buffer2DView<UINT32>& getFrameBufferView();
private:
	DC_WND(const DC_WND&) = delete;
	HDC m_hdc;
	HDC m_hcdc;
	HBITMAP m_bm;
	Buffer2DView<UINT32> m_buffer_view;
};

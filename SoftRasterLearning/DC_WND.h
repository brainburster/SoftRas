#pragma once

#include "Wnd.h"
#include <vector>

template<typename T>
struct BufferView
{
	void resize(UINT w, UINT h) 
	{
		this->w = w;
		this->h = h;
	}

	void set(UINT x, UINT y, T v)
	{
		size_t index = (size_t)y * w + x;
		if (index<0 || index>w * h - 1) return;
		buffer[index] = v;
	}
	T get(UINT x, UINT y)
	{
		size_t index = (size_t)y * w + x;
		if (index<0 || index>w * h - 1) return;
		return buffer[index];
	}

	T* buffer;
	UINT w;
	UINT h;
};

struct BufferColor 
{
	union
	{
		struct
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
		};
		UINT32 color;
		UINT32 bgra;
	};
};

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
private:
	DC_WND(const DC_WND&) = delete;
	HDC m_hdc;
	HDC m_hcdc;
	HBITMAP m_bm;
	BufferView<UINT32> m_buffer_view;
};

#pragma once
#include "DC_WND.hpp"
#include "triangles_view.hpp"
#include "buffer_view.hpp"
#include <vector>

class SoftRasterRenderer_Context
{
public:

	void CopyToScreen(Buffer2DView<Color32>& screen_buffer_view)
	{
		if (screen_buffer_view.w == m_fragment_buffer_view.w && screen_buffer_view.h == m_fragment_buffer_view.h)
		{
			memcpy(screen_buffer_view.buffer, m_fragment_buffer_view.buffer, screen_buffer_view.w * screen_buffer_view.h);
		}
		else 
		{
			for (uint32 y=0;y<screen_buffer_view.h;++y)
			{
				for (uint32 x = 0; x < screen_buffer_view.w; ++x)
				{
					screen_buffer_view.Set(x, y, m_fragment_buffer_view.Get(x, y));
				}
			}
		}
	}

	void Viewport(uint32 w, uint32 h) 
	{
		m_depth_buffer.reserve(w * h);
		m_fragment_buffer.reserve(w * h);
		m_fragment_buffer_view = { &m_fragment_buffer[0],w , h };
		m_depth_buffer_view = { &m_depth_buffer[0],w , h };
	}

	const Buffer2DView<Color32>& GetFragBufferView() 
	{
		return m_fragment_buffer_view;
	}

	const Buffer2DView<float>& GetDepthBufferView()
	{
		return m_depth_buffer_view;
	}
private:
	Buffer2DView<Color32> m_fragment_buffer_view;
	Buffer2DView<float> m_depth_buffer_view;
	std::vector<Color32> m_fragment_buffer;
	std::vector<float> m_depth_buffer;
};

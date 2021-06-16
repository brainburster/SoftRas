#pragma once
#include "DC_WND.hpp"
#include "triangles_view.hpp"
#include "buffer_view.hpp"

class SoftRasterRenderer
{
public:
	using FragmentBuffer = Buffer2DView<Color32>;
	using DepthBuffer = Buffer2DView<float>;


	void DrawToBuffer() 
	{
		
	}
	void PrintToScreen() 
	{

	}
private:
	FragmentBuffer m_fragment_buffer;
	DepthBuffer m_depth_buffer;
};

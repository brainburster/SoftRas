#pragma once
#include "DC_WND.hpp"
#include "triangles_view.hpp"
#include "buffer_view.hpp"
#include <vector>
#include <map>

namespace srr
{
	class Context
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
				for (uint32 y = 0; y < screen_buffer_view.h; ++y)
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

	//暂时这么定义
	struct Vec4
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct Impl
	{
		template<typename Vertex_Data_Type>
		static bool is_backface(const Triangle<Vertex_Data_Type>& triangle)
		{
			return false; //暂时先不culling
		}

		static Color32 trans_float4Color_to_uint32Color(Vec4 color)
		{
			//交换r通道和b通道
			return Color32{ 
				(unsigned char)(color.z * 255),
				(unsigned char)(color.y * 255),
				(unsigned char)(color.x * 255),
				(unsigned char)(color.w * 255)
			};
		}

	};

	//
	struct Vertex_Default
	{
		Vec4 position;
		Vec4 color;
	};

	struct VertexShader_Default 
	{
	public:
		using in_type = Vertex_Default;
		using out_type = Vertex_Default;
		out_type operator()(in_type v)
		{
			return v;
		}
	};

	struct FragShader_Default
	{
	public:
		using in_type = Vertex_Default;
		using out_type = Vec4;
		out_type operator()(in_type v)
		{
			return v.color;
		}
	};

	template<typename IN_Vertex,typename Processed_Vertex>
	class Renderer
	{
	public:
		using VertexShaderDelegate = std::function<Processed_Vertex(IN_Vertex)>;
		using FragmentShaderDelegate = std::function<Vec4(Processed_Vertex)>;
		void DrawTriangles(IN_Vertex* data, size_t n)
		{
			//todo: process_data = vertexShader(data);
			
			TrianglesView<IN_Vertex> tv = TrianglesView<IN_Vertex>{ data,n }; //todo: 实现
			for (const Triangle<IN_Vertex>& triangle : tv)
			{
				if (Impl::is_backface(triangle))
				{
					continue;					
				}
				//...
				//光栅化
				//获取深度和插值
				//使用片段着色器获得颜色
				//比较深度
				//混合
				//储存在颜色和深度到context中
			}
		}

		Renderer(Context& ctx,const VertexShaderDelegate& vs, const FragmentShaderDelegate& fs) :
			context{ ctx },
			vertexShader{ vs },
			fragShader{ fs }
		{
		}
	protected:
		Context& context;
		const VertexShaderDelegate& vertexShader;
		const FragmentShaderDelegate& fragShader;
	};
}


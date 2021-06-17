#pragma once
#include <vector>
#include <map>
#include "DC_WND.hpp"
#include "buffer_view.hpp"
#include "mat_math.hpp"

namespace srr
{
	class Context
	{
	public:
		Buffer2DView<uint32> fragment_buffer_view;
		Buffer2DView<float> depth_buffer_view;

		void CopyToScreen(Buffer2DView<uint32>& screen_buffer_view)
		{
			if (screen_buffer_view.w == fragment_buffer_view.w && screen_buffer_view.h == fragment_buffer_view.h)
			{
				memcpy(&screen_buffer_view.buffer[0], &fragment_buffer_view.buffer[0], screen_buffer_view.w * screen_buffer_view.h * 4);
			}
			else
			{
				for (uint32 y = 0; y < screen_buffer_view.h; ++y)
				{
					for (uint32 x = 0; x < screen_buffer_view.w; ++x)
					{
						screen_buffer_view.Set(x, y, fragment_buffer_view.Get(x, y));
					}
				}
			}
		}
		void Viewport(uint32 w, uint32 h, Color32 color = {0,0,0,0})
		{
			depth_buffer.resize(w * h, 0);
			fragment_buffer.resize(w * h, color.bgra);

			fragment_buffer_view = { &fragment_buffer[0],w , h };
			depth_buffer_view = { &depth_buffer[0],w , h };
		}

	protected:
		std::vector<uint32> fragment_buffer;
		std::vector<float> depth_buffer;
	};



	struct Impl
	{
		template<typename T>
		static bool is_backface(T* triangle)
		{
			Vec2 a = { triangle[1].position.x - triangle[0].position.x ,triangle[1].position.y - triangle[0].position.y };
			Vec2 b = { triangle[2].position.x - triangle[1].position.x ,triangle[2].position.y - triangle[1].position.y };
			Vec2 c = { triangle[0].position.x - triangle[2].position.x ,triangle[0].position.y - triangle[2].position.y };
			return a.cross(b) > 0 && b.cross(c) > 0 && c.cross(a) > 0;
		}

		static Color32 trans_float4color_to_uint32color(Vec4 color)
		{
			//交换r通道和b通道
			return Color32{ 
				(unsigned char)(color.z * 255),
				(unsigned char)(color.y * 255),
				(unsigned char)(color.x * 255),
				(unsigned char)(color.w * 255)
			};
		}

		template<typename T>
		static bool is_pixel_in_triangle(uint32 x, uint32 y, T* triangle)
		{
			Vec2 pa = { triangle[0].position.x - x ,triangle[0].position.y - y };
			Vec2 pb = { triangle[1].position.x - x ,triangle[1].position.y - y};
			Vec2 pc = { triangle[2].position.x - x ,triangle[2].position.y - y };
			
			return (pa.cross(pb) > 0 && pb.cross(pc) > 0 && pc.cross(pa) > 0) || (pa.cross(pb) < 0 && pb.cross(pc) < 0 && pc.cross(pa) < 0);
		}

		//获取插值
		template<typename T>
		static T get_interpolation(uint32 x, uint32 y, T* triangle)
		{
			float a1 = triangle[1].position.x - triangle[0].position.x;
			float b1 = triangle[2].position.x - triangle[0].position.x;
			float c1 = x - triangle[0].position.x;
			float a2 = triangle[1].position.y - triangle[0].position.y;
			float b2 = triangle[2].position.y - triangle[0].position.y;
			float c2 = y - triangle[0].position.y;
			float u = (a1 * c2 - a2 * c1) / (b2 * a1 - a2 * b1);
			float v = (b2 * c1 - b1 * c2) / (b2 * a1 - a2 * b1);
			float w = 1 - u - v;
			return (triangle[0] * w) + (triangle[1] * v) + (triangle[2] * u);
		}
	};

	//
	struct Vertex
	{
		Vec4 position;
		Vec4 color;
		Vertex operator+(const Vertex& rhs) const
		{
			return {position+rhs.position,color+rhs.color};
		}
		friend Vertex operator*(const Vertex& lhs, float rhs)
		{
			return { lhs.position * rhs,lhs.color * rhs };
		}
	};

	//基本的顶点着色器，不做处理
	struct VertexShader_Default 
	{
	public:
		using in_type = Vertex;
		using out_type = Vertex;
		out_type operator()(in_type v)
		{
			return v;
		}
	};

	//基本的像素着色器，不做处理
	struct FragShader_Default
	{
	public:
		using in_type = Vertex;
		using out_type = Vec4;
		out_type operator()(in_type v)
		{
			return v.color;
		}
	};

	template<typename IN_Vertex = Vertex,typename Processed_Vertex = Vertex>
	class Renderer
	{
	public:
		using VertexShaderDelegate = std::function<Processed_Vertex(IN_Vertex)>;
		using FragmentShaderDelegate = std::function<Vec4(Processed_Vertex)>;

		void DrawTriangles(IN_Vertex* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{

				Processed_Vertex triangle[3] = {
					{vertexShader(data[i])},
					{vertexShader(data[i + 1])},
					{vertexShader(data[i + 2])}
				};

				//正责化
				//...	
				//culling
				if (Impl::is_backface(triangle))
				{
					continue;
				}

				//生成AABB包围盒
				uint32 left = UINT_MAX, right = 0, top = 0, bottom = UINT_MAX;

				for (int i=0;i<3;++i)
				{
					if (left > triangle[i].position.x)
					{
						left = (uint32)triangle[i].position.x;
					}
					else if (right < triangle[i].position.x)
					{
						right = (uint32)triangle[i].position.x+1;
					}
					if (top < triangle[i].position.y) 
					{
						top = (uint32)triangle[i].position.y+1;
					}
					if (bottom > triangle[i].position.y)
					{
						bottom = (uint32)triangle[i].position.y;
					}
				}

				//...
				//光栅化
				for (uint32 y = bottom; y < top; ++y)
				{
					for (uint32 x = left; x < right; ++x)
					{
						if (Impl::is_pixel_in_triangle(x, y, triangle))
						{
							//获得深度与插值
							Processed_Vertex interp = Impl::get_interpolation(x, y, triangle);
							//调用像素着色器
							Vec4 fcolor = fragShader(interp);
							Color32 color = Impl::trans_float4color_to_uint32color(fcolor);
							//判断混合模式
							//float depth = interp.position.z;
							//...
							//if (depth > ...)
							//{
							//	continue;
							//}
							//写入缓冲区
							context.fragment_buffer_view.Set(x,y,color.bgra);
						}
					}
				}
			}
		}

		Renderer(Context& ctx, const VertexShaderDelegate& vs = VertexShader_Default{}, const FragmentShaderDelegate& fs = FragShader_Default{}) :
			context{ ctx },
			vertexShader{ vs },
			fragShader{ fs }
		{
		}
	protected:
		Context& context;
		VertexShaderDelegate vertexShader;
		FragmentShaderDelegate fragShader;
	};
}


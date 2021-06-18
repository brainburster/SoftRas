#pragma once
#include <vector>
#include <map>
#include "DC_WND.hpp"
#include "buffer_view.hpp"
#include "mat_math.hpp"

namespace srr
{
	using ColorF4 = Vec4<float>;

	struct Impl
	{
		template<typename T>
		static bool is_backface(T* triangle)
		{
			Vec2<> a = { triangle[1].position.x - triangle[0].position.x ,triangle[1].position.y - triangle[0].position.y };
			Vec2<> b = { triangle[2].position.x - triangle[1].position.x ,triangle[2].position.y - triangle[1].position.y };
			Vec2<> c = { triangle[0].position.x - triangle[2].position.x ,triangle[0].position.y - triangle[2].position.y };
			return a.cross(b) > 0 && b.cross(c) > 0 && c.cross(a) > 0;
		}

		static Color32 trans_float4color_to_uint32color(const ColorF4& color)
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
		static bool is_pixel_in_triangle(float x, float y, T* triangle)
		{
			Vec2<> p = { x,y };
			Vec2<> pa = (Vec2<>)triangle[0].position - p;
			Vec2<> pb = (Vec2<>)triangle[1].position - p;
			Vec2<> pc = (Vec2<>)triangle[2].position - p;;

			return (pa.cross(pb) > 0 && pb.cross(pc) > 0 && pc.cross(pa) > 0) || (pa.cross(pb) < 0 && pb.cross(pc) < 0 && pc.cross(pa) < 0);
		}

		//获取插值
		template<typename T>
		static Vec3<float> get_interpolation_rate(float x, float y, T* triangle)
		{
			//w*p0+u*p1+v*p2=p, w=1-u-v;
			float a1 = triangle[1].position.x - triangle[0].position.x;
			float b1 = triangle[2].position.x - triangle[0].position.x;
			float c1 = x - triangle[0].position.x;
			float a2 = triangle[1].position.y - triangle[0].position.y;
			float b2 = triangle[2].position.y - triangle[0].position.y;
			float c2 = y - triangle[0].position.y;
			float v = (a1 * c2 - a2 * c1) / (b2 * a1 - a2 * b1);
			float u = (b2 * c1 - b1 * c2) / (b2 * a1 - a2 * b1);
			float w = 1 - u - v;
			return { w,u,v };
		}

		//颜色混合
		static ColorF4 blend_color(ColorF4 color0, ColorF4 color1)
		{
			float a = 1.0f - (1.0f - color1.a) * (1.0f - color0.a);
			ColorF4 color = 1.0f/a*(color1 * color1.a + (1.0f - color1.a)*color0.a * color0);
			color.a = a;
			return color;
		}

		static ColorF4 lerp_color(ColorF4 color1, ColorF4 color2, float n)
		{
			return color1 * n + color2 * (1.0f - n);
		}

	};


	class Context
	{
	public:
		Buffer2DView<ColorF4> fragment_buffer_view;
		Buffer2DView<float> depth_buffer_view;

		void CopyToScreen(Buffer2DView<uint32>& screen_buffer_view)
		{
			if (!fragment_buffer_view.buffer)
			{
				return;
			}
			for (uint32 y = 0; y < screen_buffer_view.h; ++y)
			{
				for (uint32 x = 0; x < screen_buffer_view.w; ++x)
				{
					ColorF4 fcolor = fragment_buffer_view.Get(x, y);
					screen_buffer_view.Set(x, y, Impl::trans_float4color_to_uint32color(fcolor).bgra);
				}
			}
		}
		void Viewport(uint32 w, uint32 h,ColorF4 color = {0,0,0,1})
		{
			depth_buffer.resize(w * h, 0);
			fragment_buffer.resize(w * h, color);

			fragment_buffer_view = { &fragment_buffer[0],w , h };
			depth_buffer_view = { &depth_buffer[0],w , h };
		}
		Context() : fragment_buffer{}, depth_buffer{}, fragment_buffer_view{ nullptr }, depth_buffer_view{ nullptr }{};
	protected:
		std::vector<ColorF4> fragment_buffer;
		std::vector<float> depth_buffer;
	};

	//
	struct Vertex
	{
		Vec4HC position;
		Vec4<> color;

		Vertex operator+(const Vertex& rhs) const
		{
			return {position+rhs.position,color+rhs.color};
		}
		friend Vertex operator*(const Vertex& lhs, float rhs)
		{
			return { lhs.position * rhs,lhs.color * rhs };
		}
		friend Vertex operator*(float lhs, const Vertex& rhs)
		{
			return { rhs.position / lhs,rhs.color / lhs };
		}
		friend Vertex operator/(const Vertex& lhs, float rhs)
		{
			return { lhs.position / rhs,lhs.color / rhs };
		}

		Vertex& operator+=(const Vertex& rhs)
		{
			position += rhs.position;
			color += rhs.color;
			return *this;
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
		using out_type = ColorF4;
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
		using FragmentShaderDelegate = std::function<ColorF4(Processed_Vertex)>;

		void DrawTriangles(IN_Vertex* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{

				Processed_Vertex triangle[3] = {
					{vertexShader(data[i])},
					{vertexShader(data[i + 1])},
					{vertexShader(data[i + 2])}
				};
				for (auto& vertex : triangle)
				{
					vertex.position = vertex.position.normalize();
				}
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
						right = (uint32)triangle[i].position.x;
					}
					if (top < triangle[i].position.y) 
					{
						top = (uint32)triangle[i].position.y;
					}
					if (bottom > triangle[i].position.y)
					{
						bottom = (uint32)triangle[i].position.y;
					}
				}

				//...
				//光栅化
				for (uint32 y = bottom; y < top; ++y) //不包含右、上边界上的像素
				{
					for (uint32 x = left; x < right; ++x)
					{
						Processed_Vertex interp = { };
						
						//MSAA4x
						float msaa_count = 0;
						float Mn = 2;
						Vec3<float> aa_rate = {};

						for (float i = 0; i < Mn; ++i)
						{
							for (float j =0; j < Mn; ++j)
							{
								Vec3<float>rate = Impl::get_interpolation_rate(
									x + i / (Mn + 1) + 0.5f / (Mn + 1),
									y + j / (Mn + 1) + 0.5f / (Mn + 1),
									triangle);
								if ((double)rate.x * rate.y * rate.z > -1e-8)
								{
									aa_rate += rate;
									++msaa_count;
								}
							}
						}

						if (msaa_count)
						{
							aa_rate /= msaa_count;
							Processed_Vertex interp = triangle[0] * aa_rate.x + triangle[1] * aa_rate.y + triangle[2] * aa_rate.z;
							ColorF4 color = fragShader(interp);
							ColorF4 color0 = context.fragment_buffer_view.Get(x, y);
							//
							if (msaa_count < Mn * Mn) {
								float a = color.a;
								color = Impl::lerp_color(color, color0, msaa_count / (Mn * Mn));
								color.a = a;
							}
							
							//
							float depth = interp.position.z / interp.position.w;
							float depth0 = context.depth_buffer_view.Get(x, y);
							//深度测试
							if (depth>depth0) {
								//颜色混合
								if (color.a<0.99999)
								{
									color = Impl::blend_color(color0, color);
								}

								//写入fragment_buffer
								context.fragment_buffer_view.Set(x, y, color);
								//写入depth_buffer
								context.depth_buffer_view.Set(x, y, interp.position.z);
							}
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


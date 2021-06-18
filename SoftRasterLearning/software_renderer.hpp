#pragma once
#include <vector>
#include <map>
#include "dc_wnd.hpp"
#include "buffer_view.hpp"
#include "game_math.hpp"

namespace sr
{
	using namespace bview;
	using Position = gmath::Vec4_hc<float>;
	using Color = gmath::Vec4<float>;
	using Vec4 = gmath::Vec4<float>;
	using Vec3 = gmath::Vec3<float>;
	using Vec2 = gmath::Vec2<float>;
	using Mat = gmath::Mat4x4<float>;

	//默认的顶点类,只有位置和颜色
	struct Vertex
	{
		Position position;
		Color color;

		Vertex operator+(const Vertex& rhs) const
		{
			return { position + rhs.position,color + rhs.color };
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

		static Color32 trans_float4color_to_uint32color(const Color& color)
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
			Vec2 p = { x,y };
			Vec2 pa = (Vec2)triangle[0].position - p;
			Vec2 pb = (Vec2)triangle[1].position - p;
			Vec2 pc = (Vec2)triangle[2].position - p;;

			return (pa.cross(pb) > 0 && pb.cross(pc) > 0 && pc.cross(pa) > 0) || (pa.cross(pb) < 0 && pb.cross(pc) < 0 && pc.cross(pa) < 0);
		}

		//获取插值
		template<typename T>
		static Vec3 get_interpolation_rate(float x, float y, T* triangle)
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
		static Color blend_color(Color color0, Color color1)
		{
			float a = 1.0f - (1.0f - color1.a) * (1.0f - color0.a);
			Color color = 1.0f / a * (color1 * color1.a + (1.0f - color1.a) * color0.a * color0);
			color.a = a;
			return color;
		}

		static Color lerp_color(Color color1, Color color2, float n)
		{
			return color1 * n + color2 * (1.0f - n);
		}

	};


	class Context
	{
	public:
		Buffer2DView<Color> fragment_buffer_view;
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
					Color fcolor = fragment_buffer_view.Get(x, y);
					screen_buffer_view.Set(x, y, Impl::trans_float4color_to_uint32color(fcolor).bgra);
				}
			}
		}
		void Viewport(uint32 w, uint32 h, Color color = { 0,0,0,1 })
		{
			depth_buffer.resize(w * h, 0);
			fragment_buffer.resize(w * h, color);

			fragment_buffer_view = { &fragment_buffer[0],w , h };
			depth_buffer_view = { &depth_buffer[0],w , h };
		}
		Context() : fragment_buffer{}, depth_buffer{}, fragment_buffer_view{ nullptr }, depth_buffer_view{ nullptr }{};
	protected:
		std::vector<Color> fragment_buffer;
		std::vector<float> depth_buffer;
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
		using out_type = Color;
		out_type operator()(in_type v)
		{
			return v.color;
		}
	};

	//基本
	class Material_Default
	{
	public:
		using VS_IN = Vertex;
		using VS_OUT = Vertex;
		VS_OUT VS(const VS_IN& v)
		{
			return v;
		}

		Color FS(const VS_OUT& v)
		{
			return v.color;
		}
	};

	template<typename Material = Material_Default>
	class Renderer
	{
	public:
		using VS_IN = typename Material::VS_IN;
		using VS_OUT = typename Material::VS_OUT;


		void DrawTriangles(VS_IN* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{

				VS_IN triangle[3] = {
					{material.VS(data[i])},
					{material.VS(data[i + 1])},
					{material.VS(data[i + 2])}
				};
				for (auto& vertex : triangle)
				{
					vertex.position = vertex.position.normalize();
					vertex.position.x /= 2.f;
					vertex.position.y /= 2.f;
					vertex.position.x += 0.5f;
					vertex.position.y += 0.5f;
					vertex.position.x *= context.fragment_buffer_view.w;
					vertex.position.y *= context.fragment_buffer_view.h;
				}
				//...	
				//culling
				if (Impl::is_backface(triangle))
				{
					continue;
				}

				//生成AABB包围盒
				uint32 left = UINT_MAX, right = 0, top = 0, bottom = UINT_MAX;

				for (int i = 0; i < 3; ++i)
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
						//MSAA4x
						float msaa_count = 0;
						float Mn = 1;
						Vec3 aa_rate = {};

						for (float i = -Mn / 2; i < Mn / 2; ++i)
						{
							for (float j = -Mn / 2; j < Mn / 2; ++j)
							{
								Vec3 rate = Impl::get_interpolation_rate(
									x + i / (Mn + 1) + 0.5f,
									y + j / (Mn + 1) + 0.5f,
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
							VS_OUT interp = triangle[0] * aa_rate.x + triangle[1] * aa_rate.y + triangle[2] * aa_rate.z;

							Color color = material.FS(interp);
							Color color0 = context.fragment_buffer_view.Get(x, y);
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
							if (depth >= depth0) {
								//颜色混合
								if (color.a < 0.99999)
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

		Renderer(Context& ctx, const Material& m) :
			context{ ctx },
			material{ m }
		{
		}

	protected:
		Context& context;
		const Material& material;
	};
}

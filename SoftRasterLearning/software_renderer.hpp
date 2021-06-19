#pragma once
#include <vector>
#include <map>
#include "dc_wnd.hpp"
#include "buffer_view.hpp"
#include "game_math.hpp"

namespace sr
{
	using namespace bview;
	using Position = gmath::Vec4_hc<double>;
	using Color = gmath::Vec4<double>;
	using Vec4 = gmath::Vec4<double>;
	using Vec3 = gmath::Vec3<double>;
	using Vec2 = gmath::Vec2<double>;
	using Mat = gmath::Mat4x4<double>;

	//默认的顶点类,只有位置和颜色
	struct Vertex
	{
		Position position;
		Color color;

		Vertex operator+(const Vertex& rhs) const
		{
			return { position + rhs.position,color + rhs.color };
		}
		friend Vertex operator*(const Vertex& lhs, double rhs)
		{
			return { lhs.position * rhs,lhs.color * rhs };
		}
		friend Vertex operator*(double lhs, const Vertex& rhs)
		{
			return { rhs.position / lhs,rhs.color / lhs };
		}
		friend Vertex operator/(const Vertex& lhs, double rhs)
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
		static bool is_pixel_in_triangle(double x, double y, T* triangle)
		{
			Vec2 p = { x,y };
			Vec2 pa = (Vec2)triangle[0].position - p;
			Vec2 pb = (Vec2)triangle[1].position - p;
			Vec2 pc = (Vec2)triangle[2].position - p;;

			return (pa.cross(pb) > 0 && pb.cross(pc) > 0 && pc.cross(pa) > 0) || (pa.cross(pb) < 0 && pb.cross(pc) < 0 && pc.cross(pa) < 0);
		}

		//获取插值
		template<typename T>
		static Vec3 get_interpolation_rate(double x, double y, T* triangle)
		{
			//w*p0+u*p1+v*p2=p, w=1-u-v;
			double a1 = triangle[1].position.x - triangle[0].position.x;
			double b1 = triangle[2].position.x - triangle[0].position.x;
			double c1 = x - triangle[0].position.x;
			double a2 = triangle[1].position.y - triangle[0].position.y;
			double b2 = triangle[2].position.y - triangle[0].position.y;
			double c2 = y - triangle[0].position.y;
			double v = (a1 * c2 - a2 * c1) / (b2 * a1 - a2 * b1);
			double u = (b2 * c1 - b1 * c2) / (b2 * a1 - a2 * b1);
			double w = 1 - u - v;
			return { w,u,v };
		}

		//颜色混合
		static Color blend_color(Color color0, Color color1)
		{
			double a = 1.0f - (1.0f - color1.a) * (1.0f - color0.a);
			Color color = 1.0f / a * (color1 * color1.a + (1.0f - color1.a) * color0.a * color0);
			color.a = a;
			return color;
		}

		static Color lerp_color(Color color1, Color color2, double n)
		{
			return color1 * n + color2 * (1.0f - n);
		}

	};


	class Context
	{
	public:
		Buffer2DView<Color> fragment_buffer_view;
		Buffer2DView<double> depth_buffer_view;

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
					screen_buffer_view.Set(x, y, Impl::trans_float4color_to_uint32color(fragment_buffer_view.Get(x, y)).bgra);
				}
			}
		}
		void Viewport(uint32 w, uint32 h, Color color = { 0,0,0,1 })
		{
			depth_buffer.resize(w * h, 1e8);
			fragment_buffer.resize(w * h, color);

			fragment_buffer_view = { &fragment_buffer[0],w , h };
			depth_buffer_view = { &depth_buffer[0],w , h };
		}
		void Clear(Color color = { 0,0,0,1 })
		{
			for (auto& pixel : fragment_buffer)
			{
				pixel = color;
			}
			for (auto& depth : depth_buffer)
			{
				depth = 1e8;
			}
		}
		Context() : fragment_buffer{}, depth_buffer{}, fragment_buffer_view{ nullptr }, depth_buffer_view{ nullptr }{};
	protected:
		std::vector<Color> fragment_buffer;
		std::vector<double> depth_buffer;
	};

	//默认材质
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
				
				TransToScreenSpace(triangle);
				//...	
				//culling
				if (Impl::is_backface(triangle))
				{
					continue;
				}

				Rasterize_AABB(triangle);
			}
		}

		void TransToScreenSpace(VS_IN triangle[3])
		{
			for (int i=0;i<3;++i)
			{
				auto& vertex = triangle[i];
				vertex.position = vertex.position.normalize();
				vertex.position.x /= 2.;
				vertex.position.y /= 2.;
				vertex.position.x += 0.5;
				vertex.position.y += 0.5;
				vertex.position.x *= context.fragment_buffer_view.w;
				vertex.position.y *= context.fragment_buffer_view.h;
			}
		}

		void Rasterize_AABB(VS_IN  triangle[3])
		{
			//生成AABB包围盒
			int left = INT_MAX, right = -INT_MAX, top = -INT_MAX, bottom = INT_MAX;

			for (int i = 0; i < 3; ++i)
			{
				if (left > triangle[i].position.x)
				{
					left = (uint32)triangle[i].position.x;
				}
				if (right < triangle[i].position.x)
				{
					right = (uint32)triangle[i].position.x + 1;
				}
				if (top < triangle[i].position.y)
				{
					top = (uint32)triangle[i].position.y + 1;
				}
				if (bottom > triangle[i].position.y)
				{
					bottom = (uint32)triangle[i].position.y;
				}
			}

			//...
			//光栅化
			for (int y = bottom; y < top; ++y)
			{
				for (int x = left; x < right; ++x)
				{
					PixelOperate(x, y, triangle);
				}
			}
		}

		void Rasterize_Scanning(VS_IN  triangle[3])
		{
			//是否隔行扫描
			//double bInterlacing = false;
			//生成AABB包围盒
				
			
		}


		void PixelOperate(int x, int y,VS_IN  triangle[3])
		{
			//MSAA4x
			double msaa_count = 0;
			double Mn = 2;
			Vec3 aa_rate = {};

			for (double i = 0; i < Mn; ++i)
			{
				for (double j = 0; j < Mn; ++j)
				{
					Vec3 rate = Impl::get_interpolation_rate(
						x + (i + 0.5) / (Mn + 1),
						y + (j + 0.5) / (Mn + 1),
						triangle);
					if (rate.x * rate.y * rate.z > 1e-8)
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
				double depth = interp.position.z / interp.position.w;
				double depth0 = context.depth_buffer_view.Get(x, y);

				//深度测试
				if (depth < depth0+1e-8) {
					Color color = material.FS(interp);
					Color color0 = context.fragment_buffer_view.Get(x, y);

					//
					if (depth < depth0 - 1e-8&&msaa_count < Mn * Mn) {
						double a = color.a;
						color = Impl::lerp_color(color, color0, msaa_count / (Mn * Mn));
						color.a = a;
					}

					//颜色混合
					if (color.a < 0.99999)
					{
						color = Impl::blend_color(color0, color);
					}

					//写入fragment_buffer
					context.fragment_buffer_view.Set(x, y, color);
					//写入depth_buffer
					context.depth_buffer_view.Set(x, y, depth);
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

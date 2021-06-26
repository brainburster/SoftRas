#pragma once
#include <vector>
#include <map>
#include "core/dc_wnd.hpp"
#include "core/buffer_view.hpp"
#include "core/game_math.hpp"

namespace sr
{
	using namespace bview;
	using Position = gmath::Vec4<float>;
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
	};


	struct Impl
	{
		template<typename T>
		static bool is_backface(T* triangle)
		{
			Vec2 a = triangle[1].position - triangle[0].position;
			Vec2 b = triangle[2].position - triangle[1].position;
			//Vec2 c = triangle[0].position - triangle[2].position;
			return a.cross(b) < 0;//&& b.cross(c) > 0 && c.cross(a) > 0;
		}

		static Color32 trans_float4color_to_uint32color(const Color& color)
		{
			using gmath::Utility::Clamp;
			//交换r通道和b通道
			return Color32{
				(unsigned char)(Clamp(color.z) * 255),
				(unsigned char)(Clamp(color.y) * 255),
				(unsigned char)(Clamp(color.x) * 255),
				(unsigned char)(Clamp(color.w) * 255)
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

			auto w = screen_buffer_view.w;
			auto h = screen_buffer_view.h;

			for (uint32 y = 0; y < h; ++y)
			{
				for (uint32 x = 0; x < w; ++x)
				{
					screen_buffer_view.Set(x, y, Impl::trans_float4color_to_uint32color(fragment_buffer_view.Get(x, y)).bgra);
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
		void Clear(Color color = { 0,0,0,1 })
		{
			for (auto& pixel : fragment_buffer)
			{
				pixel = color;
			}
			for (auto& depth : depth_buffer)
			{
				depth = 0;
			}
		}
		Context() : fragment_buffer{}, depth_buffer{}, fragment_buffer_view{ nullptr }, depth_buffer_view{ nullptr }{};
	protected:
		std::vector<Color> fragment_buffer;
		std::vector<float> depth_buffer;
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
	private:
		//用来萃取顶点着色器的输入类型和输出类型（即像素着色器的输入类型）
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) const volatile) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) volatile) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) const) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In)) {}

	public:
		using VS_IN = std::decay_t<decltype(get_in_type<Material>(std::declval<decltype(&Material::VS)>()))>;
		using VS_OUT = std::decay_t<decltype(get_in_type<Material>(std::declval<decltype(&Material::FS)>()))>;

		void DrawIndex(VS_IN* data, size_t* index, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{
				DrawTriangle(data + index[i], data + index[i + 1], data + index[i + 2]);
			}
		}

		void DrawTriangles(VS_IN* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{
				DrawTriangle(data + i, data + i + 1, data + i + 2);
			}
		}

		void DrawQuadrangles(VS_IN* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 4)
			{
				DrawTriangle(data + i, data + i + 1, data + i + 2);
				DrawTriangle(data + i, data + i + 2, data + i + 3);
			}
		}

		void DrawSTRIP(VS_IN* data, size_t n)
		{
			for (size_t i = 2; i < n; ++i)
			{
				if (i & 1)
				{
					DrawTriangle(data + i - 3, data + i - 1, data + i);
				}
				else
				{
					DrawTriangle(data + i - 2, data + i - 1, data + i);
				}
			}
		}

		bool SimpleCull(VS_OUT triangle[3])
		{
			for (size_t i = 0; i < 3; ++i)
			{
				const auto& v = triangle[i];
				if (v.position.z < 0 ||
					v.position.w < 1e-8 ||
					v.position.z > v.position.w ||
					v.position.x > v.position.w ||
					v.position.x < -v.position.w ||
					v.position.y > v.position.w ||
					v.position.y < -v.position.w
					)
					return true;
			}
			return false;
		}

		//三个点都在CVV之外,剔除
		bool CVVCull(VS_OUT triangle[3])
		{
			float w0 = triangle[0].position.w;
			float w1 = triangle[1].position.w;
			float w2 = triangle[2].position.w;

			if ( w0 < 1e-20f || w1 < 1e-20f || w2 < 1e-20f ||
				triangle[0].position.z < 0 && triangle[1].position.z < 0 && triangle[2].position.z < 0 ||
				triangle[0].position.z > w0 && triangle[1].position.z > w1 && triangle[2].position.z > w2 ||
				triangle[0].position.x > w0 && triangle[1].position.x > w1  && triangle[2].position.x > w2 ||
				triangle[0].position.x < -w0 && triangle[1].position.x < -w1 && triangle[2].position.x < -w2 ||
				triangle[0].position.y > w0 && triangle[1].position.y > w1 && triangle[2].position.y > w2 ||
				triangle[0].position.y < -w0 && triangle[1].position.y < -w1 && triangle[2].position.y < -w2
				)
			{
				return true;
			}
		}

		//三角形与CVV相交, 裁剪并计算插值
		std::vector<VS_OUT> CVVClip(VS_OUT v[3])
		{
			std::vector<VS_OUT> polygon1{};
			std::vector<VS_OUT> polygon2{};
			polygon1.reserve(6); //假设最多6个顶点
			polygon2.reserve(6); //假设最多6个顶点
			polygon1.push_back(v[0]);
			polygon1.push_back(v[1]);
			polygon1.push_back(v[2]);

			//裁剪z>0平面
			for (size_t i = 0; i < polygon1.size(); ++i)
			{
				Vec4 p1 = polygon1[i].position;
				Vec4 p2 = polygon1[(i + 1) % polygon1.size()].position;
				
				//sutherland_hodgman算法
				//检查p1, p2 是否在内侧,
				//int b_p1_inside = 1;
				//int b_p2_inside = 1;
				//int sh_flag = b_p2_inside << 1 | b_p1_inside;

				//3. 如果, p2在内侧, p1也在内侧， push p1
				//2. 如果, p2在内侧, p1在外侧, 求交点p， push p, p2
				//1. 如果, p2在外侧, p1在内侧, 求交点p,  push p
				//0. 如果, p2在外侧, p1在外侧, 什么都不做

				switch (sh_flag)
				{
				case 1:
				case 2:
				case 3:
				case 0:
				}

			}

		}

		void DrawTriangle(VS_IN* p1, VS_IN* p2, VS_IN* p3)
		{
			//本地空间 => 裁剪空间
			VS_OUT triangle[3] = {
				{ material.VS(*p1) },
				{ material.VS(*p2) },
				{ material.VS(*p3) }
			};

			//简单CVV剔除
			//if (SimpleCull(triangle)) return;
			//CVV剔除
			if (CVVCull(triangle)) return;
			
			//std::vector<VS_OUT> polygon = CVVClip();

			//




			//转化为归一化设备坐标
			for (auto& v : triangle)
			{
				v.position /= v.position.w;
			}

			//转化为屏幕坐标
			TransToScreenSpace(triangle);

			//...	

			//
			if (Impl::is_backface(triangle))
			{
				return;
			}

			//用包围盒光栅化
			//Rasterize_AABB(triangle);
			//用线扫描的方式光栅化
			Rasterize_LineScanning(triangle);
		}

		void TransToScreenSpace(VS_OUT triangle[3])
		{
			for (int i = 0; i < 3; ++i)
			{
				auto& vertex = triangle[i];
				vertex.position.x /= 2.f;
				vertex.position.y /= 2.f;
				vertex.position.x += 0.5f;
				vertex.position.y += 0.5f;
				vertex.position.x *= context.fragment_buffer_view.w;
				vertex.position.y *= context.fragment_buffer_view.h;
			}
		}

		//使用AABB包围盒进行光栅化
		void Rasterize_AABB(VS_OUT  triangle[3])
		{
			//生成AABB包围盒
			int left = 1e8, right = -1e8, top = 1e8, bottom = -1e8;

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
			using gmath::Utility::Clamp;
			const int w = context.fragment_buffer_view.w;
			const int h = context.fragment_buffer_view.h;

			left = Clamp(left, 0, w-1);
			right = Clamp(right, 0, w-1);
			top = Clamp(top, 0, h-1);
			bottom = Clamp(bottom, 0, h-1);

			//...
			//光栅化
			for (int y = bottom; y <= top; ++y)
			{
				for (int x = left; x <= right; ++x)
				{
					PixelOperate(x, y, triangle);
				}
			}
		}

		//使用线扫描的方法
		void Rasterize_LineScanning(VS_OUT  triangle[3])
		{
			using gmath::Utility::Clamp;
			//是否隔行扫描
			//float bInterlacing = false;

			//获得三角形三个顶点
			Vec2 p[3] = {
				triangle[0].position,
				triangle[1].position,
				triangle[2].position,
			};

			//对三个顶点按y坐标从高到底进行排序

			for (int i = 0; i < 2; ++i)
			{
				for (int j = i; j < 3; ++j)
				{
					if (p[i].y < p[j].y)
					{
						Vec2 temp = p[j];
						p[j] = p[i];
						p[i] = temp;
					}
				}
			}

			float y1 = Clamp(p[2].y, 0.6f, context.fragment_buffer_view.h - 0.6f);
			float y2 = Clamp(p[0].y, 0.6f, context.fragment_buffer_view.h - 0.6f);

			//从上到下扫描
			for (float y = y2 + 0.5f; y >= y1 - 0.5f; --y)
			{
				//计算出直线 y = y 与 三角形相交2点的x坐标

				//float k = (p[2].y - p[0].y) / (p[2].x - p[0].x);
				//float b = p[0].y - k * p[0].x;
				//float x1 = ((float)y - b) / k;

				float x1 = (y + 0.5f - p[0].y) * (p[2].x - p[0].x) / (p[2].y - p[0].y) + p[0].x;
				float x2 = 0;

				if (y >= p[1].y)
				{
					//y减0.5f是为了矫正斜率
					x2 = (y - 0.5f - p[0].y) * (p[1].x - p[0].x) / (p[1].y - p[0].y) + p[0].x;
				}
				else
				{
					x2 = (y + 0.5f - p[1].y) * (p[2].x - p[1].x) / (p[2].y - p[1].y) + p[1].x;
				}

				if (x1 > x2)
				{
					float temp = x1;
					x1 = x2;
					x2 = temp;
				}

				x1 = Clamp(x1, 0, (float)context.fragment_buffer_view.w-1);
				x2 = Clamp(x2, 0, (float)context.fragment_buffer_view.w-1);

				for (int x = (int)x1 - 1; x <= (int)x2 + 1; ++x)
				{
					PixelOperate(x, (int)y, triangle);
				}
			}

		}


		void PixelOperate(int x, int y, VS_OUT triangle[3])
		{
			using gmath::Utility::Lerp;
			using gmath::Utility::BlendColor;

			//MSAA4x
			float cover_count = 0;
			float Mn = 2;
			Vec3 aa_rate = {};

			for (float i = 0; i < Mn; ++i)
			{
				for (float j = 0; j < Mn; ++j)
				{
					//对插值系数进行多次采样，而不是多次插值
					Vec3 ratio = Impl::get_interpolation_rate(
						x + (i + 0.5f) / (Mn + 1),
						y + (j + 0.5f) / (Mn + 1),
						triangle);

					//三个系数也刚好可以判断点是不是在三角形内
					if (ratio.x > 1e-8 && ratio.y > 1e-8 && ratio.z > 1e-8)
					{
						aa_rate += ratio;
						++cover_count;
					}
				}
			}

			//覆盖测试
			if (!cover_count)
			{
				return;
			}

			aa_rate /= cover_count;
			VS_OUT interp = triangle[0] * aa_rate.x + triangle[1] * aa_rate.y + triangle[2] * aa_rate.z;
			float depth = 1 / interp.position.z;
			float depth0 = context.depth_buffer_view.Get(x, y);

			//深度测试
			if (!(depth > depth0 - 1e-10))
			{
				return;
			}

			Color color = material.FS(interp);
			Color color0 = context.fragment_buffer_view.Get(x, y);

			//AA上色
			if ((cover_count < Mn * Mn) && (fabs(depth - depth0) > 1e-4)) {
				float a = color.a;
				color = Lerp(color, color0, cover_count / (Mn * Mn));
				color.a = a;
			}

			//颜色混合
			if (color.a < 0.99999f)
			{
				color = BlendColor(color0, color);
			}

			//写入fragment_buffer
			context.fragment_buffer_view.Set(x, y, color);
			//写入depth_buffer
			context.depth_buffer_view.Set(x, y, depth);

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

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
	static constexpr float epsilon = 1e-20f;

	//默认的顶点类,只有位置和颜色2个属性
	struct Vertex_Default
	{
		Position position;
		Color color;

		//顶点类需要实现+和*标量, 两个方法
		Vertex_Default operator+(const Vertex_Default& rhs) const
		{
			return { position + rhs.position,color + rhs.color };
		}
		friend Vertex_Default operator*(const Vertex_Default& lhs, float rhs)
		{
			return { lhs.position * rhs,lhs.color * rhs };
		}
	};

	//渲染上下文
	class Context
	{
	public:
		Buffer2DView<Color> fragment_buffer_view;
		Buffer2DView<float> depth_buffer_view;

		void CopyToBuffer(Buffer2DView<uint32>& screen_buffer_view)
		{
			if (!fragment_buffer_view.buffer)
			{
				return;
			}

			auto w = screen_buffer_view.w;
			auto h = screen_buffer_view.h;

			for (size_t y = 0; y < h; ++y)
			{
				for (size_t x = 0; x < w; ++x)
				{
					screen_buffer_view.Set(x, y, TransFloat4colorToUint32color(fragment_buffer_view.Get(x, y)).bgra);
				}
			}
		}

		void Viewport(size_t w, size_t h, Color color = { 0,0,0,1 })
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

		static Color32 TransFloat4colorToUint32color(const Color& color)
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
		Context() : fragment_buffer{}, depth_buffer{}, fragment_buffer_view{ nullptr }, depth_buffer_view{ nullptr }{};
	protected:
		std::vector<Color> fragment_buffer;
		std::vector<float> depth_buffer;
	};

	//默认材质
	class Material_Default
	{
	public:
		using VS_IN = Vertex_Default;
		using VS_OUT = Vertex_Default;
		VS_OUT VS(const VS_IN& v)
		{
			return v;
		}

		Color FS(const VS_OUT& v)
		{
			return v.color;
		}
	};

	//渲染器类
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

		Renderer(Context& ctx, const Material& m) :
			context{ ctx },
			material{ m }
		{
		}

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

		void DrawTriangle(VS_IN* p1, VS_IN* p2, VS_IN* p3)
		{
			//本地空间 => 裁剪空间 clip space
			VS_OUT triangle[8] = {
				{ material.VS(*p1) },
				{ material.VS(*p2) },
				{ material.VS(*p3) }
			};

			//简单CVV剔除
			//if (SimpleCull(triangle)) return;
			//CVV剔除
			if (CVVCull(triangle)) return;
			//CVV裁剪
			VS_OUT polygon[8] = {};
			size_t len = CVVClip(triangle, polygon);

			if (len < 3)
			{
				return;
			}

			//转化为归一化设备坐标ndc
			for (auto& v : polygon)
			{
				//v.position /= v.position.w;
				v.position.x /= v.position.w;
				v.position.y /= v.position.w;
				v.position.z /= v.position.w;
				//保留w信息
			}

			//转化为屏幕坐标 screen space
			TransToScreenSpace(polygon, len);

			//剔除背面(简易剔除)
			if (IsBackface(polygon))
			{
				return;
			}

			//渲染第一个三角形
			Rasterize_LineScanning(polygon);

			//渲染后面的三角形
			for (size_t i = 3; i < len; ++i)
			{
				triangle[0] = polygon[0];
				triangle[1] = polygon[i - 1];
				triangle[2] = polygon[i];
				//Rasterize_AABB(triangle);
				Rasterize_LineScanning(triangle);
			}
		}
	protected:

		//使用AABB包围盒进行光栅化
		void Rasterize_AABB(VS_OUT  triangle[3])
		{
			//生成AABB包围盒
			int left = 1e8, right = -1e8, top = 1e8, bottom = -1e8;

			for (int i = 0; i < 3; ++i) {
				if (left > triangle[i].position.x)
				{
					left = (uint32)triangle[i].position.x;
				}
				else if (right < triangle[i].position.x)
				{
					right = (uint32)triangle[i].position.x + 1;
				}

				if (top < triangle[i].position.y)
				{
					top = (uint32)triangle[i].position.y + 1;
				}
				else if (bottom > triangle[i].position.y)
				{
					bottom = (uint32)triangle[i].position.y;
				}
			}
			using gmath::Utility::Clamp;
			int w = context.fragment_buffer_view.w;
			int h = context.fragment_buffer_view.h;

			left = Clamp(left, 0, w - 1);
			right = Clamp(right, 0, w - 1);
			top = Clamp(top, 0, h - 1);
			bottom = Clamp(bottom, 0, h - 1);

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

				x1 = Clamp(x1, 0, (float)context.fragment_buffer_view.w - 1);
				x2 = Clamp(x2, 0, (float)context.fragment_buffer_view.w - 1);

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

			//简单MSAA，不想建simpler
			float cover_count = 0;
			float Mn = 2;

			for (float i = 0; i < Mn; ++i)
			{
				for (float j = 0; j < Mn; ++j)
				{
					//对插值系数进行多次采样，而不是多次插值
					Vec3 weight = GetInterpolationWeight(
						x + (i + 0.5f) / (Mn + 1),
						y + (j + 0.5f) / (Mn + 1),
						triangle);

					//三个系数也刚好可以判断点是不是在三角形内
					if (weight.x > epsilon && weight.y > epsilon && weight.z > epsilon)
					{
						++cover_count;
					}
				}
			}

			//覆盖测试
			if (!cover_count)
			{
				return;
			}

			//取中心像素的重心坐标
			Vec3 weight = GetInterpolationWeight(x + 0.5, y + 0.5, triangle);
			//对插值进行透视修复
			weight.x /= triangle[0].position.w;
			weight.y /= triangle[1].position.w;
			weight.z /= triangle[2].position.w;
			weight /= (weight.x + weight.y + weight.z);

			//求插值
			VS_OUT interp = triangle[0] * weight.x + triangle[1] * weight.y + triangle[2] * weight.z;
			float depth = 1 / (interp.position.w * interp.position.w);
			float depth0 = context.depth_buffer_view.Get(x, y);

			//深度测试
			if (!(depth > depth0 - 1e-8))
			{
				return;
			}

			Color color = material.FS(interp);
			Color color0 = context.fragment_buffer_view.Get(x, y);

			//AA上色
			if ((cover_count < Mn * Mn) && (fabs(depth - depth0) > 1e-8)) {
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

		bool SimpleCull(VS_OUT triangle[3])
		{
			for (size_t i = 0; i < 3; ++i)
			{
				const auto& v = triangle[i];
				if (v.position.z < 0 ||
					v.position.w < epsilon ||
					v.position.z > v.position.w ||
					v.position.x > v.position.w ||
					v.position.x < -v.position.w ||
					v.position.y > v.position.w ||
					v.position.y < -v.position.w
					)
				{
					return true;
				}
			}
			return false;
		}

		//如果三个点都在CVV之外,直接剔除
		bool CVVCull(VS_OUT triangle[3])
		{
			float w0 = triangle[0].position.w;
			float w1 = triangle[1].position.w;
			float w2 = triangle[2].position.w;
			float z0 = triangle[0].position.z;
			float z1 = triangle[1].position.z;
			float z2 = triangle[2].position.z;
			float x0 = triangle[0].position.x;
			float x1 = triangle[1].position.x;
			float x2 = triangle[2].position.x;
			float y0 = triangle[0].position.y;
			float y1 = triangle[1].position.y;
			float y2 = triangle[2].position.y;

			if (w0 < epsilon || w1 < epsilon || w2 < epsilon ||
				z0 < 0 && z1 < 0 && z2 < 0 ||
				z0 > w0 && z1 > w1 && z2 > w2 ||
				x0 > w0 && x1 > w1 && x2 > w2 ||
				x0 < -w0 && x1 < -w1 && x2 < -w2 ||
				y0 > w0 && y1 > w1 && y2 > w2 ||
				y0 < -w0 && y1 < -w1 && y2 < -w2
				)
			{
				return true;
			}
			return false;
		}

		bool IsInside(VS_OUT* p, int plane)
		{
			float x = p->position.x;
			float y = p->position.y;
			float z = p->position.z;
			float w = p->position.w;

			switch (plane)
			{
			case 'z>0':  return z > 0;
			case 'w>e':  return w > epsilon;
			case 'z<w':  return z < w;
			case 'x<w':  return x < w;
			case 'x>-w': return x > -w;
			case 'y<w':  return y < w;
			case 'y>-w': return y > -w;
			}
			return false;
		}

		VS_OUT GetClipIntersection(VS_OUT* p1, VS_OUT* p2, int plane)
		{
			//Q[x,y,z,w] = P1[x1,y1,z1,w1] + (P2[x2,y2,z2,w2] - P1[x1,y1,z1,w1]) * t
			float t = 0;
			float x1 = p1->position.x;
			float x2 = p2->position.x;
			float y1 = p1->position.y;
			float y2 = p2->position.y;
			float z1 = p1->position.z;
			float z2 = p2->position.z;
			float w1 = p1->position.w;
			float w2 = p2->position.w;

			switch (plane)
			{
			case 'z>0':  t = z1 / (z1 - z2); break;
			case 'w>e':  t = (epsilon - w1) / (w2 - w1); break;
			case 'z<w':  t = (w1 - z1) / ((z2 - z1) - (w2 - w1)); break;
			case 'x<w':  t = (w1 - x1) / ((x2 - x1) - (w2 - w1)); break;
			case 'x>-w': t = -(w1 + x1) / ((x2 - x1) + (w2 - w1)); break;
			case 'y<w':  t = (w1 - y1) / ((y2 - y1) - (w2 - w1)); break;
			case 'y>-w': t = -(w1 + y1) / ((y2 - y1) + (w2 - w1)); break;
			}

			return (*p1) * (1 - t) + (*p2) * t;
		}

		size_t ClipAgainstPlane(VS_OUT* polygon_in, size_t len, VS_OUT* polygon_out, int plane)
		{
			size_t len_out = 0;
			for (size_t i = 0; i < len; ++i)
			{
				VS_OUT* p1 = polygon_in + i;
				VS_OUT* p2 = polygon_in + ((i + 1) % len);
				//sutherland_hodgman算法
				//检查p1, p2 是否在内侧,
				int b_p1_inside = IsInside(p1, plane);
				int b_p2_inside = IsInside(p2, plane);
				int sh_flag = b_p2_inside << 1 | b_p1_inside;

				//0: 如果, p1在外侧, p2在外侧, 什么都不做
				//1: 如果, p1在内侧, p2在外侧, 求交点q,  add q
				//2: 如果, p1在外侧, p2在内侧, 求交点q， add q, p2
				//3: 如果, p1、p2都在内测, add p2

				switch (sh_flag)
				{
				case 1:
				{
					VS_OUT q = GetClipIntersection(p1, p2, plane);
					polygon_out[len_out++] = q;
					break; //调试了一宿，才发现是忘了break;
				}
				case 2:
				{
					VS_OUT q = GetClipIntersection(p1, p2, plane);
					polygon_out[len_out++] = q;
					polygon_out[len_out++] = *p2;
					break;
				}
				case 3:
				{
					polygon_out[len_out++] = *p2;
					break;
				}
				};
			}
			return len_out;
		}

		//三角形与CVV相交, 裁剪并计算插值
		size_t CVVClip(VS_OUT* polygon_in, VS_OUT* polygon_out)
		{
			size_t len = 3;
			len = ClipAgainstPlane(polygon_in, len, polygon_out, 'z>0');
			len = ClipAgainstPlane(polygon_out, len, polygon_in, 'w>e');
			len = ClipAgainstPlane(polygon_in, len, polygon_out, 'z<w');
			len = ClipAgainstPlane(polygon_out, len, polygon_in, 'x<w');
			len = ClipAgainstPlane(polygon_in, len, polygon_out, 'x>-w');
			len = ClipAgainstPlane(polygon_out, len, polygon_in, 'y<w');
			len = ClipAgainstPlane(polygon_in, len, polygon_out, 'y>-w');
			return len;
		}

		void TransToScreenSpace(VS_OUT* polygon, size_t len) const
		{
			for (int i = 0; i < len; ++i)
			{
				auto& vertex = polygon[i];
				vertex.position.x /= 2.f;
				vertex.position.y /= 2.f;
				vertex.position.x += 0.5f;
				vertex.position.y += 0.5f;
				vertex.position.x *= context.fragment_buffer_view.w;
				vertex.position.y *= context.fragment_buffer_view.h;
			}
		}

		bool IsBackface(VS_OUT* triangle) const
		{
			Vec2 a = triangle[1].position - triangle[0].position;
			Vec2 b = triangle[2].position - triangle[1].position;
			//Vec2 c = triangle[0].position - triangle[2].position;
			return a.cross(b) < 0;//&& b.cross(c) > 0 && c.cross(a) > 0;
		}

		bool IsPointInTriangle2D(float x, float y, VS_OUT* triangle) const
		{
			Vec2 p = { x,y };
			Vec2 pa = (Vec2)triangle[0].position - p;
			Vec2 pb = (Vec2)triangle[1].position - p;
			Vec2 pc = (Vec2)triangle[2].position - p;;

			return (pa.cross(pb) > 0 && pb.cross(pc) > 0 && pc.cross(pa) > 0) || (pa.cross(pb) < 0 && pb.cross(pc) < 0 && pc.cross(pa) < 0);
		}

		//获取插值
		Vec3 GetInterpolationWeight(float x, float y, VS_OUT* triangle) const
		{
			//t*p0+u*p1+v*p2=p, t=1-u-v;
			float a1 = triangle[1].position.x - triangle[0].position.x;
			float b1 = triangle[2].position.x - triangle[0].position.x;
			float c1 = x - triangle[0].position.x;
			float a2 = triangle[1].position.y - triangle[0].position.y;
			float b2 = triangle[2].position.y - triangle[0].position.y;
			float c2 = y - triangle[0].position.y;
			float v = (a1 * c2 - a2 * c1) / (b2 * a1 - a2 * b1);
			float u = (b2 * c1 - b1 * c2) / (b2 * a1 - a2 * b1);
			float t = 1 - u - v;
			return { t,u,v };
		}

	protected:
		Context& context;
		const Material& material;
	};
}

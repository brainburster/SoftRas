#pragma once

#include "context.hpp"

namespace core
{
	//默认着色器
	class Shader_Default
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

	enum ERenderFlag
	{
		RF_CULL_FRONT = 1,
		RF_CULL_BACK = 2,
		RF_CULL_CVV_SIMPLE = 4,
		RF_CULL_CVV_CLIP = 8,
		RF_ENABLE_MULTI_THREAD = 16,
		RF_ENABLE_SIMPLE_AA = 32,
		RF_ENABLE_BLEND = 64,
		RF_ENABLE_DEPTH_TEST = 128,
		//...
		RF_DEFAULT = RF_CULL_BACK | RF_CULL_CVV_CLIP | RF_ENABLE_MULTI_THREAD | RF_ENABLE_BLEND | RF_ENABLE_DEPTH_TEST,
		RF_DEFAULT_AA = RF_DEFAULT | RF_ENABLE_SIMPLE_AA
	};

	//渲染器类
	template<typename Shader = Shader_Default, size_t render_flag = RF_DEFAULT>
	class Renderer
	{
	private:
		//用来萃取顶点/像素着色器的输入类型
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) const volatile) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) volatile) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) const) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In)) {}

	public:
		using attribute_t = std::decay_t<decltype(get_in_type<Shader>(std::declval<decltype(&Shader::VS)>()))>;
		using varying_t = std::decay_t<decltype(get_in_type<Shader>(std::declval<decltype(&Shader::FS)>()))>;

		Renderer(Context& ctx, const Shader& m) :
			context{ ctx },
			shader{ m }
		{
		}

		void DrawIndex(attribute_t* data, size_t* index, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{
				DrawTriangle(data + index[i], data + index[i + 1], data + index[i + 2]);
			}
		}

		void DrawTriangles(attribute_t* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{
				DrawTriangle(data + i, data + i + 1, data + i + 2);
			}
		}

		void DrawTriangle(attribute_t* p0, attribute_t* p1, attribute_t* p2)
		{
			if constexpr (bool(render_flag & RF_CULL_CVV_SIMPLE))
			{
				//本地空间 => 裁剪空间 clip space
				varying_t triangle[3] = {
					{ shader.VS(*p0) },
					{ shader.VS(*p1) },
					{ shader.VS(*p2) }
				};
				//简单CVV剔除
				if (SimpleCull(triangle)) return;
				RasterizeTriangle(triangle, triangle + 1, triangle + 2);
			}
			else if constexpr (bool(render_flag & RF_CULL_CVV_CLIP)) {
				//本地空间 => 裁剪空间 clip space
				varying_t triangle[8] = {
					{ shader.VS(*p0) },
					{ shader.VS(*p1) },
					{ shader.VS(*p2) }
				};
				//CVV剔除
				if (CVVCull(triangle)) return;
				//CVV裁剪
				varying_t polygon[8] = {};
				size_t len = CVVClip(triangle, polygon);
				if (len < 3)
				{
					return;
				}

				//渲染第一个三角形
				RasterizeTriangle(polygon, polygon + 1, polygon + 2);

				//渲染后面的三角形
				for (size_t i = 3; i < len; ++i)
				{
					RasterizeTriangle(polygon, polygon + i - 1, polygon + i);
				}
			}
		}
	protected:

		void RasterizeTriangle(varying_t* p0, varying_t* p1, varying_t* p2)
		{
			//获得ndc下的三角形三个顶点 (clip space => ndc)
			Vec2 p[3] = {
				p0->position / p0->position.w,
				p1->position / p1->position.w,
				p2->position / p2->position.w,
			};

			//转化为屏幕坐标 screen space
			TransToScreenSpace(p, 3);

			if constexpr (bool(render_flag & RF_CULL_BACK))
			{
				//剔除背面
				if (IsBackface(p))
				{
					return;
				}
			}
			if constexpr (bool(render_flag & RF_CULL_FRONT))
			{
				//剔除前面
				if (!IsBackface(p))
				{
					return;
				}
			}

			//生成AABB包围盒
			float left = inf, right = -inf, top = -inf, bottom = inf;

			for (const auto& q : p) {
				if (left > q.x)
				{
					left = q.x;
				}
				if (right < q.x)
				{
					right = q.x;
				}

				if (top < q.y)
				{
					top = q.y;
				}
				if (bottom > q.y)
				{
					bottom = q.y;
				}
			}

			//对三个顶点按y坐标从高到底进行排序

			Vec2 q[3] = { p[0],p[1],p[2] };

			for (size_t i = 0; i < 2; ++i)
			{
				for (size_t j = i; j < 3; ++j)
				{
					if (q[i].y < q[j].y)
					{
						Vec2 temq = q[j];
						q[j] = q[i];
						q[i] = temq;
					}
				}
			}
			using gmath::utility::Clamp;
			float y1 = Clamp(q[2].y, 1.f, context.back_buffer_view.h - 1.f);
			float y2 = Clamp(q[0].y, 1.f, context.back_buffer_view.h - 1.f);

			//从上到下扫描
			for (float y = y2; y >= y1 - 1; --y)
			{
				//隔行扫描
				//if (((size_t)y & 1) == context.interlaced_scanning_flag) continue;

				//计算出直线 y = y 与 三角形相交2点的x坐标
				//float k = (q[2].y - q[0].y) / (q[2].x - q[0].x);
				//float b = q[0].y - k * q[0].x;
				//float x1 = ((float)y - b) / k;

				float x1 = (y + 0.5f - q[0].y) * (q[2].x - q[0].x) / (q[2].y - q[0].y) + q[0].x;
				float x2 = 0;

				if (y + 0.5f > q[1].y)
				{
					x2 = (y + 0.5f - q[0].y) * (q[1].x - q[0].x) / (q[1].y - q[0].y) + q[0].x;
				}
				else
				{
					x2 = (y + 0.5f - q[1].y) * (q[2].x - q[1].x) / (q[2].y - q[1].y) + q[1].x;
				}

				x1 = Clamp(x1, 1.f, (float)context.back_buffer_view.w - 2.f);
				x2 = Clamp(x2, 1.f, (float)context.back_buffer_view.w - 2.f);

				if (x1 > x2)
				{
					float temq = x1;
					x1 = x2;
					x2 = temq;
				}
				if (x1 - x2 < epsilon)
				{
					x1 = left;
					x2 = right;
				}
				int start = (int)x1;
				int end = (int)x2;

				if constexpr (bool(render_flag & RF_ENABLE_MULTI_THREAD))
				{
#pragma omp parallel for num_threads(4)
					for (int x = start; x <= end; ++x)
					{
						PixelProcessing(x, (int)y, p, p0, p1, p2);
					}
				}
				else
				{
					for (int x = start; x <= end; ++x)
					{
						PixelProcessing(x, (int)y, p, p0, p1, p2);
					}
				}
			}
		}

		void PixelProcessing_AA(int x, int y, Vec2* triangle, varying_t* p0, varying_t* p1, varying_t* p2)
		{
			//简易抗锯齿
			float cover_count = 0;
			constexpr size_t Mn = 9;

			constexpr Vec2 simpler[9] = {
				 Vec2{-.15f,.35f},Vec2{.35f,.15f},Vec2{.15f,-.35f},Vec2{-.35f,-.15f},
				 Vec2{-.45f,.05f},Vec2{.05f,.45f},Vec2{.45f,-.05f},Vec2{-.05f,-.45f},
				 Vec2{.0f,.0f}
			};

			constexpr float simpler_weight[9] = {
				5,5,5,5,
				4,4,4,4,
				9
			};

			constexpr float simpler_weight_sum[9] = {
				5,10,15,20,
				24,28,32,36,
				45
			};

			for (size_t i = 0; i < Mn; ++i)
			{
				Vec3 temp = GetInterpolationWeight(
					(float)x + 0.5f + simpler[i].x,
					(float)y + 0.5f + simpler[i].y,
					triangle);

				if (temp.x > -epsilon && temp.y > -epsilon && temp.z > -epsilon)
				{
					cover_count += simpler_weight[i];
				}
			}

			if (cover_count < epsilon)
			{
				return;
			}

			//取中心像素的中心坐标的三角形重心坐标
			Vec3 weight = GetInterpolationWeight(x + 0.5f, y + 0.5f, triangle);

			//对重心坐标进行透视修复
			weight.x /= p0->position.w;
			weight.y /= p1->position.w;
			weight.z /= p2->position.w;
			weight /= (weight.x + weight.y + weight.z);

			//求z轴插值
			Vec4 pos = p0->position * weight.x + p1->position * weight.y + p2->position * weight.z;
			float depth = pos.z / pos.w; //储存归一化z值
			float depth0 = context.depth_buffer_view.Get(x, y);

			if (fabs(depth - depth0) < 0.0005f / pos.w)
			{
				cover_count = 1e8f;
			}
			else
			{
				//深度测试
				if constexpr (bool(render_flag & RF_ENABLE_DEPTH_TEST))
				{
					if (depth > depth0)
					{
						return;
					}
				}
			}

			varying_t interp = *p0 * weight.x + *p1 * weight.y + *p2 * weight.z;
			Color color = shader.FS(interp);
			Color color0 = context.back_buffer_view.Get(x, y);

			using gmath::utility::Lerp;
			using gmath::utility::BlendColor;

			if (cover_count < simpler_weight_sum[Mn - 1])
			{
				float t = cover_count / simpler_weight_sum[Mn - 1];
				float a = color.a;
				color = Lerp(color0, color, t);
				color.a = a;
			}

			if constexpr (bool(render_flag & RF_ENABLE_BLEND))
			{
				//颜色混合
				if (color.a < (1.f - epsilon))
				{
					color = BlendColor(color0, color);
				}
			}

			//写入fragment_buffer
			context.back_buffer_view.Set(x, y, color);
			//写入depth_buffer
			context.depth_buffer_view.Set(x, y, depth);
		}

		void PixelProcessing_NoAA(int x, int y, Vec2* triangle, varying_t* p0, varying_t* p1, varying_t* p2)
		{
			Vec3 weight = GetInterpolationWeight(x + 0.5f, y + 0.5f, triangle);

			//判断是否在三角形内
			if (weight.x < epsilon || weight.y < epsilon || weight.z < epsilon)
			{
				return;
			}

			//对插值进行透视修复
			weight.x /= p0->position.w;
			weight.y /= p1->position.w;
			weight.z /= p2->position.w;
			weight /= (weight.x + weight.y + weight.z);

			//求插值
			varying_t interp = *p0 * weight.x + *p1 * weight.y + *p2 * weight.z;
			float depth = interp.position.z / interp.position.w;

			//深度测试
			if constexpr (bool(render_flag & RF_ENABLE_DEPTH_TEST))
			{
				float depth0 = context.depth_buffer_view.Get(x, y);
				if (depth > depth0)
				{
					return;
				}
				//写入depth_buffer
				context.depth_buffer_view.Set(x, y, depth);
			}

			Color color = shader.FS(interp);

			if constexpr (bool(render_flag & RF_ENABLE_BLEND))
			{
				//颜色混合
				if (color.a < (1.f - epsilon))
				{
					Color color0 = context.back_buffer_view.Get(x, y);
					color = gmath::utility::BlendColor(color0, color);
				}
			}
			//写入fragment_buffer
			context.back_buffer_view.Set(x, y, color);
		}
		void PixelProcessing(int x, int y, Vec2* triangle, varying_t* p0, varying_t* p1, varying_t* p2)
		{
			if constexpr (bool(render_flag & RF_ENABLE_SIMPLE_AA))
			{
				PixelProcessing_AA(x, y, triangle, p0, p1, p2);
			}
			else
			{
				PixelProcessing_NoAA(x, y, triangle, p0, p1, p2);
			}
		}

		bool SimpleCull(varying_t triangle[3])
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
		bool CVVCull(varying_t triangle[3])
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

			if (w0 < epsilon && w1 < epsilon && w2 < epsilon ||
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

		bool IsInside(varying_t* p, int plane)
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

		varying_t GetClipIntersection(varying_t* p0, varying_t* p1, int plane)
		{
			//Q[x,y,z,w] = P1[x1,y1,z1,w1] + (P2[x2,y2,z2,w2] - P1[x1,y1,z1,w1]) * t
			float t = 0;
			float x1 = p0->position.x;
			float x2 = p1->position.x;
			float y1 = p0->position.y;
			float y2 = p1->position.y;
			float z1 = p0->position.z;
			float z2 = p1->position.z;
			float w1 = p0->position.w;
			float w2 = p1->position.w;

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

			return (*p0) * (1 - t) + (*p1) * t;
		}

		size_t ClipAgainstPlane(varying_t* polygon_in, size_t len, varying_t* polygon_out, int plane)
		{
			size_t len_out = 0;
			for (size_t i = 0; i < len; ++i)
			{
				varying_t* p0 = polygon_in + i;
				varying_t* p1 = polygon_in + ((i + 1) % len);
				//sutherland_hodgman算法
				//检查p0, p1 是否在内侧,
				int b_p0_inside = IsInside(p0, plane);
				int b_p1_inside = IsInside(p1, plane);
				int sh_flag = b_p1_inside << 1 | b_p0_inside;

				//0: 如果, p0在外侧, p1在外侧, 什么都不做
				//1: 如果, p0在内侧, p1在外侧, 求交点q,  add q
				//2: 如果, p0在外侧, p1在内侧, 求交点q， add q, p1
				//3: 如果, p0、p1都在内测, add p1

				switch (sh_flag)
				{
				case 1:
				{
					varying_t q = GetClipIntersection(p0, p1, plane);
					polygon_out[len_out++] = q;
					break;
				}
				case 2:
				{
					varying_t q = GetClipIntersection(p0, p1, plane);
					polygon_out[len_out++] = q;
					polygon_out[len_out++] = *p1;
					break;
				}
				case 3:
				{
					polygon_out[len_out++] = *p1;
					break;
				}
				};
			}
			return len_out;
		}

		//三角形与CVV相交, 裁剪并计算插值
		size_t CVVClip(varying_t* polygon_in, varying_t* polygon_out)
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

		void TransToScreenSpace(varying_t* polygon, size_t len) const
		{
			for (int i = 0; i < len; ++i)
			{
				auto& vertex = polygon[i];
				vertex.position.x /= 2.f;
				vertex.position.y /= 2.f;
				vertex.position.x += 0.5f;
				vertex.position.y += 0.5f;
				vertex.position.x *= context.back_buffer_view.w;
				vertex.position.y *= context.back_buffer_view.h;
			}
		}

		void TransToScreenSpace(Vec2* triangle, size_t len) const
		{
			for (int i = 0; i < len; ++i)
			{
				auto& p = triangle[i];
				p.x /= 2.f;
				p.y /= 2.f;
				p.x += 0.5f;
				p.y += 0.5f;
				p.x *= context.back_buffer_view.w;
				p.y *= context.back_buffer_view.h;
			}
		}

		bool IsBackface(Vec2* triangle) const
		{
			Vec2 a = triangle[1] - triangle[0];
			Vec2 b = triangle[2] - triangle[1];
			//Vec2 c = triangle[0].position - triangle[2].position;
			return a.Cross(b) < 0;//&& b.Cross(c) > 0 && c.Cross(a) > 0;
		}

		bool IsPointInTriangle2D(float x, float y, varying_t* triangle) const
		{
			Vec2 p = { x,y };
			Vec2 pa = (Vec2)triangle[0].position - p;
			Vec2 pb = (Vec2)triangle[1].position - p;
			Vec2 pc = (Vec2)triangle[2].position - p;;

			return (pa.Cross(pb) > 0 && pb.Cross(pc) > 0 && pc.Cross(pa) > 0) || (pa.Cross(pb) < 0 && pb.Cross(pc) < 0 && pc.Cross(pa) < 0);
		}

		//获取插值
		Vec3 GetInterpolationWeight(float x, float y, Vec2* triangle) const
		{
			//t*p0+u*p0+v*p1=p, t=1-u-v;
			float a1 = triangle[1].x - triangle[0].x;
			float b1 = triangle[2].x - triangle[0].x;
			float c1 = x - triangle[0].x;
			float a2 = triangle[1].y - triangle[0].y;
			float b2 = triangle[2].y - triangle[0].y;
			float c2 = y - triangle[0].y;
			float v = (a1 * c2 - a2 * c1) / (b2 * a1 - a2 * b1);
			float u = (b2 * c1 - b1 * c2) / (b2 * a1 - a2 * b1);
			float t = 1 - u - v;
			return { t,u,v };
		}

	protected:
		Context& context;
		const Shader& shader;
	};
}

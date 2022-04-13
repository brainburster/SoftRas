#pragma once

#include "context.hpp"

namespace core
{
	//默认着色器，什么都没干
	class ShaderDefault
	{
	public:
		// Vertex_Default 继承自 vs_out_base 模板类，这个模板类重载了加法与乘法
		Vertex_Default VS(const Vertex_Default& v)
		{
			return v;
		}

		Color FS(const Vertex_Default& v)
		{
			return v.color;
		}
	};

	enum ERenderFlag
	{
		RF_CULL_FRONT = 1, //剔除正面 顺时针(我定义的正面是顺时针)
		RF_CULL_BACK = 2,  //剔除反面 逆时针
		RF_CULL_CVV_SIMPLE = 4, //简单的CVV剔除，三角形的顶点只要有一个在CVV之外就全部丢弃掉
		RF_CULL_CVV_CLIP = 8,   //三角形3个顶点都在CVV外面的情况，全部丢弃
		//RF_ENABLE_MULTI_THREAD = 16,
		RF_ENABLE_SIMPLE_AA = 32, //简单的抗锯齿，不带采样点深度缓存的，建议不要用
		RF_ENABLE_BLEND = 64,     //打开透明度混合，
		RF_ENABLE_DEPTH_TEST = 128, //打开深度测试
		//...
		RF_DEFAULT = RF_CULL_BACK | RF_CULL_CVV_CLIP /*| RF_ENABLE_MULTI_THREAD */ | RF_ENABLE_BLEND | RF_ENABLE_DEPTH_TEST,
		RF_DEFAULT_AA = RF_DEFAULT | RF_ENABLE_SIMPLE_AA
	};

	//渲染器类
	template<typename Shader = ShaderDefault, size_t render_flag = RF_DEFAULT>
	class Renderer
	{
	private:
		//用来萃取顶点/像素着色器的输入、输出类型，利用了函数模板类型推导，得到了返回值和输入类型，这里是只有一个输入的情况，想要获得多个输入类型的信息可以使用可变参数模板，比如typename In..., 获得其中某个类型信息的可以使用std::tuple相关的方法
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) const volatile) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) volatile) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In) const) {}
		template <typename T, typename R, typename In>
		static In get_in_type(R(T::* f)(In)) {}
		template <typename T, typename R, typename In>
		static R get_out_type(R(T::* f)(In) const volatile) {}
		template <typename T, typename R, typename In>
		static R get_out_type(R(T::* f)(In) volatile) {}
		template <typename T, typename R, typename In>
		static R get_out_type(R(T::* f)(In) const) {}
		template <typename T, typename R, typename In>
		static R get_out_type(R(T::* f)(In)) {}

	public:
		using vs_in_t = std::decay_t<decltype(get_in_type<>(std::declval<decltype(&Shader::VS)>()))>; //declval是一个没有被实现的函数，它的返回值是一个T类型的引用，它仅仅应该出现在decltype中参与编译器类型推导
		using vs_out_t = std::decay_t<decltype(get_out_type<>(std::declval<decltype(&Shader::VS)>()))>;
		using fs_in_t = std::decay_t<decltype(get_in_type<>(std::declval<decltype(&Shader::FS)>()))>;
		using fs_out_t = std::decay_t<decltype(get_out_type<>(std::declval<decltype(&Shader::FS)>()))>;

		//断言shader的合法性,顶点着色器的输入类型必须与像素着色器的输出类型相同(可以被const修饰，可以为引用)，而且顶点着色器的输出类型必须CRTP得继承自vs_out_base，这个模板类重载了+和*，并使用sse做了加速（不过编译器好像本来就能加速这个）
		static_assert(std::is_base_of_v<vs_out_base<vs_out_t>, vs_out_t>, "the output type of vs_shader must be inherited from vs_out_base");
		static_assert(std::is_same_v<vs_out_t, fs_in_t>, "the output type of vs_shader must be the same as the input type of the fs_shader");

		Renderer(Context<fs_out_t>& ctx, const Shader& m) :
			context{ ctx },
			shader{ m }
		{
		}

		// 通过顶点数组和索引数组绘制三角形，n为索引数组长度
		void DrawIndex(vs_in_t* data, size_t* index, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{
				DrawTriangle(data + index[i], data + index[i + 1], data + index[i + 2]);
			}
		}

		// 绘制n/3个三角形
		void DrawTriangles(vs_in_t* data, size_t n)
		{
			for (size_t i = 0; i < n; i += 3)
			{
				DrawTriangle(data + i, data + i + 1, data + i + 2);
			}
		}

		// 绘制一个三角形
		void DrawTriangle(vs_in_t* p0, vs_in_t* p1, vs_in_t* p2)
		{
			if constexpr (bool(render_flag & RF_CULL_CVV_SIMPLE))
			{
				//本地空间 => 裁剪空间 clip space
				vs_out_t triangle[3] = {
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
				vs_out_t triangle[8] = {
					{ shader.VS(*p0) },
					{ shader.VS(*p1) },
					{ shader.VS(*p2) }
				};
				//CVV剔除
				if (CVVCull(triangle)) return;
				//CVV裁剪
				vs_out_t polygon[8] = {};
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
			else {
				//本地空间 => 裁剪空间 clip space
				vs_out_t triangle[3] = {
					{ shader.VS(*p0) },
					{ shader.VS(*p1) },
					{ shader.VS(*p2) }
				};
				RasterizeTriangle(triangle, triangle + 1, triangle + 2);
			}
		}
	protected:

		void RasterizeTriangle(vs_out_t* p0, vs_out_t* p1, vs_out_t* p2)
		{
			//获得ndc下的三角形三个顶点 (clip space => ndc)
			Vec2 p[3] = {
				p0->position / p0->position.w,
				p1->position / p1->position.w,
				p2->position / p2->position.w,
			};

			//转化为屏幕坐标 screen space
			NDCToScreenSpace(p, 3);

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
			int y1 = (int)Clamp(q[2].y, 1.f, context.back_buffer_view.h - 1.f);
			int y2 = (int)Clamp(q[0].y, 1.f, context.back_buffer_view.h - 1.f);

			//其实仔细想想，用一次循环来扫描行，并不是很高效，用标准做法分解成上下2个3角形效率更高一点，因为不用再循环里判断点到底与那条直线相交了，不过好歹是我自己想出来的，所以保留了
			//从上到下扫描
#pragma omp parallel for num_threads(6)
			for (int y = y2; y >= y1; --y)
			{
				//隔行扫描，就是奇数帧扫描奇数行，偶数帧扫描偶数行，按理说能节省一半着色开销
				//if (((size_t)y & 1) == context.interlaced_scanning_flag) continue;

				//计算出直线 y = y 与 三角形相交2点的x坐标
				//float k = (q[2].y - q[0].y) / (q[2].x - q[0].x);
				//float b = q[0].y - k * q[0].x;
				//float x1 = ((float)y - b) / k;

				//直接把点带入直线方程
				float x1 = ((float)y + 0.5f - q[0].y) * (q[2].x - q[0].x) / (q[2].y - q[0].y) + q[0].x;
				float x2 = 0;

				if (y > q[1].y - 0.5f)
				{
					x2 = ((float)y + 0.5f - q[0].y) * (q[1].x - q[0].x) / (q[1].y - q[0].y) + q[0].x;
				}
				else
				{
					x2 = ((float)y + 0.5f - q[1].y) * (q[2].x - q[1].x) / (q[2].y - q[1].y) + q[1].x;
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

				for (int x = start; x <= end; ++x)
				{
					PixelProcessing(x, y, p, p0, p1, p2);
				}
			}
		}

		//像素着色过程，采用了简单的抗锯齿算法，利用了MSAA的思想，不过没有增加采样点深度buffer信息，所以面片之间的显示会出一些问题，混合模式改成线性叠加（还没实现）或许能够解决一部分问题
		void PixelProcessing_AA(int x, int y, Vec2* triangle, vs_out_t* p0, vs_out_t* p1, vs_out_t* p2)
		{
			static_assert(std::is_same_v<Color, fs_out_t>, "Error: 不支持GBuffer");
			//简易抗锯齿
			float cover_count = 0;
			constexpr size_t Mn = 9;

			//采样点
			constexpr Vec2 simpler[9] = {
				 Vec2{-.15f,.35f},Vec2{.35f,.15f},Vec2{.15f,-.35f},Vec2{-.35f,-.15f},
				 Vec2{-.45f,.05f},Vec2{.05f,.45f},Vec2{.45f,-.05f},Vec2{-.05f,-.45f},
				 Vec2{.0f,.0f}
			};

			//采样点权重
			constexpr float simpler_weight[9] = {
				5,5,5,5,
				4,4,4,4,
				9
			};

			//权重之和，（最后要除）
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

			vs_out_t interp = *p0 * weight.x + *p1 * weight.y + *p2 * weight.z;
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

		// 不带AA的pixel processing
		void PixelProcessing_NoAA(int x, int y, Vec2* triangle, vs_out_t* p0, vs_out_t* p1, vs_out_t* p2)
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
			vs_out_t interp = *p0 * weight.x + *p1 * weight.y + *p2 * weight.z;
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

			fs_out_t fs_out = shader.FS(interp);

			if constexpr (std::is_same_v<Color, fs_out_t> && bool(render_flag & RF_ENABLE_BLEND))
			{
				//颜色混合
				if (fs_out.a < (1.f - epsilon))
				{
					Color color0 = context.back_buffer_view.Get(x, y);
					fs_out = gmath::utility::BlendColor(color0, fs_out);
				}
			}
			//写入fragment_buffer
			context.back_buffer_view.Set(x, y, fs_out);
		}
		void PixelProcessing(int x, int y, Vec2* triangle, vs_out_t* p0, vs_out_t* p1, vs_out_t* p2)
		{
			if constexpr (bool(render_flag & RF_ENABLE_SIMPLE_AA) && std::is_same_v<Color, fs_out_t>)
			{
				PixelProcessing_AA(x, y, triangle, p0, p1, p2);
			}
			else
			{
				PixelProcessing_NoAA(x, y, triangle, p0, p1, p2);
			}
		}

		//简单剔除，如果三角形有一个点在CVV之外，就全部剔除
		bool SimpleCull(vs_out_t triangle[3])
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

		//如果三角形三个点都在CVV之外,直接剔除
		bool CVVCull(vs_out_t triangle[3])
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

		// 在某个平面内，这里的内指的是，该点和平面某点连线与平面法线的点乘为负
		bool IsInside(vs_out_t* p, int plane)
		{
			float x = p->position.x;
			float y = p->position.y;
			float z = p->position.z;
			float w = p->position.w;

			switch (plane)
			{
			case 'z>0':  return z > 0;	// 这个叫多字节字符字面量，其实直接用字符串字面量是一样的效果，这里是为了偷懒，为了不额外定义6个面的枚举
			case 'w>e':  return w > epsilon; // epsilon 是一个很小的量，直接在w=0平面上剔除，可能会导致原本z=0的顶点被剔除掉
			case 'z<w':  return z < w;
			case 'x<w':  return x < w;
			case 'x>-w': return x > -w;
			case 'y<w':  return y < w;
			case 'y>-w': return y > -w;
			}
			return false;
		}

		// 获得线段与cvv6个平面中某个平面的交点
		vs_out_t GetClipIntersection(vs_out_t* p0, vs_out_t* p1, int plane)
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

			//计算 p-p0 与 p1-p的 长度之比
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

			//根据长度之比，用相似三角形法则，通过插值计算出p点坐标
			return (*p0) * (1 - t) + (*p1) * t;
		}

		// 让多边形与某个平面裁剪，如果裁剪前是凸，的裁剪后也必定是凸的，返回值为产生的新的多边形的大小
		size_t ClipAgainstPlane(vs_out_t* polygon_in, size_t len, vs_out_t* polygon_out, int plane)
		{
			size_t len_out = 0;
			for (size_t i = 0; i < len; ++i)
			{
				vs_out_t* p0 = polygon_in + i;
				vs_out_t* p1 = polygon_in + ((i + 1) % len);
				//sutherland_hodgman算法 https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm-please-change-bmp-images-jpeg-png/
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
					vs_out_t q = GetClipIntersection(p0, p1, plane);
					polygon_out[len_out++] = q;
					break;
				}
				case 2:
				{
					vs_out_t q = GetClipIntersection(p0, p1, plane);
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

		//三角形与CVV相交，裁剪，并计算生成多边形的顶点数量
		size_t CVVClip(vs_out_t* polygon_in, vs_out_t* polygon_out)
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

		// 将三角形顶点从, NDC变换到屏幕坐标系
		void NDCToScreenSpace(vs_out_t* polygon, size_t len) const
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

		// 将三角形顶点从, NDC变换到屏幕坐标系, 也可以用一个3x3矩阵来做，
		void NDCToScreenSpace(Vec2* triangle, size_t len) const
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

		// 判断正反面， 我这里设置顺时针为正面
		bool IsBackface(Vec2* triangle) const
		{
			Vec2 a = triangle[1] - triangle[0];
			Vec2 b = triangle[2] - triangle[1];
			//Vec2 c = triangle[0].position - triangle[2].position;
			return a.Cross(b) < 0;//&& b.Cross(c) > 0 && c.Cross(a) > 0;
		}

		// 利用叉乘判断点是否在三角形中
		bool IsPointInTriangle2D(float x, float y, vs_out_t* triangle) const
		{
			Vec2 p = { x,y };
			Vec2 pa = (Vec2)triangle[0].position - p;
			Vec2 pb = (Vec2)triangle[1].position - p;
			Vec2 pc = (Vec2)triangle[2].position - p;;

			return (pa.Cross(pb) > 0 && pb.Cross(pc) > 0 && pc.Cross(pa) > 0) || (pa.Cross(pb) < 0 && pb.Cross(pc) < 0 && pc.Cross(pa) < 0);
		}

		// 获取插值 三角形重心坐标插值
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
		Context<fs_out_t>& context; //这个fs_out_t可以是color也可以是Gbuffer
		const Shader& shader;
	};
}

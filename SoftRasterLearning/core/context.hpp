#pragma once

#include "dc_wnd.hpp"
#include "buffer_view.hpp"
#include "game_math.hpp"
#include "types_and_defs.hpp"
#include "omp.h"

namespace core
{
	//渲染上下文
	template<typename FsOut = Color>
	class Context
	{
	public:
		std::vector<FsOut> back_buffer;
		std::vector<float> depth_buffer;
		Buffer2DView<FsOut> back_buffer_view;
		Buffer2DView<float> depth_buffer_view;

		Context() : back_buffer{}, depth_buffer{}, back_buffer_view{ nullptr }, depth_buffer_view{ nullptr }{};
		Context(const Context&) = default;
		Context& operator=(const Context&) noexcept = default;
		Context(Context&& other) noexcept :
			back_buffer{ std::move(other.back_buffer) },
			depth_buffer{ std::move(other.depth_buffer) },
			back_buffer_view{ std::move(other.back_buffer_view) },
			depth_buffer_view{ std::move(other.depth_buffer_view) }
		{}
		Context& operator=(Context&& other) noexcept
		{
			if (this == &other) return *this;
			back_buffer = std::move(other.back_buffer);
			depth_buffer = std::move(other.depth_buffer);
			back_buffer_view = std::move(other.back_buffer_view);
			depth_buffer_view = std::move(other.depth_buffer_view);
		}

		void CopyToBuffer(Buffer2DView<uint32>& screen_buffer_view)
		{
			static_assert(std::is_same_v<FsOut, Color>, "Error: 只有颜色buffer能输出到屏幕");

			if (!back_buffer_view.buffer)
			{
				return;
			}

			size_t w = screen_buffer_view.w;
			size_t h = screen_buffer_view.h;

#pragma omp parallel for num_threads(8)
			for (int y = 0; y < h; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					screen_buffer_view.Set((size_t)x, y, TransFloat4colorToUint32color(back_buffer_view.Get(x, y).Pow(1.f / gamma)).color);
				}
			}
		}

		void Viewport(size_t w, size_t h)
		{
			depth_buffer.resize(w * h, inf);
			back_buffer.resize(w * h);
			back_buffer_view = { back_buffer.data(), w , h };
			depth_buffer_view = { depth_buffer.data(), w , h };
		}

		void Clear(FsOut fs_out)
		{
			std::for_each(back_buffer.begin(), back_buffer.end(), [&fs_out](auto& v) { v = fs_out; });
			std::for_each(depth_buffer.begin(), depth_buffer.end(), [](auto& v) { v = inf; });
		}

		void Clear()
		{
			std::for_each(back_buffer.begin(), back_buffer.end(), [](auto& v) { v = {}; });
			std::for_each(depth_buffer.begin(), depth_buffer.end(), [](auto& v) { v = inf; });
		}

		static Color32 TransFloat4colorToUint32color(const Color& color)
		{
			using gmath::utility::Clamp;
			//交换r,b通道
			return Color32{
				(unsigned char)(Clamp(color.b) * 255U),
				(unsigned char)(Clamp(color.g) * 255U),
				(unsigned char)(Clamp(color.r) * 255U),
				(unsigned char)(Clamp(color.a) * 255U)
			};
		}
	};
}

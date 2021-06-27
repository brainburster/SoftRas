#pragma once
#include "core/dc_wnd.hpp"

class SoftRasterApp : public wnd::DC_WND
{
private:
	struct InputState
	{
		bool key[256];
		struct MouseState
		{
			bool button[3];
			int x;
			int y;
			int dx;
			int dy;
			int scroll;
		} mouse_state;
	} input_state;

	SoftRasterApp(const SoftRasterApp& other) = delete;
	SoftRasterApp& operator=(const SoftRasterApp& other) = delete;
public:
	SoftRasterApp(HINSTANCE hinst) : DC_WND{ hinst }, input_state{} {}
	SoftRasterApp(SoftRasterApp&& other) noexcept : DC_WND(static_cast<DC_WND&&>(other)), input_state{ other.input_state }
	{
	}
	SoftRasterApp& operator=(SoftRasterApp&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}
		memcpy(this, &other, sizeof(SoftRasterApp));
		memset(&other, 0, sizeof(SoftRasterApp));
		return *this;
	}

	const InputState& GetInputeState() const
	{
		return input_state;
	}
};

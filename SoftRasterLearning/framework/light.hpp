#pragma once

#include "../core/game_math.hpp"

namespace framework
{
	enum class ELightType
	{
		DirectionalLight = 0,
		PointLight = 1,
		Spotlight = 2
	};

	class ILight
	{
	public:
		using Vec3 = gmath::Vec3<float>;
		using Vec4 = gmath::Vec4<float>;
		using Mat3 = gmath::Mat3x3<float>;
		using Mat4 = gmath::Mat4x4<float>;

		virtual ELightType GetLightType() = 0;
		virtual Vec3 GetColor() = 0;
		virtual Vec3 GetPosition() = 0;
		virtual Vec3 GetDirection() = 0;
		virtual float GetCutOff() = 0;
		virtual Mat4 GetLightMartrix() = 0;
	};
};

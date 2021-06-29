#pragma once

namespace framework
{
	class IRenderEngine;
	class IRenderAble;
	class Entity;

	class IMaterial
	{
	public:
		virtual void Render(const Entity*, IRenderEngine*) = 0;
		virtual ~IMaterial() = default;
	};
}

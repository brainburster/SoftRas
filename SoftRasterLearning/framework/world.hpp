#pragma once

#include "object.hpp"
#include <memory.h>

namespace framework
{
	class World
	{
	public:
		template<typename T>
		std::shared_ptr<Object> Spawn(core::Vec3 t = core::Vec3{ 0,0,0 }, core::Vec3 r = core::Vec3{ 0,0,0 }, core::Vec3 s = core::Vec3{ 1,1 ,1 })
		{
			std::shared_ptr<Object> object = std::make_shared<T>();
			everything.push_back(object);
			return object;
		}

		auto begin() const
		{
			return  everything.begin();
		}

		auto end() const
		{
			return everything.end();
		}

	protected:
		std::vector<std::shared_ptr<Object>> everything;
	};
}

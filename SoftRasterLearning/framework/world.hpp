#pragma once

#include "object.hpp"
#include <memory.h>

namespace framework
{
	class World
	{
	public:
		template<typename T>
		std::shared_ptr<Object> Spawn(sr::Vec3 t, sr::Vec3 r, sr::Vec3 s)
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

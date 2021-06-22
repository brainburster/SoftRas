#pragma once

#include <vector>
#include <string_view>
#include <fstream>
#include "game_math.hpp"

namespace loader
{
	struct Model_Vertex
	{
		gmath::Vec3<float> position;
		gmath::Vec3<float> normal;
		gmath::Vec2<float> uv;
	};

	class Model
	{

		//...
	private:
		std::vector<Model_Vertex> data;
	};

	namespace obj
	{
		Model parse(std::wstring_view sv )
		{
			return Model{};
		}


		Model LoadFromFile(const std::wstring& path)
		{
			using namespace std::string_view_literals;

			//...
			return parse(L""sv);
		}
	}

};

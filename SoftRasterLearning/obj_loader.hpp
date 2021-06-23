#pragma once

#include <vector>
#include <string_view>
#include <fstream>
#include <tuple>
#include <optional>
#include "game_math.hpp"
#include <type_traits>

namespace obj_loader
{
	using Vec2 = gmath::Vec2<float>;
	using Vec3 = gmath::Vec3<float>;
	using Vec4 = gmath::Vec4<float>;
	using SV = std::string_view;
	using STR = std::wstring;
	template<typename T> using Opt = std::optional<T>;

	struct Model_Vertex
	{
		Vec3 position;
		Vec2 uv;
		Vec3 normal;
	};

	struct Model
	{
		std::vector<Model_Vertex> mesh;

		Model() = default;
		Model(const Model&& other) noexcept: 
			mesh{std::move(other.mesh)}
		{}
		Model& operator=(Model&& other) noexcept
		{
			this->mesh = std::move(other.mesh);
		}
		//...
	};

	namespace obj
	{
		struct ObjParser
		{
			Model ParseObj(SV src)
			{
				std::vector<Vec3> position_buffer;
				std::vector<Vec3> normal_buffer;
				std::vector<Vec3> uv_buffer;
				std::vector<Model_Vertex> mesh;

				size_t guess = src.length() / 20;

				position_buffer.reserve(guess);
				normal_buffer.reserve(guess);
				uv_buffer.reserve(guess);
				mesh.reserve(guess);

				size_t len = src.length();
				for (size_t i = 0; i < len;)
				{
					size_t tail = src.find('\n', i);
					if (tail==std::string_view::npos) 
					{
						break;
					}
					SV line_sv = src.substr(i, tail);
					//½âÎöÐÐ
					ParseLine(line_sv);
					i = tail + 1;
				}

				return std::move(*reinterpret_cast<Model*>(&mesh));
			}

			static Opt <std::tuple<SV,SV>> MatchToken(SV src)
			{
				size_t space = src.find(' ');
				
				if (space==std::string_view::npos)
				{
					return std::nullopt;
				}

				return std::make_optional(
					std::make_tuple(
						src.substr(0, space - 1),
						src.substr(space + 1)
					)
				);
			}

			template<typename Parser1, typename... ParserList>
			constexpr static decltype(auto) Combine(Parser1 parser1, ParserList... parser_list)
			{
				return [=](SV src) -> Opt<std::tuple<
					std::decay_t<decltype(std::get<0>(parser1(src).value()))>,
					std::decay_t<decltype(std::get<0>(parser_list(src).value()))>...,
					SV>> 
				{
					auto ret0 = parser1(src);

					if (!ret0.has_value())
					{
						return std::nullopt;
					}

					auto [real_ret, src0] = ret0.value();

					if constexpr (sizeof...(ParserList) == 0)
					{
						return std::make_optional(std::make_tuple(
							real_ret,
							src0
						));
					}
					else
					{
						auto ret1 = Combine(parser_list...)(src0);

						if (!ret1.has_value())
						{
							return std::nullopt;
						}

						return std::make_optional(std::tuple_cat(
							std::make_tuple(real_ret),
							ret1.value()
						));
					}
				};
			}

			void ParseLine(SV src)
			{
				auto ret = MatchToken(src);
				
				if (!ret.has_value())
				{
					return;
				}

				auto [token0, src0] = ret.value();

				auto match_3_token = Combine(MatchToken, MatchToken, MatchToken);

				if (token0 =="v")
				{
					auto ret1 = match_3_token(src0);

					if (!ret1.has_value())
					{
						return;
					}

					auto [token1, token2, token3, src1] = ret1.value();

					//std::atof(token1);
					//std::atof(token2);
					//std::atof(token3);


				}
				else if (token0 =="vt")
				{
					//auto [token1, token2, token3, src1] = match_3_token(src0);

				}
				else if (token0 == "vn")
				{
					//auto [token1, token2, token3, src1] = match_3_token(src0);

				}
				else if (token0 == "f")
				{

				}
			}

		};




		inline Opt<Model> LoadFromFile(const std::wstring& path)
		{
			std::ifstream obj_file;
			obj_file.open(path, std::ios::in);

			if (!obj_file) 
			{
				throw "can't open obj file";
			}

			obj_file.seekg(0, std::ios::end);
			size_t len = obj_file.tellg();
			obj_file.seekg(0, std::ios::beg);

			std::vector<char> buffer;
			buffer.resize(len);
			obj_file.read(&buffer[0], len);

			//...
			SV obj_str = { &buffer[0],len };

			ObjParser obj_parser = {};

			auto ret = obj_parser.ParseObj(obj_str);



			return ret;
		}
	}

};

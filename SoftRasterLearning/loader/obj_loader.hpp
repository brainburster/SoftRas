#pragma once

#include <vector>
#include <string_view>
#include <fstream>
#include <tuple>
#include <optional>
#include <type_traits>
#include "../core/types_and_defs.hpp"
#include "../core/model.hpp"

namespace loader::obj
{
	using Vec2 = core::Vec2;
	using Vec3 = core::Vec3;
	using Vec4 = core::Vec4;
	using SV = std::string_view;
	using STR = std::wstring;
	template<typename T> using Opt = std::optional<T>;
	using Model_Vertex = core::Model_Vertex;
	using Model = core::Model;

	struct ObjParser
	{
		struct IntermediateData
		{
			std::vector<Vec3> position_buffer;
			std::vector<Vec3> normal_buffer;
			std::vector<Vec2> uv_buffer;
			std::vector<Model_Vertex> mesh;
		};

		Model ParseObjStr(SV src)
		{
			IntermediateData data = {};

			size_t guess = src.length() / 20;

			data.position_buffer.reserve(guess);
			data.normal_buffer.reserve(guess);
			data.uv_buffer.reserve(guess);
			data.mesh.reserve(guess);

			size_t len = src.length();
			for (size_t i = 0; i < len;)
			{
				size_t tail = src.find('\n', i);
				if (tail == std::string_view::npos)
				{
					break;
				}
				SV line_sv = src.substr(i, tail - i);
				//解析行
				ParseLine(line_sv, data);
				i = tail + 1;
			}

			CreateTriangle(data);

			return std::move(*reinterpret_cast<Model*>(&data.mesh));
		}

		void CreateTriangle(IntermediateData& data)
		{
			size_t len = data.mesh.size();
			for (size_t i = 0; i < len; i += 3)
			{
				auto& v0 = data.mesh[i];
				auto& v1 = data.mesh[i + 1];
				auto& v2 = data.mesh[i + 2];

				Vec3 e0 = v1.position - v0.position;
				Vec3 e1 = v2.position - v0.position;
				Vec2 duv0 = v1.uv - v0.uv;
				Vec2 duv1 = v2.uv - v0.uv;

				float f = 1.0f / (duv0.x * duv1.y - duv1.x * duv0.y);

				float tx = f * (duv1.y * e0.x - duv0.y * e1.x);
				float ty = f * (duv1.y * e0.y - duv0.y * e1.y);
				float tz = f * (duv1.y * e0.z - duv0.y * e1.z);

				//求出平面的切线
				Vec3 tangent = Vec3{ tx,ty,tz }.Normalize();

				//求出每个顶点的切线（因为可能有自定义法线）
				v0.tangent = (tangent - (tangent.Dot(v0.normal) * v0.normal)).Normalize();
				v1.tangent = (tangent - (tangent.Dot(v1.normal) * v1.normal)).Normalize();
				v2.tangent = (tangent - (tangent.Dot(v2.normal) * v2.normal)).Normalize();
			}
		}

		static Opt<std::tuple<SV, SV>> MatchToken(SV src)
		{
			size_t space = src.find_first_of(" \n\r\t");

			if (space == std::string_view::npos)
			{
				if (src.size() > 0) {
					return std::make_optional(std::make_tuple(
						src,
						""
					));
				}
				return std::nullopt;
			}

			return std::make_optional(
				std::make_tuple(
					src.substr(0, space),
					src.substr(space + 1)
				)
			);
		}

		template<typename Parser1, typename... ParserList>
		constexpr static decltype(auto) Combine(Parser1 parser1, ParserList... parser_list)
		{
			return[=](SV src)->Opt<std::tuple<
				std::decay_t<decltype(std::get<0>(parser1(src).value()))>,
				std::decay_t<decltype(std::get<0>(parser_list(src).value()))>...,
				SV>>
			{
				auto ret0 = parser1(src);

				if (!ret0.has_value())
				{
					return std::nullopt;
				}

				auto&& [real_ret, src0] = std::move(ret0.value());

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

		void ParseLine(SV src, IntermediateData& data)
		{
			auto ret = MatchToken(src);

			if (!ret.has_value())
			{
				return;
			}

			auto&& [token0, src0] = std::move(ret.value());

			auto match_4_token = Combine(MatchToken, MatchToken, MatchToken, MatchToken);
			auto match_3_token = Combine(MatchToken, MatchToken, MatchToken);
			auto match_2_token = Combine(MatchToken, MatchToken);

			if (token0 == "v")
			{
				auto ret1 = match_3_token(src0);

				if (!ret1.has_value())
				{
					return;
				}

				auto&& [token1, token2, token3, src1] = std::move(ret1.value());

				float a = (float)std::atof(token1.data());
				float b = (float)std::atof(token2.data());
				float c = (float)std::atof(token3.data());

				data.position_buffer.emplace_back(a, b, c);
			}
			else if (token0 == "vt")
			{
				auto ret1 = match_2_token(src0);

				if (!ret1.has_value())
				{
					return;
				}

				auto&& [token1, token2, src1] = std::move(ret1.value());

				float a = (float)std::atof(token1.data());
				float b = (float)std::atof(token2.data());

				data.uv_buffer.emplace_back(a, b);
			}
			else if (token0 == "vn")
			{
				auto ret1 = match_3_token(src0);

				if (!ret1.has_value())
				{
					return;
				}

				auto&& [token1, token2, token3, src1] = std::move(ret1.value());

				float a = (float)std::atof(token1.data());
				float b = (float)std::atof(token2.data());
				float c = (float)std::atof(token3.data());

				data.normal_buffer.emplace_back(a, b, c);
			}
			else if (token0 == "f")
			{
				auto ret1 = match_4_token(src0);

				const char* start = nullptr;
				char* end = nullptr;
				auto match_int = [&](const char* p = nullptr) mutable {
					start = end + 1;
					(void*)((p != nullptr) && (start = p));
					end = nullptr;
					return (size_t)strtol(start, &end, 10);
				};

				if (ret1.has_value())
				{
					auto&& [token1, token2, token3, token4, src1] = std::move(ret1.value());

					size_t a[3] = { match_int(token1.data()),match_int(), match_int() };
					size_t b[3] = { match_int(token2.data()),match_int(), match_int() };
					size_t c[3] = { match_int(token3.data()),match_int(), match_int() };
					size_t d[3] = { match_int(token4.data()),match_int(), match_int() };

					data.mesh.emplace_back(data.position_buffer[a[0] - 1], data.uv_buffer[a[1] - 1], data.normal_buffer[a[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[b[0] - 1], data.uv_buffer[b[1] - 1], data.normal_buffer[b[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[c[0] - 1], data.uv_buffer[c[1] - 1], data.normal_buffer[c[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[d[0] - 1], data.uv_buffer[d[1] - 1], data.normal_buffer[d[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[a[0] - 1], data.uv_buffer[a[1] - 1], data.normal_buffer[a[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[c[0] - 1], data.uv_buffer[c[1] - 1], data.normal_buffer[c[2] - 1]);
					return;
				}

				auto ret2 = match_3_token(src0);

				if (ret2.has_value())
				{
					auto&& [token1, token2, token3, src1] = std::move(ret2.value());

					size_t a[3] = { match_int(token1.data()),match_int(), match_int() };
					size_t b[3] = { match_int(token2.data()),match_int(), match_int() };
					size_t c[3] = { match_int(token3.data()),match_int(), match_int() };

					data.mesh.emplace_back(data.position_buffer[a[0] - 1], data.uv_buffer[a[1] - 1], data.normal_buffer[a[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[b[0] - 1], data.uv_buffer[b[1] - 1], data.normal_buffer[b[2] - 1]);
					data.mesh.emplace_back(data.position_buffer[c[0] - 1], data.uv_buffer[c[1] - 1], data.normal_buffer[c[2] - 1]);

					return;
				}
			}
		}
	};

	inline std::shared_ptr<Model> LoadFromFile(const std::wstring& path)
	{
		std::ifstream obj_file;
		obj_file.open(path, std::ios::in);

		if (!obj_file)
		{
			return nullptr;
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

		return std::make_shared<Model>(obj_parser.ParseObjStr(obj_str));
	}
}

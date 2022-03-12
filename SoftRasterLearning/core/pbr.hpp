#pragma once

#include "types_and_defs.hpp"
#include "cube_map.hpp"
#include <fstream>

namespace core::pbr
{
	//菲涅尔方程F（schlick近似） F0 + (1-F0)(1-(n·v))^5
	//F0 表示垂直入射时(法线夹角0°)的反射率
	//ndotv 观察向量与法线夹角 N dot V
	Vec3 FresnelSchlick(Vec3 F0, float ndotv);

	//受到粗糙度影响的菲涅耳方程，因为粗糙微平面会遮蔽反射光
	core::Vec3 FresnelSchlickRoughness(core::Vec3 F0, float ndotv, float roughness);

	//计算f0(基础反射率), 用金属度和反照率计算
	//albedo 表示表面颜色(漫反射系数)或基础反射率
	//metalness 表示金属或非金属
	Vec3 GetF0(Vec3 albedo, float metalness);

	//(微平面法线)正态分布函数D NDF(normal distribution function) (Trowbridge-Reitz GGX) 通过粗糙度决定
	// roughness^2/pi*((n dot h)^2*(a^2-1)+1)^2
	float DistributionGGX(float NdotH, float roughness);

	//几何函数（微平面本身的遮挡率）(Schlick-GGX)
	// (n dot v) / ((n dot v) (1-k) + k)
	//k是根据粗糙度生成的
	float GeometrySchlickGGX(float cos_theta, float k);

	//考虑了观察向量和光线向量2者的几何函数 (Smith法)
	float GeometrySmith(float NdotV, float NdotL, float k);

	//Cook-Torrance BRDF的镜面反射部分
	//DFG/4(VdotN)(LdotN)
	Vec3 SpecularCooKTorrance(float D, Vec3 F, float G, float NdotV, float NdotL);

	struct IBL
	{
		std::shared_ptr<Texture> brdf_map; //BRDF积分图
		std::shared_ptr<CubeMap> diffuse_map; //漫反射部分卷积
		std::vector<std::shared_ptr<CubeMap>> specular_maps; //镜面反射部分卷积

#pragma pack(push)
#pragma pack(2)
		struct TextureHeader {
			std::uint32_t size;
			std::uint16_t w;
			std::uint16_t h;
		};
		static_assert(sizeof(TextureHeader) == 8, "the size of TextureHeader must be 8 bytes");

		struct IblFileHeader
		{
			TextureHeader brdf_map;
			TextureHeader diffuse_map;
			std::uint16_t num_of_specular_maps;
		};
		static_assert(sizeof(IblFileHeader) == 18, "the size of IblFileHeader must be 18 bytes");

#pragma pack(pop)

		void Save(const wchar_t* filename);
		void Load(const wchar_t* filename);
		IBL();
		void Init(const CubeMap& env);
		Vec2 IntegrateBRDF(float NdotV, float roughness);
		void InitBrdfMap();
		float RadicalInverseVdC(size_t bits);
		Vec2 Hammersley(size_t i, size_t N);
		//获得根据roughness随机生成的切线空间半球采样
		Vec3 ImportanceSampleGGX(Vec2 Xi, Vec3 N, float roughness);
		void InitSpecularMaps(const CubeMap& env);
		void InitDiffuseMap(const CubeMap& env);
	};
}

#include "impl/pbr.inl"

#pragma once

#include "types_and_defs.hpp"

namespace core::pbr
{
	//菲涅尔方程F（schlick近似） F0 + (1-F0)(1-(n·v))^5
	//F0 表示垂直入射时(法线夹角0°)的反射率
	//ndotv 观察向量与法线夹角 N dot V
	inline Vec3 FresnelSchlick(Vec3 F0, float ndotv)
	{
		return F0 + (1.0 - F0) * pow(1.0f - ndotv, 5.0f);
	}

	//受到粗糙度影响的菲涅耳方程，因为粗糙微平面会遮蔽反射光
	core::Vec3 FresnelSchlickRoughness(core::Vec3 F0, float ndotv, float roughness)
	{
		return F0 + (_mm_max_ps(core::Vec3(1.0f - roughness), F0) - F0) * pow(1.0f - ndotv, 5.0f);
	}

	//计算f0(基础反射率), 用金属度和反照率计算
	//albedo 表示表面颜色(漫反射系数)或基础反射率
	//metalness 表示金属或非金属
	inline Vec3 GetF0(Vec3 albedo, float metalness)
	{
		return gmath::utility::Lerp(Vec3(0.04f), albedo, metalness);
	}

	//(微平面法线)正态分布函数D NDF(normal distribution function) (Trowbridge-Reitz GGX) 通过粗糙度决定
	// roughness^2/pi*((n dot h)^2*(a^2-1)+1)^2
	inline float DistributionGGX(float NdotH, float roughness)
	{
		float roughness2 = roughness * roughness;
		float NdotH2 = NdotH * NdotH;
		float nom = roughness2;
		float denom = (NdotH2 * (roughness2 - 1.0f) + 1.0f);
		denom = pi * denom * denom + epsilon;
		return nom / denom;
	}

	//几何函数（微平面本身的遮挡率）(Schlick-GGX)
	// (n dot v) / ((n dot v) (1-k) + k)
	//k是根据粗糙度生成的
	inline float GeometrySchlickGGX(float cos_theta, float k)
	{
		float nom = cos_theta;
		float denom = cos_theta * (1.f - k) + k + epsilon;
		return nom / denom;
	}

	//考虑了观察向量和光线向量2者的几何函数 (Smith法)
	inline float GeometrySmith(float NdotV, float NdotL, float k)
	{
		float G1 = GeometrySchlickGGX(NdotV, k);
		float G2 = GeometrySchlickGGX(NdotL, k);
		return G1 * G2;
	}

	//Cook-Torrance BRDF的镜面反射部分
	//DFG/4(VdotN)(LdotN)
	inline Vec3 SpecularCooKTorrance(float D, Vec3 F, float G, float NdotV, float NdotL)
	{
		return D * G / (4 * NdotV * NdotL + epsilon) * F;
	}
}

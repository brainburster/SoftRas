#pragma once

#include "types_and_defs.hpp"

namespace core::pbr
{
	//����������F��schlick���ƣ� F0 + (1-F0)(1-(n��v))^5
	//F0 ��ʾ��ֱ����ʱ(���߼н�0��)�ķ�����
	//ndotv �۲������뷨�߼н� N dot V
	inline Vec3 FresnelSchlick(Vec3 F0, float ndotv)
	{
		return F0 + (1.0 - F0) * pow(1.0f - ndotv, 5.0f);
	}

	//�ܵ��ֲڶ�Ӱ��ķ��������̣���Ϊ�ֲ�΢ƽ����ڱη����
	core::Vec3 FresnelSchlickRoughness(core::Vec3 F0, float ndotv, float roughness)
	{
		return F0 + (_mm_max_ps(core::Vec3(1.0f - roughness), F0) - F0) * pow(1.0f - ndotv, 5.0f);
	}

	//����f0(����������), �ý����Ⱥͷ����ʼ���
	//albedo ��ʾ������ɫ(������ϵ��)�����������
	//metalness ��ʾ������ǽ���
	inline Vec3 GetF0(Vec3 albedo, float metalness)
	{
		return gmath::utility::Lerp(Vec3(0.04f), albedo, metalness);
	}

	//(΢ƽ�淨��)��̬�ֲ�����D NDF(normal distribution function) (Trowbridge-Reitz GGX) ͨ���ֲڶȾ���
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

	//���κ�����΢ƽ�汾����ڵ��ʣ�(Schlick-GGX)
	// (n dot v) / ((n dot v) (1-k) + k)
	//k�Ǹ��ݴֲڶ����ɵ�
	inline float GeometrySchlickGGX(float cos_theta, float k)
	{
		float nom = cos_theta;
		float denom = cos_theta * (1.f - k) + k + epsilon;
		return nom / denom;
	}

	//�����˹۲������͹�������2�ߵļ��κ��� (Smith��)
	inline float GeometrySmith(float NdotV, float NdotL, float k)
	{
		float G1 = GeometrySchlickGGX(NdotV, k);
		float G2 = GeometrySchlickGGX(NdotL, k);
		return G1 * G2;
	}

	//Cook-Torrance BRDF�ľ��淴�䲿��
	//DFG/4(VdotN)(LdotN)
	inline Vec3 SpecularCooKTorrance(float D, Vec3 F, float G, float NdotV, float NdotL)
	{
		return D * G / (4 * NdotV * NdotL + epsilon) * F;
	}
}

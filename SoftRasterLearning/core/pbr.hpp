#pragma once

#include "types_and_defs.hpp"
#include "cube_map.hpp"
#include <fstream>

namespace core::pbr
{
	//����������F��schlick���ƣ� F0 + (1-F0)(1-(n��v))^5
	//F0 ��ʾ��ֱ����ʱ(���߼н�0��)�ķ�����
	//ndotv �۲������뷨�߼н� N dot V
	Vec3 FresnelSchlick(Vec3 F0, float ndotv);

	//�ܵ��ֲڶ�Ӱ��ķ��������̣���Ϊ�ֲ�΢ƽ����ڱη����
	core::Vec3 FresnelSchlickRoughness(core::Vec3 F0, float ndotv, float roughness);

	//����f0(����������), �ý����Ⱥͷ����ʼ���
	//albedo ��ʾ������ɫ(������ϵ��)�����������
	//metalness ��ʾ������ǽ���
	Vec3 GetF0(Vec3 albedo, float metalness);

	//(΢ƽ�淨��)��̬�ֲ�����D NDF(normal distribution function) (Trowbridge-Reitz GGX) ͨ���ֲڶȾ���
	// roughness^2/pi*((n dot h)^2*(a^2-1)+1)^2
	float DistributionGGX(float NdotH, float roughness);

	//���κ�����΢ƽ�汾����ڵ��ʣ�(Schlick-GGX)
	// (n dot v) / ((n dot v) (1-k) + k)
	//k�Ǹ��ݴֲڶ����ɵ�
	float GeometrySchlickGGX(float cos_theta, float k);

	//�����˹۲������͹�������2�ߵļ��κ��� (Smith��)
	float GeometrySmith(float NdotV, float NdotL, float k);

	//Cook-Torrance BRDF�ľ��淴�䲿��
	//DFG/4(VdotN)(LdotN)
	Vec3 SpecularCooKTorrance(float D, Vec3 F, float G, float NdotV, float NdotL);

	struct IBL
	{
		std::shared_ptr<Texture> brdf_map; //BRDF����ͼ
		std::shared_ptr<CubeMap> diffuse_map; //�����䲿�־��
		std::vector<std::shared_ptr<CubeMap>> specular_maps; //���淴�䲿�־��

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
		//��ø���roughness������ɵ����߿ռ�������
		Vec3 ImportanceSampleGGX(Vec2 Xi, Vec3 N, float roughness);
		void InitSpecularMaps(const CubeMap& env);
		void InitDiffuseMap(const CubeMap& env);
	};
}

#include "impl/pbr.inl"

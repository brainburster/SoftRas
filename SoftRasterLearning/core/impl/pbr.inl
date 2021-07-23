//#pragma once

//#include "../pbr.hpp"

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

	//���κ�����΢ƽ�汾�����ڵ��ʣ�(Schlick-GGX)
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

	inline IBL::IBL()
	{
		//size_t w = evn_map.front->GetWidth();
		//size_t h = evn_map.front->GetHeight();
		diffuse_map = std::make_shared<CubeMap>(32, 32);
		specular_maps.reserve(5);
		specular_maps.emplace_back(std::make_shared<CubeMap>(128, 128));
		specular_maps.emplace_back(std::make_shared<CubeMap>(64, 64));
		specular_maps.emplace_back(std::make_shared<CubeMap>(32, 32));
		specular_maps.emplace_back(std::make_shared<CubeMap>(16, 16));
		specular_maps.emplace_back(std::make_shared<CubeMap>(8, 8));
		brdf_map = std::make_shared<Texture>(256, 256);
	}

	inline void IBL::init(const CubeMap& env)
	{
		init_diffuse_map(env);
		init_specular_maps(env);
		init_brdf_map();
	}

	inline Vec2 IBL::IntegrateBRDF(float NdotV, float roughness)
	{
		Vec3 V;
		V.x = sqrt(1.0f - NdotV * NdotV);
		V.y = 0.0;
		V.z = NdotV;

		float A = 0.0;
		float B = 0.0;

		Vec3 N = Vec3(0.0, 0.0, 1.0f);

		const size_t SAMPLE_COUNT = 256u;
		for (size_t i = 0u; i < SAMPLE_COUNT; ++i)
		{
			Vec2 Xi = Hammersley(i, SAMPLE_COUNT);
			Vec3 H = ImportanceSampleGGX(Xi, N, roughness);
			Vec3 L = (-V).Reflect(N);

			float NdotL = max(L.z, 0.0f);
			float NdotH = max(H.z, 0.0f);
			float VdotH = max(V.Dot(H), 0.0f);

			if (NdotL > 0.0)
			{
				float G = GeometrySmith(NdotV, NdotL, roughness * roughness / 2.f);
				float G_Vis = (G * VdotH) / (NdotH * NdotV);
				float Fc = pow(1.0f - VdotH, 5.0f);

				A += (1.0f - Fc) * G_Vis;
				B += Fc * G_Vis;
			}
		}
		A /= float(SAMPLE_COUNT);
		B /= float(SAMPLE_COUNT);
		return Vec2(A, B);
	}

	inline void IBL::init_brdf_map()
	{
		const size_t w = brdf_map->GetWidth();
		const size_t h = brdf_map->GetHeight();
		for (size_t j = 0; j < h; j++)
		{
			for (size_t i = 0; i < w; i++)
			{
				brdf_map->GetRef(i, j) = IntegrateBRDF((float)i / w, (float)j / h);
			}
		}
	}

	inline float IBL::RadicalInverse_VdC(size_t bits)
	{
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10f;
	}

	inline Vec2 IBL::Hammersley(size_t i, size_t N)
	{
		return Vec2(float(i) / float(N), RadicalInverse_VdC(i));
	}

	//��ø���roughness������ɵ����߿ռ�������
	inline Vec3 IBL::ImportanceSampleGGX(Vec2 Xi, Vec3 N, float roughness)
	{
		float a = roughness * roughness;

		float phi = 2.0f * pi * Xi.x;
		float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		// from spherical coordinates to cartesian coordinates
		Vec3 H;
		H.x = cos(phi) * sinTheta;
		H.y = sin(phi) * sinTheta;
		H.z = cosTheta;

		// from tangent-space vector to world-space sample vector
		Vec3 up = abs(N.z) < 0.999 ? Vec3(0.0, 0.0, 1.0f) : Vec3(1.0f, 0.0, 0.0);
		Vec3 tangent = up.Cross(N).Normalize();
		Vec3 bitangent = N.Cross(tangent);

		Vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
		return sampleVec.Normalize();
	}

	inline void IBL::init_specular_maps(const CubeMap& env)
	{
		const Vec3 r = { 1,0,0 };
		const Vec3 u = { 0,1,0 };
		const Vec3 f = { 0,0,1 };
		Mat3 rotate_mat[6] = {
			//front, ����Ҫ��ת(����ϵ�任),r,u,f
			{r,u,f},
			//back, -r,u,-f
			{-r,u,-f},
			//top, r,-f,u,
			{ r,-f,u },
			//bottom, r,f,-u
			{r,f,-u},
			//left, f,u,-r
			{f,u,-r},
			//right, -f,u,r
			{-f,u,r}
		};

		const size_t size_map[] = { 128,64,32,16,8 };
		for (size_t mip = 0; mip < 5; ++mip)
		{
			size_t w = size_map[mip];
			size_t h = size_map[mip];
			float roughness = mip / 4.f;
			auto* specular_arrary = reinterpret_cast<std::shared_ptr<Texture>*>(specular_maps[mip].get());
			for (size_t k = 0; k < 6; ++k)
			{
				auto& tex = specular_arrary[k];
				for (size_t j = 0; j < h; ++j)
				{
					for (size_t i = 0; i < w; ++i)
					{
						Vec3 N = Vec3{ (float)i / w - 0.5f, (float)j / h - 0.5f , 0.4999999f };
						N = rotate_mat[k] * N;
						N = N.Normalize();
						Vec3 R = N;
						Vec3 V = R;

						const size_t SAMPLE_COUNT = 256;
						float totalWeight = 0.0f;
						Vec3 prefilteredColor = 0.0f;
						for (size_t i = 0u; i < SAMPLE_COUNT; ++i)
						{
							Vec2 Xi = Hammersley(i, SAMPLE_COUNT);
							Vec3 H = ImportanceSampleGGX(Xi, N, roughness);
							Vec3 L = (-V).Reflect(H).Normalize();

							float NdotL = max(N.Dot(L), 0.0f);
							if (NdotL > 0.0f)
							{
								prefilteredColor += env.Sample(L) * NdotL;
								totalWeight += NdotL;
							}
						}

						prefilteredColor = prefilteredColor / totalWeight;
						tex->GetRef(i, j) = { prefilteredColor, 1.0f };
					}
				}
			}
		}
	}

	inline void IBL::init_diffuse_map(const CubeMap& env)
	{
		auto* diffuse_arrary = reinterpret_cast<std::shared_ptr<Texture>*>(diffuse_map.get());
		const Vec3 r = { 1,0,0 };
		const Vec3 u = { 0,1,0 };
		const Vec3 f = { 0,0,1 };

		Mat3 rotate_mat[6] = {
			//front, ����Ҫ��ת(����ϵ�任),r,u,f
			{r,u,f},
			//back, -r,u,-f
			{-r,u,-f},
			//top, r,-f,u,
			{ r,-f,u },
			//bottom, r,f,-u
			{r,f,-u},
			//left, f,u,-r
			{f,u,-r},
			//right, -f,u,r
			{-f,u,r}
		};
		for (size_t k = 0; k < 6; ++k)
		{
			auto& tex = diffuse_arrary[k];
			size_t w = tex->GetWidth();
			size_t h = tex->GetHeight();
			for (size_t j = 0; j < h; ++j)
			{
				for (size_t i = 0; i < w; ++i)
				{
					//��i,j,kӳ�䵽�߳�Ϊ1�������巽����
					//i,j => {i/w-0.5f,j/w-0.5f,0.5f}
					//k ������ת����
					//���env��������
					Vec3 normal = Vec3{ (float)i / w - 0.5f, (float)j / h - 0.5f, 0.4999999f };
					normal = rotate_mat[k] * normal;
					normal = normal.Normalize();

					//�������
					Vec3 up = { 0.0, 1.0f, 0.0 };
					Vec3 right = up.Cross(normal);
					up = normal.Cross(right);

					float sampleDelta = 0.1f;
					float nrSamples = 0.0f;

					Vec3 irradiance = {};
					for (float phi = 0.0f; phi < 2.0f * pi; phi += sampleDelta)
					{
						for (float theta = 0.0f; theta < 0.5f * pi; theta += sampleDelta)
						{
							// spherical to cartesian (in tangent space), ��������������߿ռ�Ĳ�������
							Vec3  tangentSample = Vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
							// tangent space to world �����߿ռ�ת��������ռ�
							Vec3  sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
							irradiance += env.Sample(sampleVec) * cos(theta) * sin(theta);
							nrSamples++;
						}
					}
					irradiance = pi * irradiance * (1.0f / float(nrSamples));
					tex->GetRef(i, j) = Vec4{ irradiance,1.0f };
				}
			}
		}
	}
}
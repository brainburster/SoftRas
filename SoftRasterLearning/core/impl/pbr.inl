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
	inline Vec3 FresnelSchlickRoughness(core::Vec3 F0, float ndotv, float roughness)
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

	//保存到文件
	inline void IBL::Save(const wchar_t* filename)
	{
		using namespace std;
		ofstream ofile(filename, ios_base::trunc | ios_base::out | ios_base::binary);

		const auto diff_tex0 = diffuse_map->GetTexture(0);

		IblFileHeader ibl_info = {
			{
				narrow_cast<std::uint32_t>(brdf_map->GetSize()),
				narrow_cast<std::uint16_t>(brdf_map->GetWidth()),
				narrow_cast<std::uint16_t>(brdf_map->GetHeight())
			},
			{
				narrow_cast<std::uint32_t>(diff_tex0->GetSize()),
				narrow_cast<std::uint16_t>(diff_tex0->GetWidth()),
				narrow_cast<std::uint16_t>(diff_tex0->GetHeight())
			},
			narrow_cast<std::uint16_t>(specular_maps.size())
		};

		ofile.write(reinterpret_cast<char*>(&ibl_info), sizeof ibl_info);

		ofile.write(reinterpret_cast<char*>(brdf_map->GetData().data()),ibl_info.brdf_map.size * sizeof(decltype(brdf_map->Get(0ULL, 0ULL))));

		for (size_t i = 0; i < 6; i++)
		{
			const auto& tex = *diffuse_map->GetTexture(i);
			ofile.write(reinterpret_cast<const char*>(tex.GetCData().data()), ibl_info.diffuse_map.size * sizeof(decltype(tex.Get(0ULL, 0ULL))));
		}

		for (const auto & specular_map : specular_maps)
		{
			const auto spec_tex0 = specular_map->GetTexture(0);
			TextureHeader tex_info = {
				narrow_cast<std::uint32_t>(spec_tex0->GetSize()),
				narrow_cast<std::uint16_t>(spec_tex0->GetWidth()),
				narrow_cast<std::uint16_t>(spec_tex0->GetHeight()),
			};
			ofile.write(reinterpret_cast<char*>(&tex_info), sizeof tex_info);
			for (size_t i = 0; i < 6; i++)
			{
				const auto& tex = *specular_map->GetTexture(i);
				ofile.write(reinterpret_cast<const char*>(tex.GetCData().data()), tex_info.size * sizeof(decltype(tex.Get(0ULL, 0ULL))));
			}
		}
	}

	//从文件读取
	inline void IBL::Load(const wchar_t* filename)
	{
		using namespace std;
		ifstream ifile(filename, ios_base::in | ios_base::binary);
		brdf_map = std::make_shared<Texture>();
		diffuse_map = std::make_shared<CubeMap>();
		specular_maps.clear();

		IblFileHeader ibl_info = {};
		ifile.read(reinterpret_cast<char*>(&ibl_info), sizeof ibl_info);
		brdf_map->Resize(ibl_info.brdf_map.w, ibl_info.brdf_map.h);
		ifile.read(reinterpret_cast<char*>(brdf_map->GetData().data()), ibl_info.brdf_map.size * sizeof(decltype(brdf_map->Get(0ULL, 0ULL))));
		for (size_t i = 0; i < 6; i++)
		{
			auto& tex = *diffuse_map->GetTexture(i);
			tex.Resize(ibl_info.diffuse_map.w, ibl_info.diffuse_map.h);
			ifile.read(reinterpret_cast<char*>(tex.GetData().data()), ibl_info.diffuse_map.size * sizeof(decltype(tex.Get(0ULL, 0ULL))));
		}
		specular_maps.resize(ibl_info.num_of_specular_maps);
		for (auto& specular_map : specular_maps)
		{
			specular_map = std::make_shared<CubeMap>();
			TextureHeader tex_info = {};
			ifile.read(reinterpret_cast<char*>(&tex_info), sizeof tex_info);
			for (size_t i = 0; i < 6; i++)
			{
				auto& tex = *specular_map->GetTexture(i);
				tex.Resize(tex_info.w, tex_info.h);
				ifile.read(reinterpret_cast<char*>(tex.GetData().data()), tex_info.size * sizeof(decltype(tex.Get(0ULL, 0ULL))));
			}
		}
	}

	inline IBL::IBL()
	{
		brdf_map = std::make_shared<Texture>(512, 512);
		diffuse_map = std::make_shared<CubeMap>(64, 64);
		specular_maps.reserve(5);
		specular_maps.emplace_back(std::make_shared<CubeMap>(256, 256));
		specular_maps.emplace_back(std::make_shared<CubeMap>(128, 128));
		specular_maps.emplace_back(std::make_shared<CubeMap>(64, 64));
		specular_maps.emplace_back(std::make_shared<CubeMap>(32, 32));
		specular_maps.emplace_back(std::make_shared<CubeMap>(16, 16));
	}

	inline void IBL::Init(const CubeMap& env)
	{
		InitBrdfMap();
		InitDiffuseMap(env);
		InitSpecularMaps(env);
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

		const size_t SAMPLE_COUNT = 2048u;
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

	inline void IBL::InitBrdfMap()
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

	inline float IBL::RadicalInverseVdC(size_t bits)
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
		return Vec2(float(i) / float(N), RadicalInverseVdC(i));
	}

	//获得根据roughness随机生成的切线空间半球采样
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

	inline void IBL::InitSpecularMaps(const CubeMap& env)
	{
		const Vec3 r = { 1,0,0 };
		const Vec3 u = { 0,1,0 };
		const Vec3 f = { 0,0,1 };
		Mat3 rotate_mat[6] = {
			//front, 不需要旋转(坐标系变换),r,u,f
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

		const size_t size_map[] = { 256,128,64,32,16 };
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

						const size_t SAMPLE_COUNT = 2048u;
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

	inline void IBL::InitDiffuseMap(const CubeMap& env)
	{
		auto* diffuse_arrary = reinterpret_cast<std::shared_ptr<Texture>*>(diffuse_map.get());
		const Vec3 r = { 1,0,0 };
		const Vec3 u = { 0,1,0 };
		const Vec3 f = { 0,0,1 };

		Mat3 rotate_mat[6] = {
			//front, 不需要旋转(坐标系变换),r,u,f
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
					//将i,j,k映射到边长为1的正方体方体上
					//i,j => {i/w-0.5f,j/w-0.5f,0.5f}
					//k 决定旋转矩阵
					//获得env采样方向
					Vec3 normal = Vec3{ (float)i / w - 0.5f, (float)j / h - 0.5f, 0.4999999f };
					normal = rotate_mat[k] * normal;
					normal = normal.Normalize();

					//计算卷积
					Vec3 up = { 0.0, 1.0f, 0.0 };
					Vec3 right = up.Cross(normal);
					up = normal.Cross(right);

					float sampleDelta = 0.05f;
					float nrSamples = 0.0f;

					Vec3 irradiance = {};
					for (float phi = 0.0f; phi < 2.0f * pi; phi += sampleDelta)
					{
						for (float theta = 0.0f; theta < 0.5f * pi; theta += sampleDelta)
						{
							// spherical to cartesian (in tangent space), 计算半球方向内切线空间的采样坐标
							Vec3  tangentSample = Vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
							// tangent space to world 从切线空间转换到世界空间
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

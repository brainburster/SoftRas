#include "pch.h"

TEST(GMATH, GMATH) {
	using namespace gmath;
	gmath::Vec4<float> v = { 1,2,3,4 };
	gmath::Vec4<float> v2 = { 3,4,9,1 };

	gmath::Mat4x4<float> m = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};

	for (size_t i = 0; i < 100000000; i++)
	{
		v += m * (m * v + v * v2 + m * v2);
	}

	EXPECT_NE(v.x, v.x);
	EXPECT_TRUE(true);
}

TEST(GMATH2, GMATH2) {
	using namespace gmath;
	gmath::Vec4<double> v{ 1,2,3,4 };
	gmath::Vec4<double> v2{ 3,4,9,1 };

	gmath::Mat4x4<double> m = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};

	for (size_t i = 0; i < 100000000; i++)
	{
		//v = m * v + v;
		v += m * (m * v + v * v2 + m * v2);
	}

	EXPECT_NE(v.x, v.x);
	EXPECT_TRUE(true);
}

TEST(DXMATH, DXMATH) {
	using namespace DirectX;
	XMVECTOR v = { 1,2,3,4 };
	XMVECTOR v2 = { 3,4,9,1 };

	XMMATRIX m = XMMatrixSet(
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	);
	for (size_t i = 0; i < 100000000; i++)
	{
		v += XMVector4Transform(XMVector4Transform(v, m) + v * v2 + XMVector4Transform(v2, m), m);
	}
	XMFLOAT4 f4 = {};
	XMStoreFloat4(&f4, v);
	EXPECT_NE(f4.x, f4.x);
	EXPECT_TRUE(true);
}

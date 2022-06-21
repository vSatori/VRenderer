#pragma once
#include <vector>
#include "MathUtil.h"
#include "Texture.h"
#include <memory>
struct Vertex
{
	Vector3f pos    { 0.f, 0.f, 0.f };
	Vector3f normal { 0.f, 0.f, 0.f };
	Vector2f uv     { 0.f, 0.f };
};

struct Fragment
{
	Vector2f uv{ 0.f, 0.f };
	Vector3f posM{ 0.f, 0.f, 0.f };
	Vector3f posW{ 0.f, 0.f, 0.f };
	Vector3f normalW{ 0.f, 0.f, 0.f };
	Vector4f posH{ 0.f, 0.f, 0.f, 0.f };
	Vector4f posD{ 0.f, 0.f, 0.f, 0.f };

	static const int floatSize = 19;

	static Fragment lerp(const Fragment& fm1, const Fragment& fm2, float t)
	{
		Fragment res;
		float* p =  (float*)(&res);
		float* p1 = (float*)(&fm1);
		float* p2 = (float*)(&fm2);
		for (int i = 0; i < floatSize; ++i)
		{
			p[i] = p1[i] + (p2[i] - p1[i]) * t;
		}
		return res;
	}
};


struct Material
{
	Vector3f ambient { 1.f, 1.f, 1.f };
	Vector3f diffuse { 1.f, 1.f, 1.f };
	Vector3f specular{ 1.f, 1.f, 1.f };
	float shininess = 32.f;
};



struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Vector3i> indices;
	std::shared_ptr<Texture> texture;
	Material material;
};



struct SkyBox : public Mesh
{
	std::unique_ptr<CubeMap> cubeMap;
};


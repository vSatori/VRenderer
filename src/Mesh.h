#pragma once
#include <vector>
#include "MathUtil.h"
#include "Texture.h"
#include <memory>
struct Vertex
{
	Vector3f pos;
	Vector3f normal;
	Vector2f uv;
};

struct Fragment
{
	Vector2f uv;
	Vector3f posM;
	Vector3f posW;
	Vector3f normalW;
	Vector4f posH;
	Vector4f posD;
	

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
	//std::shared_ptr<Texture> texture;
	std::shared_ptr<Texture1i> texture;
	Material material;
};



struct SkyBox : public Mesh
{
	std::unique_ptr<CubeMap> cubeMap;
};


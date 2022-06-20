#pragma once
#include <vector>
#include "MathUtil.h"
#include "Texture.h"
#include <memory>
struct Vertex
{
	Vector3f pos    { 0.f, 0.f, 0.f };
	Vector3f normal { 0.f, 0.f, 0.f };
	Vector2f tex    { 0.f, 0.f };
};

struct Fragment
{
	Vector2f uv      { 0.f, 0.f };
	Vector3f posM    { 0.f, 0.f, 0.f };
	Vector3f posW    { 0.f, 0.f, 0.f };
	Vector3f normalW { 0.f, 0.f, 0.f };
	Vector4f posH    { 0.f, 0.f, 0.f, 0.f };
	Vector4f posD    { 0.f, 0.f, 0.f, 0.f };

	static const int floatSize = 19;
	
	Fragment operator+(const Fragment& fm)const
	{
		Fragment res = *this;
		res.uv += fm.uv;
		res.posM += fm.posM;
		res.posW += fm.posW;
		res.posH += fm.posH;
		res.posD += fm.posD;
		res.normalW += fm.normalW;
		return res;
	}

	Fragment operator-(const Fragment& fm)const
	{
		Fragment res = *this;
		res.uv -= fm.uv;
		res.posM -= fm.posM;
		res.posW -= fm.posW;
		res.posH -= fm.posH;
		res.posD -= fm.posD;
		res.normalW -= fm.normalW;
		return res;
	}

	Fragment operator*(float t)const
	{
		Fragment res = *this;
		res.uv *= t;
		res.posM *= t;
		res.posW *= t;
		res.posH *= t;
		res.posD *= t;
		res.normalW *= t;
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


#pragma once
#include <vector>
#include "MathUtil.h"
#include "Texture.h"
#include <memory>
struct Vertex
{
	Vector3f pos{ 0.f, 0.f };
	Vector3f normal{ 0.f, 0.f };
	Vector2f tex{ 0.f, 0.f };
};

struct VertexOut
{
	Vector2f uv;
	Vector3f posM;
	Vector3f posW;
	Vector3f normalW;
	Vector4f posH;
	Vector4f posD;

	static const int floatSize = 19;
	
	VertexOut operator+(const VertexOut& vo)const
	{
		VertexOut res = *this;
		res.uv += vo.uv;
		res.posM += vo.posM;
		res.posW += vo.posW;
		res.posH += vo.posH;
		res.posD += vo.posD;
		res.normalW += vo.normalW;
		return res;
	}

	VertexOut operator-(const VertexOut& vo)const
	{
		VertexOut res = *this;
		res.uv -= vo.uv;
		res.posM -= vo.posM;
		res.posW -= vo.posW;
		res.posH -= vo.posH;
		res.posD -= vo.posD;
		res.normalW -= vo.normalW;
		return res;
	}

	VertexOut operator*(float t)const
	{
		VertexOut res = *this;
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
	Vector3f ambient{1.f, 1.f, 1.f};
	Vector3f diffuse{1.f, 1.f, 1.f};
	Vector3f specular{1.f, 1.f, 1.f};
	float shininess = 32.f;
	bool reflect = false;
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
	std::unique_ptr<StaticCubeMap> cubeMap;
};


#pragma once
#include <vector>
#include "MathUtility.h"
#include "Texture.h"
#include <memory>
#include "Light.h"
#include "RenderContext.h"
struct Vertex
{
	Vector3f pos;
	Vector3f normal;
	Vector2f tex;
};

struct VertexOut
{
	Vertex vin;
	Vector3f posW;
	Vector4f posH;
	Vector3f normalW;
	VertexOut operator+(const VertexOut& vo)const
	{
		VertexOut res;
		res.vin.pos += vo.vin.pos;
		res.vin.normal += vo.vin.normal;
		res.vin.tex += vo.vin.tex;
		res.posW += vo.posW;
		res.posH += vo.posH;
		res.normalW += vo.normalW;
		return res;
	}
	VertexOut operator*(float t)const
	{
		VertexOut vout;
		vout.vin.pos *= t;
		vout.vin.normal *= t;
		vout.vin.tex *= t;
		vout.posW *= t;
		vout.posH *= t;
		vout.normalW *= t;
		return vout;
	}
};




struct Mesh
{

public:
	
public:
	std::vector<Vertex> vertices;
	std::vector<Vector3i> indices;
};



struct SkyBox : public Mesh
{
	std::unique_ptr<CubeMap> cubeMap;
};


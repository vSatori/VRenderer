#pragma once
#include <vector>
#include "MathUtility.h"
#include "Texture.h"
#include <memory>
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
	unsigned int index;
	Vector4f depthPos;
	
	VertexOut operator+(const VertexOut& vo)const
	{
		VertexOut res = *this;
		res.vin.pos += vo.vin.pos;
		res.vin.normal += vo.vin.normal;
		res.vin.tex += vo.vin.tex;
		res.posW += vo.posW;
		res.posH += vo.posH;
		res.normalW += vo.normalW;
		res.depthPos += vo.depthPos;
		return res;
	}

	VertexOut operator-(const VertexOut& vo)const
	{
		VertexOut res = *this;
		res.vin.pos -= vo.vin.pos;
		res.vin.normal -= vo.vin.normal;
		res.vin.tex -= vo.vin.tex;
		res.posW -= vo.posW;
		res.posH -= vo.posH;
		res.normalW -= vo.normalW;
		res.depthPos -= vo.depthPos;
		return res;
	}

	VertexOut operator*(float t)const
	{
		VertexOut vout = *this;
		vout.vin.pos *= t;
		vout.vin.normal *= t;
		vout.vin.tex *= t;
		vout.posW *= t;
		vout.posH *= t;
		vout.normalW *= t;
		vout.depthPos *= t;	
		return vout;
	}
	
};




struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Vector3i> indices;
	std::unique_ptr<Texture> texture;
};



struct SkyBox : public Mesh
{
	std::unique_ptr<CubeMap> cubeMap;
};


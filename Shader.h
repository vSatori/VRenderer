#pragma once
#include <vector>
#include "Light.h"
#include "MathUtility.h"
#include "Mesh.h"
template <typename T>
class VertexShader
{
public:
	inline VertexOut execute(const Vertex& vin)
	{
		T* shader = static_cast<T*>(this);
		return shader->implementation(vin);
	}
public:
	Matrix4 matWorld;
	Matrix4 matView;
	Matrix4 matProject;
	Matrix4 matWVP;
};

class GenericVertexShader : public VertexShader<GenericVertexShader>
{
public:
	inline VertexOut implementation(const Vertex& vin)
	{
		VertexOut vout;
		vout.vin = vin;
		vout.posH = matWVP * vin.pos;
		return vout;
	}
};

class SkyBoxVertexShader : public VertexShader<SkyBoxVertexShader>
{
public:
	inline VertexOut implementation(const Vertex& vin)
	{
		VertexOut vout;
		vout.vin = vin;
		vout.posH = matWVP * vin.pos;
		vout.posH.z = vout.posH.w;
		return vout;
	}
};
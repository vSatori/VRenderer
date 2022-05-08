#pragma once
#include "Mesh.h"
using VSFunction = std::function<VertexOut(const Vertex&)>;

class VertexShader
{
public:
	VertexOut execute(const Vertex& vin)
	{
		return function(vin);
	}
public:
	VSFunction function;
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;

};



class GenericVertexShader : public VertexShader
{
};

class SkyVertexShader : public VertexShader
{
};


VSFunction makeGenericVSFunction(GenericVertexShader* shader)
{
	auto func = [shader](const Vertex& vin)
	{
		Vector4f posW = shader->world * vin.pos;
		Vector4f posH = shader->projection * shader->view * posW;
		return VertexOut{ vin, {posW.x, posW.y, posW.z}, posH };
	};
	return func;
}

VSFunction makeSkyVSFunction(SkyVertexShader* shader)
{
	auto func = [shader](const Vertex& vin)
	{
		shader->view.m[0][3] = 0.f;
		shader->view.m[1][3] = 0.f;
		shader->view.m[2][3] = 0.f;
		Vector4f posW = shader->world * vin.pos;
		Vector4f posH = shader->projection * shader->view * posW;
		posH.z = posH.w;
		return VertexOut{ vin, {posW.x, posW.y, posW.z}, posH };
	};
	return func;
}


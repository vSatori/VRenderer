#pragma once
#include "Mesh.h"
using VSFunction = std::function<VertexOut(const Vertex&)>;

class VertexShader
{
public:
	VertexOut operator()(const Vertex& vin)
	{
		return function(vin);
	}
public:
	VSFunction function;

};



class GenericVertexShader : public VertexShader
{
public:
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;

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


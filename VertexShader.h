#pragma once
#include <functional>
#include "Mesh.h"
using VSFunction = std::function<VertexOut(const Vertex&)>;

class VertexShader
{
public:
	inline VertexOut execute(const Vertex& vin)
	{
		return function(vin);
	}
public:
	VSFunction function;
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;
	Matrix4 vp;
	Matrix3 world3;
};



class GenericVertexShader : public VertexShader
{
public:
	Matrix4 shadow;
};

class SkyVertexShader : public VertexShader
{
};

class ShadowMapVertexShader: public VertexShader
{
};


VSFunction makeGenericVSFunction(GenericVertexShader* shader);

VSFunction makeSkyVSFunction(SkyVertexShader* shader);

VSFunction makeShadowMapVSFunction(ShadowMapVertexShader* shader);


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
	std::vector<Matrix4> matLitViews;

};



class GenericVertexShader : public VertexShader
{
};

class SkyVertexShader : public VertexShader
{
};


VSFunction makeGenericVSFunction(GenericVertexShader* shader);

VSFunction makeSkyVSFunction(SkyVertexShader* shader);


#pragma once
#include <functional>
#include "Mesh.h"
using VSFunction = std::function<VertexOut(const Vertex&)>;

class VertexShader
{
public:
	virtual VertexOut execute(const Vertex& vin) = 0;
public:
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;
	Matrix4 vp;
	Matrix3 world3;
};

class GenericVertexShader : public VertexShader
{
public:
	virtual VertexOut execute(const Vertex& vin);
public:
	Matrix4 shadow;
};

class SkyVertexShader : public VertexShader
{
public:
	virtual VertexOut execute(const Vertex& vin);
};

class ShadowMapVertexShader: public VertexShader
{
public:
	virtual VertexOut execute(const Vertex& vin);
};


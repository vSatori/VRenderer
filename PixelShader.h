#pragma once
#include <functional>
#include "Mesh.h"
#include "Light.h"



using PSFunction = std::function<Vector3f(const VertexOut&)>;




struct PixelShader
{
public:
	inline Vector3f execute(const VertexOut& vout)
	{
		return function(vout);
	}
	inline Vector3f getColor(const VertexOut& out)
	{
		Vector3f color{ 0.f, 1.f, 1.f };
		return color;
	}
public:
	PSFunction function;
	Vector3f eyePos;
};

struct GenericPixelShader : public PixelShader
{
public:
	bool reflect = false;
	float alpha = 1.f;
	Vector3f color; 
	DynamicCubeMap* envCubeMap = nullptr;
	Texture* texture = nullptr;
	Light* light;
	std::vector<Matrix4> matLitViews;
	DepthTexture* depthTexture = nullptr;
};

struct SkyPixelShader : public PixelShader
{
public:
	CubeMap* cubeMap = nullptr;
};

PSFunction makeGenericPSFunction(GenericPixelShader* shader);


PSFunction makeSkyPSFunction(SkyPixelShader* shader);



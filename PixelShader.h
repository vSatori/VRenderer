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
public:
	PSFunction function;
};

struct GenericPixelShader : public PixelShader
{
public:
	float alpha = 1.f;
	Vector3f color; 
	Material material;
	Texture* texture = nullptr;
	Light* light = nullptr;
	DepthTexture* depthTexture = nullptr;
};

struct SkyPixelShader : public PixelShader
{
public:
	CubeMap* cubeMap = nullptr;
};

struct ReflectPixelShader : public PixelShader
{
public:
	DynamicCubeMap* envCubeMap = nullptr;
};


struct OceanWavePixelShader : public PixelShader
{
public:
	float maxHeight;
	float minHeight;
	DirectionalLight* light;
};

PSFunction makeGenericPSFunction(GenericPixelShader* shader);
PSFunction makeReflectPixelShader(ReflectPixelShader* shader);
PSFunction makeSkyPSFunction(SkyPixelShader* shader);
PSFunction makeOceanWavePSFunction(OceanWavePixelShader* shader);



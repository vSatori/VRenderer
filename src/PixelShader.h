#pragma once
#include <functional>
#include "Mesh.h"
#include "Light.h"



using PSFunction = std::function<Vector3f(const Fragment&)>;

class PixelShader
{
public:
	virtual Vector3f execute(const Fragment& vout) = 0;
};

class GenericPixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& vout);
public:
	float alpha = 1.f;
	Vector3f color; 
	Material material;
	Texture* texture = nullptr;
	Light* light = nullptr;
	DepthTexture* depthTexture = nullptr;
};

class SkyPixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& vout);
public:
	CubeMap* cubeMap = nullptr;
};

class ReflectPixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& vout);
public:
	CubeMap* envCubeMap = nullptr;
};


class OceanWavePixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& vout);
public:
	float maxHeight;
	float minHeight;
	DirectionalLight* light;
};




#pragma once
#include <functional>
#include "Mesh.h"
#include "Light.h"



using PSFunction = std::function<Vector3f(const Fragment&)>;

class PixelShader
{
public:
	virtual Vector3f execute(const Fragment& fm) = 0;
};

class GenericPixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& fm);
public:
	Material material;
	Texture1i* texture{nullptr};
	Light* light{nullptr};
	TextureBase<float>* depthTexture{ nullptr };
}; 

class SkyPixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& fm);
public:
	CubeMap* cubeMap{nullptr};
};

class ReflectPixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& fm);
public:
	CubeMap* envCubeMap {nullptr};
};

class OceanWavePixelShader : public PixelShader
{
public:
	virtual Vector3f execute(const Fragment& fm);
public:
	float maxHeight;
	float minHeight;
	DirectionalLight* light{nullptr};
};




#pragma once
#include "MathUtil.h"
#include <functional>
struct Material;


using LightFunction = std::function<Vector3f(const Vector3f&, const Vector3f&, const Vector3f&, const Material& material, float shadow)>;

class Light
{

public:
	Vector3f compute1(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		return function(eye, vertexPos, normal, material, shadow);
	}
public:
	virtual Vector3f compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow) = 0;
public:
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	Vector3f pos;
	LightFunction function;
};

class DirectionalLight : public Light
{
public:
	virtual Vector3f compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow);
public:
	Vector3f direction;
};

class PointLight : public Light
{
public:
	virtual Vector3f compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow);
public:
	float constant;
	float linear;
	float quadratic;

};

class SpotLight : public Light
{
public:
	virtual Vector3f compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow);
public:
	float constant;
	float linear;
	float quadratic;
	float spotFactor;
	float cutoff;
	float outcutoff;
	Vector3f direction;
};

LightFunction makeComputeDirectLightFunction(DirectionalLight * light);

LightFunction makeComputePointLightFunction(PointLight * light);

LightFunction makeComputeSpotLightFunction(SpotLight * light);


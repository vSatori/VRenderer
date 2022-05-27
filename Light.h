#pragma once
#include "MathUtil.h"
#include <functional>
struct Material;


using LightFunction = std::function<Vector3f(const Vector3f&, const Vector3f&, const Vector3f&, const Material& material, float shadow)>;

struct Light
{

public:
	Vector3f compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		return function(eye, vertexPos, normal, material, shadow);
	}
public:
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	Vector3f pos;
	LightFunction function;
};

struct DirectionalLight : public Light
{
	Vector3f direction;
};

struct PointLight : public Light
{
	float constant;
	float linear;
	float quadratic;

};

struct SpotLight : public Light
{
	float constant;
	float linear;
	float quadratic;
	float spotFactor;
	Vector3f direction;

};

LightFunction makeComputeDirectLightFunction(DirectionalLight * light);

LightFunction makeComputePointLightFunction(PointLight * light);

LightFunction makeComputeSpotLightFunction(SpotLight * light);


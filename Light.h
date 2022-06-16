#pragma once
#include "MathUtil.h"
#include <functional>
struct Material;


using LightFunction = std::function<Vector3f(const Vector3f&, const Vector3f&, const Vector3f&, const Material& material, float shadow)>;

class Light
{
public:
	virtual Vector3f compute(const Vector3f& eye, 
		const Vector3f& vertexPos,
		const Vector3f& normal, 
		const Material& material, 
		float shadow) = 0;
public:
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	Vector3f pos;
};

class DirectionalLight : public Light
{
public:
	virtual Vector3f compute(const Vector3f& eye, 
		const Vector3f& vertexPos, 
		const Vector3f& normal, 
		const Material& material, 
		float shadow);
public:
	Vector3f direction;
};

class PointLight : public Light
{
public:
	virtual Vector3f compute(const Vector3f& eye, 
		const Vector3f& vertexPos, 
		const Vector3f& normal, 
		const Material& material, 
		float shadow);
public:
	float constant;
	float linear;
	float quadratic;

};

class SpotLight : public Light
{
public:
	virtual Vector3f compute(const Vector3f& eye,
		const Vector3f& vertexPos, 
		const Vector3f& normal,
		const Material& material, 
		float shadow);
public:
	float constant;
	float linear;
	float quadratic;
	float spotFactor;
	Vector3f direction;
};


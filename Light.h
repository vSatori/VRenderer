#pragma once
#include "MathUtility.h"
#include <functional>


using LightFunction = std::function<Vector3f(const Vector3f&, const Vector3f&, const Vector3f&)>;

struct Light
{

public:
	Vector3f compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)
	{
		return function(eye, vertexPos, normal);
	}
public:
	Vector3f ambient;
	float ambientFactor;
	Vector3f diffuse;
	float diffuseFactor;
	Vector3f specular;
	float specularFactor;
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

LightFunction makeComputeDirectLightFunction(DirectionalLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)
	{
		float dot = (light->direction * -1).dot(normal);
		if (dot <= 0.f)
		{
			return light->ambient * light->ambientFactor;
		}
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = (light->direction).reflect(normal);
		float specu = pow(dot, light->specularFactor);
		Vector3f ambientFinal = light->ambient * light->ambientFactor;
		Vector3f diffuseFinal = light->diffuse * dot * light->diffuseFactor;
		Vector3f specularFinal = light->specular * specu;
		return ambientFinal + diffuseFinal + specularFinal;
	};
	return func;

}

LightFunction makeComputePointLightFunction(PointLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)
	{
		Vector3f litDir = light->pos - vertexPos;
		float distance = litDir.length();

		litDir.normalize();
		float dot = litDir.dot(normal);
		if (dot <= 0.f)
		{
			return light->ambient * light->ambientFactor;
		}
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = (litDir * -1).reflect(normal);
		float specu = pow(dot, light->specularFactor);
		float att = 1.0f / (light->constant + light->linear * distance + light->quadratic * distance * distance);
		Vector3f ambientFinal = light->ambient * light->ambientFactor;
		Vector3f diffuseFinal = light->diffuse * dot * light->diffuseFactor * att;
		Vector3f specularFinal = light->specular * specu * att;
		return ambientFinal + diffuseFinal + specularFinal;
		return Vector3f();
	};
	return func;

}

LightFunction makeComputeSpotLightFunction(SpotLight * light)
{

	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)
	{
		Vector3f litDir = light->pos - vertexPos;
		float distance = litDir.length();

		litDir.normalize();
		float dot = litDir.dot(normal);
		if (dot <= 0.f)
		{
			return light->ambient * light->ambientFactor;
		}
		Vector3f rLitDir = litDir * -1;
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = rLitDir.reflect(normal);
		float specu = pow(dot, light->specularFactor);
		float att = 1.0f / (light->constant + light->linear * distance + light->quadratic * distance * distance);
		float spot = pow(fmaxf(rLitDir.dot(light->direction), 0.0f), light->specularFactor);
		Vector3f ambientFinal = light->ambient * light->ambientFactor * spot;
		Vector3f diffuseFinal = light->diffuse * dot * light->diffuseFactor * att;
		Vector3f specularFinal = light->specular * specu * att;
		return ambientFinal + diffuseFinal + specularFinal;
		return Vector3f();
	};
	return func;

}



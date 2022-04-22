#pragma once
#include "MathUtility.h"

struct Light
{
public:
	virtual Vector3f caculate(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)const = 0;
public:
	Vector3f ambient;
	float ambientFactor;
	Vector3f diffuse;
	float diffuseFactor;
	Vector3f specular;
	float specularFactor;
	Vector3f pos;
};

struct DirectionalLight : public Light
{
public:
	virtual Vector3f caculate(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)const override
	{
		
		float dot = (direction * -1).dot(normal);
		if (dot <= 0.f)
		{
			return ambient * ambientFactor;
		}
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = (direction).reflect(normal);
		float specu = pow(dot, specularFactor);
		Vector3f ambientFinal = ambient * ambientFactor;
		Vector3f diffuseFinal = diffuse * dot * diffuseFactor;
		Vector3f specularFinal = specular* specu;
		return ambientFinal + diffuseFinal + specularFinal;
	}
public:
	Vector3f direction;
};

struct PointLight : public Light
{
public:
	virtual Vector3f caculate(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)const override
	{
		Vector3f litDir = pos - vertexPos;
		float distance = litDir.length();

		litDir.normalize();
		float dot = litDir.dot(normal);
		if (dot <= 0.f)
		{
			return ambient * ambientFactor;
		}
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = (litDir * -1).reflect(normal);
		float specu = pow(dot, specularFactor);
		float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
		Vector3f ambientFinal = ambient * ambientFactor;
		Vector3f diffuseFinal = diffuse * dot * diffuseFactor * att;
		Vector3f specularFinal = specular * specu * att;
		return ambientFinal + diffuseFinal + specularFinal;
	}
public:
	float constant;
	float linear;
	float quadratic;

};

struct SpotLight : public Light
{
public:
	virtual Vector3f caculate(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal)const override
	{
		Vector3f litDir = pos - vertexPos;
		float distance = litDir.length();

		litDir.normalize();
		float dot = litDir.dot(normal);
		if (dot <= 0.f)
		{
			return ambient * ambientFactor;
		}
		Vector3f rLitDir = litDir * -1;
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = rLitDir.reflect(normal);
		float specu = pow(dot, specularFactor);
		float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
		float spot = pow(fmaxf(rLitDir.dot(direction), 0.0f), specularFactor);
		Vector3f ambientFinal = ambient * ambientFactor * spot;
		Vector3f diffuseFinal = diffuse * dot * diffuseFactor * att;
		Vector3f specularFinal = specular * specu * att;
		return ambientFinal + diffuseFinal + specularFinal;
	}
public:
	float constant;
	float linear;
	float quadratic;
	float spotFactor;
	Vector3f direction;

};


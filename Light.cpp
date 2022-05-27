#include "Light.h"
#include "Mesh.h"

LightFunction makeComputeDirectLightFunction(DirectionalLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		float dot = (light->direction * -1).dot(normal);
		if (dot <= 0.f)
		{
			//dot *= -1;
			return light->ambient * material.ambient;
		}
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = (light->direction).reflect(normal);
		float specuFactor = pow(std::max(viewDir.dot(reflectDir), 0.f), material.shininess);
		Vector3f ambientFinal = light->ambient * material.ambient;
		Vector3f diffuseFinal = light->diffuse * material.diffuse * (dot * shadow);
		Vector3f specularFinal = light->specular * material.specular * (specuFactor * shadow);
		return (ambientFinal + diffuseFinal + specularFinal);
	};
	return func;
}

LightFunction makeComputePointLightFunction(PointLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		Vector3f litDir = light->pos - vertexPos;
		float distance = litDir.length();

		litDir.normalize();
		float dot = litDir.dot(normal);
		if (dot <= 0.f)
		{
			return light->ambient * material.ambient;
		}
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = (litDir * -1).reflect(normal);
		float specu = pow(dot, material.shininess);
		float att = 1.0f / (light->constant + light->linear * distance + light->quadratic * distance * distance);
		Vector3f ambientFinal = light->ambient * material.ambient;
		Vector3f diffuseFinal = light->diffuse * material.diffuse * dot * att;
		Vector3f specularFinal = light->specular * material.specular * specu * att;
		return ambientFinal + diffuseFinal + specularFinal;
	};
	return func;
}

LightFunction makeComputeSpotLightFunction(SpotLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		Vector3f litDir = light->pos - vertexPos;
		float distance = litDir.length();

		litDir.normalize();
		float dot = litDir.dot(normal);
		if (dot <= 0.f)
		{
			return light->ambient * material.ambient;
		}
		Vector3f rLitDir = litDir * -1;
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f reflectDir = rLitDir.reflect(normal);
		float specu = pow(dot, material.shininess);
		float att = 1.0f / (light->constant + light->linear * distance + light->quadratic * distance * distance);
		float spot = pow(fmaxf(rLitDir.dot(light->direction), 0.0f), material.shininess);
		Vector3f ambientFinal = light->ambient * material.ambient * spot;
		Vector3f diffuseFinal = light->diffuse * material.diffuse * dot * att;
		Vector3f specularFinal = light->specular * material.specular * specu * att;
		return ambientFinal + diffuseFinal + specularFinal;
		return Vector3f();
	};
	return func;
	return LightFunction();
}

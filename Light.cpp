#include "Light.h"
#include "Mesh.h"

LightFunction makeComputeDirectLightFunction(DirectionalLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f negLitDir = light->direction * -1;
		//Vector3f reflectDir = (light->direction).reflect(normal);
		Vector3f half = viewDir + negLitDir;
		half.normalize();
		float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
		Vector3f ambientFinal  = light->ambient  * material.ambient;
		Vector3f diffuseFinal  = light->diffuse  * material.diffuse  * (std::max(negLitDir.dot(normal), 0.f) * shadow);
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
		litDir.normalize();
		Vector3f negLitDir = litDir * -1;
		float distance = litDir.length();
		
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f half = viewDir + negLitDir;
		half.normalize();
		//Vector3f reflectDir = (litDir * -1).reflect(normal);
		float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
		float att = 1.0f / (light->constant + light->linear * distance + light->quadratic * distance * distance);
		Vector3f ambientFinal  = light->ambient  * material.ambient;
		Vector3f diffuseFinal  = light->diffuse  * material.diffuse  * (litDir.dot(normal) * att * shadow);
		Vector3f specularFinal = light->specular * material.specular * (specuFactor * att * shadow);
		return ambientFinal + diffuseFinal + specularFinal;
	};
	return func;
}

LightFunction makeComputeSpotLightFunction(SpotLight * light)
{
	auto func = [light](const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
	{
		Vector3f litDir = light->pos - vertexPos;
		litDir.normalize();
		Vector3f negLitDir = litDir * -1;
		float distance = litDir.length();
		Vector3f viewDir = eye - vertexPos;
		viewDir.normalize();
		Vector3f half = viewDir + negLitDir;
		half.normalize();
		float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
		float att = 1.0f / (light->constant + light->linear * distance + light->quadratic * distance * distance);
		float spot = powf(fmaxf(negLitDir.dot(light->direction), 0.0f), material.shininess);
		Vector3f ambientFinal  = light->ambient  * material.ambient  * spot;
		Vector3f diffuseFinal  = light->diffuse  * material.diffuse  * (litDir.dot(normal) * att * shadow);
		Vector3f specularFinal = light->specular * material.specular * (specuFactor * att * shadow);
		return ambientFinal + diffuseFinal + specularFinal;
		return Vector3f();
	};
	return func;
}

Vector3f DirectionalLight::compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
{
	Vector3f viewDir = eye - vertexPos;
	viewDir.normalize();
	Vector3f negLitDir = direction * -1;
	Vector3f half = viewDir + negLitDir;
	half.normalize();
	float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
	Vector3f ambientFinal  = ambient  * material.ambient;
	Vector3f diffuseFinal  = diffuse  * material.diffuse  * (std::max(negLitDir.dot(normal), 0.f) * shadow);
	Vector3f specularFinal = specular * material.specular * (specuFactor * shadow);
	return (ambientFinal + diffuseFinal + specularFinal);
}

Vector3f PointLight::compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
{
	Vector3f litDir = vertexPos - pos;
	float distance = litDir.length();
	litDir.normalize();
	Vector3f negLitDir = litDir * -1;
	Vector3f viewDir = eye - vertexPos;
	viewDir.normalize();
	Vector3f half = viewDir + negLitDir;
	half.normalize();
	float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
	Vector3f ambientFinal  = ambient * material.ambient   * att;
	Vector3f diffuseFinal  = diffuse * material.diffuse   * (std::max(negLitDir.dot(normal), 0.f) * att * shadow);
	Vector3f specularFinal = specular * material.specular * (specuFactor * att * shadow);
	return ambientFinal + diffuseFinal + specularFinal;
}

Vector3f SpotLight::compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material, float shadow)
{
	Vector3f litDir = vertexPos - pos;
	float distance = litDir.length();
	litDir.normalize();
	Vector3f negLitDir = litDir * -1;
	
	Vector3f viewDir = eye - vertexPos;
	viewDir.normalize();
	Vector3f half = viewDir + negLitDir;
	half.normalize();
	float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
	float spot = powf(fmaxf(litDir.dot(direction), 0.0f), spotFactor);
	float att = spot / (constant + linear * distance + quadratic * distance * distance);
	Vector3f ambientFinal  = ambient * material.ambient   * spot;
	Vector3f diffuseFinal  = diffuse * material.diffuse   * (std::max(negLitDir.dot(normal), 0.f) * att * shadow);
	Vector3f specularFinal = specular * material.specular * (specuFactor * att * shadow);
	return ambientFinal + diffuseFinal + specularFinal;
}

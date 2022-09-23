#include "Light.h"
#include "Mesh.h"

void DirectionalLight::compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material)
{
	Vector3f viewDir = eye - vertexPos;
	viewDir.normalize();
	Vector3f negLitDir = direction * -1;
	Vector3f half = viewDir + negLitDir;
	half.normalize();
	float specuFactor = powf(std::max(normal.dot(half), 0.f), material.shininess);
	computedAmbient  = ambient  * material.ambient;
	computedDiffuse  = diffuse  * material.diffuse  * (std::max(negLitDir.dot(normal), 0.f));
	computedSpecular = specular * material.specular * (specuFactor);
	//return (ambientFinal + diffuseFinal + specularFinal);
}

void PointLight::compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material)
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
	computedAmbient  = ambient  * material.ambient  * att;
	computedDiffuse  = diffuse  * material.diffuse  * (std::max(negLitDir.dot(normal), 0.f) * att);
	computedSpecular = specular * material.specular * (specuFactor * att);
	//return ambientFinal + diffuseFinal + specularFinal;
}

void SpotLight::compute(const Vector3f& eye, const Vector3f& vertexPos, const Vector3f& normal, const Material& material)
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
	computedAmbient  = ambient  * material.ambient  * spot;
	computedDiffuse  = diffuse  * material.diffuse  * (std::max(negLitDir.dot(normal), 0.f) * att);
	computedSpecular = specular * material.specular * (specuFactor * att);
	//return ambientFinal + diffuseFinal + specularFinal;
}

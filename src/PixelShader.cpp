#include "PixelShader.h"
#include "RenderContext.h"

static float computeShadow(const Vector4f& pos, DepthTexture* tex)
{
	float x = (pos.x + 1.f) * 0.5f;
	float y = (1.f - pos.y) * 0.5f;
	float depth = tex->sample(x,y);
	return pos.z - 0.05f < depth ? 1.f : 0.f;
}

Vector3f GenericPixelShader::execute(const Fragment& fm)
{
	Vector3f color{ 0.f, 0.f, 0.f };
	if (texture)
	{
		color = bgr2Vector(texture->sample(fm.uv.x, fm.uv.y));
		material.diffuse = color;
		material.ambient = color;
	}
	else
	{
		color = material.diffuse;
	}
	if (light)
	{
		float shadow = 1.f;
		if (depthTexture)
		{
			shadow = computeShadow(fm.posD, depthTexture);
		}
		color = light->compute(RenderContext::eyePos, fm.posW, fm.normalW, material, shadow);
	}
	if (!RenderContext::alphaBlending)
	{
		return color;
	}
	Vector3f another = bgr2Vector(RenderContext::renderTarget[RenderContext::currentPixelIndex]);
	return color * alpha + another * (1.f - alpha);
}

Vector3f SkyPixelShader::execute(const Fragment& fm)
{
	Vector3f pos = fm.posW;
	pos.normalize();
	return bgr2Vector(cubeMap->sample(pos));
}

Vector3f ReflectPixelShader::execute(const Fragment& fm)
{
	unsigned int value = envCubeMap->sample(fm.posW);
	return bgr2Vector(value);
}

Vector3f OceanWavePixelShader::execute(const Fragment& fm)
{
	Vector3f shallowColor{ 0.f, 0.64f, 0.68f };
	Vector3f deepColor{ 0.02f, 0.05f, 0.1f };
	float diff = fm.posW.y - minHeight;
	float height = (diff) / (maxHeight - minHeight);

	Vector3f heightColor = shallowColor * height + deepColor * (1 - height);

	Vector3f viewDir = RenderContext::eyePos - fm.posW;
	viewDir.normalize();

	Vector3f skyColor{ 0.65f, 0.8f, 0.95f };
	float refCoeff = powf(std::max(fm.normalW.dot(viewDir), 0.f), 0.3f);
	Vector3f reflectColor = skyColor * (1.f - refCoeff);

	Vector3f litDir = light->pos - fm.posW;
	Vector3f reflectDir = (litDir * -1).reflect(fm.normalW);
	reflectDir.normalize();
	float specu = powf(std::max(viewDir.dot(reflectDir), 0.f), 32.f) * 3;
	if (specu > 1.f)
	{
		specu = 1.f;
	}
	else if (specu < 0.f)
	{
		specu = 0.f;
	}
	Vector3f specularFinal = light->specular * specu;
	Vector3f comnbineColor = reflectColor + heightColor;
	comnbineColor *= (1.f - specu);
	comnbineColor += specularFinal;
	return comnbineColor;
	return Vector3f();
}

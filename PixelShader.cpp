#include "PixelShader.h"
#include "RenderContext.h"

static float computeShadow(const Vector4f& pos, DepthTexture* tex)
{
	float x = (pos.x + 1.f) * 0.5f;
	float y = (1.f - pos.y) * 0.5f;
	float depth = tex->sample(x,y);
	return pos.z - 0.05f < depth ? 1.f : 0.f;
}

Vector3f GenericPixelShader::execute(const VertexOut& vout)
{
	Vector3f color{ 0.f, 0.f, 0.f };
	if (texture)
	{
		color = texture->sample(vout.uv.x, vout.uv.y);
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
			//shadow = computeShadow(vout.posD, depthTexture);
		}
		color = light->compute(RenderContext::eyePos, vout.posW, vout.normalW, material, shadow);
	}
	if (!RenderContext::alphaBlending)
	{
		return color;
	}
	Vector3f another = bgr2Vector(RenderContext::renderTarget[RenderContext::currentPixelIndex]);
	return color * alpha + another * (1.f - alpha);
	return Vector3f();
}

Vector3f SkyPixelShader::execute(const VertexOut& vout)
{
	Vector3f pos = vout.posW;
	pos.normalize();
	return cubeMap->sample(pos);
	return Vector3f();
}

Vector3f ReflectPixelShader::execute(const VertexOut& vout)
{
	unsigned int value = envCubeMap->sample(vout.posW);
	float r = ((float)((value << 8)  >> 24)) / 255.f;
	float g = ((float)((value << 16) >> 24)) / 255.f;
	float b = ((float)((value << 24) >> 24)) / 255.f;
	return Vector3f{ r, g, b };
}

Vector3f OceanWavePixelShader::execute(const VertexOut& vout)
{
	Vector3f shallowColor{ 0.f, 0.64f, 0.68f };
	Vector3f deepColor{ 0.02f, 0.05f, 0.1f };
	float diff = vout.posW.y - minHeight;
	float height = (diff) / (maxHeight - minHeight);

	Vector3f heightColor = shallowColor * height + deepColor * (1 - height);

	Vector3f viewDir = RenderContext::eyePos - vout.posW;
	viewDir.normalize();

	Vector3f skyColor{ 0.65f, 0.8f, 0.95f };
	float refCoeff = powf(std::max(vout.normalW.dot(viewDir), 0.f), 0.3f);
	Vector3f reflectColor = skyColor * (1.f - refCoeff);

	Vector3f litDir = light->pos - vout.posW;
	Vector3f reflectDir = (litDir * -1).reflect(vout.normalW);
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

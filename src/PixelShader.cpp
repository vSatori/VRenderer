#include "PixelShader.h"
#include "RenderContext.h"
static float g_offset = 1.f / 1000.f;
static Vector2f g_shadowSamplePts[]{ {-g_offset, -g_offset}, {-g_offset, g_offset}, {g_offset, -g_offset}, {g_offset, g_offset},{0.f, 0.f},{0.f, -g_offset}, { 0.f, g_offset}, {g_offset, 0.f}, {-g_offset, 0.f} };

static float computeShadow(const Vector4f& pos, TextureBase<float>* tex)
{
	float x = (pos.x + 1.f) * 0.5f;
	float y = (1.f - pos.y) * 0.5f;
	float shadowFactor = 0.f;
	for (int i = 0; i < 9; ++i)
	{
		float* p = tex->sample(x + g_shadowSamplePts[i].x, y + g_shadowSamplePts[i].y);
		float depth = p[RenderContext::cxt_currentSampleIndex];
		shadowFactor += pos.z - 0.05f < depth ? 1.f : 0.f;
	}
	return shadowFactor / 9.f;
}

static float smoothStep(float t1, float t2, float x) 
{
	x = clamp(0.f, 1.f, (x - t1) / (t2 - t1));
	return x * x * (3.f - 2.f * x);
}


Vector3f GenericPixelShader::execute(const Fragment& fm)
{
	Vector3f color{ 0.f, 0.f, 0.f };
	if (texture)
	{
		color = bgr2Vector(texture->sample0(fm.uv.x, fm.uv.y));
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
		light->compute(RenderContext::cxt_eyePos, fm.posW, fm.normalW, material);
		color = light->computedAmbient + light->computedDiffuse * shadow + light->computedSpecular * shadow;
	}
	if (RenderContext::cxt_alphaMode != AlphaMode::ALPHABLENDING)
	{
		return color;
	}
	Vector3f another = RenderContext::cxt_pixelColors[RenderContext::cxt_currentPixelIndex];
	return color * RenderContext::cxt_transparency + another * (1.f - RenderContext::cxt_transparency);
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
	float height = (fm.posW.y - minHeight) / (maxHeight - minHeight);
	Vector3f heightColor = shallowColor * height + deepColor * (1.f - height);
	Vector3f viewDir = RenderContext::cxt_eyePos - fm.posW;
	viewDir.normalize();
	Vector3f skyColor{ 0.65f, 0.8f, 0.95f };
	float refCoeff = powf(std::max(fm.normalW.dot(viewDir), 0.f), 0.3f);
	Vector3f reflectColor = skyColor * (1.f - refCoeff);
	Vector3f litDir = light->pos - fm.posW;
	Vector3f reflectDir = (litDir * -1).reflect(fm.normalW);
	reflectDir.normalize();
	float specu = powf(std::max(viewDir.dot(reflectDir), 0.f), 32.f) * 3.f;
	specu = clamp(0.f, 1.f, specu);
	Vector3f specularFinal = light->specular * specu;
	Vector3f comnbineColor = reflectColor + heightColor;
	comnbineColor *= (1.f - specu);
	comnbineColor += specularFinal;
	return comnbineColor;
}

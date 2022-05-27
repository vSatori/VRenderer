#include "PixelShader.h"
#include "RenderContext.h"

static float computeShadow(const Vector4f& pos, DepthTexture* tex)
{
	float x = (pos.x + 1.f) * 0.5f;
	float y = (1.f - pos.y) * 0.5f;
	float depth = tex->sample(x,y);
	return pos.z - 0.05f < depth ? 1.f : 0.f;
}



PSFunction makeGenericPSFunction(GenericPixelShader * shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f color{ 0.f, 0.f, 0.f };
		if (shader->texture)
		{
			color = shader->texture->sample(vout.uv.x, vout.uv.y);
			shader->material.diffuse = color;
			shader->material.ambient = color;
		}
		else
		{
			color = shader->material.diffuse;
		}
		if (shader->light)
		{
			float shadow = 1.f;
			if (shader->depthTexture)
			{
				shadow = computeShadow(vout.posD, shader->depthTexture);
			}
			color = shader->light->compute(RenderContext::eyePos, vout.posW, vout.normalW, shader->material, shadow);
		}
		if (!RenderContext::alphaBlending)
		{
			return color;
		}
		Vector3f another = toVector(RenderContext::renderTarget[RenderContext::currentPixelIndex]);
		return color * shader->alpha +  another * (1.f - shader->alpha);
	};
	return func;
}

PSFunction makeReflectPixelShader(ReflectPixelShader* shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		unsigned int value = shader->envCubeMap->sample(vout.posW);
		float r = ((float)((value << 8)  >> 24)) / 255.f;
		float g = ((float)((value << 16) >> 24)) / 255.f;
		float b = ((float)((value << 24) >> 24)) / 255.f;
		return Vector3f{ r, g, b };
	};
	return func;
}

PSFunction makeSkyPSFunction(SkyPixelShader * shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f pos = vout.posW;
		pos.normalize();
		return shader->cubeMap->sample(pos);
	};
	return func;
}

PSFunction makeOceanWavePSFunction(OceanWavePixelShader* shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f shallowColor{ 0.f, 0.64f, 0.68f };
		Vector3f deepColor{ 0.02f, 0.05f, 0.1f };
		float diff = vout.posW.y - shader->minHeight;
		float height = (diff) / (shader->maxHeight - shader->minHeight);
	
		Vector3f heightColor = shallowColor * height + deepColor * (1 - height);
	
		Vector3f viewDir = RenderContext::eyePos - vout.posW;
		viewDir.normalize();

		Vector3f skyColor{ 0.65f, 0.8f, 0.95f };
		float refCoeff = pow(std::max(vout.normalW.dot(viewDir), 0.f), 0.3f);
		Vector3f reflectColor = skyColor * (1.f - refCoeff);

		Vector3f litDir = shader->light->pos - vout.posW;
		Vector3f reflectDir = (litDir * -1).reflect(vout.normalW);
		reflectDir.normalize();
		float specu = pow(std::max(viewDir.dot(reflectDir), 0.f), 32.f) * 3;
		if (specu > 1.f)
		{
			specu = 1.f;
		}
		else if (specu < 0.f)
		{
			specu = 0.f;
		}
		Vector3f specularFinal = shader->light->specular * specu;
		Vector3f comnbineColor = reflectColor + heightColor;
		comnbineColor *= (1.f - specu);
		comnbineColor += specularFinal;
		return comnbineColor;
	};
	return func;
}

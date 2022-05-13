#include "PixelShader.h"
#include "RenderContext.h"
#include <qdebug.h>
static float computeShadow(const Vector4f& pos, DepthTexture* tex)
{
	if (pos.w < RenderContext::near)
	{
		return 0.f;
	}
	Vector4f projPos = pos / pos.w;
	float x = (projPos.x + 1.f) * 0.5f;
	float y = (projPos.y + 1.f) * 0.5f;
	//float z = (projPos.z + 1.f) * 0.5f;
	if (x < 0.f || y < 0.f || x > 1.f || y > 1.f)
	{
		return 0.f;
	}
	float depth = tex->sampleValue(x,y);
	return projPos.z - 0.005f < depth ? 1.f : 0.f;
}



PSFunction makeGenericPSFunction(GenericPixelShader * shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f color;
		Vector3f litColor{ 0.f, 0.f, 0.f };
		if (shader->reflect)
		{

			unsigned int value = shader->envCubeMap->sample(vout.posW);
			float r = ((float)((value << 8) >> 24)) / 255.f;
			float g = ((float)((value << 16) >> 24)) / 255.f;
			float b = ((float)((value << 24) >> 24)) / 255.f;
			color = { r, g, b };

		}
		else
		{
			if (shader->texture)
			{
				color = shader->texture->sample(vout.vin.tex.x, vout.vin.tex.y);
			}
			else
			{
				color = shader->color;
			}
		}
		int size = shader->lights.size();
		for (int i = 0; i < size; ++i)
		{
			Light* light = shader->lights[i];
			litColor += light->compute(RenderContext::eyePos, vout.posW, vout.vin.normal);
		}
		color = color * litColor;
		if (!RenderContext::alphaBlending)
		{
			return color;
		}
		Vector3f another = toVector(RenderContext::renderTarget[vout.index]);
		return color * shader->alpha +  another * (1.f - shader->alpha);
	};
	return func;
}

PSFunction makeSkyPSFunction(SkyPixelShader * shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f pos = vout.vin.pos;
		pos.normalize();
		return shader->cubeMap->sample(pos);
	};
	return func;
}

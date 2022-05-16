#include "PixelShader.h"
#include "RenderContext.h"

static float computeShadow(const Vector4f& pos, DepthTexture* tex)
{
	float x = (pos.x + 1.f) * 0.5f;
	float y = (1.f - pos.y) * 0.5f;
	float depth = tex->sampleValue(x,y);
	return pos.z - 0.05f < depth ? 1.f : 0.f;
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
		if (shader->light)
		{

			float shadow = 1.f;
			if (shader->depthTexture)
			{
				shadow = computeShadow(vout.depthPos, shader->depthTexture);
			}
			
			litColor += shader->light->compute(RenderContext::eyePos, vout.posW, vout.vin.normal, shadow);
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

#pragma once
#include "Mesh.h"
using PSFunction = std::function<Vector3f(const VertexOut&)>;
class PixelShader
{
public:
	Vector3f execute(const VertexOut& vout)
	{
		return function(vout);
	}
public:
	PSFunction function;
};

struct GenericPixelShader : public PixelShader
{
public:
	bool reflect;
	std::weak_ptr<DynamicCubeMap> envCubeMap;
	std::shared_ptr<Texture> texture;
	std::vector<std::shared_ptr<Light>> lights;
};

struct SkyPixelShader : public PixelShader
{
public:
	std::weak_ptr<CubeMap> cubeMap;
};

PSFunction makeGenericPSFunction(GenericPixelShader* shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f color;
		Vector3f litColor;
		if (shader->reflect)
		{
			auto cm = shader->envCubeMap.lock();
			if (cm)
			{
				unsigned int value = cm->sample(vout.posW);
				float r = ((float)((value << 8) >> 24)) / 255.f;
				float g = ((float)((value << 16) >> 24)) / 255.f;
				float b = ((float)((value << 24) >> 24)) / 255.f;
				color = { r, g, b };
			}
		}
		else
		{
			if (shader->texture)
			{
				color = shader->texture->sample(vout.vin.tex.x, vout.vin.tex.y);
			}
		}

		for (const auto& light : shader->lights)
		{
			litColor += light->compute(RenderContext::eyePos, vout.posW, vout.normalW);
		}
		return color * litColor;
	};
	return func;
}

PSFunction makeSkyPSFunction(SkyPixelShader* shader)
{
	auto func = [shader](const VertexOut& vout)
	{
		Vector3f pos = vout.vin.pos;
		pos.normalize();
		return shader->cubeMap.lock()->sample(pos);
	};
	return func;
}


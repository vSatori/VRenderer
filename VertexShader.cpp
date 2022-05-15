#include "VertexShader.h"

VSFunction makeGenericVSFunction(GenericVertexShader * shader)
{
	auto func = [shader](const Vertex& vin)
	{
		VertexOut out;
		out.vin = vin;
		Vector4f posW = shader->world * vin.pos;
		Vector4f posV = shader->view * posW;
		Vector4f posH = shader->projection * posV;
		out.posW = { posW.x, posW.y, posW.z };
		out.posH = posH;
	    out.depthPos = shader->shadowProjection * shader->matLitView * posW;
		return out;
	};
	return func;
}

VSFunction makeSkyVSFunction(SkyVertexShader * shader)
{
	auto func = [shader](const Vertex& vin)
	{
		shader->view.m[0][3] = 0.f;
		shader->view.m[1][3] = 0.f;
		shader->view.m[2][3] = 0.f;
		Vector4f posW = shader->world * vin.pos;
		Vector4f posH = shader->projection * shader->view * posW;
		posH.z = posH.w;
		return VertexOut{ vin, {posW.x, posW.y, posW.z}, posH };
	};
	return func;
}

VSFunction makeShadowMapVSFunction(ShadowMapVertexShader* shader)
{
	auto func = [shader](const Vertex& vin)
	{
		VertexOut out;
		out.vin = vin;
		Vector4f posW = shader->world * vin.pos;
		Vector4f posH = shader->projection * shader->view * posW;
		out.posW = { posW.x, posW.y, posW.z };
		out.posH = posH;
		return out;
	};
	return func;
}

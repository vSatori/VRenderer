#include "VertexShader.h"

VSFunction makeGenericVSFunction(GenericVertexShader * shader)
{
	auto func = [shader](const Vertex& vin)
	{
		VertexOut out;
		Vector4f posW = shader->world * vector3To4(vin.pos);
		Vector4f posH = shader->vp * posW;
		out.uv = vin.tex;
		out.posM = vin.pos;
		out.posW = Vector4To3(posW);
		out.posH = posH;
		out.posD = shader->shadow * posW;
		out.normalW = shader->world3 * vin.normal;
		out.normalW.normalize();
		return out;
	};
	return func;
}

VSFunction makeSkyVSFunction(SkyVertexShader * shader)
{
	auto func = [shader](const Vertex& vin)
	{
		VertexOut out;
		shader->view.m[0][3] = 0.f;
		shader->view.m[1][3] = 0.f;
		shader->view.m[2][3] = 0.f;;
		Vector4f posH = shader->projection * shader->view * vector3To4(vin.pos);
		posH.z = posH.w;
		out.uv = vin.tex;
		out.posM = vin.pos;
		out.posW = out.posM;
		out.posH = posH;
		return out;
	};
	return func;
}

VSFunction makeShadowMapVSFunction(ShadowMapVertexShader* shader)
{
	auto func = [shader](const Vertex& vin)
	{
		VertexOut out;
		Vector4f posW = shader->world * vector3To4(vin.pos);
		out.posH = shader->vp * posW;
		return out;
	};
	return func;
}

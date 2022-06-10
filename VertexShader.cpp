#include "VertexShader.h"

VertexOut GenericVertexShader::execute(const Vertex& vin)
{
	VertexOut out;
	Vector4f posW = world * vector3To4(vin.pos);
	Vector4f posH = vp * posW;
	out.uv = vin.tex;
	out.posM = vin.pos;
	out.posW = Vector4To3(posW);
	out.posH = posH;
	out.posD = shadow * posW;
	out.normalW = world3 * vin.normal;
	out.normalW.normalize();
	return out;
}

VertexOut SkyVertexShader::execute(const Vertex& vin)
{
	VertexOut out;
	view.m[0][3] = 0.f;
	view.m[1][3] = 0.f;
    view.m[2][3] = 0.f;;
	Vector4f posH = projection * view * vector3To4(vin.pos);
	posH.z = posH.w;
	out.uv = vin.tex;
	out.posM = vin.pos;
	out.posW = out.posM;
	out.posH = posH;
	return out;
}

VertexOut ShadowMapVertexShader::execute(const Vertex& vin)
{
	VertexOut out;
	Vector4f posW = world * vector3To4(vin.pos);
	out.posH = vp * posW;
	return out;
}

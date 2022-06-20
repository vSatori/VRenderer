#include "VertexShader.h"

Fragment GenericVertexShader::execute(const Vertex& vin)
{
	Fragment out;
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
#include "Transform.h"
Fragment SkyVertexShader::execute(const Vertex& vin)
{
	Fragment out;
	view.m[0][3] = 0.f;
	view.m[1][3] = 0.f;
    view.m[2][3] = 0.f;;
	Vector4f posH = projection * view * vector3To4(vin.pos);
	posH.z = posH.w * 0.99999f;
	out.posM = vin.pos;
	out.posW = out.posM;
	out.posH = posH;
	return out;
}

Fragment ShadowMapVertexShader::execute(const Vertex& vin)
{
	Fragment out;
	Vector4f posW = world * vector3To4(vin.pos);
	out.posH = vp * posW;
	return out;
}

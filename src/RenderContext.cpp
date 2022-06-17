#include "RenderContext.h"
#include "omp.h"
/*
#ifdef _OPENMP
omp_lock_t lock;
omp_init_lock(&lock);
#endif // _omp
*/

static unsigned int whiteValue = (255 << 16) + (255 << 8) + 255;
static unsigned int redValue = (255 << 16);

const float RenderContext::clipW = 0.00001f;
CullMode RenderContext::cullMode = CullMode::CULLBACKFACE;
FillMode RenderContext::fillMode = FillMode::SOLID;
DepthMode RenderContext::depthMode = DepthMode::LESS;
bool RenderContext::alphaBlending = false;
bool RenderContext::drawColor = true;
bool RenderContext::clockwise = true;
Vector3f* RenderContext::pixels = nullptr;
unsigned int* RenderContext::renderTarget = nullptr;
float* RenderContext::zbuffer = nullptr;
bool* RenderContext::pixelMask = nullptr;
Vector3f RenderContext::eyePos = { 0.f, 0.f, 0.f };
float RenderContext::nearPlane = 0.f;
float RenderContext::farPlane = 100.f;
VertexShader* RenderContext::vs = nullptr;
PixelShader* RenderContext::ps = nullptr;
unsigned int RenderContext::currentPixelIndex = 0;


void RenderContext::init()
{
	renderTarget = new unsigned int[width * height];
	zbuffer = new float[width * height];
}

void RenderContext::finalize()
{
	if (renderTarget)
	{
		delete renderTarget;
	}
	if (zbuffer)
	{
		delete zbuffer;
	}
}

void RenderContext::clear()
{
	int count = width * height;
	
	for (int i = 0; i < count; ++i)
	{
		renderTarget[i] = whiteValue;
		zbuffer[i] = 1.00001f;
	}
}

bool RenderContext::Cull(const Vector3f& vertexPos, const Vector3f& faceNormal)
{
	switch (cullMode)
	{
	case CullMode::CULLNONE:
	{
		return true;
	}
	case CullMode::CULLFRONTFACE:
	{
		bool b = (faceNormal.dot(eyePos - vertexPos) >= 0);
		if (clockwise)
		{
			return !b;
		}
		return b;

		//return ~((faceNormal.dot(eyePos - vertexPos) <= 0) ^ clockwise);
	}
	case CullMode::CULLBACKFACE:
	{
	
		bool b = (faceNormal.dot(eyePos - vertexPos) >= 0);
		if (clockwise)
		{
			return b;
		}
		return !b;

		//return ~((faceNormal.dot(eyePos - vertexPos) >= 0) ^ clockwise));
		
	}
	default:
	{
		return false;
	}
	}
}

bool RenderContext::depthTest(float z, int index)
{
	switch (depthMode)
	{
	case DepthMode::NEVER:
	{
		return false;
	}
	case DepthMode::LESS:
	{
		return z < zbuffer[index];
	}
	case DepthMode::EQUAL:
	{
		return z == zbuffer[index];
	}
	case DepthMode::LESSEQUAL:
	{
		return z <= zbuffer[index];
	}
	case DepthMode::ALWAYS:
	{
		return true;
	}
	default:
		return false;
	}
	return false;
}

void RenderContext::draw(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3)
{
	switch (fillMode)
	{
	case FillMode::SOLID:
	{
		drawFragment(vo1, vo2, vo3);
		break;
	}
	case FillMode::WIREFRAME:
	{
		Vector2f p1{ ((vo1.posH.x + 1.f) * 0.5f * width),  ((1.f - vo1.posH.y) * 0.5f * height) };
		Vector2f p2{ ((vo2.posH.x + 1.f) * 0.5f * width),  ((1.f - vo2.posH.y) * 0.5f * height) };
		Vector2f p3{ ((vo3.posH.x + 1.f) * 0.5f * width),  ((1.f - vo3.posH.y) * 0.5f * height) };
		int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
		int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
		int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);
		int h = height - 1;
		int w = width  - 1;
		x1 = clamp(0, w, x1);
		x2 = clamp(0, w, x2);
		x3 = clamp(0, w, x3);
		y1 = clamp(0, h, y1);
		y2 = clamp(0, h, y2);
		y3 = clamp(0, h, y3);
		drawLine(x1, y1, x2, y2);
		drawLine(x2, y2, x3, y3);
		drawLine(x3, y3, x1, y1);
		break;
	}
	default:
		break;
	}
}

bool RenderContext::checkClipping(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3)
{
	if (v1.x + v1.w < 0 || v2.x + v2.w < 0 || v3.x + v3.w < 0)
	{
		return true;
	}
	if (v1.x - v1.w > 0 || v2.x - v2.w > 0 || v3.x - v3.w > 0)
	{
		return true;
	}
	if (v1.y + v1.w < 0 || v2.y + v2.w < 0 || v3.y + v3.w < 0)
	{
		return true;
	}
	if (v1.y - v1.w > 0 || v2.y - v2.w > 0 || v3.y - v3.w > 0)
	{
		return true;
	}
	if (v1.z + v1.w < 0 || v2.z + v2.w < 0 || v3.y + v3.w < 0)
	{
		return true;
	}
	if (v1.z - v1.w > 0 || v2.z - v2.w > 0 || v3.y - v3.w > 0)
	{
		return true;
	}
	float w = clipW > nearPlane ? clipW : nearPlane;
	if (v1.w < w || v2.w < w || v3.w < w)
	{
		return true;
	}
	return false;
}
#include <iostream>
void RenderContext::drawFragment(const VertexOut & vo1, const VertexOut & vo2, const VertexOut & vo3)
{
	Vector2f p1{ ((vo1.posH.x + 1.f) * 0.5f * width),  ((1.f - vo1.posH.y) * 0.5f * height) };
	Vector2f p2{ ((vo2.posH.x + 1.f) * 0.5f * width),  ((1.f - vo2.posH.y) * 0.5f * height) };
	Vector2f p3{ ((vo3.posH.x + 1.f) * 0.5f * width),  ((1.f - vo3.posH.y) * 0.5f * height) };

	int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
    int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
	int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), width  - 1);
	int maxy = std::min(findMax(y1, y2, y3), height - 1);

	VertexOut vout;
	
	float* pVout = (float*)&vout;
	const float* pVo1 = (const float*)&vo1;
	const float* pVo2 = (const float*)&vo2;
	const float* pVo3 = (const float*)&vo3;

	//Vector2f samples[4] = { {-0.125f, -0.375f}, {0.375f, -0.125f}, {-0.375f, 0.125f}, {0.125f, 0.375f} };
	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			Vector2f p{ static_cast<float>(x), static_cast<float>(y) };
			float areaA = ((p - p2).corss(p3 - p2));
			float areaB = ((p - p3).corss(p1 - p3));
			float areaC = ((p - p1).corss(p2 - p1));

			if (clockwise)
			{
				bool okA = areaA < 0.00001f;
				bool okB = areaB < 0.00001f;
				bool okC = areaC < 0.00001f;

				if (!(okA && okB && okC))
				{
					continue;
				}
			}
			else
			{
				bool okD = areaA > -0.00001f;
				bool okE = areaB > -0.00001f;
				bool okF = areaC > -0.00001f;

				if (!(okD && okE && okF))
				{
					continue;
				}
			}
			float area = areaA + areaB + areaC;
			float a = areaA / area;
			float b = areaB / area;
			float c = areaC / area;

			float z = (vo1.posH.z * a + vo2.posH.z * b + vo3.posH.z * c);
			currentPixelIndex = width * y + x;
			if (!depthTest(z, currentPixelIndex))
			{
				continue;
			}
			zbuffer[currentPixelIndex] = z;
			
			if (!drawColor)
			{
				continue;
			}

			a /= vo1.posH.w;
			b /= vo2.posH.w;
			c /= vo3.posH.w;
			float hz = 1.f / (a + b + c);
			for (int i = 0; i < VertexOut::floatSize; ++i)
			{
				pVout[i] = (pVo1[i] * a + pVo2[i] * b + pVo3[i] * c) * hz;
			}

			Vector3f color = ps->execute(vout);
			float* pc = (float*)&color;
			for (int i = 0; i < 3; ++i)
			{
				if (pc[i] > 1.f)
				{
					pc[i] = 1.f;
				}
			}
		
			renderTarget[currentPixelIndex] = colorValue(color);
			
		}
	}
}
void RenderContext::resolve()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = y * width + x;
			int count = 0;
			float coef = 0.f;
			for (int i = 0; i < 4; ++i)
			{
				if (pixelMask[index * 4 + i])
				{
					coef += 1.f;
				}
			}
			coef /= 4.f;
			Vector3f& color = pixels[index];
			color *= coef;
			renderTarget[index] = colorValue(color);
		}
	}
}
void RenderContext::drawLine(int x1, int y1, int x2, int y2)
{
	bool steep = false;
	if (std::abs(x1 - x2) < std::abs(y1 - y2))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		steep = true;
	}

	if (x1 > x2) 
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	int dx = x2 - x1;
	int dy = y2 - y1;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y1;
	int index = 0;
	for (int x = x1; x <= x2; x++)
	{
		if (steep)
		{
			renderTarget[y + width * x] = redValue;
		}
		else
		{
			renderTarget[x + width * y] = redValue;
		}
		error2 += derror2;
		if (error2 > dx)
		{
			y += (y2 > y1 ? 1 : -1);
			error2 -= dx * 2;
		}
		++index;
	}
}



std::vector<VertexOut> RenderContext::polygonClipping(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3)
{
	std::vector<VertexOut> output;
	std::vector<VertexOut> inputW{ vo1, vo2, vo3 };
	for (int i = 0; i < inputW.size(); ++i)
	{
		auto& current = inputW[i];
		auto& last = inputW[(i + inputW.size() - 1) % inputW.size()];

		if (inside(current.posH, 6))
		{
			if (!inside(last.posH, 6))
			{
				output.push_back(clipWPlane(current, last));

			}
			output.push_back(current);
		}
		else if (inside(last.posH, 6))
		{
			output.push_back(clipWPlane(current, last));
		}
	}
	for (int i = 0; i < 6; ++i)
	{
		std::vector<VertexOut> input(output);
		output.clear();
		int size = input.size();
		for (int j = 0; j < size; ++j)
		{
			auto& current = input[j];
			auto& last = input[(j + size - 1) % size];
			if (inside(current.posH, i))
			{
				if (!inside(last.posH, i))
				{
					output.push_back(clipPlane(last, current, i));
				}
				output.push_back(current);
			}
			else if (inside(last.posH, i))
			{
				output.push_back(clipPlane(last, current, i));
			}
		}
	}
	return output;
}

VertexOut RenderContext::clipPlane(const VertexOut& vo1, const VertexOut& vo2, int side)
{
	float k1, k2;
	switch (side)
	{
	case 0:
	case 1:
	{
		k1 = vo1.posH.x;
		k2 = vo2.posH.x;
		break;
	}
	case 2:
	case 3:
	{
		k1 = vo1.posH.y;
		k2 = vo2.posH.y;
		break;
	}
	case 4:
	case 5:
	{
		k1 = vo1.posH.z;
		k2 = vo2.posH.z;
		break;
	}
	case 6:
	{
		k1 = vo1.posH.w;
		k2 = vo2.posH.w;
		break;
	}
	}
	if (side == 0 || side == 3 || side == 4)
	{
		k1 *= -1;
		k2 *= -1;
	}
	float t = (vo1.posH.w - k1) / ((vo1.posH.w - k1) - (vo2.posH.w - k2));
	/*
	float c1 = v1.dot(side);
	float c2 = v2.dot(side);
	float weight = c2 / (c2 - c1);
	*/
	return vo1 + (vo2 - vo1) * t;
}

VertexOut RenderContext::clipWPlane(const VertexOut& vo1, const VertexOut& vo2)
{
	float w = clipW > nearPlane ? clipW : nearPlane;
	float t = (vo1.posH.w - w) / (vo1.posH.w - vo2.posH.w);
	VertexOut vo = vo1 + (vo2 - vo1) * t;
	return vo;
}

bool RenderContext::inside(const Vector4f& pos, int side)
{
	bool in = true;
	switch (side)
	{
	case 0:
	{
		in = pos.x >= -pos.w;
		break;
	}
	case 1:
	{
		in = pos.x <= pos.w;
		break;
	}
	case 2:
	{
		in = pos.y <= pos.w;
		break;
	}
	case 3:
	{
		in = pos.y >= -pos.w;

		break;
	}
	case 4:
	{
		in = pos.z >= -pos.w;
		break;
	}
	case 5:
	{
		in = pos.z <= pos.w;
		break;
	}
	case 6:
	{
		float w = clipW > nearPlane ? clipW : nearPlane;
		in = pos.w >= w;

		break;
	}
	}
	return in;
}

static void lerp(VertexOut& vo, const VertexOut& vo1, const VertexOut& vo2, float t)
{
	float* pv = (float*)(&vo);
	const float* pv1 = (const float*)(&vo1);
	const float* pv2 = (const float*)(&vo2);
	for (int i = 0; i < VertexOut::floatSize; ++i)
	{
		pv[i] = pv1[i] + (pv2[i] - pv1[i]) * t;
	}
}
/*
static void lerp(VertexOut& vo, const VertexOut& vo1, const VertexOut& vo2, float t)
{
	float* pv = (float*)(&vo1);
	const float* pv1 = (const float*)(&vo1);
	const float* pv2 = (const float*)(&vo2);
	float z1 = vo1.posH.w;
	float z2 = vo2.posH.w;
	for (int i = 0; i < 19; ++i)
	{
		pv[i] = pv1[i] / z1 + (pv2[i] / z2 - pv1[i] / z1) * t;

	}
	float z = vo.posH.w;
	for (int i = 0; i < 19; ++i)
	{
		pv[i] *= z;
	}
}
*/

/*
void RenderContext::drawFragmentByScanLine(VertexOut & vo1, VertexOut & vo2, VertexOut & vo3)
{
	if (vo1.posH.y == vo2.posH.y)
	{
		if (vo1.posH.y > vo3.posH.y)
		{
			drawTopTriangle(vo1, vo2, vo3);
			return;
		}
		drawBottomTriangle(vo3, vo1, vo2);
		return;
		
	}
	if (vo1.posH.y == vo3.posH.y)
	{
		if (vo1.posH.y > vo2.posH.y)
		{
			drawTopTriangle(vo1, vo3, vo2);
			return;
		}
		drawBottomTriangle(vo2, vo1, vo3);
		return;

	}
	if (vo2.posH.y == vo3.posH.y)
	{
		if (vo2.posH.y > vo1.posH.y)
		{
			drawTopTriangle(vo2, vo3, vo1);
			return;
		}
		
		drawBottomTriangle(vo1, vo2, vo3);
		return;
	}
	std::vector<VertexOut> temp{ vo1, vo2, vo3 };
	std::sort(temp.begin(), temp.end(), [](const VertexOut& v1, const VertexOut& v2)
	{
		return v1.posH.y > v2.posH.y;
	});

	VertexOut v1 = temp[0];
	VertexOut v2= temp[1];
	VertexOut v3= temp[2];

	//sort(vo1, vo2, vo3);
	VertexOut v4;
	float t = (v2.posH.y - v1.posH.y) / (v3.posH.y - v1.posH.y);
	lerp(v4, v1, v3, t);
	drawBottomTriangle(v1, v2, v4);
	drawTopTriangle(v2, v4, v3);
}


void RenderContext::drawTopTriangle(VertexOut & vo1, VertexOut & vo2, VertexOut & vo3)
{
	if (vo1.posH.x > vo2.posH.x)
	{
		std::swap(vo1, vo2);
	}

	Vector2f p1{ ((vo1.posH.x + 1.f) * 0.5f * width),  ((1.f - vo1.posH.y) * 0.5f * height) };
	Vector2f p2{ ((vo2.posH.x + 1.f) * 0.5f * width),  ((1.f - vo2.posH.y) * 0.5f * height) };
	Vector2f p3{ ((vo3.posH.x + 1.f) * 0.5f * width),  ((1.f - vo3.posH.y) * 0.5f * height) };

	int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
	int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
	int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);

	int dy1 = y3 - y1;
	int dy2 = y3 - y2;
	VertexOut start;
	VertexOut end;
	VertexOut vout;
	float z1, z2, z3;
	float zStart, zEnd;
	for (int y = y1; y < y3; ++y)
	{
		z1 = vo1.posH.z;
		z2 = vo2.posH.z;
		z3 = vo3.posH.z;
		int t1 = 0;
		int t2 = 0;
		if (dy1 != 0)
		{
			t1 = (float)(y - y1) / (float)dy1;
			
		};
		if (dy2 != 0)
		{
			t2 = (float)(y - y2) / (float)dy2;
		}
		lerp(start, vo1, vo3, t1);
		lerp(end, vo2, vo3, t2);
		int xStart = x1 + (x3 - x1) * t1;
		int xEnd = x2 + (x3 - x2) * t2;
		zStart = z1 + (z3 - z1) * t1;
		zEnd = z2 + (z3 - z2) * t2;
		int diffx = xEnd - xStart;
		for (int x = xStart; x < xEnd; ++x)
		{
			currentPixelIndex = y * width + x;
			float t = 0;
			if (diffx != 0)
			{
				t = (float)(x - xStart) / (float)diffx;
			}
			float z = zStart + (zEnd - zStart) * t;
			if (zbuffer[currentPixelIndex] < z)
			{
				continue;
			}
			zbuffer[currentPixelIndex] = z;
			lerp(vout, start, end, t);
			Vector3f color = ps->execute(vout);
			int red = color.x * 255;
			int green = color.y * 255;
			int blue = color.z * 255;
			unsigned int colorValue = (red << 16) + (green << 8) + blue;
			renderTarget[currentPixelIndex] = colorValue;
		}

	}


}

void RenderContext::drawBottomTriangle(VertexOut & vo1, VertexOut & vo2, VertexOut & vo3)
{
	if (vo2.posH.x > vo3.posH.x)
	{
		std::swap(vo2, vo3);
	}

	Vector2f p1{ ((vo1.posH.x + 1.f) * 0.5f * width),  ((1.f - vo1.posH.y) * 0.5f * height) };
	Vector2f p2{ ((vo2.posH.x + 1.f) * 0.5f * width),  ((1.f - vo2.posH.y) * 0.5f * height) };
	Vector2f p3{ ((vo3.posH.x + 1.f) * 0.5f * width),  ((1.f - vo3.posH.y) * 0.5f * height) };

	int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
	int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
	int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);
	
	int dy1 = y2 - y1;
	int dy2 = y3 - y1;
	VertexOut start;
	VertexOut end;
	VertexOut vout;
	float z1, z2, z3;
	float zStart, zEnd;
	for (int y = y1; y < y3; ++y)
	{
		z1 = vo1.posH.z;
		z2 = vo2.posH.z;
		z3 = vo3.posH.z;
		float t1 = 0;
		float t2 = 0;
		if (dy1 != 0)
		{
			t1 = (float)(y - y1) / (float)dy1;
		}
		if (dy2 != 0)
		{
			t2 = (float)(y - y1) / (float)dy2;
		}
		lerp(start, vo1, vo2, t1);
		lerp(end, vo1, vo3, t2);
		int xStart = x1 + (x2 - x1) * t1;
		int xEnd = x1 + (x3 - x1) * t2;
		zStart = z1 + (z2 - z1) * t1;
		zEnd = z1 + (z3 - z1) * t2;
		int diffx = xEnd - xStart;
		for (int x = xStart; x < xEnd; ++x)
		{
			currentPixelIndex = y * width + x;
			
			float t = 0;
			if (diffx != 0)
			{
				t = (float)(x - xStart) / (float)diffx;
			}
			float z = zStart + (zEnd - zStart) * t;
			if (zbuffer[currentPixelIndex] < z)
			{
				continue;
			}
			zbuffer[currentPixelIndex] = z;
			lerp(vout, start, end, t);
			Vector3f color = ps->execute(vout);
			int red = color.x * 255;
			int green = color.y * 255;
			int blue = color.z * 255;
			unsigned int colorValue = (red << 16) + (green << 8) + blue;
		}

	}
}
*/
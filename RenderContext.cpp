#include "RenderContext.h"
#include "omp.h"
/*
#ifdef _OPENMP
omp_lock_t lock;
omp_init_lock(&lock);
#endif // _omp
*/

const float RenderContext::clipW = 0.00001f;
CullMode RenderContext::cullMode = CullMode::CULLBACKFACE;
FillMode RenderContext::fillMode = FillMode::SOLID;
bool RenderContext::alphaBlending = false;
bool RenderContext::drawColor = true;
bool RenderContext::posArea = false;
unsigned int* RenderContext::renderTarget = nullptr;
float* RenderContext::zbuffer = nullptr;
Vector3f RenderContext::eyePos = { 0.f, 0.f, 0.f };
float RenderContext::nearPlane = 0.f;
float RenderContext::farPlane = 100.f;
VertexShader* RenderContext::vs = nullptr;
PixelShader* RenderContext::ps = nullptr;
unsigned int RenderContext::currentPixelIndex = 0;
int RenderContext::count = 0;
int RenderContext::allCount = 0;

void RenderContext::init()
{
	cullMode = CullMode::CULLBACKFACE;
	fillMode = FillMode::SOLID;
	renderTarget = new unsigned int[width * height];
	zbuffer = new float[width * height];
	vs = nullptr;
	ps = nullptr;
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
	//memset(renderTarget, 0, width * height * 4);

		//memset(zbuffer, 1, width * height * 4);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			renderTarget[i * width + j] = whiteValue;
		}
	}

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			zbuffer[i * width + j] = 1.001f;
		}
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
		return faceNormal.dot(eyePos - vertexPos) <= 0;
	}
	case CullMode::CULLBACKFACE:
	{
		return faceNormal.dot(eyePos - vertexPos) >= 0;
	}
	default:
	{
		return false;
	}
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
	if (v1.w < clipW || v2.w < clipW || v3.w < clipW)
	{
		return true;
	}
	return false;
}

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
	int maxx = std::min(findMax(x1, x2, x3), width - 1);
	int maxy = std::min(findMax(y1, y2, y3), height - 1);

	
	VertexOut vout;
	
	float* pVout = (float*)&vout;
	const float* pVo1 = (const float*)&vo1;
	const float* pVo2 = (const float*)&vo2;
	const float* pVo3 = (const float*)&vo3;
	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			++allCount;
			Vector2f p{ static_cast<float>(x), static_cast<float>(y)};
			float areaA = ((p - p2).corss(p3 - p2));
			float areaB = ((p - p3).corss(p1 - p3));
			float areaC = ((p - p1).corss(p2 - p1));
			if (!posArea)
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
				bool okA = areaA > -0.00001f;
				bool okB = areaB > -0.00001f;
				bool okC = areaC > -0.00001f;
				if (!(okA && okB && okC))
				{
					continue;
				}
			}
			++count;
			
			float area = areaA + areaB + areaC;
			float a = areaA / area;
			float b = areaB / area;
			float c = areaC / area;
			
			float z = (vo1.posH.z * a + vo2.posH.z * b + vo3.posH.z * c);
			currentPixelIndex = width * y + x;
			if (zbuffer[currentPixelIndex] <= z)
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
			for (int i = 0; i < 19; ++i)
			{
				pVout[i] = (pVo1[i] * a + pVo2[i] * b + pVo3[i] * c) * hz;
			}
			Vector3f color =  ps->execute(vout);
			float* pc = (float*)&color;
			for (int i = 0; i < 3; ++i)
			{
				if (*(pc + i) > 1.f)
				{
					*(pc + i) = 1.f;
				}
			}
			int red = static_cast<int>(color.x * 255.f);
			int green = static_cast<int>(color.y * 255.f);
			int blue = static_cast<int>(color.z * 255.f);
			unsigned int colorValue = (red << 16) + (green << 8) + blue;
			renderTarget[currentPixelIndex] = colorValue;
		}
	}
} 

static void sort(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3)
{
	if (vo1.posH.y > vo2.posH.y)
	{
		if (vo2.posH.y > vo3.posH.y)
		{
			return;
		}
		if (vo1.posH.y > vo3.posH.y)
		{
			std::swap(vo2, vo3);
			return;
		}
		std::swap(vo1, vo3);
		return;
	}
	if (vo2.posH.y > vo3.posH.y)
	{
		if (vo1.posH.y > vo3.posH.y)
		{
			std::swap(vo1, vo2);
			return;
		}
		std::swap(vo1, vo2);
		std::swap(vo2, vo3);
		return;

	}
	std::swap(vo1, vo3);
	std::swap(vo2, vo3);
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
	return std::vector<VertexOut>();
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
	float t = (vo1.posH.w - clipW) / (vo1.posH.w - vo2.posH.w);
	return vo1 + (vo2 - vo1) * t;
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
		in = pos.w >= clipW;

		break;
	}
	}
	return in;
}


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
	if (vo1.posH.y == vo3.posH.z)
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

	vo1 = temp[0];
	vo2 = temp[1];
	vo3 = temp[2];

	//sort(vo1, vo2, vo3);
	VertexOut vo4;
	float t = (vo2.posH.y - vo1.posH.y) / (vo3.posH.y - vo1.posH.y);
	//ModelLerpFunction(vo4, vo1, vo3, t);
	drawBottomTriangle(vo1, vo2, vo4);
	drawTopTriangle(vo2, vo4, vo3);
}


void RenderContext::drawTopTriangle(VertexOut & vo1, VertexOut & vo2, VertexOut & vo3)
{

	if (vo1.posH.x > vo2.posH.x)
	{
		std::swap(vo1, vo2);
	}

	Vector2f p1{ ((vo1.posH.x + 1.f) * 0.5f * width),  int((1.f - vo1.posH.y) * 0.5f * height) };
	Vector2f p2{ ((vo2.posH.x + 1.f) * 0.5f * width),  int((1.f - vo2.posH.y) * 0.5f * height) };
	Vector2f p3{ ((vo3.posH.x + 1.f) * 0.5f * width),  int((1.f - vo3.posH.y) * 0.5f * height) };

	int x1 = p1.x; int y1 = p1.y;
	int x2 = p2.x; int y2 = p2.y;
	int x3 = p3.x; int y3 = p3.y;

	float dy = y3 - y1;
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
		float t1 = (y - y1) / dy;
		ModelLerpFunction(start, vo1, vo2, t1, 1.f / z1, 1.f / z2);
		ModelLerpFunction(end, vo1, vo3, t1, 1.f / z1, 1.f / z3);
		int xStart = x1 + (x3 - x1) * t1;
		int xEnd = x2 + (x3 - x2) * t1;
		zStart = z1 + (z3 - z1) * t1;
		zEnd = z2 + (z3 - z2) * t1;
		for (int x = xStart; x < xEnd; ++x)
		{
			currentPixelIndex = y * width + x;
			float t = (x - xStart) / (xEnd - xStart);
			float z = zStart + (zEnd - zStart) * t;
			if (zbuffer[currentPixelIndex] < z)
			{
				continue;
			}
			zbuffer[currentPixelIndex] = z;
			ModelLerpFunction(vout, start, end, (x - xStart) / (xEnd - xStart), zStart, zEnd);
			Vector3f color = ps->execute(vout);
			int red = color.x * 255;
			int green = color.y * 255;
			int blue = color.z * 255;
			unsigned int colorValue = (255 << 16) + (0 << 8) + 0;
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

	int x1 = p1.x; int y1 = p1.y;
	int x2 = p2.x; int y2 = p2.y;
	int x3 = p3.x; int y3 = p3.y;
	
	float dy = y2 - y1;
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
		float t1 = (y - y1) / dy;
		ModelLerpFunction(start, vo1, vo2, t1, 1.f / z1, 1.f / z2);
		ModelLerpFunction(end, vo1, vo3, t1, 1.f / z1, 1.f / z3);
		int xStart = x1 + (x2 - x1) * t1;
		int xEnd = x1 + (x3 - x1) * t1;
		zStart = z1 + (z2 - z1) * t1;
		zEnd = z1 + (z3 - z1) * t1;
		for (int x = xStart; x < xEnd; ++x)
		{
			currentPixelIndex = y * width + x;
			float t = (x - xStart) / (xEnd - xStart);
			float z = zStart + (zEnd - zStart) * t;
			if (zbuffer[currentPixelIndex] < z)
			{
				continue;
			}
			zbuffer[currentPixelIndex] = z;
			ModelLerpFunction(vout, start, end, t, 1.f / zStart, 1.f / zEnd);
			Vector3f color = ps->execute(vout);
			int red = color.x * 255;
			int green = color.y * 255;
			int blue = color.z * 255;
			unsigned int colorValue = (255 << 16) + (0 << 8) + 0;
			renderTarget[currentPixelIndex] = colorValue;
		}

	}
}

*/
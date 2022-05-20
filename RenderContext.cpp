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
FragmentLerpFunction RenderContext::lerpFunc = genericLerpFunction;


void genericLerpFunction(VertexOut& vout, const VertexOut & vo1, const VertexOut & vo2, const VertexOut & vo3, float a, float b, float c)
{
	//float z1 = 1.f / vo1.posH.w;
	//float z2 = 1.f / vo2.posH.w;
	//float z3 = 1.f / vo3.posH.w;
	float hz = 1.f / (a + b + c);
	vout = (vo1 * a + vo2 * b + vo3 * c) * hz;
}
void CubeMapLerpFunction(VertexOut& vout, const VertexOut & vo1, const VertexOut & vo2, const VertexOut & vo3, float a, float b, float c)
{
	//float z1 = 1.f / vo1.posH.w;
	//float z2 = 1.f / vo2.posH.w;
	//float z3 = 1.f / vo3.posH.w;
	float hz = 1.f / (a + b + c);
    vout.posW = (vo1.posW * a + vo2.posW * b + vo3.posW * c) * hz;
	//vout = (vo1 * a + vo2 * b + vo3 * c) * hz;

}

void ModelLerpFunction(VertexOut & vout, const VertexOut & vo1, const VertexOut & vo2, const VertexOut & vo3, float a, float b, float c)
{
	//float z1 = 1.f / vo1.posH.w;
	//float z2 = 1.f / vo2.posH.w;
	//float z3 = 1.f / vo3.posH.w;
	float hz = 1.f / (a + b + c);
	vout.posH = (vo1.posH * a + vo2.posH * b + vo3.posH * c) * hz;
}


void ModelLerpFunction(VertexOut & vout, const VertexOut & vo1, const VertexOut & vo2, float t, float hz1, float hz2)
{
	float z = 1.f / (hz1 + hz2);
	vout.uv = vo1.uv * hz1 + (vo2.uv * hz2 - vo1.uv * hz1) * (t * z);
}

void ModelLerpFunction(VertexOut & vout, const VertexOut & vo1, const VertexOut & vo2, float t)
{
	float hz1 = 1.f / vo1.posH.w;
	float hz2 = 1.f / vo2.posH.w;
	float z = 1.f / (hz1 + hz2);
	vout.uv = vo1.uv + (vo2.uv - vo1.uv) * t;
	//vout.uv = vo1.uv * hz1 + (vo2.uv * hz2 - vo1.uv * hz1) * (t * z);
	//vout.posH = vo1.posH * hz1 + (vo2.posH * hz2 - vo1.posH * hz1) * (t * z);
	vout.posH = vo1.posH + (vo2.posH - vo1.posH) * t;
}


void RenderContext::drawFragment(const VertexOut & vo1, const VertexOut & vo2, const VertexOut & vo3)
{
	Vector2f p1{ ((vo1.posH.x + 1.f) * 0.5f * width),  int((1.f - vo1.posH.y) * 0.5f * height) };
	Vector2f p2{ ((vo2.posH.x + 1.f) * 0.5f * width),  int((1.f - vo2.posH.y) * 0.5f * height) };
	Vector2f p3{ ((vo3.posH.x + 1.f) * 0.5f * width),  int((1.f - vo3.posH.y) * 0.5f * height) };

	int x1 = p1.x; int y1 = p1.y;
	int x2 = p2.x; int y2 = p2.y;
	int x3 = p3.x; int y3 = p3.y;

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), width - 1);
	int maxy = std::min(findMax(y1, y2, y3), height - 1);

	
	VertexOut vout;
	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			Vector2f p{ float(x) + 0.5f, float(y) + 0.5f };
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
			
			
			lerpFunc(vout, vo1, vo2, vo3, a, b, c);
			Vector3f color =  ps->execute(vout);
			
			
			float* pc = (float*)&color;
			for (int i = 0; i < 3; ++i)
			{
				if (*(pc + i) > 1.f)
				{
					*(pc + i) = 1.f;
				}
			}
			
			int red = color.x * 255;
			int green = color.y * 255;
			int blue = color.z * 255;
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
	ModelLerpFunction(vo4, vo1, vo3, t);
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

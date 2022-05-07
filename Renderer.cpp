#include "Renderer.h"
#include <qpainter.h>
#include "Texture.h"
#include "Scene.h"
#include "Light.h"
#include <qevent.h>
#include "SkyBox.h"

static const int clientWidth = 1280;
static const int clientHeight = 720;
static const float clipw = 0.00001f;

Renderer::Renderer(QWidget *parent)
    : QWidget(parent), m_lastX(0), m_lastY(0), m_fov(60.f), m_interact(false), m_scene(nullptr),
	m_sides{ { -1, 0, 0, 1 }, { 1, 0, 0, 1}, { 0, 1, 0, 1 }, { 0, -1, 0, 1 },{ 0, 0, -1, 1 },{ 0, 0, 1, 1 } }
{
	
  
	setFixedWidth(clientWidth);
	setFixedHeight(clientHeight);
	m_frameData = new unsigned int[clientHeight * clientWidth];
	m_zbuffer = new float[clientHeight * clientWidth];

	m_sides.resize(6);
	
}

Renderer::~Renderer()
{
	delete[] m_frameData;
	delete[] m_zbuffer;
}
#include <qdebug.h>

float Renderer::s_minX = 2.f;
float Renderer::s_minY = 2.f;
float Renderer::s_maxX = -2.f;
float Renderer::s_maxY = -2.f;

void Renderer::render()
{
	if (!m_scene)
	{
		return;
	}
	generateCubeMap();
	clear();
	m_curFrameData = m_frameData;
	m_scene->camera.update();
	Matrix4 matView = m_scene->camera.viewMat;
	Matrix4 matpers = perspectiveMat(m_scene->nearPlane, m_scene->farPlane);
	Matrix4 mat = matpers * matView;
	for (int index = 0; index < m_scene->meshes.size(); ++index)
	{
		Mesh& mesh = m_scene->meshes[index];
		Matrix4 matModel = modelMat();
		int faceSize = mesh.indices.size();
		for (int i = 0; i < faceSize; ++i)
		{
			Vertex& v1 = mesh.vertices[mesh.indices[i].u];
			Vector4f w1 = matModel * Vector4f(v1.pos);
			Vector3f v{ w1.x, w1.y, w1.z };
			Vector3f dir = m_scene->camera.pos - v;
			if (dir.dot(v1.normal) <= 0)
			{
				continue;
			}
			Vertex& v2 = mesh.vertices[mesh.indices[i].v];
			Vertex& v3 = mesh.vertices[mesh.indices[i].w];
			Vector4f t1(v1.pos);
			Vector4f t2(v2.pos);
			Vector4f t3(v3.pos);
			
			Matrix4 mvp = mat * matModel;
			t1 = mvp * t1;
		    t2 = mvp * t2;
		    t3 = mvp * t3;
			t1 /= t1.w;
			t2 /= t2.w;
			t3 /= t3.w;
			m_reflect = false;
			Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight};
			Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
			Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
		
			VertexOut vo1{ v1, p1, 1.f / t1.w };
			VertexOut vo2{ v2, p2, 1.f / t2.w };
			VertexOut vo3{ v3, p3, 1.f / t3.w };
			drawFragment(vo1, vo2, vo3);
		}
	}
	for (int index = 0; index < m_scene->reflectMeshes.size(); ++index)
	{
		Mesh& mesh = m_scene->reflectMeshes[index];
		int faceSize = mesh.indices.size();
		for (int i = 0; i < faceSize; ++i)
		{
			Vertex& v1 = mesh.vertices[mesh.indices[i].u];
			Vector3f dir = m_scene->camera.pos - v1.pos;
			if (dir.dot(v1.normal) <= 0)
			{
				continue;
			}
			Vertex& v2 = mesh.vertices[mesh.indices[i].v];
			Vertex& v3 = mesh.vertices[mesh.indices[i].w];
			Vector4f t1(v1.pos);
			Vector4f t2(v2.pos);
			Vector4f t3(v3.pos);
			t1 = mat * t1;
			t2 = mat * t2;
			t3 = mat * t3;
			t1 /= t1.w;
			t2 /= t2.w;
			t3 /= t3.w;
			m_reflect = true;
			Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight };
			Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
			Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
			VertexOut vo1{ v1, p1, 1.f / t1.w };
			VertexOut vo2{ v2, p2, 1.f / t2.w };
			VertexOut vo3{ v3, p3, 1.f / t3.w };
			drawFragment(vo1, vo2, vo3);
		}
	}
	if (m_scene->sky)
	{
		m_pixels.clear();
		SkyBox * sky = m_scene->sky.get();
		matView.m[0][3] = 0.f;
		matView.m[1][3] = 0.f;
		matView.m[2][3] = 0.f;

		mat = matpers * matView;


		int faceSize = sky->indices.size();
		Vector3f origin{ 0.f, 0.f, 0.f };
		for (int i = 0 ; i < faceSize; ++i)
		{
			int u = sky->indices[i].u;
			int v = sky->indices[i].v;
			int w = sky->indices[i].w;
			Vector3f& v1 = m_scene->sky->vertices[u];
			Vector3f& v2 = m_scene->sky->vertices[v];
			Vector3f& v3 = m_scene->sky->vertices[w];
			Vector4f t1(v1);
			Vector4f t2(v2);
			Vector4f t3(v3);
			t1 = mat * t1;
			t2 = mat * t2;
			t3 = mat * t3;
			//t1.z = t1.w;
			//t2.z = t2.w;
			//t3.z = t3.w;
			
			if (checkCull(t1, t2, t3))
			{
				m_currentvs.clear();
				m_currentnvs.clear();
				m_inputvs.clear();
				m_currentvs.push_back(v1);
				m_currentvs.push_back(v2);
				m_currentvs.push_back(v3);
				std::vector<Vector4f> vs = sidesClip(t1, t2, t3);
				std::vector<Vector4f> ts(vs);
				for (auto& v : ts)
				{
					v.x /= v.w;
					v.y /= v.w;
					v.z /= v.w;
				}
				int size = vs.size() - 3 + 1;
				for (int j = 0; j < size; ++j)
				{
					int vIndex1 = 0;
					int vIndex2 = j + 1;
					int vIndex3 = j + 2;

					
					Vector3f cv1{ m_currentnvs[vIndex1].x, m_currentnvs[vIndex1].y, m_currentnvs[vIndex1].z };
					Vector3f cv2{ m_currentnvs[vIndex2].x, m_currentnvs[vIndex2].y, m_currentnvs[vIndex2].z };
					Vector3f cv3{ m_currentnvs[vIndex3].x, m_currentnvs[vIndex3].y, m_currentnvs[vIndex3].z };

					

					
					Vector2f cp1{ (ts[vIndex1].x + 1.f) * 0.5f * clientWidth,  (1.f - ts[vIndex1].y) * 0.5f * clientHeight };
					Vector2f cp2{ (ts[vIndex2].x + 1.f) * 0.5f * clientWidth,  (1.f - ts[vIndex2].y) * 0.5f * clientHeight };
					Vector2f cp3{ (ts[vIndex3].x + 1.f) * 0.5f * clientWidth,  (1.f - ts[vIndex3].y) * 0.5f * clientHeight };
					drawSkyFragment(cv1, cp1, ts[vIndex1].w, cv2, cp2, ts[vIndex2].w, cv3, cp3, ts[vIndex3].w, sky->texture.get());
				}
			}
			
			else
			{
				t1 /= (t1.w);
				t2 /= (t2.w);
				t3 /= (t3.w);
				Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight };
				Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
				Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
				drawSkyFragment(v1, p1, t1.w, v2, p2, t2.w, v3, p3, t3.w, sky->texture.get());
			}
			
			
		}
		


	}
	

	update();

}

void Renderer::drawTexture2D(const QString & file)
{
	/*
	Texture tex(file.toStdString().c_str());
	for (int col = 0; col < clientHeight; ++col)
	{
		for (int row = 0; row < clientWidth; ++row)
		{
			float x = (float)row / (float)clientWidth;
			float y = (float)col / (float)clientHeight;
			m_frameData[col * clientWidth + row] = tex.sample(x, y);
		}
	}
	update();
	*/
}

void Renderer::setScene(Scene * scene)
{
	m_scene = scene;
	
}

void Renderer::drawTriangle()
{
	clear();
	int x1 = 1200; int y1 = 200;
	int x2 = 1200; int y2 = 700;
	int x3 = 100; int y3 = 700;

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), clientWidth - 1);
	int maxy = std::min(findMax(y1, y2, y3), clientHeight - 1);

	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			int index = clientWidth * y + x;



			int dx1 = x1 - x;
			int dy1 = y1 - y;
			int dx2 = x2 - x;
			int dy2 = y2 - y;
			int dx3 = x3 - x;
			int dy3 = y3 - y;


			long long c1 = dx1 * dy2 - dx2 * dy1;
			long long c2 = dx2 * dy3 - dx3 * dy2;
			long long c3 = dx3 * dy1 - dx1 * dy3;

			long long v1 = c1 * c2;
			long long v2 = c1 * c3;
			long long v3 = c2 * c3;


			if ((v1 >= 0) && (v2 >= 0) && (v3 >= 0))
			{
				m_frameData[index] = (255 << 16) + (255 << 8) + 0;
			}
			else
			{
				if (x == maxx)
				{
					m_frameData[index] = (0 << 16) + (255 << 8) + 0;
				}
				
			}
		}
	}
	update();
}



void Renderer::paintEvent(QPaintEvent * e)
{
	QPainter p(this);
	QImage image((uchar*)m_frameData, clientWidth, clientHeight, QImage::Format_RGB32);
	p.drawImage(0, 0, image);
}

void Renderer::mousePressEvent(QMouseEvent * e)
{
	static bool first = true;
	if (first)
	{
		//first = false;
		m_lastX = e->pos().x();
		m_lastY = e->pos().y();
	}
	m_interact = true;
}

void Renderer::mouseMoveEvent(QMouseEvent * e)
{
	if (!m_scene)
	{
		return;
	}
	int x = e->pos().x();
	int y = e->pos().y();
	float dx = (0.25f * (x - m_lastX));
	float dy = (0.25f * (y - m_lastY));

    m_scene->camera.yaw -= dx;
	m_scene->camera.pitch += dy;
	if (m_scene->camera.pitch > 89.f)
	{
		m_scene->camera.pitch = 89.f;
	}
	if (m_scene->camera.pitch < -89.f)
	{
		m_scene->camera.pitch = -89.f;
	}
	m_lastX = x;
	m_lastY = y;
	//render();
}

void Renderer::mouseReleaseEvent(QMouseEvent * e)
{
	m_interact = false;
}

void Renderer::wheelEvent(QWheelEvent * event)
{
	if (m_fov >= 1.0f && m_fov <= 90.0f)
	{
		m_fov -= event->delta() * 0.1;
	}

	if (m_fov <= 1.0f)
	{
		m_fov = 1.0f;
	}

	else if (m_fov >= 90.0f)
	{
		m_fov = 90.0f;
	}
}

Matrix4 Renderer::perspectiveMat(float near, float far)
{
	Matrix4 mat;
	float tanVar = tan((m_fov / 2) * (3.14159 / 180.f));
	float ratio = (float)width() / (float)height();
	mat.m[0][0] = 1.f / (ratio * tanVar);
	mat.m[1][1] = 1.f / tanVar;
	//mat[2][2] = near / (far - near);
	//mat[2][3] = near * far / (far - near);
	mat.m[2][2] = (far + near) / (far - near);
	mat.m[2][3] = near * far / (near - far);
	mat.m[3][2] = 1;
	mat.m[3][3] = 0;
	return mat;
}

Matrix4 Renderer::modelMat()
{
	Matrix4 mat;
	//mat.m[2][3] = -6.f;
	mat.m[0][3] = cosf(m_count * 3.14159f / 180.f) * 7;
	mat.m[2][3] = sinf(m_count * 3.14159f / 180.f) * 7;
	return mat;
}

void Renderer::drawFragment(const VertexOut & vertex1, const VertexOut & vertex2, const VertexOut & vertex3)
{
	int x1 = vertex1.point.x; int y1 = vertex1.point.y;
	int x2 = vertex2.point.x; int y2 = vertex2.point.y;
	int x3 = vertex3.point.x; int y3 = vertex3.point.y;

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), clientWidth - 1);
	int maxy = std::min(findMax(y1, y2, y3), clientHeight - 1);

	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			int dx1 = x1 - x;
			int dy1 = y1 - y;
			int dx2 = x2 - x;
			int dy2 = y2 - y;
			int dx3 = x3 - x;
			int dy3 = y3 - y;


			qint64 v1 = dx1 * dy2 - dx2 * dy1;
			qint64 v2 = dx2 * dy3 - dx3 * dy2;
			qint64 v3 = dx3 * dy1 - dx1 * dy3;

			if (v1 * v2 >= 0 && v1 * v3 >= 0 && v2 * v3 >= 0)
			{
				Vector2f p{ x, y };
				float areaA = (p - vertex2.point).corss(vertex3.point - vertex2.point);
				float areaB = (p - vertex3.point).corss(vertex1.point - vertex3.point);
				float areaC = (p - vertex1.point).corss(vertex2.point - vertex1.point);
				float area = areaA + areaB + areaC;
				float a = areaA / area;
				float b = areaB / area;
				float c = areaC / area;
				

				float z = vertex1.z * a + vertex2.z * b + vertex3.z * c;
				int index = clientWidth * y + x;
				if (m_zbuffer[index] > z)
				{
					continue;
				}

				
				Vector3f pos = vertex1.vin.pos * a + vertex2.vin.pos * b + vertex3.vin.pos * c;
				
				Vector3f normal = vertex1.vin.normal * a + vertex2.vin.normal * b + vertex3.vin.normal * c;
				if (m_reflect)
				{
					Vector3f viewDir = m_scene->camera.pos - pos;
					viewDir.normalize();
					Vector3f dir = (viewDir * -1).reflect(normal);
					dir.normalize();
					unsigned int color = m_scene->envCubeMap->sample(dir);
					
					
					
					
					m_zbuffer[index] = z;
					m_curFrameData[index] = color;
					continue;
				
				}
				else
				{
					Vector3f color = { 1.f, 0.f, 0.f };
					int red = color.x * 255;
					int green = color.y * 255;
					int blue = color.z * 255;
					unsigned int colorValue = (red << 16) + (green << 8) + blue;
					m_zbuffer[index] = z;
					m_curFrameData[index] = colorValue;
					continue;
				}
				Vector3f color;
				if (m_shaderInfo.texture)
				{
					float u = vertex1.vin.tex.x * a + vertex2.vin.tex.x * b + vertex3.vin.tex.x * c;
					float v = vertex1.vin.tex.y * a + vertex2.vin.tex.y * b + vertex3.vin.tex.y * c;
					color = m_shaderInfo.texture->sample(u, v);
				}
				else
				{
					color = m_shaderInfo.color;
				}
				Vector3f litColor;
				for (const Light& light : m_scene->lights)
				{
					litColor = litColor + light.caculate(m_scene->camera.pos, pos, normal);
				}
				float* pc = (float*)&litColor;
				for (int i = 0; i < 3; ++i)
				{
					if (*(pc + i) > 1.f)
					{
						*(pc + i) = 1.f;
					}
				}
				color = color * litColor;
				int red = color.x * 255;
				int green = color.y * 255;
				int blue = color.z * 255;
				unsigned int colorValue = (red << 16) + (green << 8) + blue;
				m_zbuffer[index] = z;
				m_curFrameData[index] = colorValue;
			}


		}
	}

}

void Renderer::drawSkyFragment(const Vector3f&v1, const Vector2f& p1, float z1,
	const Vector3f& v2, const Vector2f& p2, float z2,
	const Vector3f& v3, const Vector2f& p3, float z3, CubeMap* texture)
{
	int x1 = p1.x; int y1 = p1.y;
	int x2 = p2.x; int y2 = p2.y;
	int x3 = p3.x; int y3 = p3.y;

	float hz1 = 1.f / z1;
	float hz2 = 1.f / z2;
	float hz3 = 1.f / z3;

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), clientWidth - 1);
	int maxy = std::min(findMax(y1, y2, y3), clientHeight - 1);

	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			int index = clientWidth * y + x;
			if (m_zbuffer[index] > 0.f)
			{
				continue;
			}
			
			int dx1 = x1 - x;
			int dy1 = y1 - y;
			int dx2 = x2 - x;
			int dy2 = y2 - y;
			int dx3 = x3 - x;
			int dy3 = y3 - y;


			qint64 c1 = dx1 * dy2 - dx2 * dy1;
			qint64 c2 = dx2 * dy3 - dx3 * dy2;
			qint64 c3 = dx3 * dy1 - dx1 * dy3;


			if (c1 * c2 >= 0 && c1 * c3 >= 0 && c2 * c3 >= 0)
			{
				Vector2f p{ x, y };
				float areaA = (p - p2).corss(p3 - p2);
				float areaB = (p - p3).corss(p1 - p3);
				float areaC = (p - p1).corss(p2 - p1);
				float area = areaA + areaB + areaC;
				float a = areaA / area;
				float b = areaB / area;
				float c = areaC / area;

				
				float hz = 1.f / ((a * hz1) + (b * hz2) + (c * hz3));

				Vector3f dir = (v1 * a * hz1 + v2 * b * hz2 + v3 * c * hz3) / hz;
				

				dir.normalize();
				Vector3f color = texture->sample(dir);
				
				int red = color.x * 255;
				int green = color.y * 255;
				int blue = color.z * 255;
				unsigned int colorValue = (red << 16) + (green << 8) + blue;
				m_curFrameData[index] = colorValue;

			}
		}
	}


				
}

void Renderer::clear()
{
	memset(m_zbuffer, 0, clientWidth * clientHeight * 4);
	memset(m_frameData, 0, clientWidth * clientHeight * 4);
}

bool Renderer::checkCull(const Vector4f & v1, const Vector4f & v2, const Vector4f & v3)
{
	if (v1.w < m_scene->nearPlane || v2.w < m_scene->nearPlane || v3.w < m_scene->nearPlane)
	{
		return true;
	}
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
	if (v1.w < clipw || v2.w < clipw || v3.w < clipw)
	{
		return true;
	}
	return false;
}

std::vector<Vector4f> Renderer::sidesClip(const Vector4f & v1, const Vector4f & v2, const Vector4f & v3)
{
	std::vector<Vector4f> output;
	std::vector<Vector4f> inputW{ v1, v2, v3 };

	m_inputvs.clear();
	for (int i = 0; i < inputW.size(); ++i)
	{
		auto& current = inputW[i];
		auto& last = inputW[(i + inputW.size() - 1) % inputW.size()];

		m_index1 = i;
		m_index2 = (i+inputW.size() - 1) % inputW.size();
		if (inside(current, 6))
		{
			if (!inside(last, 6))
			{
				output.push_back(clipLineW(current, last));
				
			}
			m_currentnvs.push_back(m_currentvs[m_index1]);
			output.push_back(current);
		}
		else if (inside(last, 6))
		{
			output.push_back(clipLineW(current, last));
		}
	}
	for (int i = 0; i < 6; ++i)
	{
		m_inputvs = m_currentnvs;
		m_currentnvs.clear();
		std::vector<Vector4f> input(output);
		output.clear();
		int size = input.size();
		for (int j = 0; j < size; ++j)
		{
			auto& current = input[j];
			auto& last = input[(j + size - 1) % size];

			m_index2 = j;
			m_index1 = (j + size - 1) % size;
			if (inside(current, i))
			{
				if(!inside(last, i))
				{
					output.push_back(clipLine(last, current, m_sides[i], i));
				}
				output.push_back(current);
				m_currentnvs.push_back(m_inputvs[m_index2]);

			}
			else if (inside(last, i))
			{
				output.push_back(clipLine(last, current, m_sides[i], i));
				//output.push_back(current);
			}
		}
	}
	return output;
}

Vector4f Renderer::clipLine(const Vector4f & v1, const Vector4f & v2, const Vector4f & side, int index)
{
	float k1, k2;
	switch (index)
	{
	case 0:
	case 1:
	{
		k1 = v1.x;
		k2 = v2.x;
		break;
	}
	case 2:
	case 3:
	{
		k1 = v1.y;
		k2 = v2.y;
		break;
	}
	case 4:
	case 5:
	{
		k1 = v1.z;
		k2 = v2.z;
		break;
	}
	case 6:
	{
		k1 = v1.w;
		k2 = v2.w;
		break;
	}
	}
	if (index == 0 || index == 3 || index == 4)
	{
		k1 *= -1;
		k2 *= -1;
	}
	float t = (v1.w - k1) / ((v1.w - k1) - (v2.w - k2));
	/*
	float c1 = v1.dot(side);
	float c2 = v2.dot(side);
	float weight = c2 / (c2 - c1);
	*/

	m_currentnvs.push_back(m_inputvs[m_index1] + (m_inputvs[m_index2] - m_inputvs[m_index1]) * t);
	return v1 + (v2 - v1) * t;
}

Vector4f Renderer::clipLineW(const Vector4f & v1, const Vector4f & v2)
{
	float t = (v1.w - clipw) / (v1.w - v2.w);

	m_currentnvs.push_back(m_currentvs[m_index1] + (m_currentvs[m_index2] - m_currentvs[m_index1]) * t);
	
	return v1 + (v2 - v1) * t;
}

bool Renderer::inside(const Vector4f & v, const Vector4f & line)
{
	float dot = v.dot(line);
	return dot >= 0;
}

bool Renderer::inside(const Vector4f & v, int index)
{
	bool in = true;
	switch (index)
	{
		case 0:
		{
			in = v.x > -v.w;
			break;
		}
		case 1:
		{
			in = v.x < v.w;
			break;
		}
		case 2:
		{
			in = v.y < v.w;
			break;
		}
		case 3:
		{
			in = v.y > -v.w;

			break;
		}
		case 4:
		{
			in = v.z > -v.w;
			break;
		}
		case 5:
		{
			in = v.z < v.w;
			break;
		}
		case 6:
		{
			in = v.w > clipw;
		
			break;
		}
	}
	return in;
}

void Renderer::generateCubeMap()
{
	Camera camera;
	camera.useSphereMode = false;
	camera.pos = { 0.f, 0.f, 0.f };
	Vector3f targets[6] =
	{
		{-1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{0.f, 1.f, 0.01f},
		{0.f, -1.f, 0.01f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, 1.f},
	};

	Vector3f ups[6] =
	{
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
	};

	Matrix4 matPers = perspectiveMat(m_scene->nearPlane, m_scene->farPlane);
	
	
	for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
	{
		unsigned int* buff = new unsigned int[clientWidth * clientHeight];
		m_curFrameData = buff;
	
		camera.target = targets[faceIndex];
		camera.up = ups[faceIndex];
		camera.update();
		Matrix4 matView = camera.viewMat;
		Matrix4 mat = matPers * matView;
		m_reflect = false;
		clear();
		for (int index = 0; index < m_scene->meshes.size(); ++index)
		{
			Mesh& mesh = m_scene->meshes[index];
			Matrix4 matModel = modelMat();
			int faceSize = mesh.indices.size();
			for (int i = 0; i < faceSize; ++i)
			{
				Vertex& v1 = mesh.vertices[mesh.indices[i].u];
				Vector4f w1 = matModel * Vector4f(v1.pos);
				Vector3f v{ w1.x, w1.y, w1.z };
				Vector3f dir = m_scene->camera.pos - v;
				if (dir.dot(v1.normal) <= 0)
				{
					continue;
				}
				Vertex& v2 = mesh.vertices[mesh.indices[i].v];
				Vertex& v3 = mesh.vertices[mesh.indices[i].w];
				Vector4f t1(v1.pos);
				Vector4f t2(v2.pos);
				Vector4f t3(v3.pos);
				
				Matrix4 mvp = mat * matModel;
				t1 = mvp * t1;
				t2 = mvp * t2;
				t3 = mvp * t3;
			
					t1 /= t1.w;
					t2 /= t2.w;
					t3 /= t3.w;
					Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight };
					Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
					Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
					
					VertexOut vo1{ v1, p1, 1.f / t1.w };
					VertexOut vo2{ v2, p2, 1.f / t2.w };
					VertexOut vo3{ v3, p3, 1.f / t3.w };
					drawFragment(vo1, vo2, vo3);
				




				
			}
		}
		if (m_scene->sky)
		{
			m_pixels.clear();
			SkyBox * sky = m_scene->sky.get();
			camera.viewMat.m[0][3] = 0.f;
			camera.viewMat.m[1][3] = 0.f;
			camera.viewMat.m[2][3] = 0.f;

			mat = matPers * matView;


			int faceSize = sky->indices.size();
			Vector3f origin{ 0.f, 0.f, 0.f };
			for (int i = 0; i < faceSize; ++i)
			{
				int u = sky->indices[i].u;
				int v = sky->indices[i].v;
				int w = sky->indices[i].w;
				Vector3f& v1 = m_scene->sky->vertices[u];
				Vector3f& v2 = m_scene->sky->vertices[v];
				Vector3f& v3 = m_scene->sky->vertices[w];
				Vector4f t1(v1);
				Vector4f t2(v2);
				Vector4f t3(v3);
				t1 = mat * t1;
				t2 = mat * t2;
				t3 = mat * t3;
	

				if (checkCull(t1, t2, t3))
				{
					m_currentvs.clear();
					m_currentnvs.clear();
					m_inputvs.clear();
					m_currentvs.push_back(v1);
					m_currentvs.push_back(v2);
					m_currentvs.push_back(v3);
					std::vector<Vector4f> vs = sidesClip(t1, t2, t3);
					std::vector<Vector4f> ts(vs);
					for (auto& v : ts)
					{
						v.x /= v.w;
						v.y /= v.w;
						v.z /= v.w;
					}
					int size = vs.size() - 3 + 1;
					for (int j = 0; j < size; ++j)
					{
						int vIndex1 = 0;
						int vIndex2 = j + 1;
						int vIndex3 = j + 2;


						Vector3f cv1{ m_currentnvs[vIndex1].x, m_currentnvs[vIndex1].y, m_currentnvs[vIndex1].z };
						Vector3f cv2{ m_currentnvs[vIndex2].x, m_currentnvs[vIndex2].y, m_currentnvs[vIndex2].z };
						Vector3f cv3{ m_currentnvs[vIndex3].x, m_currentnvs[vIndex3].y, m_currentnvs[vIndex3].z };




						Vector2f cp1{ (ts[vIndex1].x + 1.f) * 0.5f * clientWidth,  (1.f - ts[vIndex1].y) * 0.5f * clientHeight };
						Vector2f cp2{ (ts[vIndex2].x + 1.f) * 0.5f * clientWidth,  (1.f - ts[vIndex2].y) * 0.5f * clientHeight };
						Vector2f cp3{ (ts[vIndex3].x + 1.f) * 0.5f * clientWidth,  (1.f - ts[vIndex3].y) * 0.5f * clientHeight };
						drawSkyFragment(cv1, cp1, ts[vIndex1].w, cv2, cp2, ts[vIndex2].w, cv3, cp3, ts[vIndex3].w, sky->texture.get());
					}
				}

				else
				{
					t1 /= (t1.w);
					t2 /= (t2.w);
					t3 /= (t3.w);
					Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight };
					Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
					Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
					drawSkyFragment(v1, p1, t1.w, v2, p2, t2.w, v3, p3, t3.w, sky->texture.get());
				}


			}



		}

	

		m_scene->envCubeMap->setRowData(static_cast<DynamicCubeMap::Direction>(faceIndex), buff, clientWidth, clientHeight);
		delete[] buff;
	}
}

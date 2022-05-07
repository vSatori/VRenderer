#include "Scene.h"
#include "RenderContext.h"
void DynamicEnviromentMappingScene::render(unsigned int * frame)
{
	
}

void Scene::drawMesh(const Mesh & mesh)
{
	int faceSize = mesh.indices.size();
	for (int faceIndex = 0; faceIndex < faceSize; ++faceIndex)
	{
		const Vector3i& face = mesh.indices[faceIndex];
		const Vertex& v1 = mesh.vertices[face.u];
		Vector4f w1 = RenderContext::vs.world * v1.pos;
		Vector4f n1 = RenderContext::vs.world * v1.normal;
		Vector3f v{ w1.x, w1.y, w1.z };
		Vector3f n{ n1.x, n1.y, n1.z };
		if (!RenderContext::Cull(v, n))
		{
			continue;
		}
		const Vertex& v2 = mesh.vertices[face.v];
		const Vertex& v3 = mesh.vertices[face.w];
		VertexOut vo1 = RenderContext::vs(v1);
		VertexOut vo2 = RenderContext::vs(v2);
		VertexOut vo3 = RenderContext::vs(v3);
		if (checkClipping(vo1.posH, vo2.posH, vo3.posH))
		{

		}
		else
		{
			vo1.posH /= vo1.posH.w;
			vo2.posH /= vo2.posH.w;
			vo3.posH /= vo3.posH.w;
		}
		RenderContext::drawFragment(vo1, vo2, vo3);
	}
}

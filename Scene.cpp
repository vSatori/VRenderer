#include "Scene.h"
#include "RenderContext.h"
void DynamicEnviromentMappingScene::render(unsigned int * frame)
{
	camera.update();
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	m_sphereVS->view = camera.viewMat;
	//..
	RenderContext::vs = m_sphereVS;
	RenderContext::ps = m_spherePS;
	for (const auto& movingSphere : m_movingSpheres)
	{
		//m_sphereVS->world = 
		drawMesh(movingSphere);
	}
	RenderContext::cullMode = CullMode::CULLNONE;
	//RenderContext::depthMode = 
	drawMesh(*m_sky.get());


}

void Scene::drawMesh(const Mesh & mesh)
{
	int faceSize = mesh.indices.size();
	for (int faceIndex = 0; faceIndex < faceSize; ++faceIndex)
	{
		const Vector3i& face = mesh.indices[faceIndex];
		const Vertex& v1 = mesh.vertices[face.u];
		Vector4f w1 = RenderContext::vs->world * v1.pos;
		Vector4f n1 = RenderContext::vs->world * v1.normal;
		Vector3f v{ w1.x, w1.y, w1.z };
		Vector3f n{ n1.x, n1.y, n1.z };
		if (!RenderContext::Cull(v, n))
		{
			continue;
		}
		const Vertex& v2 = mesh.vertices[face.v];
		const Vertex& v3 = mesh.vertices[face.w];
		VertexOut vo1 = RenderContext::vs->execute(v1);
		VertexOut vo2 = RenderContext::vs->execute(v2);
		VertexOut vo3 = RenderContext::vs->execute(v3);
		if (RenderContext::checkClipping(vo1.posH, vo2.posH, vo3.posH))
		{
			std::vector<VertexOut> vos = RenderContext::polygonClipping(vo1, vo2, vo3);
			for (auto& v : vos)
			{
				v.posH /= v.posH.w;
				v.posH /= v.posH.w;
				v.posH /= v.posH.w;
			}
			int size = vos.size() - 3 + 1;
			for (int j = 0; j < size; ++j)
			{
				RenderContext::drawFragment(vos[0], vos[j + 1], vos[j + 2]);
			}
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

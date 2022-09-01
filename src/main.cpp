
#include <Windows.h>
#include "Scene.h"
#include "RenderView.h"
#include "RenderContext.h"


int main(int argc, char** argv)
{
	auto ins = GetModuleHandle(nullptr);
	RenderContext::init();
	renderer->init(ins);
	DynamicEnviromentMappingScene scene;
	renderer->setScene(&scene);
	int res = renderer->run();
	RenderContext::finalize();
	return res;
}
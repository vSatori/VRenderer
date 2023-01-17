
#include <Windows.h>
#include "Scene.h"
#include "RenderView.h"
#include "RenderContext.h"

std::string g_resourcePath;

std::string getResouecePath(HINSTANCE ins)
{
	char buff[256];
	GetModuleFileName(ins, buff, sizeof(buff));
	std::string path(buff);
	int index = path.find_last_of("\\");
	path.replace(index + 1, path.size() - index, "");
	return path + "../../resources/";
}


int main(int argc, char** argv)
{
	auto ins = GetModuleHandle(nullptr);
	g_resourcePath = getResouecePath(ins);
	RenderContext::init();
	renderer->init(ins);
	ShadowMappingScene scene;
	renderer->setScene(&scene);
	int res = renderer->run();
	RenderContext::finalize();
	return res;
}
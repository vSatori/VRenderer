
#include <QtWidgets/QApplication>
#include <QtCore/qtimer.h>
#include <Windows.h>
#include "Scene.h"
#include "RenderView.h"
#include <QtCore/qdebug.h>
#include "RenderContext.h"

bool exitApp = false;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RenderView view;
	//PmxModelScene scene("D:/Project/VRenderer/models/hutao/��װ����/����.pmx");
	//scene.onlyDrawPmxModel = false;
	DynamicEnviromentMappingScene scene;
	view.setScene(&scene);
	RenderContext::vs;
	QTimer timer;
	timer.setInterval(500);
	timer.setSingleShot(true);
	unsigned int fps = 0;
	unsigned long long diff = 0;

	QObject::connect(&timer, &QTimer::timeout, [&view, &diff, &fps]()
		{
			while (!exitApp)
			{
				if (diff >= 1000)
				{
					diff = 0;
					view.fps = fps;
					fps = 0;
				}
				auto s = GetTickCount64();
				view.renderScene();
				QApplication::processEvents();
				diff += GetTickCount64() - s;
				++fps;
				//Sleep(100);
			}
		});
	view.show();
	view.renderScene();
	timer.start();
    return a.exec();
}

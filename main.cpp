
#include <QtWidgets/QApplication>
#include <QtCore/qtimer.h>
#include <Windows.h>
#include "Scene.h"
#include "RenderView.h"
#include "RayTracingScene.h"
#include <QTcore/qdebug.h>
#include <iostream>
#include <xmmintrin.h>
#include <iostream>

bool exitApp = false;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RenderView view;
	OceanWaveScene scene;
	view.setScene(&scene);
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
			}
		});
	view.show();
	view.renderScene();
	//timer.start();
    return a.exec();
}

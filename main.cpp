
#include <QtWidgets/QApplication>
#include <qtimer.h>
#include <Windows.h>
#include <qtimer.h>
#include "Scene.h"
#include "RenderView.h"
#include <qdebug.h>
#include <iostream>
#include <xmmintrin.h>
std::vector<Vector3f> temp;
int main(int argc, char *argv[])
{
	/*
	temp.resize(1000000);
	VertexOut* buff = new VertexOut[1000000];
	auto ss = GetTickCount64();
	for (int i = 0; i < 1000000; ++i)
	{
		VertexOut vout = buff[i];
		temp[i] = vout.posM;rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
	}
	qDebug() << GetTickCount64() - ss;
	*/
	QApplication a(argc, argv);
	RenderView view;
	DynamicEnviromentMappingScene scene;
	view.setScene(&scene);
	QTimer timer;
	timer.setInterval(500);
	timer.setSingleShot(true);
	unsigned int fps = 0;
	unsigned long long diff = 0;
	QObject::connect(&timer, &QTimer::timeout, [&view, &diff, &fps]()
	{
			while (true)
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

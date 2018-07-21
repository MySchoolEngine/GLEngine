#include "stdafx.h"
#include "Editor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	C_Editor w;
	w.resize(100, 100);
	w.show();
	return a.exec();
}

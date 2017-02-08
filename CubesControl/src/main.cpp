#include "cubescontrol.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CubesControl w;
	w.show();

	return a.exec();
}

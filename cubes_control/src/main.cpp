#include "cubes_control.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	cubes_control w;
	w.show();

	return a.exec();
}

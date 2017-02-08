#ifndef CUBES_CONTROL_H
#define CUBES_CONTROL_H

#include <QMainWindow>
#include <QString>
#include <QTextStream>
#include <QSerialPort>

namespace Ui {
class cubes_control;
}

class cubes_control : public QMainWindow
{
	Q_OBJECT

public:
	explicit cubes_control(QWidget *parent = 0);
	~cubes_control();

private:
	Ui::cubes_control	*ui;

	QSerialPort			m_serialPort;
};

#endif // CUBES_CONTROL_H

#ifndef CUBESCONTROL_H
#define CUBESCONTROL_H

#include <QMainWindow>
#include <QString>
#include <QTextStream>
#include <QSerialPort>

namespace Ui {
class CubesControl;
}

class CubesControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit CubesControl(QWidget *parent = 0);
    ~CubesControl();

private:
    Ui::CubesControl	*ui;

    QSerialPort			m_serialPort;
};

#endif // CUBESCONTROL_H

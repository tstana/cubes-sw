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

private slots:
    void on_btnOpen_clicked();

    void on_cbSerialPorts_currentIndexChanged(const QString &arg1);

    void on_textToSend_textChanged(const QString &arg1);

private:
    Ui::CubesControl    *ui;

    QSerialPort         m_serialPort;
    QString             m_serialPortName;
};

#endif // CUBESCONTROL_H

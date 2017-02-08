#ifndef CUBESHARDWARECOMM_H
#define CUBESHARDWARECOMM_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QLabel>

class CubesHardwareComm : public QObject
{
    Q_OBJECT

public:
    explicit CubesHardwareComm(QSerialPort *port, QLabel *lblMessages, QObject *parent = 0);
    ~CubesHardwareComm();

private slots:
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort		*m_port;
    QLabel			*m_lblMessages;
    char			m_writeData;
    QTimer			m_timer;
};

#endif // CUBESHARDWARECOMM_H

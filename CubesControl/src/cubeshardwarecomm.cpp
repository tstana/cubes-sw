#include "CubesHardwareComm.h"

#include <QtSerialPort/QSerialPort>
#include <QLabel>

CubesHardwareComm::CubesHardwareComm(QSerialPort *port, QLabel *lblMessages, QObject *parent)
    : QObject(parent)
    , m_port(port)
    , m_lblMessages(lblMessages)
{
    m_timer.setSingleShot(true);
    QObject::connect( m_port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                      this, &CubesHardwareComm::handleError );
    QObject::connect( &m_timer, &QTimer::timeout,
                      this, &CubesHardwareComm::handleTimeout );
}

CubesHardwareComm::~CubesHardwareComm()
{
}

void CubesHardwareComm::handleError(QSerialPort::SerialPortError error)
{
    m_lblMessages->setText("Serial port error occured: " + QString::number(error) + "\n");
}

void CubesHardwareComm::handleTimeout()
{
    m_lblMessages->setText("Operation timed out for port " + m_port->portName() + "; error" + m_port->errorString() + "\n");
}

#include "cubescontrol.h"
#include "cubeshardwarecomm.h"
#include "ui_cubescontrol.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <string>

CubesControl::CubesControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CubesControl)
{
    QStringList     ports;
    int             baudRates[] = {9600, 19200, 38400, 115200};

    ui->setupUi(this);
    ui->cbSerialPorts->addItem("<Click to select available Serial Port>");

//    CubesHardwareComm	cubesComm(&m_serialPort, ui->lblMsgs);
    connect(&m_serialPort, &QSerialPort::readyRead, this, &CubesControl::on_SerialPort_ReadyRead);

    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPorts) {
        ports.append(info.portName() + ": " + info.description());
    }

    ports.sort();

    ui->cbSerialPorts->addItems(ports);

    for (uint32_t i = 0; i < sizeof(baudRates)/sizeof(baudRates[0]) ; i++) {
        ui->cbBaudRates->addItem(QString::number(baudRates[i]));
    }
    int idx = ui->cbBaudRates->findText("115200");
    ui->cbBaudRates->setCurrentIndex(idx);
}

CubesControl::~CubesControl()
{
    delete ui;
}

void CubesControl::on_btnOpen_clicked()
{
    if (ui->cbSerialPorts->currentIndex() == 0) {
        ui->lblMsgs->setText("Port not opened; please select a valid port from the drop-down above.");
        return;
    }
    m_serialPort.setPortName(m_serialPortName);
    m_serialPort.setBaudRate(ui->cbBaudRates->currentText().toInt());

    m_serialPort.open(QIODevice::ReadWrite);

    ui->lblMsgs->setText("Opened " + m_serialPort.portName() + " @ " + QString::number(m_serialPort.baudRate()) + " bps");
}

void CubesControl::on_cbSerialPorts_currentIndexChanged(const QString &arg1)
{
    const auto serialPorts = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : serialPorts) {
        if (arg1.contains(info.portName())) {
            m_serialPortName = info.portName();
            break;
        }
    }
}

void CubesControl::on_textToSend_textChanged(const QString &arg1)
{
    QByteArray writeData = arg1.toUtf8();
    m_serialPort.write(writeData);
}

void CubesControl::on_SerialPort_ReadyRead()
{
    ui->lblMsgs->setText(QString::fromUtf8(m_serialPort.readAll()));
}

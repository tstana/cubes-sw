#include "cubes_control.h"
#include "ui_cubes_control.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <string>

cubes_control::cubes_control(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cubes_control)
{
    QStringList ports;
    int baudRates[] = {9600, 19200, 38400, 115200};

    ui->setupUi(this);
    ui->cbSerialPorts->addItem("<Click to select available Serial Port>");

    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPorts) {
        ports.append(info.portName() + ": " + info.description());
    }

    ports.sort();

    ui->cbSerialPorts->addItems(ports);

    QString s;
    for (int i = 0; i < sizeof(baudRates)/sizeof(baudRates[0]) ; i++) {
        ui->cbBaudRates->addItem(QString::number(baudRates[i]));
    }
    int idx = ui->cbBaudRates->findText("115200");
    ui->cbBaudRates->setCurrentIndex(idx);
}

cubes_control::~cubes_control()
{
    delete ui;
}


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

    ui->setupUi(this);
    ui->cbSerialPorts->addItem("<Click to select available Serial Port>");

    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPorts) {
        ports.append(info.portName() + ": " + info.description());
    }

    ports.sort();

    ui->cbSerialPorts->addItems(ports);

//    for (int baud = QSerialPort::Baud1200; baud != QSerialPort::UnknownBaud; baud++) {
//        const QString s = QString::number(baud) + "; ";
//        ui->textOutpMessages->setPlainText(s);
////        ui->cbBaudRates->addItem(QString::number(baud));
//    }
}

cubes_control::~cubes_control()
{
    delete ui;
}

void cubes_control::on_plainTextEdit_textChanged()
{

}

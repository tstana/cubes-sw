/*
 *==============================================================================
 * KTH Royal Institute of Technology Stockholm
 * CUBES Control Program
 *==============================================================================
 *
 * author: Theodor Stana (theodor.stana@gmail.com)
 *
 * date of creation: 2017-02-08
 *
 * version: 1.0
 *
 * description:
 *
 * dependencies:
 *
 * references:
 *
 *==============================================================================
 * GNU LESSER GENERAL PUBLIC LICENSE
 *==============================================================================
 * This source file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version. This source is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details. You should have
 * received a copy of the GNU Lesser General Public License along with this
 * source; if not, download it from http://www.gnu.org/licenses/lgpl-2.1.html
 *==============================================================================
 * last changes:
 *    2017-02-08   theodor.stana@gmail.com     File created
 *==============================================================================
 * TODO: -
 *==============================================================================
 */

#include <cubescontrol.h>
#include "cubesprotouartpmod.h"
#include "ui_cubescontrol.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <string>

#include <cubescommands.h>

CubesControl::CubesControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CubesControl)
{
    QStringList     ports;
    int             baudRates[] = {9600, 19200, 38400, 115200};
    uint32_t        i;

    ui->setupUi(this);
    ui->cbSerialPorts->addItem("<Click to select available Serial Port>");

    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPorts) {
        ports.append(info.portName() + ": " + info.description());
    }

    ports.sort();

    ui->cbSerialPorts->addItems(ports);
//itzi was here :*
    for (i = 0; i < sizeof(baudRates)/sizeof(baudRates[0]) ; i++) {
        ui->cbBaudRates->addItem(QString::number(baudRates[i]));
    }

    int idx = ui->cbBaudRates->findText("115200");
    ui->cbBaudRates->setCurrentIndex(idx);

    ui->cbCommands->addItem("<Click to select available commands...>");

    for (i = 0; i < NUM_COMMANDS; i++) {
        ui->cbCommands->addItem("0x" + QString::number(COMMANDS[i].code, 16) + " : " + \
                                    COMMANDS[i].description);
    }

    ui->btnClose->setEnabled(false);
}

CubesControl::~CubesControl()
{
    delete ui;
}

void CubesControl::on_btnOpen_clicked()
{
    QSerialPort *p_serialPort = new QSerialPort();

    if (ui->cbSerialPorts->currentIndex() == 0) {
        ui->lblMsgs->setText("Port not opened; please select a valid port from the drop-down above.");
        return;
    }
    p_serialPort->setPortName(ui->cbSerialPorts->currentText().split(":")[0]);
    p_serialPort->setBaudRate(ui->cbBaudRates->currentText().toInt());

    cubes = new CubesProtoUartPmod(p_serialPort);

    QObject::connect(cubes, &CubesProtoUartPmod::dataReceived,
                     this, &CubesControl::on_cubes_dataReceived);
    QObject::connect(cubes, &CubesProtoUartPmod::devErrorOccured,
                     this, &CubesControl::on_cubes_devErrorOccured);

    cubes->devOpen(QIODevice::ReadWrite);

    if (cubes->devError() == 0) {
        ui->lblMsgs->setText("Opened " + p_serialPort->portName() + " @ " +
                             QString::number(p_serialPort->baudRate()) + " bps");
        ui->cbSerialPorts->setEnabled(false);
        ui->cbBaudRates->setEnabled(false);
        ui->btnOpen->setEnabled(false);
        ui->btnClose->setEnabled(true);
    }
}

void CubesControl::on_btnClose_clicked()
{
    ui->cbSerialPorts->setEnabled(true);
    ui->cbBaudRates->setEnabled(true);
    ui->btnOpen->setEnabled(true);

    cubes->devClose();
    ui->lblMsgs->setText("Closed " + cubes->devName());

    ui->btnClose->setEnabled(false);
}

void CubesControl::on_btnSend_clicked()
{
    int             cmdData = ui->spinData->value();
    unsigned char   cmdCode = selectedCommandCode();
    QByteArray      data;

    if (cmdCode == 0x91) {
        data.resize(4);

        data[0] = 0x00;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = cmdData & 0xff;
    } else if (cmdCode == 0x93) {
        data.resize(8);
        data[0] = 0x00;
        data[1] = 0x00;
        data[2] = 0x01;
        data[3] = (ui->spinAddress->value() << 1) | ui->chkbWrite->isChecked();
        data[4] = (cmdData & 0x03000000) >> 24;
        data[5] = (cmdData & 0x00ff0000) >> 16;
        data[6] = (cmdData & 0x0000ff00) >>  8;
        data[7] = cmdData & 0x000000ff;
    }

    cubes->sendCommand(cmdCode, data);
}

void CubesControl::on_cubes_dataReceived()
{
    QByteArray readData = cubes->readAll();
    QString s;

    if (selectedCommandCode() == 0x11) {
        for (int i = 0; i < 12; i++) {
            int j = i*4;
            s += "0x" + QString("%1").arg(QString::number(j, 16), 2, '0') + ": " +
                 "0x" + readData.mid(j, 4).toHex() + "\n";
        }
    } else if (selectedCommandCode() == 0x90) {
        s = QString::fromUtf8(readData);
    } else if (selectedCommandCode() == 0x92) {
        s = "0x" + readData.toHex();
    }

    ui->lblMsgs->setText(s);
}

void CubesControl::on_cubes_devErrorOccured(int error)
{
    ui->lblMsgs->setText("Hardware port error: " + QString::number(error));
}

inline int CubesControl::selectedCommandCode()
{
    return COMMANDS[ui->cbCommands->currentIndex()-1].code;
}

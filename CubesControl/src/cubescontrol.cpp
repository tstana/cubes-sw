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

    ui->btnClose->setEnabled(false);
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
    ui->cbSerialPorts->setEnabled(false);
    ui->cbBaudRates->setEnabled(false);
    ui->btnOpen->setEnabled(false);
    ui->btnClose->setEnabled(true);
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

void CubesControl::on_btnClose_clicked()
{
    ui->cbSerialPorts->setEnabled(true);
    ui->cbBaudRates->setEnabled(true);
    ui->btnOpen->setEnabled(true);

    m_serialPort.close();
    ui->lblMsgs->setText("Closed " + m_serialPort.portName());

    ui->btnClose->setEnabled(false);
}

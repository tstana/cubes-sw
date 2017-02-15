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
#include "cubesserialport.h"
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

    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPorts) {
        ports.append(info.portName() + ": " + info.description());
    }

    ports.sort();

    ui->cbSerialPorts->addItems(ports);
//itzi was here :*
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
    QSerialPort *p_serialPort = new QSerialPort();

    if (ui->cbSerialPorts->currentIndex() == 0) {
        ui->lblMsgs->setText("Port not opened; please select a valid port from the drop-down above.");
        return;
    }
    p_serialPort->setPortName(ui->cbSerialPorts->currentText().split(":")[0]);
    p_serialPort->setBaudRate(ui->cbBaudRates->currentText().toInt());

    m_hwPort = new CubesSerialPort(p_serialPort);

    QObject::connect(m_hwPort, &CubesSerialPort::readyRead, this, &CubesControl::on_hwPort_readyRead);
    QObject::connect(m_hwPort, &CubesSerialPort::errorOccured, this, &CubesControl::on_hwPort_errorOccured);

    m_hwPort->open(QIODevice::ReadWrite);

    if (m_hwPort->error() == 0) {
        ui->lblMsgs->setText("Opened " + p_serialPort->portName() + " @ " + QString::number(p_serialPort->baudRate()) + " bps");
        ui->cbSerialPorts->setEnabled(false);
        ui->cbBaudRates->setEnabled(false);
        ui->btnOpen->setEnabled(false);
        ui->btnClose->setEnabled(true);
    }
}

void CubesControl::on_textToSend_textChanged(const QString &arg1)
{
    QByteArray writeData = arg1.toUtf8();
    m_hwPort->write(writeData);
}

void CubesControl::on_btnClose_clicked()
{
    ui->cbSerialPorts->setEnabled(true);
    ui->cbBaudRates->setEnabled(true);
    ui->btnOpen->setEnabled(true);

    m_hwPort->close();
    ui->lblMsgs->setText("Closed " + m_hwPort->portName());

    ui->btnClose->setEnabled(false);
}

void CubesControl::on_hwPort_readyRead()
{
    ui->lblMsgs->setText(QString::fromUtf8(m_hwPort->readAll()));
}

void CubesControl::on_hwPort_errorOccured(int error)
{
    ui->lblMsgs->setText("Hardware port error: " + QString::number(error));
}


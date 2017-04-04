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

#include <QLabel>
#include <QString>
#include <QMessageBox>

#include <cubescommands.h>


CubesControl::CubesControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CubesControl),
    commSettings(new CommSettingsDialog)
{
    ui->setupUi(this);

    lblConnStatus = new QLabel;
    lblConnStatus->setFixedWidth(100);
    lblConnStatus->setFixedHeight(15);
    lblConnStatus->setIndent(5);
    connStatus = 0;
    showConnStatus(connStatus);
    statusBar()->addPermanentWidget(lblConnStatus);

    connect(ui->actionConfigConnection, &QAction::triggered,
            commSettings, &CubesControl::show);

    serialPort = new QSerialPort();
    cubes = new CubesProtoUartPmod(serialPort, this);
    connect(cubes, &CubesProtoUartPmod::devErrorOccured,
            this, &CubesControl::on_cubes_devErrorOccured);
}

CubesControl::~CubesControl()
{
    delete ui;
    delete commSettings;
    delete cubes;
}

void CubesControl::on_actionClose_triggered()
{
    this->close();
}

const QString STYLE_CONN_UP =
        "QLabel { font-weight : bold; color: white; background-color : green; border : 1px solid black }";
const QString STYLE_CONN_DOWN =
        "QLabel { font-weight : bold; color: black; background-color : red; border : 1px solid black }";

void CubesControl::showConnStatus(int connUp)
{
    if (connUp) {
        lblConnStatus->setStyleSheet(STYLE_CONN_UP);
        lblConnStatus->setText(commSettings->getSettings()->port + " / " +
                               QString::number(commSettings->getSettings()->baud));
    } else {
        lblConnStatus->setStyleSheet(STYLE_CONN_DOWN);
        lblConnStatus->setText("Not connected");
    }
}

void CubesControl::on_actionConnect_triggered()
{
    QString msg;
    CommSettingsDialog::CommSettings *currentSettings = commSettings->getSettings();

    if (currentSettings == 0) {
        msg = "No connection type selected. Please select one via \"Connection > Configure\".";
        statusBar()->showMessage(msg, 5000);
    }

    switch (currentSettings->type) {
    case CommSettingsDialog::None:
    {
        msg = "No connection type selected. Please select one via \"Connection > Configure\".";
        statusBar()->showMessage(msg, 5000);
        break;
    }
    case CommSettingsDialog::SerialPort:
    {
        cubes->dev()->setPortName(currentSettings->port);
        cubes->dev()->setBaudRate(currentSettings->baud);
        if (cubes->devOpen(QSerialPort::ReadWrite))
            connStatus = 1;
        showConnStatus(connStatus);
        break;
    }
    }
}

void CubesControl::on_actionDisconnect_triggered()
{
    CommSettingsDialog::CommSettings *currentSettings = commSettings->getSettings();
    QString msg;

    if (connStatus == 0) {
        msg = "No connection open!";
        statusBar()->showMessage(msg, 5000);
        return;
    }

    switch (currentSettings->type) {
    case CommSettingsDialog::None:
    {
        msg = "No connection type selected. Please select one via 'Connection > Configure'.";
        statusBar()->showMessage(msg, 5000);
        break;
    }
    case CommSettingsDialog::SerialPort:
    {
        cubes->devClose();
        connStatus = 0;
        showConnStatus(connStatus);
        break;
    }
    }

}

void CubesControl::on_cubes_devErrorOccured(int error)
{
    QString msg = "";
    CommSettingsDialog::CommSettings *currentSettings = commSettings->getSettings();

    switch (currentSettings->type) {
    case CommSettingsDialog::SerialPort:
        msg += "QSerialPort error "+ QString::number(error);
        switch (error) {
        case 1:
            msg += " : Device not found.";
            break;
        case 2:
            msg += " : Port already open or not enough permissions to access.";
            break;
        case 3:
            msg += " : Device already open.";
            break;
        case 9:
            msg += " : I/O error.";
            cubes->devClose();
            connStatus = 0;
            showConnStatus(connStatus);
            break;
        default:
            msg += ".";
            break;
        }
        break;
    }

    if (error)
        statusBar()->showMessage(msg, 5000);
}

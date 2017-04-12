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

#ifndef CUBESCONTROL_H
#define CUBESCONTROL_H

#include <cubesprotouartpmod.h>
#include <commsettingsdialog.h>

#include <QMainWindow>
#include <QString>
#include <QLabel>

#include <QSerialPort>

namespace Ui {
class CubesControl;
}

class CommSettingsDialog;

class CubesControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit CubesControl(QWidget *parent = 0);
    ~CubesControl();

private slots:
    void on_actionExit_triggered();
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void on_actionLoadSiphraConfig_triggered();

    void on_anyLedCheckbox_clicked();
    void on_btnReadAllCubesRegs_clicked();
    void on_btnExpandSiphraRegTable_clicked();
    void on_btnCollapseSiphraRegTable_clicked();
    void on_btnLoadSiphraConfig_clicked();

    void on_cubes_devErrorOccured(int error);
    void on_cubes_devReadReady();

private:
    Ui::CubesControl    *ui;

    QLabel              *lblConnStatus;
    int                 connStatus;

    CubesProtoUartPmod  *cubes;
    CommSettingsDialog  *commSettings;

    QSerialPort         *serialPort;

    void    showConnStatus(int connUp);
};

#endif // CUBESCONTROL_H

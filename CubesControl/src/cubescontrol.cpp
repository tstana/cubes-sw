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

#include <siphratreewidgetitem.h>
#include <cubescontrol.h>
#include "cubesprotouartpmod.h"
#include "ui_cubescontrol.h"

#include <QLabel>
#include <QString>
#include <QMessageBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

CubesControl::CubesControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CubesControl),
    commSettings(new CommSettingsDialog)
{
    ui->setupUi(this);

    /* Create a new label in the status bar to show the connection status */
    lblConnStatus = new QLabel;
    lblConnStatus->setFixedWidth(120);
    lblConnStatus->setFixedHeight(15);
    lblConnStatus->setIndent(5);
    connStatus = 0;
    showConnStatus(connStatus);
    statusBar()->addPermanentWidget(lblConnStatus);

    /* Make CommSettingsDialog modal and attach action to show it */
    commSettings->setModal(true);
    connect(ui->actionConfigConnection, &QAction::triggered,
            commSettings, &CubesControl::show);

    /* Connect LED check box clicks to single handler */
    connect(ui->checkboxLed0, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed1, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed2, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed3, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed4, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed5, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed6, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);
    connect(ui->checkboxLed7, &QCheckBox::clicked,
            this, on_anyLedCheckbox_clicked);

    /* Resize address table in CUBES register view */
    ui->tableCubesRegs->resizeColumnToContents(1);

    /* Adjustments to CUBES register table view */
    for (int r = 0; r < ui->tableCubesRegs->rowCount(); r++) {
        for (int c = 0; c < ui->tableCubesRegs->columnCount(); c++) {
            /* Create widget item if non-existing */
            if (ui->tableCubesRegs->item(r,c) == 0) {
                ui->tableCubesRegs->setItem(r,c, new QTableWidgetItem);
            }

            /* Make the item non-editable. */
            Qt::ItemFlags flags = ui->tableCubesRegs->item(r, c)->flags();
            flags &= ~(Qt::ItemIsSelectable | Qt::ItemIsEditable);
            ui->tableCubesRegs->item(r, c)->setFlags(flags);
        }
    }

    /* Add items to SIPHRA register view */
    for (int i = 0; i < 1+SIPHRA_PARITY_ERR_REG; i++) {
        ui->treeSiphraRegMap->insertTopLevelItem(i,
                    new SiphraTreeWidgetItem(ui->treeSiphraRegMap, i, true));
    }
    ui->treeSiphraRegMap->setColumnWidth(0, 100);
    ui->treeSiphraRegMap->setColumnWidth(1, 200);
    ui->treeSiphraRegMap->setColumnWidth(2, 100);
    ui->treeSiphraRegMap->header()->setSectionResizeMode(3, QHeaderView::Stretch);

    /* Create communication objects */
    serialPort = new QSerialPort();

    cubes = new CubesProtoUartPmod(serialPort, this);
    connect(cubes, &CubesProtoUartPmod::devErrorOccured,
            this, &CubesControl::on_cubes_devErrorOccured);
    connect(cubes, &CubesProtoUartPmod::devReadReady,
            this, &CubesControl::on_cubes_devReadReady);
}

CubesControl::~CubesControl()
{
    delete ui;
    delete commSettings;
    delete cubes;
}

void CubesControl::on_actionExit_triggered()
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
                               QString::number(commSettings->getSettings()->baud) + " bps");
    } else {
        lblConnStatus->setStyleSheet(STYLE_CONN_DOWN);
        lblConnStatus->setText("Not connected");
    }
}

void CubesControl::on_actionConnect_triggered()
{
    QString msg = "";
    CommSettingsDialog::CommSettings *currentSettings = commSettings->getSettings();

    switch (currentSettings->type) {
    case CommSettingsDialog::None:
    {
        msg += "No connection type selected. Please select one via 'Connection > Configure'.";
        break;
    }
    case CommSettingsDialog::SerialPort:
    {
        cubes->dev()->setPortName(currentSettings->port);
        cubes->dev()->setBaudRate(currentSettings->baud);
        if (cubes->devOpen(QSerialPort::ReadWrite)) {
            msg += "Serial port connection open on " +
                    currentSettings->port + " at " +
                    QString::number(currentSettings->baud) + " bps";
            connStatus = 1;
        }
        showConnStatus(connStatus);
        break;
    }
    }

    statusBar()->showMessage(msg, 5000);
}

void CubesControl::on_actionDisconnect_triggered()
{
    QString msg;

    if (connStatus == 0) {
        msg = "Connection not open!";
    } else {
        cubes->devClose();
        connStatus = 0;
        showConnStatus(connStatus);
        msg = "Connection closed.";
    }

    statusBar()->showMessage(msg, 5000);
}

void CubesControl::on_actionLoadSiphraConfig_triggered()
{
    int configData;
    int reg = 0;
    int bitField = 0;
    bool isNumber;
    SiphraTreeWidgetItem *currentItem;

    QString fileName = QFileDialog::getOpenFileName(this, "Open file",
                            QString(), "CSV files (*.csv)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            statusBar()->showMessage("Could not open " + fileName, 5000);
            return;
        }

        QTextStream fileData(&file);

        while (!fileData.atEnd()) {
            QString s = fileData.readLine();
            configData = s.toInt(&isNumber);
            if (!isNumber) {
                continue;
            }
            currentItem = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(reg);
            switch (reg) {
            /* Registers with single multiple-bit field */
            case SIPHRA_READOUT_FIXED_LIST:
            case SIPHRA_ADC_CLK_DIV_FACTOR:
            case SIPHRA_CMD_DCAL:
            case SIPHRA_CMD_READOUT:
            case SIPHRA_TRIGGER_LATCHES:
            case SIPHRA_ADC_OUT:
                currentItem->setRegisterValue(configData);
                reg++;
                break;
            /* Other registers */
            default:
                currentItem->setRegisterValue(bitField, configData);
                bitField++;
                bitField %= ui->treeSiphraRegMap->topLevelItem(reg)->childCount();
                if (bitField == 0)
                    reg++;
                break;
            }
        }

        file.close();

        statusBar()->showMessage("Applied config file \"" +
                                 fileName.split('/').last() + "\".", 5000);
    }
}

void CubesControl::on_cubes_devReadReady()
{
    QString s;

    QByteArray data;
    data.resize(cubes->currentCommand()->dataBytes());
    data = cubes->readAll();

    switch (cubes->currentCommand()->code()){
    case CMD_READ_ALL_REGS:
        for (int i = 0; i < data.size(); i += 4) {
            s = "0x" + QString::number((quint8)data[i+0], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+1], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+2], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+3], 16).rightJustified(2, '0');
            ui->tableCubesRegs->item(i/4, 2)->setText(s);
        }
        break;
    default:
        break;
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

void CubesControl::on_anyLedCheckbox_clicked()
{
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    quint8 leds =
            (ui->checkboxLed7->isChecked() << 7) |
            (ui->checkboxLed6->isChecked() << 6) |
            (ui->checkboxLed5->isChecked() << 5) |
            (ui->checkboxLed4->isChecked() << 4) |
            (ui->checkboxLed3->isChecked() << 3) |
            (ui->checkboxLed2->isChecked() << 2) |
            (ui->checkboxLed1->isChecked() << 1) |
            (ui->checkboxLed0->isChecked());

    QByteArray data;

    data.resize(4);

    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = leds;

    cubes->sendCommand(CMD_SET_LEDS, data);
}

void CubesControl::on_btnReadAllCubesRegs_clicked()
{
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    QByteArray dummy;

    cubes->sendCommand(CMD_READ_ALL_REGS, dummy);
}

void CubesControl::on_btnExpandSiphraRegTable_clicked()
{
    ui->treeSiphraRegMap->expandAll();
}

void CubesControl::on_btnCollapseSiphraRegTable_clicked()
{
    ui->treeSiphraRegMap->collapseAll();
}

void CubesControl::on_btnLoadSiphraConfig_clicked()
{
    on_actionLoadSiphraConfig_triggered();
}

void CubesControl::on_btnWriteAllSiphraRegs_clicked()
{
    SiphraTreeWidgetItem *item;
    QByteArray data;
    data.resize(8);

    static int count = 0;
    QFile f(QString("command%1.txt").arg(count++));
    f.open(QIODevice::WriteOnly);
    QTextStream s(&f);

    /* Issue write commands to the ASIC registers */
    for (int i = 0; i < ui->treeSiphraRegMap->topLevelItemCount(); i++) {
        item = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(i);
        data[0] = 0x00;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = (i << 1) | 0x1;
        data[4] = (item->registerValue() >> 24);
        data[5] = (item->registerValue() >> 16);
        data[6] = (item->registerValue() >>  8);
        data[7] = (item->registerValue());

//        cubes->sendCommand(CMD_SIPHRA_REG_OP, data);

        s << "0x";
        for (int j = 0; j < data.size(); j++) {
            s << QString::number((int)data[j]&0xff, 16).rightJustified(2, '0', true);
        }
        s << '\n';
    }

    f.close();

    statusBar()->showMessage("Write all SIPHRA registers finished.", 5000);
}

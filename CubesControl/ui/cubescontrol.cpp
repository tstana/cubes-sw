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
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>

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

    /* CommSettingsDialog interaction to main UI */
    commSettings->setModal(true);
    connect(ui->actionConfigConnection, &QAction::triggered,
            commSettings, &CubesControl::show);
    connect(commSettings, SIGNAL(accepted()),
            ui->actionConnect, SIGNAL(triggered()));

    /* Prepare ADC poll */
    ui->lblAdcChan->setText("");
    m_siphraAdcValue = 0.0;
    ui->lblAdcValue->setText(QString::number(m_siphraAdcValue));
    m_siphraAdcPollEnabled = false;
    ui->btnToggleAdcPoll->setText("Enable");
    ui->btnToggleAdcPollHisto->setText("Enable");

    tmrSiphraAdcPoll = new QTimer(this);
    connect(tmrSiphraAdcPoll, &QTimer::timeout,
            this, &CubesControl::on_tmrSiphraAdcPoll_timeout);

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

    /* Preparations for making _only_ value columns editable */
    ui->treeSiphraRegMap->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for (int i = 0; i < ui->treeSiphraRegMap->topLevelItemCount(); i++) {
        ui->treeSiphraRegMap->topLevelItem(i)->setFlags(
                    ui->treeSiphraRegMap->topLevelItem(i)->flags()|Qt::ItemIsEditable);
        for (int j = 0; j < ui->treeSiphraRegMap->topLevelItem(i)->childCount(); j++) {
            ui->treeSiphraRegMap->topLevelItem(i)->child(j)->setFlags(
                        ui->treeSiphraRegMap->topLevelItem(i)->child(j)->flags()|Qt::ItemIsEditable);
        }
    }

    m_changedByUser = false;

    /* Set register map view visual properties */
    ui->treeSiphraRegMap->setColumnWidth(0, 100);
    ui->treeSiphraRegMap->setColumnWidth(1, 200);
    ui->treeSiphraRegMap->setColumnWidth(2, 100);
    ui->treeSiphraRegMap->header()->setSectionResizeMode(3, QHeaderView::Stretch);

    /* Prepare treeSiphraRegMap for a custom context menu */
    ui->treeSiphraRegMap->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeSiphraRegMap, &QWidget::customContextMenuRequested,
            this, &CubesControl::on_treeSiphraRegMap_contextMenuRequested);

    /* Create histogram chart view */
    histogramNumBins = ui->cbNumBins->currentText().toInt();
    histogramData.fill(0, histogramNumBins);

    QBarSet *set = new QBarSet("Channel amplitude");
    for (int i = 0; i < histogramNumBins; i++) {
        set->append(histogramData[i]);
        set->setColor(QColor(0, 0, 128, 255));
        set->setBorderColor(QColor(0, 0, 128, 255));
    }
    QBarSeries *series = new QBarSeries;
    series->append(set);
    series->setBarWidth(1);

    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->setLabelsVisible(false);
    axisX->setGridLineVisible(false);
    for (int i = 0; i < histogramNumBins; i++) {
        axisX->append(QString::number(i));
    }

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 64);
    axisY->setLabelFormat("%d");

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("Histogram");
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    chart->legend()->setVisible(false);

    ui->histogram->setChart(chart);
//    ui->histogram->setRenderHint(QPainter::Antialiasing);

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

/* TODO: Move this define to a more generic place... */
#define ADC_MAX_VALUE           4096;

void CubesControl::updateHistogram()
{
    int idx = m_siphraAdcValue*histogramNumBins / ADC_MAX_VALUE;
    ++histogramData[idx];

    if (ui->tabWidget->currentIndex() == 1) {
        QBarSeries *series = (QBarSeries *)ui->histogram->chart()->series()[0];
        series->barSets()[0]->replace(idx, histogramData[idx]);

        QValueAxis *axisY = (QValueAxis *)ui->histogram->chart()->axisY(series);
        if (histogramData[idx] > axisY->max()) {
            axisY->setMax(2*axisY->max());
        }
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

void CubesControl::on_actionSaveSiphraConfig_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save file",
                                                    QString(), "CSV files (*.csv)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        statusBar()->showMessage("Could not open " + fileName, 5000);
        return;
    }

    QTextStream fileData(&file);
    SiphraTreeWidgetItem *item;

    for (int i = 0; i < ui->treeSiphraRegMap->topLevelItemCount(); i++) {
        item = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(i);
        fileData << "\n";
        fileData << "# " + item->registerName() + "\n";
        if ((i == SIPHRA_READOUT_FIXED_LIST) || (i == SIPHRA_TRIGGER_LATCHES)) {
            fileData << QString::number(item->registerValue()) + "\n";
            continue;
        }
        for (int j = 0; j < ui->treeSiphraRegMap->topLevelItem(i)->childCount(); j++) {
            fileData << QString::number(item->registerValue(j)) + "\n";
        }
    }
    file.close();
}

void CubesControl::on_actionReadSiphraReg_triggered()
{
    /* The command can't be executed without an open connection */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    /* Initiate CUBES command to read SIPHRA register */
    QByteArray data;
    data.resize(8);
    for (int i = 0; i < 7; i++) {
        data [i] = 0x00;
    }
    data[7] = (m_siphraRegAddr << 1);

    statusBar()->showMessage("Reading SIPHRA register at address 0x" +
            QString::number(m_siphraRegAddr & 0xff, 16).rightJustified(2, '0').toUpper());

    cubes->sendCommand(CMD_SIPHRA_REG_OP, data);

    /* Finally, initiate command to read out the register value */
    m_readAllSiphraRegs = false;
    cubes->sendCommand(CMD_GET_SIPHRA_DATAR, data);
}

void CubesControl::on_actionWriteSiphraReg_triggered()
{
    /* The command can't be executed without an open connection */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    /* m_siphraRegVal = 0xffffffff if no value is set in the value column */
    if (m_siphraRegVal == 0xffffffff) {
        statusBar()->showMessage("Please set a register value to write!", 5000);
        return;
    }

    /* Initiate CUBES command to write SIPHRA register*/
    QByteArray data;
    data.resize(8);
    data[0] = (m_siphraRegVal >> 24) & 0xff;
    data[1] = (m_siphraRegVal >> 16) & 0xff;
    data[2] = (m_siphraRegVal >>  8) & 0xff;
    data[3] = (m_siphraRegVal & 0xff);
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = (m_siphraRegAddr << 1) | 0x1;

    statusBar()->showMessage("Writing SIPHRA register at address 0x" +
            QString::number(m_siphraRegAddr & 0xff, 16).rightJustified(2, '0').toUpper());

    cubes->sendCommand(CMD_SIPHRA_REG_OP, data);
}

void CubesControl::on_actionToggleAdcPoll_triggered()
{
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    if (m_siphraAdcPollEnabled)
        m_siphraAdcPollEnabled = false;
    else
        m_siphraAdcPollEnabled = true;

    on_actionToggleAdcPoll_triggered(m_siphraAdcPollEnabled);
}

void CubesControl::on_actionToggleAdcPoll_triggered(bool checked)
{
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    m_siphraAdcPollEnabled = checked;

    if (checked) {
        ui->btnToggleAdcPoll->setText("Disable");
        ui->btnToggleAdcPollHisto->setText("Disable");
        tmrSiphraAdcPoll->start(10);
    } else {
        ui->btnToggleAdcPoll->setText("Enable");
        ui->btnToggleAdcPollHisto->setText("Enable");
        tmrSiphraAdcPoll->stop();
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
    case CMD_GET_SIPHRA_DATAR:
    {
        /* Apply the value to the tree widget item */
        SiphraTreeWidgetItem *reg;
        reg = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(m_siphraRegAddr);
        reg->setRegisterValue( ((data[0] & 0xff) << 24) |
                               ((data[1] & 0xff) << 16) |
                               ((data[2] & 0xff) <<  8) |
                               ((data[3] & 0xff)) );

        /* Continue if reading all registers */
        if (m_readAllSiphraRegs) {
            ++m_siphraRegAddr;
            if (m_siphraRegAddr >= ui->treeSiphraRegMap->topLevelItemCount()) {
                m_siphraRegAddr = 0;
                statusBar()->showMessage("Read all SIPHRA registers finished.");
            } else {
                for (int i = 0; i < 7; ++i) {
                    data[i] = 0x00;
                }
                data[7] = (m_siphraRegAddr << 1);
                cubes->sendCommand(CMD_SIPHRA_REG_OP, data);
                cubes->sendCommand(CMD_GET_SIPHRA_DATAR, data);
            }
        }
        break;
    }
    case CMD_GET_SIPHRA_ADCR:
    {
        /*
         * Get the trigger channel and data, and update the histogram data set.
         * The viewing widgets are updated only if they are visible (the tab
         * which they are a child of is being dispalyed).
         */
        bool trigged = (data[0] & 0x80);
        m_siphraAdcChan = data[1] & 0x1f;
        m_siphraAdcValue = (double)(((data[2] & 0xf) << 8) | (data[3] & 0xff));
        if (m_siphraAdcPollEnabled && trigged) {
            updateHistogram();
            if (ui->tabWidget->currentIndex() == 2) {
                /* Adjust the data to 610uV per MSB before displaying */
                m_siphraAdcValue *= 0.610;
                ui->lblAdcChan->setText(QString::number(m_siphraAdcChan));
                ui->lblAdcValue->setText(QString::number(m_siphraAdcValue));
            }
        }
        break;
    }
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

void CubesControl::on_btnToggleAdcPoll_clicked()
{
    on_actionToggleAdcPoll_triggered();
}

void CubesControl::on_tmrSiphraAdcPoll_timeout()
{
    QByteArray dummy;
    dummy.resize(4);
    cubes->sendCommand(CMD_GET_SIPHRA_ADCR, dummy);
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

void CubesControl::on_btnSaveSiphraConfig_clicked()
{
    on_actionSaveSiphraConfig_triggered();
}

void CubesControl::on_btnWriteAllSiphraRegs_clicked()
{
    SiphraTreeWidgetItem *item;
    QByteArray data;
    data.resize(8);

    /* The command can't be executed without an open connection */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    /* Issue write commands to the ASIC registers */
    for (int i = 0; i < ui->treeSiphraRegMap->topLevelItemCount(); i++) {
        item = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(i);
        data[0] = (item->registerValue() >> 24) & 0xff;
        data[1] = (item->registerValue() >> 16) & 0xff;
        data[2] = (item->registerValue() >>  8) & 0xff;
        data[3] = (item->registerValue()) & 0xff;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = (i << 1) | 0x1;

        cubes->sendCommand(CMD_SIPHRA_REG_OP, data);
    }

    statusBar()->showMessage("Write all SIPHRA registers finished.", 5000);
}

void CubesControl::on_btnReadAllSiphraRegs_clicked()
{
    /* The command can't be executed without an open connection */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    QByteArray data;
    data.resize(8);
    for (int i = 0; i < 8; ++i) {
        data[i] = 0x00;
    }
    cubes->sendCommand(CMD_SIPHRA_REG_OP, data);
    m_readAllSiphraRegs = true;
    m_siphraRegAddr = 0x00;
    cubes->sendCommand(CMD_GET_SIPHRA_DATAR, data);
}

void CubesControl::on_cbNumBins_currentTextChanged(const QString &arg1)
{
    tmrSiphraAdcPoll->stop();

    histogramNumBins = arg1.toInt();
    histogramData.fill(0, histogramNumBins);

    QBarSet *set = new QBarSet("ADC value", this);
    for (int i = 0; i < histogramNumBins; i++) {
        set->append(histogramData[i]);
        set->setColor(QColor(0, 0, 128, 255));
        set->setBorderColor(QColor(0, 0, 128, 255));
    }
    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart;
    chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->addSeries(series);

    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->setLabelsVisible(false);
    axisX->setGridLineVisible(false);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 64);
    axisY->setLabelFormat("%d");

    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    chart->legend()->setVisible(false);

    QChart *oldChart = ui->histogram->chart();
    ui->histogram->setChart(chart);
    delete oldChart;

    if (m_siphraAdcPollEnabled)
        tmrSiphraAdcPoll->start();
}

void CubesControl::on_btnToggleAdcPollHisto_clicked()
{
    on_actionToggleAdcPoll_triggered();
}

void CubesControl::on_treeSiphraRegMap_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (column == 2) {
        /* Ignore editing whole register value -- accept only bit field edits */
        if (item->parent() == (QTreeWidgetItem *)ui->treeSiphraRegMap) {
            return;
        }

        /* Edit the item and allow for itemChanged() event to operate */
        ui->treeSiphraRegMap->editItem(item, column);
        m_changedByUser = true;
        m_textBeforeChange = item->text(column);
    }
}

void CubesControl::on_treeSiphraRegMap_itemChanged(QTreeWidgetItem *item, int column)
{
    /* If by _any chance_, _somehow_, another column gets edited, bail !!! */
    if (column != 2) {
        return;
    }

    /* Also bail out on change not due to user double clicking */
    if (!m_changedByUser) {
        return;
    }

    m_changedByUser = false;

    /* Try to convert the value into a number, give up if NaN */
    bool isNumber;
    int bitFieldValue;
    bitFieldValue = item->text(column).toInt(&isNumber);
    if (!isNumber) {
        item->setText(column, m_textBeforeChange);
        return;
    }

    /* The clicked item should be a child of a SiphraTreeWidgetItem */
    SiphraTreeWidgetItem *siphraItem = (SiphraTreeWidgetItem *)item->parent();

    /* Update the value and prevent user from changing other items */
    ui->treeSiphraRegMap->setEditTriggers(QAbstractItemView::CurrentChanged);
    siphraItem->setRegisterValue(siphraItem->indexOfChild(item), bitFieldValue);
    ui->treeSiphraRegMap->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void CubesControl::on_treeSiphraRegMap_contextMenuRequested(const QPoint &p)
{
    SiphraTreeWidgetItem *item = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->itemAt(p);

    /*
     * Make sure we write the whole register, not just the bit field value
     *
     * If the user right-clicks the top-level item in the tree view, parent()
     * returns 0, therefore we can leave the SiphraTreeWidgetItem as previously
     * set.
     *
     * Alternatively, right-clicking on a bit-field value in the
     * SiphraTreeWidgetItem will result in parent() returning a pointer to the
     * top-level item, which holds the full value of the register we need to write.
     *
     * This all needs to be done because of the current implementation of
     * SiphraTreeWidgetItem, which doesn't hold a member for the register value,
     * but instead it relies on the value written in column 2 of the tree widget.
     */
    if (item->parent() != 0) {
        item = (SiphraTreeWidgetItem *)item->parent();
    }

    m_siphraRegAddr = item->registerAddress();
    m_siphraRegVal = item->registerValue();

    QMenu menu;
    menu.addAction(ui->actionReadSiphraReg);
    menu.addAction(ui->actionWriteSiphraReg);
    menu.exec(QCursor::pos());
}

void CubesControl::on_tabWidget_currentChanged(int index)
{
    /* Update histogram chart with new values */
    if (index == 1) {
        QBarSeries *series = (QBarSeries *)ui->histogram->chart()->series()[0];
        QBarSet *set = series->barSets()[0];
        for (int i = 0; i < histogramNumBins; i++) {
            set->replace(i, histogramData[i]);
        }
    }
}

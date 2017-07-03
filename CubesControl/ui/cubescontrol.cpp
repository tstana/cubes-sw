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

#include <QDebug>

/*============================================================================
 * CONSTRUCTOR AND DESTRUCTOR
 *============================================================================*/
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

    /* Prepare ADC poll */
    ui->lblAdcChan->setText("");
    m_siphraAdcValue = 0.0;
    ui->lblAdcValue->setText(QString::number(m_siphraAdcValue));
    m_siphraAdcPollEnabled = false;
    ui->btnToggleAdcPoll->setText("Enable");

    tmrSiphraAdcPoll = new QTimer(this);
    tmrSiphraAdcPoll->setInterval(500);
    connect(tmrSiphraAdcPoll, &QTimer::timeout,
            this, &CubesControl::on_tmrSiphraAdcPoll_timeout);

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
    ui->comboboxNumBins->setCurrentText("1024");
    histogramNumBins = 1024;
    histogramData.resize(histogramNumBins);
    histogramData.fill(0);

    QLineSeries *series = new QLineSeries;
    for (int i = 0; i < histogramNumBins; i++) {
        series->append(i, histogramData[i]);
        series->setColor(QColor(0, 0, 128, 255));
    }

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

    /* Other histogram view objects */
    if (ui->comboboxHistogramRunType->currentIndex() == 0) {
        ui->lblHistogramRunTime->setEnabled(false);
        ui->spinboxHistogramRuntime->setEnabled(false);
    }
    ui->btnStartStopHistogram->setText("Start");
    ui->lblHistogramStatus->setText("Histogram not running.");

    tmrHistogramAdcPoll = new QTimer(this);
    tmrHistogramAdcPoll->setInterval(10);
    connect(tmrHistogramAdcPoll, &QTimer::timeout,
            this, &CubesControl::on_tmrHistogramAdcPoll_timeout);

    histogramAdcPollEnabled = false;

    /* Event rate readout */
    ui->lblEventRate->setText("0");
    tmrEventRateReadout = new QTimer;
    tmrEventRateReadout->setInterval(1000);
    connect(tmrEventRateReadout, &QTimer::timeout,
            this, &CubesControl::on_tmrEventRateReadout_timeout);

    /* Update TH delay time label */
    ui->lblThDelayTime->setText(QString::number(thDelayTime(ui->sliderTune->value(),
                                                            ui->sliderDelay->value())));

    /* Create communication objects */
    serialPort = new QSerialPort();

    cubes = new CubesProtoUartPmod(serialPort, this);
    connect(cubes, &CubesProtoUartPmod::devErrorOccured,
            this, &CubesControl::on_cubes_devErrorOccured);
    connect(cubes, &CubesProtoUartPmod::devReadReady,
            this, &CubesControl::on_cubes_devReadReady);
}

/*----------------------------------------------------------------------------
 * Destructor
 *----------------------------------------------------------------------------*/
CubesControl::~CubesControl()
{
    delete ui;
    delete commSettings;
    delete cubes;
}
/*============================================================================*/

/*============================================================================
 * PRIVATE METHODS
 *============================================================================*/
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

void CubesControl::updateHistogram(bool updateAll)
{
    /* No update to be done if not connected or polling not enabled! */
    if ((!connStatus) || (!histogramAdcPollEnabled))
        return;

    /* Update all values and return - this is in case the user changes to the histogram pane */
    if (updateAll) {
        QLineSeries *series = (QLineSeries *)ui->histogram->chart()->series()[0];
        for (int i = 0; i < histogramNumBins; i++) {
            series->replace(i, i, histogramData[i]);
        }
        return;
    }

    /*
     * Update histogram iff triggered channel is the one we should update AND
     * we are on the histogram pane
     */
    if (m_siphraAdcChan == ui->spinboxHistogramChannel->value()) {
        int idx = m_siphraAdcValue*histogramNumBins / ADC_MAX_VALUE;
        ++histogramData[idx];

        if (ui->tabWidget->currentIndex() == 2) {
            QLineSeries *series = (QLineSeries *)ui->histogram->chart()->series()[0];
            series->replace(idx, idx, histogramData[idx]);

            QValueAxis *axisY = (QValueAxis *)ui->histogram->chart()->axisY(series);
            int currentMax = axisY->max();
            /* Adapt Y axis range based on max value in histogram */
            if (histogramData[idx] > currentMax) {
                (currentMax < 256) ? axisY->setMax(currentMax * 2) :
                                     axisY->setMax(currentMax + 256);
            }
        }
    }
}

int CubesControl::uiSiphraChannelRegValue()
{
    int value;

    value = (ui->sliderQcThreshold->value() << 5) |
            (ui->sliderQcHysteresis->value() << 2) |
            ((ui->checkboxPowerUpChannel->checkState() == 2) << 1) |
            (ui->checkboxEnableChannelTriggering->checkState() == 2);

    /*
     * OR register value with currently existing bits, masking settings the user
     * makes via the UI, before applying
     */
    SiphraTreeWidgetItem *siphraReg =
            (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(ui->spinboxSiphraChannelToConfig->value() - 1);

    value |= siphraReg->registerValue() & 0x03ffe000;

    return value;
}

void CubesControl::decodeShapingTime(int setting,
                                     int *bias,
                                     int *feedback_cap,
                                     int *feedback_res,
                                     int *hold_cap,
                                     int *input_cap)
{
    /* Values below obtained from SIHPRA datasheet */
    switch (setting) {
    // 200 ns
    case 0:
        *feedback_cap = 2;
        *feedback_res = 1;
        *input_cap = 2;
        *hold_cap = 0;
        *bias = 7;
        break;
    // 400 ns
    case 1:
        *feedback_cap = 5;
        *feedback_res = 1;
        *input_cap = 4;
        *hold_cap = 1;
        *bias = 6;
        break;
    // 800 ns
    case 2:
        *feedback_cap = 14;
        *feedback_res = 4;
        *input_cap = 5;
        *hold_cap = 3;
        *bias = 6;
        break;
    // 1600 ns
    case 3:
        *feedback_cap = 15;
        *feedback_res = 7;
        *input_cap = 14;
        *hold_cap = 7;
        *bias = 5;
        break;
    // fail -- assume all zeroes
    default:
        *feedback_cap = 0;
        *feedback_res = 0;
        *input_cap = 0;
        *hold_cap = 0;
        *bias = 0;
        break;
    }
}

int CubesControl::decodeShaperSettings(int bias,
                                       int feedback_cap,
                                       int feedback_res,
                                       int hold_cap,
                                       int input_cap)
{

    /* 200 ns */
    if ((bias == 7) && (feedback_cap == 2) && (feedback_res == 1) &&
            (input_cap == 2) && (hold_cap == 0))
        return 0;

    /* 400 ns */
    if ((bias == 6) && (feedback_cap == 5) && (feedback_res == 1) &&
            (input_cap == 4) && (hold_cap == 1))
        return 1;

    /* 800 ns */
    if ((bias == 6) && (feedback_cap == 14) && (feedback_res == 4) &&
            (input_cap == 5) && (hold_cap == 3))
        return 2;

    /* 1600 ns */
    if ((bias == 5) && (feedback_cap == 15) && (feedback_res == 7) &&
            (input_cap == 14) && (hold_cap == 7))
        return 3;

    /*
     * If we've reached this point, an invalid SIPHRA setting has been detected.
     * Warn the user and revert previous setting.
     */
    QMessageBox box;
    QString text;
    QTextStream textStream(&text);
    textStream << "Invalid shaping time settings detected, value shown here is no longer valid!\n"
               << "\n"
               << "Read the CHANNEL_CONFIG register in the register map tab to inspect the settings.";
    box.setText(text);
    box.exec();

    return ui->sliderShapingTime->value();
}

int CubesControl::uiSiphraChannelConfigValue()
{
    int value, cmis_gain, shaper_bias, shaper_feedback_cap, shaper_feedback_res,
            shaper_hold_cap, shaper_input_cap;

    /* CMIS gain setting */
    cmis_gain = (1 << ui->sliderCmisGain->value())-1;

    /* Decolde shaper settings */
    decodeShapingTime(ui->sliderShapingTime->value(),
                      &shaper_bias,
                      &shaper_feedback_cap,
                      &shaper_feedback_res,
                      &shaper_hold_cap,
                      &shaper_input_cap);

    /* Apply the CHANNEL_CONFIG register fields */
    value = (cmis_gain << 21) |
            (ui->sliderCiGain->value() << 19) |
            (ui->checkboxCiCompmode->isChecked() << 18) |
            (shaper_bias << 14) |
            (shaper_feedback_cap << 10) |
            (shaper_feedback_res << 7) |
            (shaper_hold_cap << 4) |
            (shaper_input_cap);

    return value;
}

int CubesControl::uiSiphraChannelControlValue()
{
    int value = ui->comboboxCbInput->currentIndex() << 11;

    SiphraTreeWidgetItem *siphraReg =
            (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(0x12);

    value |= siphraReg->registerValue() & 0x7fe7ff;

    return value;
}

int CubesControl::uiSiphraReadoutModeValue()
{
    int value;

    // make sure readout_en_int_hold_start is ON, to avoid confusion as to why
    // the ASIC isn't triggering...
    value = (ui->sliderTune->value() << 10) |
            (ui->sliderDelay->value() << 6) |
            (ui->comboboxHoldSource->currentIndex() << 4) |
            0x1;

    /*
     * OR register value with currently existing bits, masking settings the user
     * makes via the UI, before applying
     */
    SiphraTreeWidgetItem *siphraReg =
            (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(0x18);

    value |= siphraReg->registerValue() & 0xe;

    return value;
}

void CubesControl::writeSiphraChannelReg(int value)
{
    /* Exit if the connection is not open */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open, not applying SIHPRA setting!", 5000);
        return;
    }

    /* Channel reg address is spinbox setting minus 1 (SIPHRA CTRL_CH_01 is at 0x00) */
    m_siphraRegAddr = ui->spinboxSiphraChannelToConfig->value();
    m_siphraRegAddr -= 1;
    m_siphraRegVal = value;
    on_actionWriteSiphraReg_triggered();

    /*
     * Also write the READOUT_FIXED_LIST register if triggering on the channel
     * is enabled. The currently set bits in READOUT_FIXED_LIST are read for
     * now from the current tree view.
     *
     * NB! This last part is temporary and may lead to mismatches with the
     * actual register value.
     */
    m_siphraRegAddr = 0x17;
    SiphraTreeWidgetItem *item = new SiphraTreeWidgetItem;
    item = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(m_siphraRegAddr);
    m_siphraRegVal = item->registerValue();
    if (ui->checkboxEnableChannelTriggering->isChecked()) {
        m_siphraRegVal |= 1 << ui->spinboxSiphraChannelToConfig->value();
    } else {
        m_siphraRegVal &= ~(1 << ui->spinboxSiphraChannelToConfig->value());
    }

    on_actionWriteSiphraReg_triggered();
}

void CubesControl::writeSiphraChannelConfig(int value)
{
    /* Exit if the connection is not open */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open, not applying SIHPRA setting!", 5000);
        return;
    }

    /* Initiate write to SIPHRA register */
    m_siphraRegAddr = 0x11;
    m_siphraRegVal = value;
    on_actionWriteSiphraReg_triggered();
}

void CubesControl::writeSiphraChannelControl(int value)
{
    /* Exit if the connection is not open */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open, not applying SIHPRA setting!", 5000);
        return;
    }

    /* Initiate write to SIPHRA register */
    m_siphraRegAddr = 0x12;
    m_siphraRegVal = value;
    on_actionWriteSiphraReg_triggered();
}

void CubesControl::writeSiphraReadoutMode(int value)
{
    /* Exit if the connection is not open */
    if (!connStatus) {
        statusBar()->showMessage("Connection not open, not applying SIHPRA setting!", 5000);
        return;
    }

    /* Initiate write to SIPHRA register */
    m_siphraRegAddr = 0x18;
    m_siphraRegVal = value;
    on_actionWriteSiphraReg_triggered();
}

void CubesControl::setUiSiphraChannelRegValue(bool powerUpChannel,
                                              bool enableTriggering,
                                              int qcThreshold,
                                              int qcHysteresis)
{
    ui->checkboxPowerUpChannel->setChecked(powerUpChannel);
    ui->checkboxEnableChannelTriggering->setChecked(enableTriggering);
    ui->sliderQcThreshold->setValue(qcThreshold);
    ui->spinboxQcThreshold->setValue(qcThreshold);
    ui->sliderQcHysteresis->setValue(qcHysteresis);
    ui->comboboxQcHysteresis->setCurrentIndex(qcHysteresis);
}

void CubesControl::setUiSiphraChannelConfigValue(int cmisGain,
                                                 int ciGain, bool ciCompmode,
                                                 int shapingTime)
{
    ui->sliderCmisGain->setValue(cmisGain);
    ui->comboboxCmisGain->setCurrentIndex(cmisGain);
    ui->sliderCiGain->setValue(ciGain);
    ui->comboboxCiGain->setCurrentIndex(ciGain);
    ui->checkboxCiCompmode->setChecked(ciCompmode);
    ui->sliderShapingTime->setValue(shapingTime);
    ui->comboboxShapingTime->setCurrentIndex(shapingTime);
}

void CubesControl::setUiSiphraChannelControlValue(int cbInput)
{
    ui->comboboxCbInput->setCurrentIndex(cbInput);
}

void CubesControl::setUiSiphraReadoutModeValue(int thDelay,
                                               int thTune,
                                               int holdSource)
{
    ui->sliderDelay->setValue(thDelay);
    ui->spinboxDelay->setValue(thDelay);
    ui->sliderTune->setValue(thTune);
    ui->spinboxTune->setValue(thTune);
    ui->comboboxHoldSource->setCurrentIndex(holdSource);
}

double CubesControl::thDelayTime(int thTune, int thDelay)
{
    return 1700 * (1 + 0.0748*thTune)/(1 + (15 - thDelay));
}

/*============================================================================
 * ACTIONS
 *============================================================================*/
void CubesControl::on_actionExit_triggered()
{
    this->close();
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

    /*
     * Issue read all SIPHRA regs on connect
     *      TODO: Fix ugly hack of calling the on_clicked() of the button
     */
    on_btnReadAllSiphraRegs_clicked();

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

    /* Update SIPHRA register tree item */
    SiphraTreeWidgetItem *reg =
            (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(m_siphraRegAddr);
    reg->setRegisterValue(m_siphraRegVal);
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
        tmrSiphraAdcPoll->start();
    } else {
        ui->btnToggleAdcPoll->setText("Enable");
        tmrSiphraAdcPoll->stop();
    }
}

void CubesControl::on_actionExportHistogram_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save file",
                                                    QString(), "*.bin");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly)) {
            statusBar()->showMessage("Could not open " + fileName, 5000);
            return;
        }

        QByteArray data;

        /* One byte for QcThreshold value */
        data.append(ui->spinboxQcThreshold->value());
        data.append('_');

        /* Four ASCII chars for QC hysteresis; '0' and 'negligible' are treated as the same */
        if (ui->comboboxQcHysteresis->currentIndex() >= 2) {
            data.append(ui->comboboxQcHysteresis->currentText().right(4));
        } else {
            data.append("   0");
        }
        data.append('_');

        /* Five ASCII chars, right-justified for CMIS gain */
        data.append(ui->comboboxCmisGain->currentText().rightJustified(5, ' '));
        data.append('_');

        /* Ten ASCII chars, right-justified for CI gain */
        data.append(ui->comboboxCiGain->currentText().rightJustified(10, ' '));
        data.append('_');

        /* Seven ASCII chars, right-justified for shaping time */
        data.append(ui->comboboxShapingTime->currentText().rightJustified(7, ' '));
        data.append('_');

        /* Two bytes for number of histograms */
        data.append((histogramNumBins & 0xff00) >> 8);
        data.append(histogramNumBins & 0xff);

        /*
         * Then the actual histogram counts. This currently assumes no more than
         * 65535 (16-bit) counts per bin.
         */
        for (int i = 0; i < 2*histogramNumBins; i+=2) {
            data.append((histogramData[i/2] & 0xff00) >> 8);
            data.append(histogramData[i/2] & 0xff);
        }

        file.write(data);

        statusBar()->showMessage("Histogram values exported to " + fileName, 5000);

        file.close();
    }
}

void CubesControl::on_actionImportHistogram_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file",
                                                    QString(), "*.bin");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            statusBar()->showMessage("Could not open " + fileName, 5000);
            return;
        }

        QByteArray data;

        /* Skip ASIC config data for histogram import */
        file.read(32);

        /* First apply histogram number of bins reading */
        data = file.read(2);
        histogramNumBins = ((data[0] & 0xff) << 8) | (data[1] & 0xff);
        ui->comboboxNumBins->setCurrentText(QString::number(histogramNumBins));

        /* Then, read the histogram data and apply. Histo data is two bytes per bin. */
        data = file.read(2*histogramNumBins);
        for (int i = 0; i < data.size(); i+=2) {
            histogramData[i/2] = ((data[i] & 0xff) << 8) | (data[i+1] & 0xff);
        }

        /* Prepare new chart for display */
        QLineSeries *series = new QLineSeries;
        for (int i = 0; i < histogramNumBins; i++) {
            series->append(i, histogramData[i]);
            series->setColor(QColor(0, 0, 128, 255));
        }

        QChart *chart = new QChart;
        chart->setTitle("Histogram");
        chart->setAnimationOptions(QChart::NoAnimation);
        chart->addSeries(series);

        QBarCategoryAxis *axisX = new QBarCategoryAxis;
        axisX->setLabelsVisible(false);
        axisX->setGridLineVisible(false);
        QValueAxis *axisY = new QValueAxis;

        /* Adjust Y axis range */
        int currentMax = histogramData[0];
        for (int i = 1; i < histogramNumBins; i++) {
            if (histogramData[i] > currentMax)
                currentMax = histogramData[i];
        }
        int range = (currentMax < 256) ? (256) :
                                         (currentMax - currentMax%256 + 256);

        axisY->setRange(0, range);
        axisY->setLabelFormat("%d");

        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);

        chart->legend()->setVisible(false);

        QChart *oldChart = ui->histogram->chart();
        ui->histogram->setChart(chart);
        delete oldChart;

        statusBar()->showMessage("Finished reading histogram data from " + fileName, 5000);
        file.close();
    }
}

/*============================================================================
 * CUBES OBJECT SLOTS
 *============================================================================*/
void CubesControl::on_cubes_devReadReady()
{
    QString s;

    QByteArray data;
    data.resize(cubes->currentCommand()->dataBytes());
    data = cubes->readAll();

    switch (cubes->currentCommand()->code()){
    case CMD_READ_ALL_REGS:
    {
        int j = 0;
        for (int i = 0; i < 0x20; i += 4) {
            s = "0x" + QString::number((quint8)data[i+0], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+1], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+2], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+3], 16).rightJustified(2, '0');
            ui->tableCubesRegs->item(j++, 2)->setText(s);
        }
        for (int i = 0x200; i < 0x310; i += 4) {
            s = "0x" + QString::number((quint8)data[i+0], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+1], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+2], 16).rightJustified(2, '0') +
                       QString::number((quint8)data[i+3], 16).rightJustified(2, '0');
            ui->tableCubesRegs->item(j++, 2)->setText(s);
        }
        break;
    }
    case CMD_GET_SIPHRA_DATAR:
    {
        /* Apply the value to the tree widget item */
        SiphraTreeWidgetItem *reg;
        reg = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(m_siphraRegAddr);
        reg->setRegisterValue( ((data[0] & 0xff) << 24) |
                               ((data[1] & 0xff) << 16) |
                               ((data[2] & 0xff) <<  8) |
                               ((data[3] & 0xff)) );

        /* Update visual setting if in the visual config tab */
        if (ui->tabWidget->currentIndex() == 1) {
            if (m_siphraRegAddr == ui->spinboxSiphraChannelToConfig->value()-1) {
                bool enableChannelTriggering = reg->registerValue(7),
                     powerUpChannel = reg->registerValue(6);
                int qcHysteresis = reg->registerValue(5),
                    qcThreshold = reg->registerValue(4);
                setUiSiphraChannelRegValue(powerUpChannel, enableChannelTriggering,
                                           qcThreshold, qcHysteresis);
            } else if (m_siphraRegAddr == 0x11) {
                int cmis_gain, ci_gain, shaper_bias, shaper_feedback_cap,
                        shaper_feedback_res, shaper_hold_cap, shaper_input_cap,
                        shapingTime;
                bool ci_compmode;
                cmis_gain = reg->registerValue(0);
                switch (cmis_gain) {
                case 0:
                    cmis_gain = 0;
                    break;
                case 1:
                    cmis_gain = 1;
                    break;
                case 3:
                    cmis_gain = 2;
                    break;
                case 7:
                    cmis_gain = 3;
                    break;
                default:
                {
                    cmis_gain = ui->sliderCmisGain->value();
                    QMessageBox box;
                    QString text;
                    QTextStream textStream(&text);
                    textStream << "Invalid CMIS gain setting detected, value shown here is no longer valid!\n"
                               << "\n"
                               << "Read the CHANNEL_CONFIG register in the register map tab to inspect the setting.";
                    box.setText(text);
                    box.exec();
                    break;
                }
                }
                ci_gain = reg->registerValue(1);
                ci_compmode = reg->registerValue(2);
                shaper_bias = reg->registerValue(3);
                shaper_feedback_cap = reg->registerValue(4);
                shaper_feedback_res = reg->registerValue(5);
                shaper_hold_cap = reg->registerValue(6);
                shaper_input_cap = reg->registerValue(7);
                shapingTime = decodeShaperSettings(shaper_bias, shaper_feedback_cap,
                                                   shaper_feedback_res, shaper_hold_cap,
                                                   shaper_input_cap);
                setUiSiphraChannelConfigValue(cmis_gain, ci_gain, ci_compmode, shapingTime);
            } else if (m_siphraRegAddr == 0x12) {
                int cb_input = reg->registerValue(3);
                if (cb_input > 2) {
                    QMessageBox box;
                    QString text;
                    QTextStream textStream(&text);
                    textStream << "Invalid CB input setting detected, value shown here no longer valid!\n"
                               << "\n"
                               << "Read the CHANNEL_CONTROL register in the register map tab to inspect the setting.";
                    box.setText(text);
                    box.exec();
                    cb_input = ui->comboboxCbInput->currentIndex();
                }
                setUiSiphraChannelControlValue(cb_input);
            } else if (m_siphraRegAddr == 0x18) {
                int int_hold_tune;
                int int_hold_delay;
                int int_hold_source;

                int_hold_tune = reg->registerValue(0);
                int_hold_delay = reg->registerValue(1);
                int_hold_source = reg->registerValue(2);

                setUiSiphraReadoutModeValue(int_hold_delay, int_hold_tune,
                                            int_hold_source);
            }
        }

        /* Continue if reading all registers */
        if (m_readAllSiphraRegs) {
            ++m_siphraRegAddr;
            if (m_siphraRegAddr >= ui->treeSiphraRegMap->topLevelItemCount()) {
                m_siphraRegAddr = 0;
                m_readAllSiphraRegs = false;
                statusBar()->showMessage("Read all SIPHRA registers finished.", 5000);
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

        if (histogramAdcPollEnabled && trigged) {
            updateHistogram();
        }

        if (m_siphraAdcPollEnabled && trigged) {
            if (ui->tabWidget->currentIndex() == 3) {
                /* Adjust the data to 610uV per MSB before displaying */
                m_siphraAdcValue *= 0.610;
                ui->lblAdcChan->setText(QString::number(m_siphraAdcChan));
                ui->lblAdcValue->setText(QString::number(m_siphraAdcValue));
            }
        }
        break;
    }
    case CMD_GET_CH01_REGS:
    case CMD_GET_CH02_REGS:
    case CMD_GET_CH03_REGS:
    case CMD_GET_CH04_REGS:
    case CMD_GET_CH05_REGS:
    case CMD_GET_CH06_REGS:
    case CMD_GET_CH07_REGS:
    case CMD_GET_CH08_REGS:
    case CMD_GET_CH09_REGS:
    case CMD_GET_CH10_REGS:
    case CMD_GET_CH11_REGS:
    case CMD_GET_CH12_REGS:
    case CMD_GET_CH13_REGS:
    case CMD_GET_CH14_REGS:
    case CMD_GET_CH15_REGS:
    case CMD_GET_CH16_REGS:
    {
        int regValue = ((data[2] << 8) & 0xff00) | (data[3] & 0xff);
        ui->lblEventRate->setText(QString::number(regValue));
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

/*============================================================================
 * UI WIDGET SLOTS
 *============================================================================*/
void CubesControl::on_btnToggleAdcPoll_clicked()
{
    on_actionToggleAdcPoll_triggered();
}

void CubesControl::on_tmrSiphraAdcPoll_timeout()
{
    QByteArray dummy;

    /* Iff ADC_IN readout enabled, trigger forced readout command by writing to CMD_READOUT reg */
    if (ui->checkboxUseAdcIn->isChecked()) {
        m_siphraRegAddr = 0x1d;
        m_siphraRegVal = 1;
        on_actionWriteSiphraReg_triggered();
    }

    /* Finally, read out ADCR value from FPGA */
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

void CubesControl::on_btnClearHistogram_clicked()
{
    histogramData.fill(0);

    QLineSeries *series = (QLineSeries *)ui->histogram->chart()->series()[0];
    for (int i = 0; i < histogramNumBins; i++) {
        series->replace(i, i, 0);
    }
    ui->histogram->chart()->axisY(series)->setRange(0, 64);
}

void CubesControl::on_comboboxNumBins_currentTextChanged(const QString &arg1)
{
    tmrSiphraAdcPoll->stop();

    histogramNumBins = arg1.toInt();
    histogramData.fill(0, histogramNumBins);

    QLineSeries *series = new QLineSeries;
    for (int i = 0; i < histogramNumBins; i++) {
        series->append(i, histogramData[i]);
        series->setColor(QColor(0, 0, 128, 255));
    }

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

void CubesControl::on_btnStartStopHistogram_clicked()
{
    if (!connStatus) {
        statusBar()->showMessage("Connection not open!", 5000);
        return;
    }

    if (!histogramAdcPollEnabled) {
        if (ui->comboboxHistogramRunType->currentIndex() == 0) {
            ui->lblHistogramStatus->setText("Histogram running in continous mode.");
        } else {
            histogramRunTime = ui->spinboxHistogramRuntime->value() *
                    (60000 / tmrHistogramAdcPoll->interval());
        }
        ui->btnStartStopHistogram->setText("Stop");
        tmrHistogramAdcPoll->start();
        histogramAdcPollEnabled = true;
    } else {
        ui->lblHistogramStatus->setText("Histogram not running.");
        ui->btnStartStopHistogram->setText("Start");
        tmrHistogramAdcPoll->stop();
        histogramAdcPollEnabled = false;
    }
}

void CubesControl::on_tmrHistogramAdcPoll_timeout()
{
    if (ui->comboboxHistogramRunType->currentIndex() == 1) {
        --histogramRunTime;

        /* Update histogram status label once a second; should only be done if user is in the relevant tab */
        if (ui->tabWidget->currentIndex() == 2) {
            int seconds = histogramRunTime * tmrHistogramAdcPoll->interval()/1000;
            QString s = "Histogram running for " + QString::number(seconds) + " s.";
            ui->lblHistogramStatus->setText(s);
        }

        /* Mimic user clicking the button to stop histogramming */
        if (histogramRunTime == 0) {
            on_btnStartStopHistogram_clicked();
        }

    }

    if (ui->tabWidget->currentIndex() == 2) {
        QByteArray dummy;
        cubes->sendCommand(CMD_GET_SIPHRA_ADCR, dummy);
    }
}

void CubesControl::on_btnExportHistogram_clicked()
{
    on_actionExportHistogram_triggered();
}

void CubesControl::on_btnImportHistogram_clicked()
{
    on_actionImportHistogram_triggered();
}

void CubesControl::on_comboboxHistogramRunType_currentIndexChanged(int index)
{
    if (index == 1) {
        ui->lblHistogramRunTime->setEnabled(true);
        ui->spinboxHistogramRuntime->setEnabled(true);
    } else {
        ui->lblHistogramRunTime->setEnabled(false);
        ui->spinboxHistogramRuntime->setEnabled(false);
    }

    /* If histogramming is running, disable it by "clicking" the start/stop button */
    if (histogramAdcPollEnabled) {
        on_btnStartStopHistogram_clicked();
    }
}

void CubesControl::on_checkboxPowerUpChannel_clicked()
{
    int regValue = uiSiphraChannelRegValue();

    writeSiphraChannelReg(regValue);
}

void CubesControl::on_checkboxEnableChannelTriggering_clicked()
{
    writeSiphraChannelReg(uiSiphraChannelRegValue());
}

void CubesControl::on_sliderQcThreshold_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->spinboxQcThreshold->setValue(value);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelReg(uiSiphraChannelRegValue());
}

void CubesControl::on_spinboxQcThreshold_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderQcThreshold->setValue(value);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelReg(uiSiphraChannelRegValue());
}

void CubesControl::on_sliderQcHysteresis_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->comboboxQcHysteresis->setCurrentIndex(value);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelReg(uiSiphraChannelRegValue());
}

void CubesControl::on_comboboxQcHysteresis_currentIndexChanged(int index)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderQcHysteresis->setValue(index);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelReg(uiSiphraChannelRegValue());
}

void CubesControl::on_sliderCmisGain_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->comboboxCmisGain->setCurrentIndex(value);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_comboboxCmisGain_currentIndexChanged(int index)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderCmisGain->setValue(index);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_sliderCiGain_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->comboboxCiGain->setCurrentIndex(value);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_comboboxCiGain_currentIndexChanged(int index)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderCiGain->setValue(index);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_sliderShapingTime_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->comboboxShapingTime->setCurrentIndex(value);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_comboboxShapingTime_currentIndexChanged(int index)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderShapingTime->setValue(index);

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_sliderTune_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->spinboxTune->setValue(value);

    /* Update TH delay time label */
    ui->lblThDelayTime->setText(QString::number(thDelayTime(ui->sliderTune->value(),
                                                            ui->sliderDelay->value())));

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraReadoutMode(uiSiphraReadoutModeValue());
}

void CubesControl::on_spinboxTune_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderTune->setValue(value);

    /* Update TH delay time label */
    ui->lblThDelayTime->setText(QString::number(thDelayTime(ui->sliderTune->value(),
                                                            ui->sliderDelay->value())));

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraReadoutMode(uiSiphraReadoutModeValue());
}

void CubesControl::on_sliderDelay_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->spinboxDelay->setValue(value);

    /* Update TH delay time label */
    ui->lblThDelayTime->setText(QString::number(thDelayTime(ui->sliderTune->value(),
                                                            ui->sliderDelay->value())));

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraReadoutMode(uiSiphraReadoutModeValue());
}

void CubesControl::on_spinboxDelay_valueChanged(int value)
{
    /*
     * This widget has been changed by another widget, not the user. In this case,
     * no further operation is needed, clear the visual reg change notification
     * member and exit.
     */
    if (siphraVisualRegChange) {
        siphraVisualRegChange = false;
        return;
    }

    /*
     * If we've made it up to here, the user has initiated the value change.
     * Update the other widget, set the visual reg change member to notice the
     * other widget's value/indexChanged slot and issue the register operation
     * before exiting.
     */
    siphraVisualRegChange = true;
    ui->sliderDelay->setValue(value);

    /* Update TH delay time label */
    ui->lblThDelayTime->setText(QString::number(thDelayTime(ui->sliderTune->value(),
                                                            ui->sliderDelay->value())));

    /*
     * The value of the control can also be changed by a read all regs command;
     * in this case, no writing of the channel should be made
     */
    if (!m_readAllSiphraRegs)
        writeSiphraReadoutMode(uiSiphraReadoutModeValue());
}

void CubesControl::on_spinboxSiphraChannelToConfig_valueChanged(int value)
{
    m_siphraRegAddr = value-1;
    on_actionReadSiphraReg_triggered();
}

void CubesControl::on_tmrEventRateReadout_timeout()
{
    if (!connStatus) {
        return;
    }

    QByteArray dummy;
    qint8 cmd = CMD_GET_CH01_REGS + ui->spinboxSiphraChannelToConfig->value()-1;
    cubes->sendCommand(cmd, dummy);
}

void CubesControl::on_comboboxCbInput_currentIndexChanged(int index)
{
    if (!m_readAllSiphraRegs)
        writeSiphraChannelControl(uiSiphraChannelControlValue());
}

void CubesControl::on_comboboxHoldSource_currentIndexChanged(int index)
{
    if (!m_readAllSiphraRegs)
        writeSiphraReadoutMode(uiSiphraReadoutModeValue());
}

void CubesControl::on_checkboxCiCompmode_clicked()
{
    if (!m_readAllSiphraRegs)
        writeSiphraChannelConfig(uiSiphraChannelConfigValue());
}

void CubesControl::on_checkboxUseAdcIn_clicked(bool checked)
{
    /* Warn user about what checking the box would entail */
    if (checked) {
        QMessageBox warn;
        QString text;
        QTextStream s(&text);
        s << "Due to current limitations, please make sure you clear all"
          << "other bits in READOUT_FIXED_LIST register."
          << "\n\n"
          << "This can be done easiest from the SIPHRA register map tab.";
        warn.setWindowTitle("Warning");
        warn.setText(text);
        warn.exec();
    }

    /* Apply ADC_IN bit to current value of READOUT_FIXED_LIST register */
    m_siphraRegAddr = 0x17;

    SiphraTreeWidgetItem *reg =
            (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(m_siphraRegAddr);
    m_siphraRegVal = reg->registerValue();

     if (checked)
        m_siphraRegVal |= (1 << 18);
    else
        m_siphraRegVal &= ~(1 << 18);

    on_actionWriteSiphraReg_triggered();

    /* Apply EN_SPI_FORCED_START bit to current value of READOUT_MODE register */
    m_siphraRegAddr = 0x18;
    reg = (SiphraTreeWidgetItem *)ui->treeSiphraRegMap->topLevelItem(m_siphraRegAddr);
    m_siphraRegVal = reg->registerValue();

    if (checked)
       m_siphraRegVal |= (1 << 2);
   else
       m_siphraRegVal &= ~(1 << 2);

    on_actionWriteSiphraReg_triggered();
}

/*============================================================================
 * UI CONTAINER SLOTS
 *============================================================================*/
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
    if (index == 1) {
        m_siphraRegAddr = 0x00;
        m_readAllSiphraRegs = true;
        on_actionReadSiphraReg_triggered();
        tmrEventRateReadout->start();
    } else {
        tmrEventRateReadout->stop();
    }

    /*
     * Avoid redundant calling of the histogram ADC poll timer callback. This
     * should however only be done for non-time-based histogramming. For
     * time-based histogramming, the timer should run, so the histogramming
     * finishes at the right time.
     *
     * The timer callback makes sure no transfers are performed if the user is
     * not looking at the histogram tab, so the transfer medium to the device
     * talking to the SIHPRA doesn't get redundantly used up.
     */
    if (ui->comboboxHistogramRunType->currentIndex() == 0) {
        if (index == 2) {
            if (histogramAdcPollEnabled)
                tmrHistogramAdcPoll->start();
        } else {
            if (histogramAdcPollEnabled)
                tmrHistogramAdcPoll->stop();
        }
    }

    /* Avoid redundant calling of the ADC poll's timer callback (diagnostics tab). */
    if (index == 3) {
        if (m_siphraAdcPollEnabled)
            tmrSiphraAdcPoll->start();
    } else {
        if (m_siphraAdcPollEnabled)
            tmrSiphraAdcPoll->stop();
    }
}

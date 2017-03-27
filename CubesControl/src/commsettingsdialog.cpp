/*
 *==============================================================================
 * Source file for Communication Settings Dialog box
 *==============================================================================
 *
 * author: Theodor Stana (theodor.stana@gmail.com)
 *
 * date of creation: 2017-03-24
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
 *    2017-03-24   Theodor Stana     File created
 *==============================================================================
 * TODO: -
 *==============================================================================
 */

#include <commsettingsdialog.h>
#include <ui_commsettingsdialog.h>

CommSettingsDialog::CommSettingsDialog(QWidget *parent) :
    QDialog{parent},
    ui{new Ui::CommSettingsDialog}
{
    ui->setupUi(this);

    connect(ui->btnOk, &QPushButton::clicked,
            this, &CommSettingsDialog::on_btnOk_clicked);
    connect(ui->btnCancel, &QPushButton::clicked,
            this, &CommSettingsDialog::on_btnCancel_clicked);
    connect(ui->cbCommType,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CommSettingsDialog::on_cbCommType_currentIndexChanged);

    vboxCommSettings = new QVBoxLayout;

    ui->btnOk->setEnabled(false);
    ui->btnCancel->setEnabled(false);

    addCommTypes();
}

CommSettingsDialog::~CommSettingsDialog()
{
    delete m_commSettings;
    delete ui;
    delete vboxCommSettings;
}

void CommSettingsDialog::addCommTypes()
{
    ui->cbCommType->addItem("<Select...>");

    ui->cbCommType->addItem("Serial Port");
}

void CommSettingsDialog::on_btnOk_clicked()
{
    // updateSettings();
    hide();
}

void CommSettingsDialog::on_btnCancel_clicked()
{
    hide();
}

void CommSettingsDialog::on_cbCommType_currentIndexChanged(int index)
{
    populateCommSettings(index);
}

void CommSettingsDialog::populateCommSettings(int commType)
{
    if ((int)m_commType == commType)
        return;

    m_commType = (enum CommType)commType;

    switch (commType) {
    case None:
    {
        clearLayout(vboxCommSettings);
        ui->btnOk->setEnabled(false);
        ui->btnCancel->setEnabled(false);
        break;
    }
    case SerialPort:
    {
        QHBoxLayout *hboxPort = new QHBoxLayout;
        QLabel *lblPort = new QLabel(tr("Port:"));
        QComboBox *cbPort = new QComboBox;
        cbPort->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        hboxPort->addWidget(lblPort);
        hboxPort->addWidget(cbPort);
        vboxCommSettings->addLayout(hboxPort);
        ui->groupCommSettings->setLayout(vboxCommSettings);
        break;
    }
    default:
        break;
    }
}

void CommSettingsDialog::clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

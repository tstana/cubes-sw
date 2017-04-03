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

#include <cubescommands.h>


CubesControl::CubesControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CubesControl),
    commSettings(new CommSettingsDialog)
{
    ui->setupUi(this);

    lblConnStatus = new QLabel;
    lblConnStatus->setFixedWidth(150);
    lblConnStatus->setFixedHeight(13);
    lblConnStatus->setIndent(5);
    lblConnStatus->setStyleSheet("QLabel { font-weight : bold; background-color : red; border : 1px solid black }");
    lblConnStatus->setText("Not connected");
    statusBar()->addPermanentWidget(lblConnStatus);

    connect(ui->actionConfigConnection, &QAction::triggered,
            commSettings, &CubesControl::show);

//itzi was here :*
}

CubesControl::~CubesControl()
{
    delete ui;
    delete commSettings;
}

void CubesControl::on_actionClose_triggered()
{
    this->close();
}

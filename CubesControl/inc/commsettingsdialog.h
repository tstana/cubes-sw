/*
 *==============================================================================
 * Header file for Communication Settings Dialog box
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

#ifndef COMMSETTINGSDIALOG_H
#define COMMSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class CommSettingsDialog;
}

class CommSettingsDialog : public QDialog
{
public:
    enum CommType {
        SerialPort
    };

    struct CommSettings {
        CommType type;
        qint32 port;
    };

    explicit CommSettingsDialog(QWidget *parent = 0);
    ~CommSettingsDialog();

private:
    Ui::CommSettingsDialog *ui;
    CommSettings *settings;
};
#endif // COMMSETTINGSDIALOG_H

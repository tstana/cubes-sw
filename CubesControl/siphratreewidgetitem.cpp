/*
 *==============================================================================
 * Source file for SIPHRA Tree Widget Item class
 *==============================================================================
 *
 * author: Theodor Stana (theodor.stana@gmail.com)
 *
 * date of creation: 2017-04-11
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
 *    2017-04-11   Theodor Stana     File created
 *==============================================================================
 * TODO: -
 *==============================================================================
 */

#include "siphratreewidgetitem.h"

#include <QTreeWidgetItem>

SiphraTreeWidgetItem::SiphraTreeWidgetItem(QTreeWidget *parent,
                                           qint8 registerAddress,
                                           bool registerView) :
    QTreeWidgetItem(parent)
{
    QStringList itemStrings;

    /*
     * If we display a register view, there are three columns in the QTreeWidget:
     *      - Offset
     *      - Name
     *      - Value
     */
    if (registerView) {
        switch (registerAddress) {
        case SIPHRA_CTRL_CHAN_01:
        case SIPHRA_CTRL_CHAN_02:
        case SIPHRA_CTRL_CHAN_03:
        case SIPHRA_CTRL_CHAN_04:
        case SIPHRA_CTRL_CHAN_05:
        case SIPHRA_CTRL_CHAN_06:
        case SIPHRA_CTRL_CHAN_07:
        case SIPHRA_CTRL_CHAN_08:
        case SIPHRA_CTRL_CHAN_09:
        case SIPHRA_CTRL_CHAN_10:
        case SIPHRA_CTRL_CHAN_11:
        case SIPHRA_CTRL_CHAN_12:
        case SIPHRA_CTRL_CHAN_13:
        case SIPHRA_CTRL_CHAN_14:
        case SIPHRA_CTRL_CHAN_15:
        case SIPHRA_CTRL_CHAN_16:
            setText(0, "0x" + QString::number(registerAddress, 16).rightJustified(2, QLatin1Char('0')));
            setText(1, "CTRL_CH_" + QString::number(registerAddress+1).rightJustified(2, QLatin1Char('0')));
            setText(2, "");

            for (int i = 0; i < 8; i++) {
                insertChild(i, new QTreeWidgetItem(this));
                child(i)->setTextAlignment(0, Qt::AlignCenter);
                child(i)->setTextAlignment(2, Qt::AlignRight);
                child(i)->setText(2, "");
            }
            child(0)->setText(0, "25:18");
            child(0)->setText(1, "cmis_detector_voffset");
            child(1)->setText(0, "17:15");
            child(1)->setText(1, "cmis_detector_ioffset");
            child(2)->setText(0, "14");
            child(2)->setText(1, "cmis_impedance_reduction");
            child(3)->setText(0, "13");
            child(3)->setText(1, "cal_select_channel");
            child(4)->setText(0, "12:5");
            child(4)->setText(1, "qc_threshold");
            child(5)->setText(0, "4:2");
            child(5)->setText(1, "qc_hysteresis");
            child(6)->setText(0, "1");
            child(6)->setText(1, "powerup_channel");
            child(7)->setText(0, "0");
            child(7)->setText(1, "enable_triggering");
            break;
        default:
            break;
        }
    }
}

SiphraTreeWidgetItem::~SiphraTreeWidgetItem()
{

}


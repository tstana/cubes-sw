/*
 *==============================================================================
 * Header file for SIPHRA Tree Widget Item class
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

#ifndef SIPHRATREEWIDGETITEM_H
#define SIPHRATREEWIDGETITEM_H

#include <QObject>
#include <QWidget>
#include <QTreeWidget>

class SiphraTreeWidgetItem : public QTreeWidgetItem
{
public:
    SiphraTreeWidgetItem();
    SiphraTreeWidgetItem(QTreeWidget *parent, qint8 registerAddress, bool registerView);
    ~SiphraTreeWidgetItem();

    void        setRegisterValue(qint32 value);
    void        setRegisterValue(qint8 bitFieldIndex, qint32 value);
    qint32      registerValue();
    qint32      registerValue(qint8 bitFieldIndex);
    qint8       registerAddress();
    QString&    registerName();

private:
    qint8   m_registerAddress;
    bool    m_registerView;
    QString m_registerName;

    void    updateRegisterValue();
    void    updateRegisterBitFields();
};

#define SIPHRA_CTRL_CH_01               0x00
#define SIPHRA_CTRL_CH_02               0x01
#define SIPHRA_CTRL_CH_03               0x02
#define SIPHRA_CTRL_CH_04               0x03
#define SIPHRA_CTRL_CH_05               0x04
#define SIPHRA_CTRL_CH_06               0x05
#define SIPHRA_CTRL_CH_07               0x06
#define SIPHRA_CTRL_CH_08               0x07
#define SIPHRA_CTRL_CH_09               0x08
#define SIPHRA_CTRL_CH_10               0x09
#define SIPHRA_CTRL_CH_11               0x0a
#define SIPHRA_CTRL_CH_12               0x0b
#define SIPHRA_CTRL_CH_13               0x0c
#define SIPHRA_CTRL_CH_14               0x0d
#define SIPHRA_CTRL_CH_15               0x0e
#define SIPHRA_CTRL_CH_16               0x0f
#define SIPHRA_CTRL_CH_SUM              0x10
#define SIPHRA_CHANNEL_CONFIG           0x11
#define SIPHRA_CHANNEL_CONTROL          0x12
#define SIPHRA_ADC_CONFIG               0x13
#define SIPHRA_CAL_DAC                  0x14
#define SIPHRA_POWER_MODULES            0x15
#define SIPHRA_CAL_CTRL                 0x16
#define SIPHRA_READOUT_FIXED_LIST       0x17
#define SIPHRA_READOUT_MODE             0x18
#define SIPHRA_AMUX_CTRL                0x19
#define SIPHRA_ADC_CLK_DIV_FACTOR       0x1a
#define SIPHRA_SYSCLOCK_CTRL            0x1b
#define SIPHRA_CMD_DCAL                 0x1c
#define SIPHRA_CMD_READOUT              0x1d
#define SIPHRA_TRIGGER_LATCHES          0x1e
#define SIPHRA_ADC_OUT                  0x1f
#define SIPHRA_PARITY_ERR_REG           0x20

#endif // SIPHRATREEWIDGETITEM_H

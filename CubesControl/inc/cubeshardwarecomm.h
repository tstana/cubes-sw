/*
 *==============================================================================
 * KTH Royal Institute of Technology Stockholm
 * CUBES Hardware Level
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

#ifndef CUBESHARDWARECOMM_H
#define CUBESHARDWARECOMM_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QLabel>

class CubesHardwareComm : public QObject
{
    Q_OBJECT

public:
    explicit CubesHardwareComm(QSerialPort *port, QLabel *lblMessages, QObject *parent = 0);
    ~CubesHardwareComm();

    void write(const QByteArray &writeData);

private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort		*m_port;
    QLabel			*m_lblMessages;
    char			m_writeData;
    QTimer			m_timer;
};

#endif // CUBESHARDWARECOMM_H

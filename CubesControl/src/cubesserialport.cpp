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

#include "cubesserialport.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>

CubesSerialPort::CubesSerialPort(QSerialPort *port, QObject *parent)
    : QObject(parent)
    , m_port(port)
{
//    QObject::connect( m_port, &QSerialPort::readyRead, this, &CubesSerialPort::handleReadyRead);
//    QObject::connect( m_port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
//                      this, &CubesSerialPort::handleError );
}

CubesSerialPort::~CubesSerialPort()
{
}

inline void CubesSerialPort::write(const QByteArray &writeData)
{
    m_port->write(writeData);
}

//void CubesSerialPort::handleReadyRead()
//{
//}

//void CubesSerialPort::handleError(QSerialPort::SerialPortError error)
//{
//}

//void CubesSerialPort::handleTimeout()
//{
//}

/*==============================================================================
 * KTH Royal Institute of Technology Stockholm
 * CUBES Protocol UART Pmod implementation
 *==============================================================================
 *
 * author: Theodor Stana (theodor.stana@gmail.com)
 *
 * date of creation: 2017-02-15
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
 *    2017-02-15   theodor.stana@gmail.com     File created
 *==============================================================================
 * TODO: -
 *==============================================================================
 */

#include <cubesprotouartpmod.h>

#include <QObject>
#include <QSerialPort>

CubesProtoUartPmod::CubesProtoUartPmod(QSerialPort *device, QObject *parent) :
    m_device{device}
{
    QObject::connect(m_device, &QSerialPort::readyRead, this, &CubesProtoUartPmod::devReadReady);
    QObject::connect(m_device, &QSerialPort::errorOccurred, this, &CubesProtoUartPmod::devErrorOccured);
}

CubesProtoUartPmod::~CubesProtoUartPmod()
{
    delete m_device;
}

bool CubesProtoUartPmod::devOpen(QSerialPort::OpenMode mode)
{
    return m_device->open(mode);
}

void CubesProtoUartPmod::devClose()
{
    m_device->close();
}

QString CubesProtoUartPmod::devName()
{
    return m_device->portName();
}

int CubesProtoUartPmod::devError()
{
    return m_device->error();
}

qint64 CubesProtoUartPmod::write(QByteArray &data)
{
    return m_device->write(data);
}

QByteArray CubesProtoUartPmod::readAll()
{
    return m_device->readAll();
}

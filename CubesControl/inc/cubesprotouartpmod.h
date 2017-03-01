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
#ifndef CUBESPROTOUARTPMOD_H
#define CUBESPROTOUARTPMOD_H

#include <QObject>
#include <QSerialPort>

#define CUBES_I2C_ADDRESS       0x70

class CubesProtoUartPmod : public QObject //, public ICubesProtocol
{
    Q_OBJECT

public:
    CubesProtoUartPmod(QSerialPort *device, QObject *parent = 0);
    ~CubesProtoUartPmod();

    bool        devOpen(QSerialPort::OpenMode mode);
    void        devClose();
    int         devError();
    QString     devName();

    qint64      write(QByteArray &data);
    QByteArray  readAll();

    qint64      sendCommand(unsigned char cmd, QByteArray &cmdData);

private slots:
    void on_serialPort_readReady();

signals:
    void devReadReady();
    void devErrorOccured(int error);
    void dataReceived();

private:
    QSerialPort     *m_device;

    qint32         m_bytesLeft;
};

#endif // CUBESPROTOUARTPMOD_H

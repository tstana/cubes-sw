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

#include <cubescommand.h>

#include <QObject>
#include <QSerialPort>

#define CUBES_I2C_ADDRESS       0x70
#define CUBES_NL_MTU            507  // Size in bytes
#define CUBES_NL_HEADER_SIZE    5 // Is 9 bytes in MIST . But 4 bytes of FCS are gazoomped for now!!

class CubesProtoUartPmod : public QObject //, public ICubesProtocol
{
    Q_OBJECT

public:
    CubesProtoUartPmod(QSerialPort *device, QObject *parent = 0);
    ~CubesProtoUartPmod();

    bool            devOpen(QSerialPort::OpenMode mode);
    void            devClose();
    int             devError();
    QString         devName();
    QSerialPort*    dev();

    CubesCommand*   currentCommand();

    void            setLEDs(quint8 leds);
    QByteArray      getLEDs(QByteArray &status);
    void            setRegValues(quint8 reg);
    QByteArray      getRegValues(QByteArray &data, quint8 reg);

    qint64          write(QByteArray &data);
    QByteArray      readAll();

    qint64          sendCommand(quint8 cmd, QByteArray &cmdData);
private slots:
    void on_serialPort_readReady();

signals:
    void devReadReady();
    void devErrorOccured(int error);

private:

    qint8           requestNL(quint8 cmdCode, QByteArray &datarray);
    qint8           sendNL(quint8 cmd, QByteArray &cmdData);

    QByteArray      readI2C();
    qint64          writeI2C(QByteArray &data);

    QSerialPort     *m_device;

    CubesCommand    *m_currentCommand;

    qint32          m_bytesLeft;
};

#endif // CUBESPROTOUARTPMOD_H

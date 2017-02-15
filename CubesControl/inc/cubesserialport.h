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

#ifndef CUBESHARDWARESERIAL_H
#define CUBESHARDWARESERIAL_H

#include <QObject>
#include <QIODevice>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QLabel>

class CubesSerialPort : public QIODevice
{
    Q_OBJECT

public:
    CubesSerialPort(QSerialPort *port);
    ~CubesSerialPort();

    inline bool open(QIODevice::OpenMode mode)
    {
        return m_port->open(mode);
    }

    inline void close()
    {
        return m_port->close();
    }

    inline QString portName()
    {
        return m_port->portName();
    }

    inline int error()
    {
        return m_port->error();
    }

    inline void write(const QByteArray &writeData)
    {
        m_port->write(writeData);
    }

    inline QByteArray read(qint64 maxSize)
    {
        return m_port->read(maxSize);
    }

    inline QByteArray readAll()
    {
        return m_port->readAll();
    }

    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

signals:
    void errorOccured(int error);

private:

    QSerialPort *m_port;
};
#endif // CUBESHARDWARESERIAL_H

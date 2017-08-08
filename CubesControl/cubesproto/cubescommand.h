/*
 *==============================================================================
 * Header file for CUBES commands
 *==============================================================================
 *
 * author: Theodor Stana (theodor.stana@gmail.com)
 *
 * date of creation: 2017-02-28
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
 *    2017-02-28   Theodor Stana     File created
 *==============================================================================
 * TODO: -
 *==============================================================================
 */

#ifndef __CUBESCOMMANDS_H_
#define __CUBESCOMMANDS_H_

#include <QString>

/*-----------------------------------------------------------------------------
 * Class definition
 *---------------------------------------------------------------------------*/
class CubesCommand
{
public:

    /* Data direction is same as I2C R/nW bit: '1' = Read, '0' = Write. */
    typedef enum Direction {
        Write = 0,
        Read = 1
    } DataDirection;

    CubesCommand();
    CubesCommand(const quint8 code);

    quint8      setCommand(const quint8 code);

    quint8      code();
    qint32      dataBytes();
    qint32      dataDirection();
    QString&    description();

private:
    quint8      m_code;
    qint32      m_dataBytes;
    qint32      m_dataDirection;
    QString     m_description;

    quint8      decode(const quint8 code);
};


/*-----------------------------------------------------------------------------
 * Command definitions -- Changed to OPCODE definitions for compatibility with
 * MIST on 24 July 2017
 * Thus -- cannot use any CMD with value > 0x7F as 1 bit is used for Frame ID
 *---------------------------------------------------------------------------*/

#define CMD_READ_ALL_REGS           0x61
#define CMD_GET_CUBES_ID            0x40
#define CMD_SET_LEDS                0x41
#define CMD_GET_LEDS                0x42
#define CMD_SIPHRA_REG_OP           0x43
#define CMD_GET_SIPHRA_DATAR        0x44
#define CMD_GET_SIPHRA_ADCR         0x45
#define CMD_GET_CH01_REGS           0x50
#define CMD_GET_CH02_REGS           0x51
#define CMD_GET_CH03_REGS           0x52
#define CMD_GET_CH04_REGS           0x53
#define CMD_GET_CH05_REGS           0x54
#define CMD_GET_CH06_REGS           0x55
#define CMD_GET_CH07_REGS           0x56
#define CMD_GET_CH08_REGS           0x57
#define CMD_GET_CH09_REGS           0x58
#define CMD_GET_CH10_REGS           0x59
#define CMD_GET_CH11_REGS           0x5a
#define CMD_GET_CH12_REGS           0x5b
#define CMD_GET_CH13_REGS           0x5c
#define CMD_GET_CH14_REGS           0x5d
#define CMD_GET_CH15_REGS           0x5e
#define CMD_GET_CH16_REGS           0x5f
#define CMD_NULL_FRAME              0x00
#define CMD_DATA_FRAME              0x01
#define CMD_F_ACK                   0x02
#define CMD_T_ACK                   0x03
#define CMD_REQ_PAYLOAD             0x10
#define CMD_REQ_HK                  0x11
#define CMD_REQ_PUS                 0x12
#define CMD_EXP_SEND                0x20
#define CMD_SEND_TIME               0x30
#define CMD_SEND_PUS                0x31
#define CMD_ACTIVE                  0x70
#define CMD_SLEEP                   0x71
#define CMD_POWER_OFF               0x72
#endif // __CUBESCOMMANDS_H_

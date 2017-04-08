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

    typedef enum Direction {
        Read = 0,
        Write = 1
    } DataDirection;

    CubesCommand();
    CubesCommand(const unsigned char code);

    unsigned char   setCommand(const unsigned char code);

    unsigned char   code();
    qint32          dataBytes();
    qint32          dataDirection();
    QString&        description();

private:
    unsigned char   m_code;
    qint32          m_dataBytes;
    qint32          m_dataDirection;
    QString         m_description;

    unsigned char   decode(const unsigned char code);
};


/*-----------------------------------------------------------------------------
 * Command definitions
 *---------------------------------------------------------------------------*/

#define CMD_READ_ALL_REGS           0x11
#define CMD_GET_CUBES_ID            0x90
#define CMD_SET_LEDS                0x91
#define CMD_GET_LEDS                0x92
#define CMD_SIPHRA_REG_OP           0x93

#endif // __CUBESCOMMANDS_H_

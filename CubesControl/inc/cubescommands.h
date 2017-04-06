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
class CubesCommands
{
public:
    CubesCommands() { }

    CubesCommands(const unsigned char code, const QString &desc) :
        code{code},
        description{desc}
    {
    }

    unsigned char   code;
    QString         description;
};

/*-----------------------------------------------------------------------------
 * Command definitions
 *---------------------------------------------------------------------------*/

/* 1. Command code definitions */

#define CMD_GET_HOUSEKEEPING_DATA   0x11
#define CMD_GET_CUBES_ID            0x90
#define CMD_SET_LEDS                0x91
#define CMD_GET_LEDS                0x92
#define CMD_SIPHRA_REG_OP           0x93

/* 2. Associating command codes to command descriptions */

#define NUM_COMMANDS        5

const CubesCommands COMMANDS[NUM_COMMANDS] {
    { CMD_GET_HOUSEKEEPING_DATA,    "Request housekeeping data" },
    { CMD_GET_CUBES_ID,             "Get ID" },
    { CMD_SET_LEDS,                 "Set LED reg." },
    { CMD_GET_LEDS,                 "Get LED reg." },
    { CMD_SIPHRA_REG_OP,            "SIPHRA register operation" }
};

#endif // __CUBESCOMMANDS_H_

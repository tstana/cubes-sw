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
#define NUM_COMMANDS        3

const CubesCommands COMMANDS[NUM_COMMANDS] {
    { 0x90, "Get ID" },
    { 0x91, "Set LED reg." },
    { 0x92, "Get LED reg." }
};

#endif // __CUBESCOMMANDS_H_

/*==============================================================================
 * KTH Royal Institute of Technology Stockholm
 * CUBES Protocol interface
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

#ifndef ICUBESPROTOCOL_H
#define ICUBESPROTOCOL_H

#include <QObject>
#include <QIODevice>

class ICubesProtocol
{
public:
    virtual ~ICubesProtocol() {};

    virtual bool        devOpen(int mode) = 0;
    virtual void        devClose() = 0;
    virtual int         devError() = 0;
    virtual QString     devName() = 0;

    virtual qint64      write(QByteArray &data) = 0;
    virtual QByteArray  readAll() = 0;

/* Connect signals and slots as part of the implementing class */
signals:
    virtual void devReadReady() = 0;
    virtual void devErrorOccured(int error) = 0;

};

Q_DECLARE_INTERFACE(ICubesProtocol, "ICubesProtocol")

#endif // ICUBESPROTOCOL_H

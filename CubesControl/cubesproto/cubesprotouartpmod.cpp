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

#include "cubesprotouartpmod.h"

#include <cubescommand.h>

#include <QObject>
#include <QSerialPort>

CubesProtoUartPmod::CubesProtoUartPmod(QSerialPort *device, QObject *parent) :
    QObject(parent),
    m_device{device}
{
    QObject::connect(m_device, &QSerialPort::readyRead,
                     this, &CubesProtoUartPmod::on_serialPort_readReady);
    QObject::connect(m_device, &QSerialPort::errorOccurred,
                     this, &CubesProtoUartPmod::devErrorOccured);

    connect(m_device, &QSerialPort::readyRead,
            this, &CubesProtoUartPmod::read_NL);

    ready = new QState();
    runreq = new QState();
    runsend = new QState();
    done = new QFinalState();

    reqack = new QState(runreq);
    rx = new QState(runreq);
    resp = new QState(runreq);
    runreq->setInitialState(resp);

    tx = new QState(runsend);
    sendack = new QState(runsend);
    runsend->setInitialState(sendack);

    ready->addTransition(this, SIGNAL(obc_tx_SEND()), runsend);
    ready->addTransition(this,SIGNAL(obc_tx_REQ()), runreq);
    ready->addTransition(this,SIGNAL(dev_close()), done);

    runsend->addTransition(this,SIGNAL(exp_rx_UNEX()),ready);
    runreq->addTransition(this,SIGNAL(exp_rx_UNEX()),ready);

    tx->addTransition(this,SIGNAL(obc_tx_data()),sendack);
    sendack->addTransition(this,SIGNAL(exp_rx_FACK()),tx);
    sendack->addTransition(this,SIGNAL(exp_rx_TACK()),ready);
    /* lasfri TODO - check if better to abstract runsend and have a done state
     * in it and transit to that state in above, and then transit to ready on
     * finished() - see http://doc.qt.io/qt-5/statemachine-api.html */

    rx->addTransition(this,SIGNAL(exp_rx_data()),reqack);
    reqack->addTransition(this,SIGNAL(obc_tx_TACK()),ready);
    reqack->addTransition(this,SIGNAL(obc_tx_FACK()),rx);
    resp->addTransition(this,SIGNAL(exp_rx_EXPS()),reqack);

    connect(tx,SIGNAL(entered()),this,SLOT(write_NL()));
    connect(reqack,SIGNAL(entered()),this,SLOT(write_NL()));

    machine = new QStateMachine();
    machine->addState(ready);
    machine->addState(runreq);
    machine->addState(runsend);
    machine->addState(done);
    machine->setInitialState(ready);
    machine->start();


    m_currentCommand = new CubesCommand;
    qsrand(432223); // initiate the random number generator
}

CubesProtoUartPmod::~CubesProtoUartPmod()
{
    delete m_device;
    delete m_currentCommand;
    delete ready;
    delete runsend;
    delete runreq;
    delete resp;
    delete reqack;
    delete rx;
    delete sendack;
    delete tx;
    delete done;

}

quint32 CubesProtoUartPmod::getdatalength()
{
    auto s = machine->configuration();
    if (s.contains(ready) | s.contains(reqack) | s.contains(resp) | s.contains(sendack)){
        return CUBES_NL_HEADER_SIZE;
    }
    if (s.contains(tx) | s.contains(rx))
    {
        return m_currfragsize;
    }
    return 0; // code should ideally never hit this point
}

void CubesProtoUartPmod::write_NL()
{
    auto s = machine->configuration();
    quint16 hdrcmd;
    if (s.contains(ready))
    {
        hdrcmd = m_header[0] & 0x7F;
        if ((hdrcmd == CMD_SET_LEDS) || (hdrcmd == CMD_SEND_PUS) || (hdrcmd == CMD_SEND_TIME))
        {
            emit obc_tx_SEND();
            writeI2C(m_header,true);
            writeI2C(m_header,false); // second write is for next frame rx from exp
            return;
        }
        if ((hdrcmd == CMD_REQ_HK))
        {
            emit obc_tx_REQ();
            writeI2C(m_header,true);
            writeI2C(m_header,false);
            return;
        }

     }
    if (s.contains(tx))
    {
        QByteArray cdata;
        quint32 spos = m_spos;
        m_currfragsize = m_datasize - m_spos;
        if (m_currfragsize > CUBES_NL_MTU)
        {
            m_currfragsize = CUBES_NL_MTU;
            m_spos = m_spos + CUBES_NL_MTU;
        }
        else
            m_spos = m_datasize; // setting it at last position
        cdata.resize(m_currfragsize + 1); // for header and no fcs
        if (spos == 0)
        {
            m_fid = ~m_tfid; // flip of tfid
        }
        else
        {
            m_fid = ~m_fid; // bit flip
        }

        cdata[0] = (m_fid << 7) | CMD_DATA_FRAME;
        for(quint32 i=1;i<=m_currfragsize;i++)
        {
            cdata[i] = m_data[spos + i - 1];
        }
        writeI2C(cdata,true);
        writeI2C(cdata,false);
        emit obc_tx_data();
        return;
    }

    if (s.contains(reqack))
    {

    }

    // Unexpected case - Write NULL frame and return to ready state and inform UI
    m_header.resize(CUBES_NL_HEADER_SIZE);
    m_header[0] = (m_tfid << 7) | CMD_NULL_FRAME;
    m_header[1] = 0x00;
    m_header[2] = 0x00;
    m_header[3] = 0x00;
    m_header[4] = 0x00;
    writeI2C(m_header,true);
    emit exp_rx_UNEX();
    emit devErrorOccured(15);
    return;
}

void CubesProtoUartPmod::read_NL()
{
    QByteArray data;
    quint16 opcode;
    quint32 readlen = getdatalength();
    quint8 fid;
    data.resize(readlen);
    data = m_device->read(readlen); // Not using readall !!
    auto s = machine->configuration();

    opcode = data[0] & 0x7F;
    fid = ((data[0] & 0x80) >> 7); // fid check not implemented yet - lasfri TODO
    switch (opcode) {
    case CMD_F_ACK:
        if (s.contains(sendack))
        {
            if (fid != m_tfid)
            {
                m_spos = (m_spos > CUBES_NL_MTU) ? m_spos - CUBES_NL_MTU : 0 ; // to resend the previous data mtu
            }
            emit exp_rx_FACK();
            return;
        }

        break;
    case CMD_T_ACK:
        if (s.contains(sendack))
        {
            emit exp_rx_TACK();
            return;
        }
    case CMD_EXP_SEND:
        if (s.contains(resp))
        {
            m_datasize = (data[1] >> 24) + (data[2] >> 16) + (data[3] >> 8) + data[4];
            m_fid = fid;
            emit exp_rx_EXPS();
            return;
        }
    case CMD_DATA_FRAME:
        if (s.contains(rx))
        {

        }
    default:
        break;
    }
    //Unexpected operation
    m_header.resize(CUBES_NL_HEADER_SIZE);
    m_header[0] = (m_tfid << 7) | CMD_NULL_FRAME;
    m_header[1] = 0x00;
    m_header[2] = 0x00;
    m_header[3] = 0x00;
    m_header[4] = 0x00;
    writeI2C(m_header,true);
    emit exp_rx_UNEX();
    emit devErrorOccured(15);
    return;
}



QSerialPort* CubesProtoUartPmod::dev()
{
    return m_device;
}

bool CubesProtoUartPmod::devOpen(QSerialPort::OpenMode mode)
{
    return m_device->open(mode);
}

void CubesProtoUartPmod::devClose()
{
    emit dev_close();
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

void CubesProtoUartPmod::on_serialPort_readReady()
{
    m_bytesLeft -= m_device->bytesAvailable();
    if (m_bytesLeft <= 0) {
        emit devReadReady();
    }
}

void CubesProtoUartPmod::setLEDs(quint8 leds)
{
    m_data.resize(1);
    m_data[0] = leds;
    m_datasize = m_data.size();
    m_spos = 0;

    m_tfid = qrand()%2; // generate random tfid 0 or 1.
    m_header.resize(CUBES_NL_HEADER_SIZE);
    m_header[0] = (m_tfid << 7) | CMD_SET_LEDS;
    m_header[1] = (m_datasize >> 24) & 0xff;
    m_header[2] = (m_datasize >> 16) & 0xff;
    m_header[3] = (m_datasize >> 8) & 0xff;
    m_header[4] = m_datasize & 0xff;

    m_currentCommand->setCommand(CMD_SET_LEDS);

    write_NL();
}

QByteArray CubesProtoUartPmod::getLEDs(QByteArray &status)
{
    qint8 stat;
    stat = requestNL(CMD_GET_LEDS, status);
    if (stat == -1){
        emit devErrorOccured(13);
    }
    return status;
}

void CubesProtoUartPmod::setRegValues(quint8 reg){

    QByteArray data;
    data.resize(8);
    for (int i = 0; i < 7; i++) {
        data [i] = 0x00;
    }
    data[7] = (reg << 1);

    if(sendNL(CMD_SIPHRA_REG_OP,data)){
        emit devErrorOccured(13);
    }

}

QByteArray CubesProtoUartPmod::getRegValues(QByteArray &data, quint8 reg)
{
    qint8 status;

    status = requestNL(CMD_GET_CH15_REGS, data);
    if (status == -1){
        emit devErrorOccured(13);
    }
    return data;
}

qint64 CubesProtoUartPmod::writeI2C(QByteArray &cmdData)
{
    QByteArray      data;
    int             numDataBytes;
    char            read, write;



    /*
     * Create I2C R/nW ('1' = Read, '0' = Write) bit based on current command's
     * data direction, and assign number of data bytes and I2C Address + R/nW
     * byte to mimic an I2C transfer over UART.
     */
    write = 0x1 & (~m_currentCommand->dataDirection());
    read = 0x1 & (~write);
    m_bytesLeft = m_currentCommand->dataBytes();

    numDataBytes = 1 + (cmdData.size() * write);
    data.resize(numDataBytes);

    data[0] = (CUBES_I2C_ADDRESS << 1) | (read);
    for (int i = 0; i < numDataBytes-1; i++)
        data[i+1] = cmdData[i];

    /* Clear data buffers and issue command */
    if(data.length() != m_device->write(data)){
        return -1;
    }
    return 0;

}

/* Overloading function so that I don't break functionality - lasfri
 * TODO - delete one after code works */
qint64 CubesProtoUartPmod::writeI2C(QByteArray &cmdData, bool write)
{
    QByteArray      data;
    int             numDataBytes;


    numDataBytes = 1 + (cmdData.size() * write);
    data.resize(numDataBytes);

    data[0] = write ? (CUBES_I2C_ADDRESS << 1) :
                      ((CUBES_I2C_ADDRESS << 1) | 0x01);
    for (int i = 0; i < numDataBytes-1; i++)
        data[i+1] = cmdData[i];

    /* Clear data buffers and issue command */
    if(data.length() != m_device->write(data)){
        return -1;
    }
    return 0;

}



CubesCommand* CubesProtoUartPmod::currentCommand()
{
    return m_currentCommand;
}

qint8 CubesProtoUartPmod::sendNL(quint8 cmdCode, QByteArray &data)
/* It is extremely unlikely that the OBC will send more than
 CUBES_NL_MTU bytes to the EXP. However, the provision to
 to do so is kept irrespective of this - lasfri */
{
    QByteArray frag, headerFrame, rechdrFrame;
    //Note : This places limit on max numfrags = 65536. Seems sufficient. lasfri TODO - check if so.
    quint16 numfrags,lastsize ;
    quint8 fid, tfid, opcode;

    /* Bail out if unknown command */
    if (m_currentCommand->setCommand(cmdCode) != cmdCode) {
        return -1;
    }

    headerFrame.resize(CUBES_NL_HEADER_SIZE);

    numfrags = data.size()/CUBES_NL_MTU;
    lastsize = data.size()%CUBES_NL_MTU;
    if (lastsize) {
        numfrags += 1;
    } else {
        lastsize = CUBES_NL_MTU;
    }


    /* lasfri TODO : Lots of magic numbers here. Consider making headerFrame and dataFrame as
    * classes deriving from a Frame class, where the number of bytes for different fields
    * is set during object instantiation ? Also maybe allows to define traits. In below, I
    * don't use traits at all */
    tfid=0;
    headerFrame[0] = (tfid << 7) | cmdCode;
    headerFrame[1] = (data.size() >> 24) & 0xff;
    headerFrame[2] = (data.size() >> 16) & 0xff;
    headerFrame[3] = (data.size() >> 8) & 0xff;
    headerFrame[4] = data.size() & 0xff;
    if(writeI2C(headerFrame)) {
        return -1;
    }

    rechdrFrame = readI2C();
    opcode = rechdrFrame[0];
    /* Check Frame ID
    if ((opcode & 0x80 >> 7) != tfid ){
        return -1;
    }*/

    /*Check if Frame ACK recd*/
    if ((opcode & 0x7F) != CMD_F_ACK) {
        return -1; // lasfri TODO : How to handle error cases ?
    }

    for (int i=1; i<numfrags; i++) {

        fid = (i)%2; // Even is 0, odd is 1, loop starts wiht i=1 for fid 1.
        frag.resize(CUBES_NL_MTU + 1 ); // Additional byte for opcode field - No FCS
        frag[0] = (fid << 7) | CMD_DATA_FRAME;
        int spos = (i-1)*CUBES_NL_MTU;
        for (int j=1; j<=CUBES_NL_MTU; j++){
             frag[j] = data[spos+j-1];
        }
        if(writeI2C(frag)) {
            return -1;
        }
        rechdrFrame = readI2C();
        opcode = rechdrFrame[0];
        /* Check Frame ID
        if ((opcode & 0x80 >> 7) != fid ){
            return -1;
        }*/

        /* Check ACK*/
        if((opcode & 0x7f) != CMD_F_ACK){
            return -1;
        }
    }


    /*Send last frag*/
    frag.resize(lastsize + 1);
    frag[0] = (fid << 7) | CMD_DATA_FRAME;
    for (int j=1; j<=lastsize; j++){
         frag[j] = (data.right(lastsize))[j-1]; // & 0xff;
    }
    if(writeI2C(frag)) {
        return -1;
    }

    rechdrFrame = readI2C();
    opcode = rechdrFrame[0];

    // Check Frame ID against Transaction frame-id
    /*if ((opcode & 0x80 >> 7) != tfid ){
        return -1;
    }*/

    /* Check T_ACK */
    if((opcode & 0x7f) != CMD_T_ACK){
        return -1;
    }

    return 0;
}

qint8 CubesProtoUartPmod::requestNL(quint8 cmdCode, QByteArray &datarray){
    QByteArray frag, headerFrame, rechdrFrame;

    //Note : This places limit on max numfrags = 65536. Seems sufficient. lasfri TODO - check if so.
    quint16 numfrags,lastsize ;
    quint8 fid, tfid, opcode;
    quint32 dlen;


    /* Bail out if unknown command */
    if (m_currentCommand->setCommand(cmdCode) != cmdCode) {
        return -1;
    }

    headerFrame.resize(CUBES_NL_HEADER_SIZE);

    tfid=0;
    headerFrame[0] = (tfid << 7) | cmdCode;
    headerFrame[1] = 0;
    headerFrame[2] = 0;
    headerFrame[3] = 0;
    headerFrame[4] = 0;
    if(writeI2C(headerFrame)){
        return -1;
    }

    rechdrFrame = readI2C();
    opcode = rechdrFrame[0];
    tfid = (opcode & 0x80 >> 7);

    /*Check if EXPT sending something ? */
    if ((opcode & 0x7F) != CMD_EXP_SEND) {
        return -1;
    }

    dlen = (rechdrFrame.mid(1,4)).toUInt();
    numfrags = dlen/CUBES_NL_MTU;
    lastsize = dlen%CUBES_NL_MTU;
    frag.resize(CUBES_NL_MTU);
    if(lastsize == 0) lastsize = CUBES_NL_MTU;
    fid = tfid;

    for(int i=1;i<numfrags;i++){

        headerFrame[0] = (fid << 7) | CMD_F_ACK;
        if(writeI2C(headerFrame)) {
            return -1;
        }

        rechdrFrame = readI2C();
        opcode = rechdrFrame[0];
        fid = (opcode & 0x80 >> 7);
        if((opcode & 0x7F) != CMD_DATA_FRAME){
            return -1; // lasfri TODO : Send NULL frame here and then return
        }

        int spos = (i-1)*CUBES_NL_MTU;
        frag = rechdrFrame.mid(spos,CUBES_NL_MTU) ;
        if(i==1) {
            datarray = frag;
        }
        else {
            datarray = datarray.append(frag);
        }
    }

    /*Read last frame*/
    headerFrame[0] = (fid << 7) | CMD_F_ACK;
    if(writeI2C(headerFrame)) {
        return -1;
    }
    rechdrFrame = readI2C();
    opcode = rechdrFrame[0];
    if((opcode & 0x7F) != CMD_DATA_FRAME){
        return -1;
    }
    datarray.append(rechdrFrame.right(lastsize));

    /*Send T_ACK*/
    headerFrame[0] = (tfid << 7) | CMD_T_ACK;
    writeI2C(headerFrame);

    return 0;

}

QByteArray CubesProtoUartPmod::readI2C()
{
    QByteArray data;
    data.resize(1);
    data[0] = -1;

    if(m_device->waitForReadyRead()){
        return m_device->readAll();
    }
    return data;
}


qint64 CubesProtoUartPmod::write(QByteArray &data)
{
    return m_device->write(data);
}

QByteArray CubesProtoUartPmod::readAll()
{
    return m_device->readAll();
}

qint64 CubesProtoUartPmod::sendCommand(quint8 cmdCode, QByteArray &cmdData)
{
    QByteArray      data;
    int             numDataBytes;
    char            read, write;

    /* Bail out if unknown command */
    if (m_currentCommand->setCommand(cmdCode) != cmdCode) {
        return -1;
    }

    /*
     * Create I2C R/nW ('1' = Read, '0' = Write) bit based on current command's
     * data direction, and assign number of data bytes and I2C Address + R/nW
     * byte to mimic an I2C transfer over UART.
     */
    write = 0x1 & (~m_currentCommand->dataDirection());
    read = 0x1 & (~write);
    m_bytesLeft = m_currentCommand->dataBytes();

    numDataBytes = 2 + (cmdData.size() * write);
    data.resize(numDataBytes);

    data[0] = (CUBES_I2C_ADDRESS << 1) | (read);
    data[1] = cmdCode;
    for (int i = 0; i < numDataBytes-2; i++)
        data[i+2] = cmdData[i];

    /* Clear data buffers and issue command */
    return m_device->write(data);
}

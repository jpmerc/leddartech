// *****************************************************************************
// Module..: SerialDemo
//
/// \file    Modbus.h
///
/// \brief   Declarations for the Modbus layer of the demo.
///
// Copyright (c) 2014 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#ifndef _MOBDUS_H_
#define _MOBDUS_H_

#include "OS.h"

#define MODBUS_MAX_PAYLOAD 252
#define MODBUS_SERVER_ID   0x11

LtBool
ModbusConnected( void );

LtResult
ModbusSend( LtByte aFunction, LtByte *aBuffer, LtByte aLength );

LtResult
ModbusReceive( LtByte *aBuffer );

LtResult
ModbusConnect( char *aPortName, LtByte aAddress );

void
ModbusDisconnect( void );

LtResult
ModbusReadInputRegisters( LtU16 aNo, LtU16 aCount, LtU16 *aValue );

LtResult
ModbusReadHoldingRegister( LtU16 aNo, LtU16 *aValue );

LtResult
ModbusWriteRegister( LtU16 aNo, LtU16 aValue );

#endif

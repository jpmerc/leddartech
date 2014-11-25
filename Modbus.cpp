// *****************************************************************************
// Module..: SerialDemo
//
/// \file    Modbus.c
///
/// \brief   Function definitions for the Modbus layer of the demo.
///
// Copyright (c) 2014 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#include <string.h>

#include "Modbus.h"

// Some Modbus constants
#define MODBUS_HEADER_LEN  2
#define MODBUS_CRC_LEN     2
#define MODBUS_OVERHEAD    (MODBUS_HEADER_LEN+MODBUS_CRC_LEN)
#define MODBUS_MAX_LEN     (MODBUS_MAX_PAYLOAD+MODBUS_OVERHEAD)

// Table of CRC values for high–order byte
static LtByte CRC_HI[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};

// Table of CRC values for low–order byte
static LtByte CRC_LO[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

static LtHandle gHandle = LT_INVALID_HANDLE;
static LtByte   gAddress;
static LtByte   gLastFunction = 0;

// *****************************************************************************
// Function: CRC16
//
/// \brief   Compute a CRC16 using the Modbus recipe.
///
/// \param   aBuffer  Array containing the data to use.
/// \param   aLength  Number of byte in aBuffer.
/// \param   aCheck   If true, the two bytes after aLength in aBuffer are
///                   supposed to contain the CRC and we verify that it is
///                   the same as what was just computed.
///
/// \return  If aCheck is false, always returns true, if aCheck is true,
///          return true if the CRC compares ok, false otherwise.
// *****************************************************************************

static LtBool
CRC16( LtByte *aBuffer, LtByte aLength, LtBool aCheck )
{
    LtByte lCRCHi = 0xFF; // high byte of CRC initialized
    LtByte lCRCLo = 0xFF; // low byte of CRC initialized
    int    i;

    for( i = 0; i<aLength; ++i )
    {
        int lIndex = lCRCLo ^ aBuffer[i]; // calculate the CRC
        lCRCLo = lCRCHi ^ CRC_HI[lIndex];
        lCRCHi = CRC_LO[lIndex];
    }

    if ( aCheck )
    {
        return ( aBuffer[aLength] == lCRCLo ) && ( aBuffer[aLength+1] == lCRCHi );
    }

    aBuffer[aLength] = lCRCLo;
    aBuffer[aLength+1] = lCRCHi;
    return LT_TRUE;
}

// *****************************************************************************
// Function: ModbusConnected
//
/// \brief   Indicates if we have successfully called ModbusConnect, without
///          calling ModbusDisconnect.
///
/// \return  LT_TRUE or LT_FALSE.
// *****************************************************************************

LtBool
ModbusConnected( void )
{
    return gHandle != LT_INVALID_HANDLE;
}

// *****************************************************************************
// Function: ModbusSend
//
/// \brief   Send a Modbus message.
///
/// \param   aFunction  Modbus function number.
/// \param   aBuffer    Message payload.
/// \param   aLength    Number of bytes in aBuffer (maximum MODBUS_MAX_PAYLOAD).
///
/// \return  LT_SUCCESS or LT_ERROR or LT_INVALID_ARGUMENT.
// *****************************************************************************

LtResult
ModbusSend( LtByte aFunction, LtByte *aBuffer, LtByte aLength )
{
    LtByte lMessage[MODBUS_MAX_LEN];

    // Verify validity of parameters.
    if (    ( aLength > MODBUS_MAX_PAYLOAD )
         || ( aFunction < 1 )
         || ( aFunction > 127 ) )
    {
        return LT_INVALID_ARGUMENT;
    }

    lMessage[0] = gAddress;
    lMessage[1] = aFunction;
    if ( aLength > 0 )
    {
        memcpy( lMessage+MODBUS_HEADER_LEN, aBuffer, aLength );
    }

    aLength += MODBUS_HEADER_LEN;
    CRC16( lMessage, aLength, LT_FALSE );
    aLength += MODBUS_CRC_LEN;

    if ( WriteToSerialPort( gHandle, lMessage, aLength ) == aLength )
    {
        gLastFunction = aFunction;
        return LT_SUCCESS;
    }

    return LT_ERROR;
}

// *****************************************************************************
// Function: ModbusReceive
//
/// \brief   Receive a response (assumes a Send was done before).
///
/// \param   aBuffer  Buffer where the message payload will be copied. Must be
///                   at least MODBUS_MAX_PAYLOAD bytes long to accomodate the
///                   longest Modbus payload. Can be NULL if response is
///                   ignored.
///
/// \return  If a message was successfully received, return the number of
///          payload bytes received (0 is a valid answer).
///          Any negative value indicates an error or no message received.
///          LT_TIMEOUT for timeout (nothing or incomplete message received).
///          LT_BAD_CRC Invalid CRC (which may also be caused by an incomplete message).
///          LT_ERROR Any Windows error while trying to read serial port.
// *****************************************************************************

LtResult
ModbusReceive( LtByte *aBuffer )
{
    LtResult lRead;
    LtByte   lMessage[MODBUS_MAX_LEN];

    lRead = ReadFromSerialPort( gHandle, lMessage, sizeof(lMessage) );

    if ( lRead < 0 )
    {
        return LT_ERROR;
    }

    if ( lRead >= MODBUS_OVERHEAD )
    {
        if ( CRC16( lMessage, lRead-MODBUS_CRC_LEN, LT_TRUE ) )
        {
            if (    ( lMessage[0] == gAddress )
                 && ( lMessage[1] == gLastFunction ) )
            {
                gLastFunction = 0;
                if ( aBuffer != NULL )
                {
                    memcpy( aBuffer, lMessage+MODBUS_HEADER_LEN, lRead-MODBUS_OVERHEAD );
                }
                return lRead-MODBUS_OVERHEAD;
            }

            return LT_PROTOCOL_ERROR;
        }

        return LT_BAD_CRC;
    }

    return LT_TIMEOUT;
}

// *****************************************************************************
// Function: ModbusConnect
//
/// \brief   Try to connect to a sensor on the given serial port.
///
/// Modbus does not have the concept of a connection, but we try a dummy
/// communication to verify that a sensor is plugged in after successfully
/// opening the serial port.
///
/// \param   aPortName  Name of serial port to open (e.g.: COM4 on Windows or
///                     ttyUSB0 on Linux).
/// \param   aAddress   The Modbus address of the sensor to talk to.
///
/// \return  LT_SUCCESS or any one of the LT error codes.
// *****************************************************************************

LtResult
ModbusConnect( char *aPortName, LtByte aAddress )
{
    LtResult lResult = OpenSerialPort( aPortName, &gHandle );

    if ( lResult == LT_SUCCESS )
    {
        gAddress = aAddress;

        lResult = ModbusSend( MODBUS_SERVER_ID, NULL, 0 );
        
        if ( lResult == LT_SUCCESS )
        {
            LtByte lDummy[MODBUS_MAX_PAYLOAD];

            lResult = ModbusReceive( lDummy );
            
            if ( lResult >= 0 )
            {
                return LT_SUCCESS;
            }
        }
    }

    CloseSerialPort( gHandle );
    gHandle = LT_INVALID_HANDLE;
    return lResult;
}

// *****************************************************************************
// Function: ModbusDisconnect
//
/// \brief   Disconnect. Has no effect if was not connected.
// *****************************************************************************

void
ModbusDisconnect( void )
{
    CloseSerialPort( gHandle );
    gHandle = LT_INVALID_HANDLE;
}

// *****************************************************************************
// Function: ModbusReadInputRegister
//
/// \brief   Implements the standard Modbus function 4 (but with only 1
///          register).
///
/// \param   aNo      Address of first register to read.
/// \param   aCount   The number of consecutive registers to read.
/// \param   aValues  On output contains the values if LT_SUCCESS is returned.
///
/// \return  LT_SUCCESS or any one of the LT error codes.
// *****************************************************************************

LtResult
ModbusReadInputRegisters( LtU16 aNo, LtU16 aCount, LtU16 *aValues )
{
    LtByte   lPayload[MODBUS_MAX_PAYLOAD];
    LtResult lResult;

    lPayload[0] = aNo >> 8;
    lPayload[1] = aNo & 0xFF;
    lPayload[2] = aCount >> 8;
    lPayload[3] = aCount & 0xFF;

    lResult = ModbusSend( 4, lPayload, 4 );
    
    if ( lResult == LT_SUCCESS )
    {
        lResult = ModbusReceive( lPayload );

        if ( lResult >= 0 )
        {
            LtU16 i=0;

            for( ; i<aCount; ++i )
            {
                aValues[i] = lPayload[i*2+1]*256 + lPayload[i*2+2];
            }
            return LT_SUCCESS;
        }
    }

    return lResult;
}

// *****************************************************************************
// Function: ModbusReadHoldingRegister
//
/// \brief   Implements the standard Modbus function 3 (but with only 1
///          register).
///
/// \param   aNo     Address of register to read.
/// \param   aValue  On output contains the value if LT_SUCCESS is returned.
///
/// \return  LT_SUCCESS or any one of the LT error codes.
// *****************************************************************************

LtResult
ModbusReadHoldingRegister( LtU16 aNo, LtU16 *aValue )
{
    LtByte   lPayload[MODBUS_MAX_PAYLOAD];
    LtResult lResult;

    lPayload[0] = aNo >> 8;
    lPayload[1] = aNo & 0xFF;
    lPayload[2] = 0;
    lPayload[3] = 1;

    lResult = ModbusSend( 3, lPayload, 4 );
    
    if ( lResult == LT_SUCCESS )
    {
        lResult = ModbusReceive( lPayload );

        if ( lResult >= 0 )
        {
            *aValue = lPayload[1]*256 + lPayload[2];
            return LT_SUCCESS;
        }
    }

    return lResult;
}

// *****************************************************************************
// Function: ModbusWriteRegister
//
/// \brief   Implements the standard Modbus function 6.
///
/// \param   aNo     Address of register to write.
/// \param   aValue  New value to put in register.
///
/// \return  LT_SUCCESS or any one of the LT error codes.
// *****************************************************************************

LtResult
ModbusWriteRegister( LtU16 aNo, LtU16 aValue )
{
    LtByte   lPayload[MODBUS_MAX_PAYLOAD];
    LtResult lResult;

    lPayload[0] = aNo >> 8;
    lPayload[1] = aNo & 0xFF;
    lPayload[2] = aValue >> 8;
    lPayload[3] = aValue & 0xFF;

    lResult = ModbusSend( 6, lPayload, 4 );
    
    if ( lResult == LT_SUCCESS )
    {
        lResult = ModbusReceive( lPayload );

        if ( lResult >= 0 )
        {
            return LT_SUCCESS;
        }
    }

    return lResult;
}

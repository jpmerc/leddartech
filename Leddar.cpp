// *****************************************************************************
// Module..: SerialDemo
//
/// \file    Leddar.c
///
/// \brief   Function definitions for the Leddar layer of the demo.
///
// Copyright (c) 2014 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#include <stdlib.h>
#include <string.h>

#include "Leddar.h"
#include "Modbus.h"

// *****************************************************************************
// Function: LeddarConnect
//
/// \brief   Try to connect to a sensor on the given serial port.
///
/// \param   aPortName  Name of serial port to open (e.g.: COM4 on Windows or
///                     ttyUSB0 on Linux).
/// \param   aAddress   The Modbus address of the sensor to talk to.
///
/// \return  LT_SUCCESS or any one of the LT error codes.
// *****************************************************************************

LtResult
LeddarConnect( char *aPortName, LtByte aAddress )
{
    LtResult lResult;

    lResult = ModbusConnect( aPortName, aAddress );
    
    if ( lResult == LT_SUCCESS )
    {
        lResult = ModbusSend( MODBUS_SERVER_ID, NULL, 0 );
        
        if ( lResult == LT_SUCCESS )
        {
            LtByte lId[MODBUS_MAX_PAYLOAD];

            lResult = ModbusReceive( lId );
            
            if ( lResult >= 0 )
            {
                // Make sure we are talking to a Leddar One
                if ( ( lId[50] == 10 ) && ( lId[51] == 0 ) )
                {
                    return LT_SUCCESS;
                }

                // Unrecognized device!
                lResult = LT_ERROR;
            }
        }

        ModbusDisconnect();
    }

    return lResult;
}

// *****************************************************************************
// Function: LeddarDisconnect
//
/// \brief   Disconnect. Has no effect if was not connected.
// *****************************************************************************

void
LeddarDisconnect( void )
{
    ModbusDisconnect();
}

// *****************************************************************************
// Function: LeddarGetResults
//
/// \brief   Read input registers to get the timestamp, temperature and echoes.
///
/// \param   aDetections  Pointer to array where detections will be written.
///
/// \return  LT_SUCCESS or any of the LT error codes.
// *****************************************************************************

LtResult
LeddarGetResults( LtAcquisition *aAcquisition )
{
    LtU16 lValues[10];

    LtResult lResult = ModbusReadInputRegisters( 20, 10, lValues );

    if ( lResult == LT_SUCCESS )
    {
        int i;
        LtDetection *lDetections = aAcquisition->mDetections;
        
        aAcquisition->mTimestamp = lValues[0] + (lValues[1]<<16);
        aAcquisition->mTemperature = lValues[2]/256.f;
        aAcquisition->mDetectionCount = lValues[3] < LEDDAR_MAX_DETECTIONS ? lValues[3] : LEDDAR_MAX_DETECTIONS;

        for( i=0; i<aAcquisition->mDetectionCount; ++i )
        {
            lDetections[i].mDistance = lValues[i*2+4]/1000.f;
            lDetections[i].mAmplitude = lValues[i*2+5]/256.f;
        }
    }

    return lResult;
}

// *****************************************************************************
// Function: LeddarGetParameter
//
/// \brief   Generic configuration parameter read access function (for integer
///          parameters).
///
/// \param   aNo     Number of parameter to read (one of the LEDDAR_CONFIG_*
///                  constants).
/// \param   aValue  Pointer to a variable that on output will contain the
///                  value if LT_SUCCESS is returned.
///
/// \return  LT_SUCCESS or any of the LT error codes.
// *****************************************************************************

LtResult
LeddarGetParameter( LtU16 aNo, LtU16 *aValue )
{
    return ModbusReadHoldingRegister( aNo, aValue );
}

// *****************************************************************************
// Function: LeddarSetParameter
//
/// \brief   Generic configuration parameter write access function (for integer
///          parameters).
///
/// \param   aNo     Number of parameter to write (one of the LEDDAR_CONFIG_*
///                  constants).
/// \param   aValue  The new value to set.
///
/// \return  LT_SUCCESS or any of the LT error codes.
// *****************************************************************************

LtResult
LeddarSetParameter( LtU16 aNo, LtU16 aValue )
{
    return ModbusWriteRegister( aNo, aValue );
}

// *****************************************************************************
// Function: LeddarWriteConfiguration
//
/// \brief   Ask the sensor to write all current configuration parameters to
///          permanent memory.
///
/// \return  LT_SUCCESS or any of the LT error codes.
// *****************************************************************************

LtResult
LeddarWriteConfiguration( void )
{
    ModbusSend( 0x46, NULL, 0 );

    return ModbusReceive( NULL );
}
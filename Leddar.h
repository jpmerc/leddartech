// *****************************************************************************
// Module..: SerialDemo
//
/// \file    Leddar.h
///
/// \brief   Declarations for the Leddar layer of the demo.
///
// Copyright (c) 2014 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#ifndef _LEDDAR_H_
#define _LEDDAR_H_

#include "OS.h"

#define LEDDAR_MAX_DETECTIONS 3

// Register addresses for configuration parameters
#define LEDDAR_CONFIG_ACCUMULATION   0
#define LEDDAR_CONFIG_OVERSAMPLING   1
#define LEDDAR_CONFIG_SAMPLE_COUNT   2
#define LEDDAR_CONFIG_LED_POWER      4
#define LEDDAR_CONFIG_BAUD_RATE      29
#define LEDDAR_CONFIG_MODBUS_ADDRESS 30

typedef struct _LtDetection
{
    float mDistance;
    float mAmplitude;
} LtDetection;

typedef struct _LtAcquisition
{
    LtU32 mTimestamp;
    float mTemperature;
    LtU16 mDetectionCount;
    
    LtDetection mDetections[LEDDAR_MAX_DETECTIONS];
} LtAcquisition;

LtResult
LeddarConnect( char *aPortName, LtByte aAddress );

void
LeddarDisconnect( void );

LtResult
LeddarGetResults( LtAcquisition *aAcquisition );

LtResult
LeddarGetParameter( LtU16 aNo, LtU16 *aValue );

LtResult
LeddarSetParameter( LtU16 aNo, LtU16 aValue );

LtResult
LeddarWriteConfiguration( void );

#endif

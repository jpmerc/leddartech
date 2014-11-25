// *****************************************************************************
// Module..: SDK -- Software development kit for Leddar products. RS-485
//           demonstration program.
//
/// \file    OS.h
///
/// \brief   Definitions for the OS dependant part of the demo.
///
/// You may have to modify definitions in this file if you use a non-standard
/// operating system. Definitions provided are correct for Windows and
/// Fedora Linux.
///
// Copyright (c) 2013 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#ifndef _OS_H_
#define _OS_H_

#include "UserDefs.h"

// These definitions assumes 8-bit chars, 16-bit shorts and 32-bit ints.
// If your platform uses different sizes you will have to modify them.
typedef unsigned char  LtBool;
typedef unsigned char  LtByte;
typedef int            LtResult;
typedef unsigned short LtU16;
typedef short          Lt16;
typedef unsigned int   LtU32;

#ifdef LT_WINDOWS

typedef void *LtHandle;

#define LT_INVALID_HANDLE     ((void *)-1)
#define LT_MAX_PORT_NAME_LEN  5

#endif // LT_WINDOWS

#ifdef LT_LINUX

typedef int LtHandle;

#define LT_INVALID_HANDLE     (-1)
#define LT_MAX_PORT_NAME_LEN  24

#endif // LT_LINUX

LtBool
KeyPressed( void );

int
GetKey( void );

LtResult
OpenSerialPort( char *aPortName, LtHandle *aHandle );

void
CloseSerialPort( LtHandle aHandle );

LtResult
WriteToSerialPort( LtHandle aHandle, LtByte *aData, int aLength );

LtResult
ReadFromSerialPort( LtHandle aHandle, LtByte *aData, int aMaxLength );

#endif

// End of file OS.h

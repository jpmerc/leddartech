// *****************************************************************************
// Module..: SDK -- Software development kit for Leddar products. RS-485
//           demonstration program.
//
/// \file    UserDefs.h
///
/// \brief   Definitions that may have to be modified according to preferences.
///
// Copyright (c) 2013 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#ifndef _USER_DEFS_H_
#define _USER_DEFS_H_

#include "Definitions.h"

// Here we use the fact that the VisualStudio project defines WIN32 to
// automatically set the correct definition, but you can edit this by hand
// to force the proper definition for special needs.
#ifdef WIN32
#define LT_WINDOWS
#else
#define LT_LINUX
#endif

// The values specified here must be valid for your platform and must
// correspond to what is configured in the Leddar sensor.
#define LT_SERIAL_SPEED 115200
#define LT_PARITY       LT_PARITY_NONE
#define LT_STOP_BITS    1

#endif

// End of file UserDefs.h

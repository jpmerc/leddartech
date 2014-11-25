// *****************************************************************************
// Module..: SDK -- Software development kit for Leddar products. RS-485
//           demonstration program.
//
/// \file    OS.c
///
/// \brief   Function definitions for the OS dependant part of the demo.
///
/// You may have to modify this file if you use a non-standard operating
/// system. Definitions provided are correct for Windows and Fedora Linux.
///
// Copyright (c) 2013 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#include <stdio.h>
#include "OS.h"

#ifdef LT_WINDOWS
#include <windows.h>
#include <conio.h>
#endif

#ifdef LT_LINUX
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>

// *****************************************************************************
// Function: SetNonBlocking
//
/// \brief   Special helper function to implement the equivalent of _kbhit and
///          _getch in Linux.
///
/// \param   aState  If LT_FALSE, the terminal will be put in the state in
///                  which it was at the start of the program which is
///                  normally CANONICAL with ECHO. Otherwise the canonical and
///                  echo mode will be disabled.
// *****************************************************************************

static void
SetNonBlocking( LtBool aState )
{
	static int sInitDone = LT_FALSE;
	static struct termios sOriginalParams;

	if ( !sInitDone )
	{
		tcgetattr( STDIN_FILENO, &sOriginalParams );
		sInitDone = LT_TRUE;
	}

	if ( aState )
	{
		struct termios lNewParams = sOriginalParams;

		lNewParams.c_lflag = ~( ICANON | ECHO );
	    tcsetattr( STDIN_FILENO, TCSANOW, &lNewParams );
	}
	else
	{
	    tcsetattr( STDIN_FILENO, TCSANOW, &sOriginalParams );
	}
}
#endif // LT_LINUX

// *****************************************************************************
// Function: KeyPressed
//
/// \brief   Verify if a key was pressed and is waiting in the buffer (used
///          to check if the user has requested to stop when continuously
///          displaying detections.
///
/// \return  LT_TRUE or LT_FALSE.
// *****************************************************************************

LtBool
KeyPressed( void )
{
#ifdef LT_WINDOWS
    return _kbhit();
#endif
#ifdef LT_LINUX
    struct timeval lTimeout;
    fd_set lFds;

    lTimeout.tv_sec = 0;
    lTimeout.tv_usec = 0;

    FD_ZERO( &lFds );
    FD_SET( STDIN_FILENO, &lFds );

    SetNonBlocking( LT_TRUE );
    select( STDIN_FILENO+1, &lFds, NULL, NULL, &lTimeout );
    SetNonBlocking( LT_FALSE );
    return FD_ISSET( STDIN_FILENO, &lFds );
#endif
}

// *****************************************************************************
// Function: GetKey
//
/// \brief   Returns the next character in the keyboard buffer without a need
///          to press enter (used to navigate the menu).
///
/// \return  The character (may have some special values for non printable
///          keys).
// *****************************************************************************

int
GetKey( void )
{
#ifdef LT_WINDOWS
    return _getch();
#endif
#ifdef LT_LINUX
    int lResult;

    SetNonBlocking( LT_TRUE );
    lResult = getchar();
    SetNonBlocking( LT_FALSE );

    return lResult;
#endif
}

// *****************************************************************************
// Function: OpenSerialPort
//
/// \brief   Open the serial port with the given name.
///
/// \param   aPortName  Name of the device to open (must be valid for the
///                     platform).
/// \param   aHandle    Pointer to variable that will receive the handle on
///                     output.
///
/// \return  LT_SUCCESS or LT_ERROR.
// *****************************************************************************

LtResult
OpenSerialPort( char *aPortName, LtHandle *aHandle )
{
#ifdef LT_WINDOWS
    wchar_t      lPortName[LT_MAX_PORT_NAME_LEN+5];
    DCB          lDCB;
    COMMTIMEOUTS lCTO;

    swprintf( lPortName, sizeof(lPortName)/sizeof(lPortName[0]),
              L"\\\\.\\%S", aPortName );

    *aHandle = CreateFile( lPortName, 
                           GENERIC_READ | GENERIC_WRITE, 0, NULL,
                           OPEN_EXISTING, 0, NULL );

    if ( *aHandle == INVALID_HANDLE_VALUE )
    {
        return LT_ERROR;
    }

    // The inter character timeout should be much shorter but it does not seem
    // reliable when we put it lower.
    lCTO.ReadIntervalTimeout = 25;
    lCTO.ReadTotalTimeoutMultiplier = 0;
    lCTO.ReadTotalTimeoutConstant = 1000;
    lCTO.WriteTotalTimeoutMultiplier = 10;
    lCTO.WriteTotalTimeoutConstant = 1000;

    if ( SetCommTimeouts( *aHandle, &lCTO ) )
    {
        memset( &lDCB, 0, sizeof(lDCB) );
        lDCB.DCBlength = sizeof(lDCB);
        lDCB.fBinary = TRUE;
        lDCB.fParity = FALSE;
        lDCB.fOutxCtsFlow = FALSE;
        lDCB.fOutxDsrFlow = FALSE;
        lDCB.fDtrControl = DTR_CONTROL_DISABLE;
        lDCB.fDsrSensitivity = FALSE;
        lDCB.fTXContinueOnXoff = TRUE;
        lDCB.fOutX = FALSE;
        lDCB.fInX = FALSE;
        lDCB.fErrorChar = FALSE;
        lDCB.fNull = FALSE;
        lDCB.fRtsControl = RTS_CONTROL_DISABLE;
        // Must be false otherwise we have to call ClearCommError on every error.
        lDCB.fAbortOnError = FALSE;
        lDCB.XonLim = 2;
        lDCB.XoffLim = 4;

        lDCB.BaudRate = LT_SERIAL_SPEED;
        lDCB.ByteSize = 8;
        switch( LT_PARITY )
        {
            case LT_PARITY_ODD:
                lDCB.Parity = ODDPARITY;
                break;
            case LT_PARITY_EVEN:
                lDCB.Parity = EVENPARITY;
                break;
            default:
                lDCB.Parity = NOPARITY;
                break;
        }
        if ( LT_STOP_BITS == 2 )
        {
            lDCB.StopBits = TWOSTOPBITS;
        }
        else
        {
            lDCB.StopBits = ONESTOPBIT;
        }

        if ( SetCommState( *aHandle, &lDCB ) )
        {
            return LT_SUCCESS;
        }
    }

    CloseHandle( *aHandle );
#endif // LT_WINDOWS
#ifdef LT_LINUX
    char lPortName[LT_MAX_PORT_NAME_LEN+6];

    sprintf( lPortName, "/dev/%s", aPortName );
    *aHandle = open( lPortName, O_RDWR | O_NOCTTY );

    if ( *aHandle >= 0 )
    {
        struct termios tio;

        if ( !tcgetattr( *aHandle, &tio ) )
        {
        	// 8 bits per char, ignore modem control lines, enable receiver.
			tio.c_cflag = CS8 | CLOCAL | CREAD;
			switch( LT_SERIAL_SPEED )
			{
			    case 9600:
			    	tio.c_cflag |= B9600;
				    break;
			    case 19200:
			    	tio.c_cflag |= B19200;
				    break;
			    case 38400:
			    	tio.c_cflag |= B38400;
				    break;
			    case 57600:
			    	tio.c_cflag |= B57600;
				    break;
			    default:
				    tio.c_cflag |= B115200;
				    break;
			}
			if ( LT_STOP_BITS == 2 )
			{
				tio.c_cflag |= CSTOPB;
			}
			switch( LT_PARITY )
			{
			    case LT_PARITY_ODD:
			    	tio.c_cflag |= PARENB | PARODD;
			    	break;
			    case LT_PARITY_EVEN:
			    	tio.c_cflag |= PARENB;
			    	break;
			}
			// Enable parity checking on input
			tio.c_iflag = INPCK;
			// So special output processing
			tio.c_oflag = 0;
			// Raw mode
			tio.c_lflag = 0;
			// None of the 4 modes provided by VMIN and VTIME correspond to
			// what we need with Modbus, so we set 0 on both which gives
			// immediate return on call to read whatever the availability
			// of data.
			tio.c_cc[VMIN] = 0;
			tio.c_cc[VTIME] = 0;

			if ( !tcsetattr( *aHandle, TCSANOW, &tio ) )
			{
		    	return LT_SUCCESS;
			}
        }

    	close( *aHandle );
    }
#endif // LT_LINUX

    return LT_ERROR;
}

// *****************************************************************************
// Function: CloseSerialPort
//
/// \brief   Close the serial port for the given handle.
///
/// \param   aHandle  Handle returned by OpenSerialPort.
// *****************************************************************************

void
CloseSerialPort( LtHandle aHandle )
{
    if ( aHandle != LT_INVALID_HANDLE )
    {
#ifdef LT_WINDOWS
        CloseHandle( aHandle );
#endif
#ifdef LT_LINUX
        close( aHandle );
#endif
    }
}

// *****************************************************************************
// Function: WriteToSerialPort
//
/// \brief   Write data to the serial port.
///
/// \param   aHandle  Handle returned by OpenSerialPort.
/// \param   aData    Pointer to data to write.
/// \param   aLength  Number of bytes from aData to write.
///
/// \return  The number of bytes actually written or LT_ERROR.
// *****************************************************************************

LtResult
WriteToSerialPort( LtHandle aHandle, LtByte *aData, int aLength )
{
#ifdef LT_WINDOWS
    DWORD lWritten;

    // Discard any bytes that could have arrived unexpectedly.
    if ( PurgeComm( aHandle, PURGE_RXCLEAR ) )
    {
        if ( WriteFile( aHandle, aData, aLength, &lWritten, NULL ) )
        {
            return (LtResult) lWritten;
        }
    }

    return LT_ERROR;
#endif
#ifdef LT_LINUX
    if ( tcflush( aHandle, TCIFLUSH ) )
    {
    	return LT_ERROR;
    }
    return write( aHandle, aData, aLength );
#endif
}

// *****************************************************************************
// Function: ReadFromSerialPort
//
/// \brief   Read data from the serial port.
///
/// \param   aHandle  Handle returned by OpenSerialPort.
/// \param   aData    Pointer to where to put the data read.
/// \param   aLength  Maximum number of bytes to read (number read may
///                   actually be lower).
///
/// \return  The number of bytes actually read or LT_ERROR.
// *****************************************************************************

LtResult
ReadFromSerialPort( LtHandle aHandle, LtByte *aData, int aMaxLength )
{
#ifdef LT_WINDOWS
    DWORD lRead;

    if ( !ReadFile( aHandle, aData, aMaxLength, &lRead, NULL ) )
    {
        return LT_ERROR;
    }

    return (LtResult) lRead;
#endif
#ifdef LT_LINUX
    // Wait for the first byte with a long timeout to let time for the sensor
    // to process the command.
    struct timeval lTimeout;
    fd_set         lFds;
    LtResult       lRead = 0;
    int            lMicroseconds = 20000000/LT_SERIAL_SPEED;

    lTimeout.tv_sec = 1;
    lTimeout.tv_usec = 0;

    FD_ZERO( &lFds );
    FD_SET( aHandle, &lFds );

    if ( select( aHandle+1, &lFds, NULL, NULL, &lTimeout ) <= 0 )
    {
    	return LT_ERROR;
    }

    // In theory we want an inter-character timeout of 2 character but
    // in practice setting a value too low is not reliable.
    lMicroseconds = lMicroseconds < 2000 ? 2000 : lMicroseconds;
    // Now read the data with a short inter-byte timeout.
    // We end either when we have received the number of bytes or
    // there is a too long interval between 2 bytes (indicating
    // the end of the message).
    while( lRead < aMaxLength )
    {
    	int lResult;

        lTimeout.tv_sec = 0;
        lTimeout.tv_usec = lMicroseconds;

        FD_ZERO( &lFds );
        FD_SET( aHandle, &lFds );

        lResult = select( aHandle+1, &lFds, NULL, NULL, &lTimeout );

        if ( lResult < 0 )
        {
        	return LT_ERROR;
        }
        else if ( lResult == 0 )
        {
        	return lRead;
        }

    	lResult = read( aHandle, aData+lRead, aMaxLength-lRead );

    	if ( lResult < 0 )
    	{
    		return LT_ERROR;
    	}

    	lRead += lResult;
    }

    return lRead;
#endif
}

// End of file OS.c

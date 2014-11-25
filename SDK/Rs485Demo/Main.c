// *****************************************************************************
// Module..: SDK -- Software development kit for Leddar products. RS-485
//           demonstration program.
//
/// \file    Main.c
///
/// \brief   This is the main file containing the menu driver.
///
// Copyright (c) 2013 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "Leddar.h"

// *****************************************************************************
// Function: GetMenuChoice
//
/// \brief   Loops until a key is entered that is valid in a menu.
///
/// \return  The key entered.
// *****************************************************************************

static int
GetMenuChoice( void )
{
    int lResult = 0;

    while( ( lResult < '1' ) || ( lResult > '9' ) )
    {
        lResult = toupper( GetKey() );

        if ( lResult == 'Q' )
        {
            break;
        }
    }

    return lResult;
}

// *****************************************************************************
// Function: DisplayDetections
//
/// \brief   Display detection as well as other information in a continuous
///          loop until the user presses a key.
// *****************************************************************************

static void
DisplayDetections( void )
{
    LtAcquisition lAcquisition;
    LtBool        lGetTemperature = LeddarConfigurationLevel() != LEDDAR_NO_CONFIGURATION;

    puts( "\nPress a key to start and then press a key again to stop." );
    GetKey();

    while( !KeyPressed() )
    {
        float lTemperature;

        if ( lGetTemperature )
        {
            if ( LeddarGetTemperature( &lTemperature ) != LT_SUCCESS )
            {
                puts( "Communication error, aborting." );
                break;
            }
        }

        if ( LeddarGetResults( &lAcquisition ) == LT_SUCCESS )
        {
            int i;
            LtDetection *lDetections = lAcquisition.mDetections;

            printf( "\nTimestamp    : %d\n", lAcquisition.mTimestamp );
            if ( lGetTemperature )
            {
                printf( "Temperature  : %.1f deg C\n", lTemperature );
            }
            printf( "LED Intensity: %d%%\n",
                    lAcquisition.mStates & LEDDAR_STATES_INTENSITY_MASK );
            printf( "Demerge status: %d\n\n",
                    (lAcquisition.mStates>>LEDDAR_STATES_DEMERGE_SHIFT) & 1 );

            for( i=0; i<lAcquisition.mDetectionCount; ++i )
            {
                printf( "%2d %6.2f %6.2f %2d\n", lDetections[i].mSegment,
                        lDetections[i].mDistance, lDetections[i].mAmplitude,
                        lDetections[i].mFlags );
            }
        }
        else
        {
            puts( "Communication error, aborting." );
            break;
        }
    }

    // Absorb the key used to stop the loop.
    GetKey();
}

// *****************************************************************************
// Function: DisplayConfiguration
//
/// \brief   Display the current configuration parameters. What is displayed
///          is adapted for the type or sensor (assumed not called when
///          sensor does not support configuration).
// *****************************************************************************

static void
DisplayConfiguration( void )
{
#define DC_COMM_ERROR "Communication Error!"

    LtU16 lValue;
    int   lLevel = LeddarConfigurationLevel();
    float lValuef;

    puts( "" );

    if ( lLevel == LEDDAR_FULL_CONFIGURATION )
    {
        printf( "Accumulation           : " );
        if ( LeddarGetParameter( LEDDAR_CONFIG_ACCUMULATION, &lValue ) == LT_SUCCESS )
        {
            printf( "%d (%d)\n", lValue, 1<<lValue );
        }
        else
        {
            puts( DC_COMM_ERROR );
        }

        printf( "Oversampling           : " );
        if ( LeddarGetParameter( LEDDAR_CONFIG_OVERSAMPLING, &lValue ) == LT_SUCCESS )
        {
            printf( "%d (%d)\n", lValue, 1<<lValue );
        }
        else
        {
            puts( DC_COMM_ERROR );
        }

        printf( "Base sample count      : " );
        if ( LeddarGetParameter( LEDDAR_CONFIG_SAMPLE_COUNT, &lValue ) == LT_SUCCESS )
        {
            printf( "%d\n", lValue );
        }
        else
        {
            puts( DC_COMM_ERROR );
        }
    }
    else if ( lLevel == LEDDAR_SIMPLE_CONFIGURATION )
    {
        printf( "Sampling rate          : " );
        if ( LeddarGetParameter( LEDDAR_CONFIG_RATE, &lValue ) == LT_SUCCESS )
        {
            printf( "%d (%g Hz)\n", lValue, 12800.f/(1<<lValue) );
        }
        else
        {
            puts( DC_COMM_ERROR );
        }
    }

    printf( "Detection threshold    : " );
    if ( LeddarGetThreshold( &lValuef ) == LT_SUCCESS )
    {
        printf( "%.2f\n", lValuef );
    }
    else
    {
        puts( DC_COMM_ERROR );
    }

    printf( "LED power              : " );
    if ( LeddarGetParameter( LEDDAR_CONFIG_LED_POWER, &lValue ) == LT_SUCCESS )
    {
        printf( "%d%%\n", lValue );
    }
    else
    {
        puts( DC_COMM_ERROR );
    }

    printf( "Change delay           : " );
    if ( LeddarGetParameter( LEDDAR_CONFIG_CHANGE_DELAY, &lValue ) == LT_SUCCESS )
    {
        printf( "%d\n", lValue );
    }
    else
    {
        puts( DC_COMM_ERROR );
    }

    if ( LeddarGetParameter( LEDDAR_CONFIG_OPTIONS, &lValue ) == LT_SUCCESS )
    {
        printf( "Automatic LED intensity: %s\n",
                (lValue&LEDDAR_OPTION_AUTO_LED) ? "Enabled" : "Disabled" );
        printf( "Object demerging       : %s\n",
                (lValue&LEDDAR_OPTION_DEMERGE) ? "Enabled" : "Disabled" );
    }
    else
    {
        printf( "Automatic LED intensity: %s\n", DC_COMM_ERROR );
        printf( "Object demerging       : %s\n", DC_COMM_ERROR );
    }

    printf( "Maximum detections     : " );
    if ( LeddarGetParameter( LEDDAR_CONFIG_MAX_DETECTIONS, &lValue ) == LT_SUCCESS )
    {
        printf( "%d\n", lValue );
    }
    else
    {
        puts( DC_COMM_ERROR );
    }

}

// *****************************************************************************
// Function: ConfigurationMenu
//
/// \brief   Display a menu to allow changing configuration parameters.
// *****************************************************************************

static void
ConfigurationMenu( void )
{
    for(;;)
    {
        int      lChoice;
        float    lValue;
        LtResult lResult;
        int      lLevel = LeddarConfigurationLevel();

        puts( "\n" );
        if ( lLevel == LEDDAR_FULL_CONFIGURATION )
        {
            puts( "1. Change accumulation" );
            puts( "2. Change oversampling" );
            puts( "3. Change base sample count" );
        }
        else if ( lLevel == LEDDAR_SIMPLE_CONFIGURATION )
        {
            puts( "4. Change measurement rate" );
        }
        puts( "5. Change detection threshold offset" );
        puts( "6. Change LED power" );
        puts( "7. Change options" );
        puts( "8. Change Change delay" );
        puts( "9. Change maximum detection count" );
        puts( "Q. Exit" );

        lChoice = GetMenuChoice();

        if ( lChoice == 'Q' )
        {
            break;
        }

        printf( "\nEnter new value: " );
        scanf( "%f", &lValue );

        switch( lChoice )
        {
            case '1':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_ACCUMULATION, (LtU16) lValue );
                break;
            case '2':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_OVERSAMPLING, (LtU16) lValue );
                break;
            case '3':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_SAMPLE_COUNT, (LtU16) lValue );
                break;
            case '4':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_RATE, (LtU16) lValue );
                break;
            case '5':
                lResult = LeddarSetThreshold( lValue );
                break;
            case '6':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_LED_POWER, (LtU16) lValue );
                break;
            case '7':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_OPTIONS, (LtU16) lValue );
                break;
            case '8':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_CHANGE_DELAY, (LtU16) lValue );
                break;
            case '9':
                lResult = LeddarSetParameter( LEDDAR_CONFIG_MAX_DETECTIONS, (LtU16) lValue );
                break;
            default:
                lResult = LT_SUCCESS;
                break;
        }

        if ( lResult != LT_SUCCESS )
        {
            puts( "Operation failed!" );
        }
    }
}

// *****************************************************************************
// Function: ConnectMenu
//
/// \brief   Display a menu of actions that can be performed when connected.
// *****************************************************************************

static void
ConnectMenu( void )
{
    char  lBuffer[LT_MAX_PORT_NAME_LEN+1];
    int   lAddress;

    printf( "\nPlease enter the port name: " );
    scanf( "%s", lBuffer );
    printf( "Please enter the MODBUS address: " );
    scanf( "%d", &lAddress );

    if ( LeddarConnect( lBuffer, lAddress ) == LT_SUCCESS )
    {
        for(;;)
        {
            int lChoice;

            puts( "\n\n1. Display detections" );
            if ( LeddarConfigurationLevel() > LEDDAR_NO_CONFIGURATION )
            {
                puts( "2. Display configuration" );
                puts( "3. Change configuration" );
            }
            puts( "4. Disconnect" );

            lChoice = GetMenuChoice();

            switch( lChoice )
            {
                case '1':
                    DisplayDetections();
                    break;
                case '2':
                    DisplayConfiguration();
                    break;
                case '3':
                    ConfigurationMenu();
                    break;
                case '4':
                    LeddarDisconnect();
                    return;
            }
        }
    }
    else
    {
        puts( "\nConnection failed!" );
    }
}

// *****************************************************************************
// Function: MainMenu
//
/// \brief   Display the main menu (connect or quit).
// *****************************************************************************

static void
MainMenu( void )
{
    for(;;)
    {
        int lChoice;

        puts( "\n\n1. Connect" );
        puts( "2. Quit" );

        lChoice = GetMenuChoice();

        switch( lChoice )
        {
            case '1':
                ConnectMenu();
                break;
            case '2':
            case 'Q':
                return;
        }
    }
}

// *****************************************************************************
// Function: main
//
/// \brief   Standard C entry point!
// *****************************************************************************

int
main( int argc, char *argv[] )
{
    puts( "*******************************************************" );
    puts( "* Welcome to the Leddar RS-485 Demonstration Program! *" );
    puts( "*******************************************************" );

    MainMenu();

    return 0;
}

// End of file Main.c

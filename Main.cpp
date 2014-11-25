#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "Leddar.h"

static void
DisplayDetections( void )
{
    LtAcquisition lAcquisition;

    puts( "\nPress a key to start and then press a key again to stop." );
    GetKey();

    while( !KeyPressed() )
    {
        if ( LeddarGetResults( &lAcquisition ) == LT_SUCCESS )
        {
            int i;
            LtDetection *lDetections = lAcquisition.mDetections;

            printf( "\nTimestamp    : %d\n", lAcquisition.mTimestamp );
            {
                printf( "Temperature  : %.1f deg C\n", lAcquisition.mTemperature );
            }

            for( i=0; i<lAcquisition.mDetectionCount; ++i )
            {
                printf( "%7.3f %6.2f    ",
                        lDetections[i].mDistance, lDetections[i].mAmplitude );
            }

            puts( "" );
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

int main( int argc, char *argv[] )
{
    puts( "*******************************************************" );
    puts( "* Welcome to the Leddar Serial Demonstration Program! *" );
    puts( "*******************************************************" );

    char  lBuffer[LT_MAX_PORT_NAME_LEN+1] = "ttyUSB0";
    int   lAddress = 1;

    if ( LeddarConnect( lBuffer, lAddress ) == LT_SUCCESS )
    {
        DisplayDetections();
        LeddarDisconnect();
    }
    else
    {
        puts( "\nConnection failed!" );
    }

    return 0;
}

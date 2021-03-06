/*
A module to handle the actual orientation of an alt-azimuth mount telescope

Author and copyright of this file:
Chris Dick, 2015

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "TelescopeOrientation.h"
#include "HalMagnetometer.h"
#include "HalAccelerometer.h"
#include "Config.h"
#include <math.h>
#ifdef CALIBRATE_DEBUG
#include <stdio.h>
#endif
TelescopeOrientation TelescopeOrientation::Orient;

/* TelescopeOrientation
 *  Constructor
 */
TelescopeOrientation::TelescopeOrientation( void )
{
}

/* initialise the sensors used
 */
bool TelescopeOrientation::TelescopeOrientationInit( void )
{
    HalAccelerometer::Accelerometer.HalAccelerometerInit();
    HalMagnetometer::Magneto.HalMagnetometerInit();
    return true;
}


/* TelescopeOrientationRun
 *  Runs the Accelerometer and the Magnetometer.
 */
void TelescopeOrientation::Run( void )
{
    HalMagnetometer::Magneto.Run();
    HalAccelerometer::Accelerometer.Run();
#ifdef CALIBRATE_DEBUG
    float Mx, My, Mz;
    /*
       keep track of the Magnetometer calibration values
    */
    HalMagnetometer::Magneto.HalMagnetometerGetAll( &Mx, &My, &Mz );
    if (Mx > MxMax)
    {
        MxMax = Mx;
    }
    if (Mx < MxMin)
    {
        MxMin = Mx;
    }
    if (My > MyMax)
    {
        MyMax = My;
    }
    if (My < MyMin)
    {
        MyMin = My;
    }
    if (Mz > MzMax)
    {
        MzMax = Mz;
    }
    if (Mz < MzMin)
    {
        MzMin = Mz;
    }
    printf ("MxMax: %f MxMin: %f  MyMax: %f  MyMin: %f  MzMax: %f  MzMin: %f  \n\r", MxMax, MxMin, MyMax, MyMin, MzMax, MzMin ); // debug
#endif
}

/*
 *
 */
float TelescopeOrientation::TelescopeOrientationGetHeading( void )
{
    float heading = 0;
    float XComponent = 0;
    float YComponent = 0;
    float Mx, My, Mz;
    float Ax, Ay, Az, Axn, Azn;
    float Mxo, Myo, Mzo; /* magneto values with offset */
    /*
        get filtered sensor data
    */
    HalAccelerometer::Accelerometer.HalAccelerometerGetAll( &Ax, &Ay, &Az );
    HalMagnetometer::Magneto.HalMagnetometerGetAll( &Mx, &My, &Mz );

    /*
        remove Hard Iron effects
    */
    Mxo = Mx - CONFIG_MX_OFFSET;
    Myo = My - CONFIG_MY_OFFSET;
    Mzo = Mz - CONFIG_MZ_OFFSET;
    
    /*
        Normalise
    */
    Mxo = Mxo / (CONFIG_MxMax - CONFIG_MX_OFFSET);
    Myo = Myo / (CONFIG_MyMax - CONFIG_MY_OFFSET);
    Mzo = Mzo / (CONFIG_MzMax - CONFIG_MZ_OFFSET);
    Axn = Ax / ( sqrt((Ax*Ax) + (Az*Az)));
    Azn = Az / ( sqrt((Ax*Ax) + (Az*Az)));
    /*
        Calculate heading
    */
    XComponent = (Mxo*Azn) + (Mzo*Axn) ;
    YComponent = Myo;
    heading = atan2(-YComponent,XComponent);
    /*
        heading must be between 0' and 360'
    */
    if (heading > (2*M_PI)) 
    {
        heading -= (2*M_PI);
    }
    if (heading < 0) 
    {
        heading += (2*M_PI);
    }
    /* 
        need to invert the heading so east is +90'
    */
    return (heading);
}


/* Calculates the  Pitch
 * see http://www.st.com/web/en/resource/technical/document/application_note/CD00268887.pdf
 */
float TelescopeOrientation::TelescopeOrientationGetPitch( void )
{
    float Pitch;
    float Ax, Ay, Az;
    HalAccelerometer::Accelerometer.HalAccelerometerGetAll( &Ax, &Ay, &Az );

    /*
        Calculate Pitch
    */
    Pitch = asin ( Ax / ( sqrt( (Ax*Ax) + (Ay*Ay) + ( Az*Az) ) ) );
    return Pitch;
}

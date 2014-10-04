// Implementation of the DGE G-sensor.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>

#include "LcStdev.h"

namespace LCSYS
{
	TLC_SENSOR	g_sensor[LC_MAX_SENSOR]={{0,},};
}



INT LcStv_SensorOpen()	{	return LC_OK;	}
INT LcStv_SensorClose()	{	return LC_OK;	}


// Write data
INT LcStv_SensorWrite(INT n, TLC_SENSOR* v)
{
	memcpy(&LCSYS::g_sensor[n], v, sizeof(TLC_SENSOR));
	return LC_OK;
}


// Read data
INT LcStv_SensorRead(INT n, TLC_SENSOR* v)
{
	memcpy(v, &LCSYS::g_sensor[n], sizeof(TLC_SENSOR));
	return LC_OK;
}


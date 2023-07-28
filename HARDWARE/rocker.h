#ifndef __ROCKER_H
#define __ROCKER_H	 
#include "sys.h"

uint16_t ROCKER_AdcToUint8(uint16_t uiadc);
uint16_t ROCKER_Uint8ToAdc(uint16_t ui8);
uint16_t ROCKER_Uint8ToVol(uint16_t ui8);

#endif

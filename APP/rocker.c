#include "rocker.h"

uint16_t ROCKER_AdcToUint8(uint16_t uiadc)
{
    return (uiadc>>4);//12bit to 8bit 
}

uint16_t ROCKER_Uint8ToAdc(uint16_t ui8)
{
    return (ui8<<4);//8bit to 12bit 
}

uint16_t ROCKER_Uint8ToVol(uint16_t ui8)
{
    return (ui8 * 3300 / 255);//8bit to 12bit 
}


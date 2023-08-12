#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

void Adc_PinInit(void);
void Adc_InitAdc1(void);
// void Adc_InitAdc2(void);
u16  Adc_GetAdc1(u8 ch); 
// u16  Adc_GetAdc2(u8 ch); 
u16 Adc_GetAdcAverage1(u8 ch,u8 times); 
// u16 Adc_GetAdcAverage2(u8 ch,u8 times); 
 
#endif 

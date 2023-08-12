#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H	 
#include "sys.h"

void bsp_interrupt_callback(void);
void bsp_delay_us(uint32_t delay_ust);
void bsp_delay_ms(uint32_t delay_mst);

#endif

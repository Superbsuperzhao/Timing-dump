#include "bsp_delay.h"

volatile uint32_t TickIntCount = 0;
void bsp_interrupt_callback(void)
{
    TickIntCount++;
}

uint32_t bsp_get_system_time(void)
{
    return (((SysTick->LOAD - SysTick->VAL) / 72) + (TickIntCount * 1000));
}

void bsp_delay_us(uint32_t delay_ust)
{
    uint32_t temp_us = 0;
    temp_us = bsp_get_system_time();
    while (delay_ust > (bsp_get_system_time() - temp_us))
        ;
}

void bsp_delay_ms(uint32_t delay_mst)
{
    bsp_delay_us(delay_mst * 1000);
}

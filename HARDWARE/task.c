#include "led.h"

unsigned int count_1ms=0;
unsigned int count_10ms=0;
unsigned int count_50ms=0;
unsigned int count_100ms=0;
unsigned int count_1000ms=0;
unsigned int count_always=0;
void task_counter(void)
{
	count_always++;
	count_1ms++;
	count_10ms++;
	count_50ms++;
	count_100ms++;
	count_1000ms++;
}




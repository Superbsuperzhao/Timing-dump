#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_MINI_BOARD PCout(13)	// 
#define LED_SYS PDout(2)	// 
#define LED_BAT  PBout(12)	// 
#define KEY_PIN  PBin(11)	// 

#define MOS_G PBout(9)	// 

#define BUZZER_IO PAout(8)	// 

void LED_Init(void);//初始化
void BUZZER_Init(void);//初始化

#endif

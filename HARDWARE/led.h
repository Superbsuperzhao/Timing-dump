#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED_MINI_BOARD PCout(13)	// 
#define LED_SYS PDout(2)	// 
#define LED_BAT PBout(5)	// 

#define MOS_G PCout(0)	// 

#define BUZZER_IO PAout(8)	// 


void LED_Init(void);//��ʼ��
void BUZZER_Init(void);//��ʼ��
#endif

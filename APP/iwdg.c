#include "iwdg.h"
#include "led.h"


void IWDG_Init(u8 prer,u16 rlr)//���ʱ��= Ԥ��Ƶֵ*����װ��ֵ+1��/40 ms
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
	
	IWDG_SetPrescaler(prer);  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64 0->4��Ƶ 1->8��Ƶ 4->64��Ƶ 
	
	IWDG_SetReload(rlr);  //����IWDG��װ��ֵ
	
	IWDG_ReloadCounter();  //����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
	
	IWDG_Enable();  //ʹ��IWDG
}
//ι�������Ź�
void IWDG_Feed(void)
{   
 	IWDG_ReloadCounter();										   
}


 


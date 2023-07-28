#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"



#define RB_IO PBout(12) //��ǰ��
#define RF_IO PBout(14) //�����
#define LF_IO PBout(15) //�Һ���
#define LB_IO PBout(13) //��ǰ��

typedef enum
{
    STOP_OR_SPIN = 0,
    FORWORD,
    BACKWORD,
    TURN_LEFT,
    TURN_RIGHT,
    FOCRO_STOP 
}CAR_STATE;
//#define in1 PAout(0)
//#define in2 PAout(1)
//#define in3 PAout(2)
//#define in4 PAout(3)

void car_state(int vx,int vy);
void MotorIO_Init(void);               // PB7 PB8 PB9   for pwm
int32_t SlidingMedianFilter(int32_t FilterIn);//������ֵ�˲�
int16_t CalculateSmoothYaw(int16_t MutatedYaw);//



#endif

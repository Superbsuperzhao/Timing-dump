#include "motor.h"
#include "usart.h"

 
void MotorIO_Init(void)               // PB7 PB8 PB9   for pwm
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
		
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	//motor io
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
	 GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

}


void SingleMotorPower_LF(uint8_t Dirction,uint16_t Power)//Max Power：1600
{
	LF_IO = Dirction;
	if(Power > 1599)
		Power = 1599;
	if(Dirction == 1)
	{
		Power = 1599 - Power;
	}
	TIM_SetCompare4(TIM4,Power);
}

void SingleMotorPower_LB(uint8_t Dirction,uint16_t Power)
{
	LB_IO = Dirction;
	if(Power > 1599)
		Power = 1599;
	if(Dirction == 1)
	{
		Power = 1599 - Power;
	}
	TIM_SetCompare2(TIM4,Power);
}

void SingleMotorPower_RF(uint8_t Dirction,uint16_t Power)
{
	RF_IO = Dirction;
	if(Power > 1599)
		Power = 1599;
	if(Dirction == 1)
	{
		Power = 1599 - Power;
	}
	TIM_SetCompare3(TIM4,Power);
}

void SingleMotorPower_RB(uint8_t Dirction,uint16_t Power)
{
	RB_IO = Dirction;
	if(Power > 1599)
		Power = 1599;
	if(Dirction == 1)
	{
		Power = 1599 - Power;
	}
	TIM_SetCompare1(TIM4,Power);
}
CAR_STATE direction;
u16 debug_data = 0;
u16 LeftPower = 0;
u16 RightPower = 0;
u8 LeftDir = 0;
u8 RightDir = 0;
void car_state(int vx,int vy)
{
	static CAR_STATE last_direction;
	static int last_yaw = 0;
	static int adj_coefficient = 100;
	if(vy < 1500)
	{
		if(last_direction == BACKWORD)
		{
			direction = FOCRO_STOP;//防止电流冲击
		}
		else
		{
			direction = FORWORD;
		}
	}
	else if(vy > 1800)
	{
		if(last_direction == FORWORD)
		{
			direction = FOCRO_STOP;//防止电流冲击
		}
		else
		{
			direction = BACKWORD;
		}
	}
	else
	{
		if(vx < 1500)//left
		{
			if(last_direction == TURN_RIGHT)
			{
				direction = FOCRO_STOP;//防止电流冲击
			}
			else
			{			
				direction = TURN_LEFT;//
			}
		}
		else if(vx > 1800)//right
		{
			if(last_direction == TURN_LEFT)
			{
				direction = FOCRO_STOP;//防止电流冲击
			}
			else
			{			
				direction = TURN_RIGHT;//
			}
		}
		else
		{
			direction = STOP_OR_SPIN;//停止或原地旋转
		}
	}
	last_direction = direction;
	switch(direction)
	{
		case STOP_OR_SPIN://停止或原地旋转

			LeftDir = 0;
			RightDir = 0;
			LeftPower = 0;
			RightPower = 0;
		
			last_yaw = yaw_smooth;
			break;

		case TURN_LEFT:
			LeftDir = 1;
			RightDir = 0;
			LeftPower = 1500;
			RightPower = 1500;
			break;

		case TURN_RIGHT:
			LeftDir = 0;
			RightDir = 1;
			LeftPower = 1500;
			RightPower = 1500;
			break;
			
		case FORWORD://forword
			LeftDir = 0;
			RightDir = 0;
			LeftPower = (1500-vy)*1600/1500;
			RightPower = (1500-vy)*1600/1500;

			adj_coefficient = (yaw_smooth - last_yaw)*5;
			if(adj_coefficient>50)
				adj_coefficient = 50;
			else if(adj_coefficient < -50)
				adj_coefficient = -50;
			
			LeftPower = LeftPower * (100+adj_coefficient) /100;
			RightPower = RightPower * (100 -adj_coefficient) /100;

		break;
		
		case BACKWORD://backword
			LeftDir = 1;
			RightDir = 1;
			LeftPower = (vy-1800)*1600/1500;
			RightPower = (vy-1800)*1600/1500;

			adj_coefficient = (yaw_smooth - last_yaw)*5;
			if(adj_coefficient>50)
				adj_coefficient = 50;
			else if(adj_coefficient < -50)
				adj_coefficient = -50;
			
			LeftPower = LeftPower * (100-adj_coefficient) /100;
			RightPower = RightPower * (100 +adj_coefficient) /100;
		break;

		case FOCRO_STOP://test
			LeftDir = 0;
			RightDir = 0;
			LeftPower = 0;
			RightPower = 0;
			break;

		default:
			LeftDir = 0;
			RightDir = 0;
			LeftPower = 0;
			RightPower = 0;
			break;
	}
	SingleMotorPower_RF(RightDir,RightPower);
	SingleMotorPower_RB(RightDir,RightPower);
	SingleMotorPower_LF(LeftDir,LeftPower);
	SingleMotorPower_LB(LeftDir,LeftPower);

}

int16_t CalculateSmoothYaw(int16_t MutatedYaw)//
{
	static int Currentloop = 0;
	static int LastYaw = 0;
	static int SmoothYaw = 0;
	if(MutatedYaw - LastYaw > 300)
	{
		Currentloop++;
	}
	else if(MutatedYaw - LastYaw < -300)
	{
		Currentloop--;
	}
	else
	{

	}
	LastYaw = MutatedYaw;
	SmoothYaw = MutatedYaw - Currentloop*360;
	return SmoothYaw;
}


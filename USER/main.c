#include "led.h"
#include "delay.h"
#include "sys.h"
// #include "key.h"
#include "adc.h"
#include "spi.h"
#include "timer.h"
#include "task.h"
#include "motor.h"
#include "usart.h"
#include "iwdg.h"
#include "main.h"
#include "rocker.h"

u16 ADC_BatVol, ADC_CH17, ADC_MotorCur; // ADC采样值

int Pack_Vol;  // mv
int Motor_Cur; // ma
char Battery_SOC = 0;
u16 SOC_DisplayCount = 0; // 电量指示闪烁

void TASK_1MS(void)
{
}

uint16_t NrfSendCount = 0; // 统计1s内发送packet数
uint16_t debugCount = 0;   //

void Remoto_Task(void) // 2.4G通讯任务
{
    static uint8_t CommDir = 0;    // 通讯方向 send 1  recv 0
    static uint8_t SendStatus = 0; // 通讯方向 send 1  recv 0

    static int NrfRxTimeout = 0; // 600ms
    static int vx, vy;

    debugCount++;
}

void TASK_10MS(void)
{
    static int NrfRxTimeout = 0; // 600ms
    static int vx, vy;

    Remoto_Task();
}

#define DIVIDER_RESISTOR_L 4700.0f
#define DIVIDER_RESISTOR_H 1000.0f
// #define DIVIDER_COEFFICENT ((float)(DIVIDER_RESISTOR_L+DIVIDER_RESISTOR_H)/DIVIDER_RESISTOR_L)
#define DIVIDER_COEFFICENT (5.7f)

#define SERIES_NUM (4)

void TASK_50MS(void)
{
    if ((Pack_Vol / SERIES_NUM) <= 330)
    {
        Battery_SOC = 0;
    }
    else
    {
        Battery_SOC = ((Pack_Vol / SERIES_NUM) - 330);
    }

    if (Battery_SOC > 100)
        Battery_SOC = 100;

    SOC_DisplayCount++;
    if (Battery_SOC > 0)
    {
        if (SOC_DisplayCount < Battery_SOC)
        {
            if (SOC_DisplayCount % 5 == 0) // 0.5s亮一下
            {
                LED_BAT = ~LED_BAT; // wink
            }
        }
        else if ((SOC_DisplayCount >= Battery_SOC) && (SOC_DisplayCount < (Battery_SOC + 60))) // 闪灯后灭3s
        {
            LED_BAT = 1; // off light
        }
        else
        {
            SOC_DisplayCount = 0;
        }

        BUZZER_IO = 0;
    }
    else // 电量过低
    {
        LED_BAT = ~LED_BAT; // wink

        BUZZER_IO = 1; // low power
    }
}

int Vref = 0; // mv

void TASK_100MS(void)
{
    ADC_MotorCur = Get_Adc_Average(ADC_Channel_1, 1); // 电机电流采样
    Motor_Cur = Vref * (float)ADC_MotorCur / (4095 * 0.05);
}

#define RUN_SOC 25
#define STOP_SOC 20
void TASK_1000MS(void)
{
    LED_SYS = ~LED_SYS; // 系统工作指示灯

    ADC_CH17 = Get_Adc_Average(ADC_Channel_17, 1);
    Vref = 4095 * 1.2 * 1000 / (float)ADC_CH17;                                 // 典型值3300mv
    ADC_BatVol = Get_Adc_Average(ADC_Channel_0, 1);                             // 电池电压采样
    Pack_Vol = DIVIDER_COEFFICENT * 100 * 1.2 * (float)ADC_BatVol / (ADC_CH17); // 单位0.01v

    if (Battery_SOC >= RUN_SOC)
    {
        MOS_G = 1; // turn on the dump
    }
    else if(Battery_SOC < STOP_SOC)
    {
        MOS_G = 0; // turn off the dump
    }

    IWDG_Feed(); // 喂狗
}

void CONTROL_Loop(void)
{
    if (count_1ms >= 1)
    {
        count_1ms = 0;
        TASK_1MS();
    }
    if (count_10ms >= 10)
    {
        count_10ms = 0;
        TASK_10MS();
        Remoto_Task();
    }
    if (count_50ms >= 50)
    {
        count_50ms = 0;
        TASK_50MS();
    }
    if (count_100ms >= 100)
    {
        count_100ms = 0;
        TASK_100MS();
    }
    if (count_1000ms >= 1000)
    {
        count_1000ms = 0;
        TASK_1000MS();
    }
}

int main(void)
{
    DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);

    SysTick_Config(SystemCoreClock / 1000);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组2
    // uart1_Init(115200);	 	//串口初始化
    usart2_Init(115200); // 串口初始化
    LED_Init();          // 初始化与LED连接的硬件接口
    BUZZER_Init();       // 初始化与LED连接的硬件接口
    Adc_Init();          // ADC初始化	  1 is x  2 is y
    MotorIO_Init();
    MotorPWM_Init(1599, 9); // 36M / 1600 / 9 = 2.5K
    // MotorPWM_Init(1599,450);//36M / 1600 / 9 = 2.5K
    car_state(3300 / 2, 3300 / 2); // 上电停车
    IWDG_Init(5, 467);             // 预分频数为64,重载值为467,溢出时间为1.5s

    while (1)
    {
        CONTROL_Loop();
    }
}

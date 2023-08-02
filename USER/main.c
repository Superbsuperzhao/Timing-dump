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

u16 ADC_BatVol, ADC_CH17, ADC_MotorCur; // ADC����ֵ

int Pack_Vol;  // mv
int Motor_Cur; // ma
char Battery_SOC = 0;
u16 SOC_DisplayCount = 0; // ����ָʾ��˸

void TASK_1MS(void)
{
}

uint16_t NrfSendCount = 0; // ͳ��1s�ڷ���packet��
uint16_t debugCount = 0;   //

void Remoto_Task(void) // 2.4GͨѶ����
{
    static uint8_t CommDir = 0;    // ͨѶ���� send 1  recv 0
    static uint8_t SendStatus = 0; // ͨѶ���� send 1  recv 0

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
            if (SOC_DisplayCount % 5 == 0) // 0.5s��һ��
            {
                LED_BAT = ~LED_BAT; // wink
            }
        }
        else if ((SOC_DisplayCount >= Battery_SOC) && (SOC_DisplayCount < (Battery_SOC + 60))) // ���ƺ���3s
        {
            LED_BAT = 1; // off light
        }
        else
        {
            SOC_DisplayCount = 0;
        }

        BUZZER_IO = 0;
    }
    else // ��������
    {
        LED_BAT = ~LED_BAT; // wink

        BUZZER_IO = 1; // low power
    }
}

int Vref = 0; // mv

void TASK_100MS(void)
{
    ADC_MotorCur = Get_Adc_Average(ADC_Channel_1, 1); // �����������
    Motor_Cur = Vref * (float)ADC_MotorCur / (4095 * 0.05);
}

#define RUN_SOC 25
#define STOP_SOC 20
void TASK_1000MS(void)
{
    LED_SYS = ~LED_SYS; // ϵͳ����ָʾ��

    ADC_CH17 = Get_Adc_Average(ADC_Channel_17, 1);
    Vref = 4095 * 1.2 * 1000 / (float)ADC_CH17;                                 // ����ֵ3300mv
    ADC_BatVol = Get_Adc_Average(ADC_Channel_0, 1);                             // ��ص�ѹ����
    Pack_Vol = DIVIDER_COEFFICENT * 100 * 1.2 * (float)ADC_BatVol / (ADC_CH17); // ��λ0.01v

    if (Battery_SOC >= RUN_SOC)
    {
        MOS_G = 1; // turn on the dump
    }
    else if(Battery_SOC < STOP_SOC)
    {
        MOS_G = 0; // turn off the dump
    }

    IWDG_Feed(); // ι��
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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����2
    // uart1_Init(115200);	 	//���ڳ�ʼ��
    usart2_Init(115200); // ���ڳ�ʼ��
    LED_Init();          // ��ʼ����LED���ӵ�Ӳ���ӿ�
    BUZZER_Init();       // ��ʼ����LED���ӵ�Ӳ���ӿ�
    Adc_Init();          // ADC��ʼ��	  1 is x  2 is y
    MotorIO_Init();
    MotorPWM_Init(1599, 9); // 36M / 1600 / 9 = 2.5K
    // MotorPWM_Init(1599,450);//36M / 1600 / 9 = 2.5K
    car_state(3300 / 2, 3300 / 2); // �ϵ�ͣ��
    IWDG_Init(5, 467);             // Ԥ��Ƶ��Ϊ64,����ֵΪ467,���ʱ��Ϊ1.5s

    while (1)
    {
        CONTROL_Loop();
    }
}

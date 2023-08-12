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
#include "oled.h"
#include "string.h"

#define DIVIDER_RESISTOR_H 10.0f   // K
#define DIVIDER_RESISTOR_L 2.0f    // K
#define CUR_SAMPLE_RESISTOR_L 0.1f // R

#define ROW_HIGHT (12)

#define COLUMN_OFFSET (64)

#define PAGE_CONTENT (8) // 每页显示的数量

#define DIVIDER_COEFFICENT (((float)(DIVIDER_RESISTOR_L + DIVIDER_RESISTOR_H)) / DIVIDER_RESISTOR_L)
// #define DIVIDER_COEFFICENT (6.0f)

#define DEFAULT_DEFAULT_SERIES_NUM (4)

#define LI_BATTERY_UNDER_VOL 3300

#define RUN_SOC 25
#define STOP_SOC 20

SOLAR_SYSTEM_T SolarSystem;
ADC_RAW_T AdcRaw;
DISPLAY_T Display[16];

uint16_t SOC_DisplayCount = 0; // 电量指示闪烁

uint8_t SeriesNum = DEFAULT_DEFAULT_SERIES_NUM;

uint16_t SlidingMedianFilter(uint16_t FilterIn, uint16_t *pNoOrder) // 滑动中值滤波
{
    uint16_t OrderArr[FIlTER_LENTH] = {0}; // 从小到大排列
    int8_t i, j;
    uint16_t TempDate;

    for (i = 0; i < FIlTER_LENTH - 1; i++)
    {
        pNoOrder[i] = pNoOrder[i + 1];
    }
    pNoOrder[FIlTER_LENTH - 1] = FilterIn;
    memcpy(OrderArr, pNoOrder, sizeof(OrderArr));

    for (j = 0; j < FIlTER_LENTH - 1; j++)
    {
        for (i = 0; i < FIlTER_LENTH - 1 - j; i++)
        {
            if (OrderArr[i] > OrderArr[i + 1])
            {
                TempDate = OrderArr[i];
                OrderArr[i] = OrderArr[i + 1];
                OrderArr[i + 1] = TempDate;
            }
        }
    }
    TempDate = OrderArr[FIlTER_LENTH / 2];
    return TempDate;
}

void TASK_CalculateSeriesNum(void)
{
    if (SolarSystem.Pack_Vol > LI_BATTERY_UNDER_VOL * 4 * 0.95)
    {
        SeriesNum = 4;
    }
    else if (SolarSystem.Pack_Vol > LI_BATTERY_UNDER_VOL * 3 * 0.95)
    {
        SeriesNum = 3;
    }
    else if (SolarSystem.Pack_Vol > LI_BATTERY_UNDER_VOL * 2 * 0.95)
    {
        SeriesNum = 2;
    }
    else if (SolarSystem.Pack_Vol > LI_BATTERY_UNDER_VOL * 1 * 0.95)
    {
        SeriesNum = 1;
    }
    else
    {
        SeriesNum = DEFAULT_DEFAULT_SERIES_NUM;
    }
}

void TASK_Display(void)
{
    static uint8_t PageCount = 0;
    static uint8_t PageNumber = 0;
    static uint8_t PageIndex = 1;
    uint8_t DisplayIndex = 0;
    uint8_t XCoordinate = 0;
    uint8_t YCoordinate = 0;
    uint8_t NumberOffset = 0;
    uint8_t UnitOffset = 0;
    uint8_t i = 0;

    PageCount++;
    while (Display[DisplayIndex].Name[0] != 0)
    {
        PageNumber++;
        DisplayIndex++;
    }
    PageNumber <= 8 ? PageNumber = 1 : (PageNumber = PageNumber / 8 + 1);

    if (PageCount > 3 && PageNumber > 1)
    {
        PageCount = 0;
        if (PageIndex >= PageNumber)
        {
            PageIndex = 1;
        }
        else
        {
            PageIndex++;
        }
        
        OLED_Clear();
    }
    DisplayIndex = (PageIndex - 1) * 8;
    for (i = 0; i < 8; i++)
    {
        if(Display[DisplayIndex].Name[0] == 0)
        {
            break;
        }
        XCoordinate = (DisplayIndex % 2) * COLUMN_OFFSET;
        YCoordinate = (((DisplayIndex%8) / 2) + 1) * ROW_HIGHT;

        NumberOffset = 6 * strlen(Display[DisplayIndex].Name) + XCoordinate;
        UnitOffset = 6 * Display[DisplayIndex].NumberLenth + NumberOffset;

        OLED_ShowString(XCoordinate, YCoordinate, Display[DisplayIndex].Name, 12);
        OLED_ShowNum(NumberOffset, YCoordinate, Display[DisplayIndex].Number, Display[DisplayIndex].NumberLenth, 12); // 显示ASCII字符的码值
        OLED_ShowString(UnitOffset, YCoordinate, Display[DisplayIndex].Unit, 12);
        DisplayIndex++;
    }
    OLED_ShowString(20, 0, "Solar System", 12);

    // while (Display[DisplayIndex].Name[0] != 0)
    // {
    //     XCoordinate = (DisplayIndex % 2) * COLUMN_OFFSET;
    //     YCoordinate = ((DisplayIndex / 2) + 1) * ROW_HIGHT;
    //     NumberOffset = 6 * strlen(Display[DisplayIndex].Name) + XCoordinate;
    //     UnitOffset = 6 * Display[DisplayIndex].NumberLenth + NumberOffset;

    //     OLED_ShowString(XCoordinate, YCoordinate, Display[DisplayIndex].Name, 12);
    //     OLED_ShowNum(NumberOffset, YCoordinate, Display[DisplayIndex].Number, Display[DisplayIndex].NumberLenth, 12); // 显示ASCII字符的码值
    //     OLED_ShowString(UnitOffset, YCoordinate, Display[DisplayIndex].Unit, 12);

    //     DisplayIndex++;
    // }
}

void TASK_1MS(void)
{
}

void TASK_10MS(void)
{
}

void TASK_50MS(void)
{
    static char BlinkSoc = 0;
    if ((SolarSystem.Pack_Vol / SeriesNum) <= 3300)
    {
        SolarSystem.Battery_SOC = 0;
    }
    else
    {
        SolarSystem.Battery_SOC = ((SolarSystem.Pack_Vol / SeriesNum) - 3300) / 9;
    }

    if (SolarSystem.Battery_SOC > 100)
        SolarSystem.Battery_SOC = 100;

    BlinkSoc = SolarSystem.Battery_SOC / 10 * 10;
    SOC_DisplayCount++;
    if (BlinkSoc > 0)
    {
        if (SOC_DisplayCount < (BlinkSoc))
        {
            if (SOC_DisplayCount % 5 == 0) // 0.5s亮一下
            {
                LED_BAT = ~LED_BAT; // wink
            }
        }
        else if ((SOC_DisplayCount >= BlinkSoc) && (SOC_DisplayCount < (BlinkSoc + 60))) // 闪灯后灭3s
        {
            LED_BAT = 0; // off light
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

void TASK_200MS(void)
{
    AdcRaw.ADC_CH17 = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_17, 1), AdcRaw.ADC_CH17NoOrder);
    AdcRaw.ADC_SolarCur = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_0, 1), AdcRaw.ADC_SolarCurNoOrder);
    AdcRaw.ADC_LedCur = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_1, 1), AdcRaw.ADC_LedCurNoOrder);
    AdcRaw.ADC_SolarVol = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_2, 1), AdcRaw.ADC_SolarVolNoOrder);
    AdcRaw.ADC_SystemCur = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_3, 1), AdcRaw.ADC_SystemCurNoOrder);
    AdcRaw.ADC_BatVol = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_6, 1), AdcRaw.ADC_BatVolNoOrder);
}

void TASK_1000MS(void)
{
    LED_SYS = ~LED_SYS;               // 系统工作指示灯
    LED_MINI_BOARD = ~LED_MINI_BOARD; // 系统工作指示灯

    Vref = 4095 * 1.2 * 1000 / (float)AdcRaw.ADC_CH17;                                                        // 典型值3300mv
    SolarSystem.Pack_Vol = DIVIDER_COEFFICENT * 1000 * 1.2 * (float)AdcRaw.ADC_BatVol / (AdcRaw.ADC_CH17);    // 单位0.01v
    SolarSystem.Solar_Vol = DIVIDER_COEFFICENT * 1000 * 1.2 * (float)AdcRaw.ADC_SolarVol / (AdcRaw.ADC_CH17); // 单位0.01v
    SolarSystem.Solar_Cur = Vref * (float)AdcRaw.ADC_SolarCur / (4095 * CUR_SAMPLE_RESISTOR_L);
    SolarSystem.Led_Cur = Vref * (float)AdcRaw.ADC_LedCur / (4095 * CUR_SAMPLE_RESISTOR_L);
    SolarSystem.System_Cur = Vref * (float)AdcRaw.ADC_SystemCur / (4095 * CUR_SAMPLE_RESISTOR_L);

    SolarSystem.Charge_Power = SolarSystem.Solar_Vol * SolarSystem.Solar_Cur / 1000;
    SolarSystem.Light_Power = SolarSystem.Pack_Vol * SolarSystem.Led_Cur / 1000;
    SolarSystem.System_Power = 5000 * SolarSystem.System_Cur / 1000;

    SolarSystem.Charge_Energy_mws += SolarSystem.Charge_Power;
    SolarSystem.Discharge_Energy_mws += SolarSystem.Light_Power;
    SolarSystem.Discharge_Energy_mws += SolarSystem.System_Power;

    SolarSystem.Charge_Energy_mwh = SolarSystem.Charge_Energy_mws / 3600;
    SolarSystem.Discharge_Energy_mwh = SolarSystem.Discharge_Energy_mws / 3600;

    SolarSystem.Charge_Energy_wh = SolarSystem.Charge_Energy_mwh / 1000;
    SolarSystem.Discharge_Energy_wh = SolarSystem.Discharge_Energy_mwh / 1000;

    if (SolarSystem.Battery_SOC >= RUN_SOC && SolarSystem.Charge_Power > 1000)
    {
        // OLED_ShowString(0, ROW_COORDINATE_4, "DumpOn ", 12);

        MOS_G = 1; // turn on the dump
    }
    else if (SolarSystem.Battery_SOC < STOP_SOC)
    {
        // OLED_ShowString(0, ROW_COORDINATE_4, "DumpOff", 12);

        MOS_G = 0; // turn off the dump
    }


    // Display[0].Name = "PV:";
    memcpy(Display[0].Name, "PV:", strlen("PV:"));
    Display[0].Number = SolarSystem.Pack_Vol;
    Display[0].NumberLenth = 5;
    memcpy(Display[0].Unit, "mv", strlen("mv"));

    memcpy(Display[1].Name, "SV:", strlen("SV:"));
    Display[1].Number = SolarSystem.Solar_Vol;
    Display[1].NumberLenth = 5;
    memcpy(Display[1].Unit, "mv", strlen("mv"));

    memcpy(Display[2].Name, "CP:", strlen("CP:"));
    Display[2].Number = SolarSystem.Charge_Power;
    Display[2].NumberLenth = 5;
    memcpy(Display[2].Unit, "mw", strlen("mw"));

    memcpy(Display[3].Name, "LP:", strlen("LP:"));
    Display[3].Number = SolarSystem.Light_Power;
    Display[3].NumberLenth = 5;
    memcpy(Display[3].Unit, "mw", strlen("mw"));

    memcpy(Display[4].Name, "SOC:", strlen("SOC:"));
    Display[4].Number = SolarSystem.Battery_SOC;
    Display[4].NumberLenth = 3;
    memcpy(Display[4].Unit, "%", strlen("%"));

    memcpy(Display[5].Name, "SP:", strlen("SP:"));
    Display[5].Number = SolarSystem.System_Power;
    Display[5].NumberLenth = 5;
    memcpy(Display[5].Unit, "mw", strlen("mw"));

    memcpy(Display[6].Name, "CE:", strlen("CE:"));
    if (SolarSystem.Charge_Energy_wh == 0)
    {
        Display[6].Number = SolarSystem.Charge_Energy_mwh;
        Display[6].NumberLenth = 4;
        memcpy(Display[6].Unit, "mwh", strlen("mwh"));
    }
    else
    {
        Display[6].Number = SolarSystem.Charge_Energy_wh;
        Display[6].NumberLenth = 4;
        memcpy(Display[6].Unit, "wh ", strlen("wh "));
    }

    memcpy(Display[7].Name, "DE:", strlen("DE:"));
    if (SolarSystem.Discharge_Energy_wh == 0)
    {
        Display[7].Number = SolarSystem.Discharge_Energy_mwh;
        Display[7].NumberLenth = 4;
        memcpy(Display[7].Unit, "mwh", strlen("mwh"));
    }
    else
    {
        Display[7].Number = SolarSystem.Discharge_Energy_wh;
        Display[7].NumberLenth = 4;
        memcpy(Display[7].Unit, "wh ", strlen("wh "));
    }

    memcpy(Display[8].Name, "SP:", strlen("SP:"));
    Display[8].Number = SolarSystem.System_Power;
    Display[8].NumberLenth = 5;
    memcpy(Display[8].Unit, "mw", strlen("mw"));


    TASK_Display();

    OLED_Refresh_Gram(); // 更新显示到OLED

    TASK_CalculateSeriesNum();
    IWDG_Feed(); // 喂狗
}

void TASK_5000MS(void)
{
    OLED_ReInit();
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
    }
    if (count_50ms >= 50)
    {
        count_50ms = 0;
        TASK_50MS();
    }
    if (count_200ms >= 200)
    {
        count_200ms = 0;
        TASK_200MS();
    }
    if (count_1000ms >= 1000)
    {
        count_1000ms = 0;
        TASK_1000MS();
    }
    if (count_5000ms >= 5000)
    {
        count_5000ms = 0;
        TASK_5000MS();
    }
}

int main(void)
{
    DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);

    SysTick_Config(SystemCoreClock / 1000);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组2
    // uart1_Init(115200);
    // usart2_Init(115200);
    LED_Init();
    BUZZER_Init();

    Adc_PinInit();
    Adc_InitAdc1();

    OLED_IoInit();
    OLED_Init();

    // OLED_ShowString(20, 0, "Electronic", 16);
    // OLED_ShowString(40, 20, "scale", 16);

    // OLED_ShowString(0, 43, "Weight:", 16);
    // OLED_ShowString(105, 40, "g", 16);

    OLED_Refresh_Gram(); // 更新显示到OLED

    IWDG_Init(5, 467); // 预分频数为64,重载值为467,溢出时间为1.5s

    while (1)
    {
        CONTROL_Loop();
    }
}

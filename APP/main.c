#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
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
#include "stdbool.h"

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
static uint8_t RollCount = 0;

bool UsbState = 0;     //
bool AutoTurnPage = 1; //默认自动翻页

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

void TASK_LoadingShowValue(void)
{
    memcpy(Display[0].Name, "PV:", strlen("PV:"));
    Display[0].Number = SolarSystem.Pack_Vol;
    Display[0].NumberLenth = 5;
    memcpy(Display[0].Unit, "mv", strlen("mv"));

    memcpy(Display[1].Name, "SV:", strlen("SV:"));
    Display[1].Number = SolarSystem.Solar_Vol;
    Display[1].NumberLenth = 5;
    memcpy(Display[1].Unit, "mv", strlen("mv"));

    memcpy(Display[2].Name, "GP:", strlen("GP:"));
    Display[2].Number = SolarSystem.Generation_Power;
    Display[2].NumberLenth = 5;
    memcpy(Display[2].Unit, "mw", strlen("mw"));

    memcpy(Display[3].Name, "BP:", strlen("BP:"));
    Display[3].Number = SolarSystem.Battery_Power;
    Display[3].NumberLenth = 5;
    memcpy(Display[3].Unit, "mw", strlen("mw"));

    memcpy(Display[4].Name, "LP:", strlen("LP:"));
    Display[4].Number = SolarSystem.Light_Power;
    Display[4].NumberLenth = 5;
    memcpy(Display[4].Unit, "mw", strlen("mw"));

    memcpy(Display[5].Name, "OP:", strlen("OP:"));
    Display[5].Number = SolarSystem.Output_Power;
    Display[5].NumberLenth = 5;
    memcpy(Display[5].Unit, "mw", strlen("mw"));

    memcpy(Display[6].Name, "CE:", strlen("CE:")); // energy of charging
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

    memcpy(Display[7].Name, "DE:", strlen("DE:")); // energy of discharging
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

    memcpy(Display[8].Name, "SOC:", strlen("SOC:"));
    Display[8].Number = SolarSystem.Battery_SOC;
    Display[8].NumberLenth = 3;
    memcpy(Display[8].Unit, "%", strlen("%"));

    memcpy(Display[9].Name, "Dump:", strlen("Dump:"));
    Display[9].Number = UsbState;
    Display[9].NumberLenth = 2;
    memcpy(Display[9].Unit, " ", strlen(" "));

    memcpy(Display[10].Name, "CT:", strlen("CT:")); // energy of charging
    if (SolarSystem.DailyChargeTime <= 3600)
    {
        Display[10].Number = SolarSystem.DailyChargeTime / 60; // show unit min
        Display[10].NumberLenth = 4;
        memcpy(Display[10].Unit, "min", strlen("min"));
    }
    else
    {
        Display[10].Number = SolarSystem.DailyChargeTime / 3600; // show unit hour
        Display[10].NumberLenth = 4;
        memcpy(Display[10].Unit, " h ", strlen(" h "));
    }

    memcpy(Display[11].Name, "RT:", strlen("RT:")); // energy of charging
    if (SolarSystem.RunningTime <= 3600)
    {
        Display[11].Number = SolarSystem.RunningTime / 60; // show unit min
        Display[11].NumberLenth = 4;
        memcpy(Display[11].Unit, "min", strlen("min"));
    }
    else
    {
        Display[11].Number = SolarSystem.RunningTime / 3600; // show unit hour
        Display[11].NumberLenth = 4;
        memcpy(Display[11].Unit, " h ", strlen(" h "));
    }

    memcpy(Display[12].Name, "DCE:", strlen("DCE:")); // energy of charging
    Display[12].Number = SolarSystem.Daily_Energy_wh;
    Display[12].NumberLenth = 4;
    memcpy(Display[12].Unit, "wh ", strlen("wh "));
}

void TASK_Display(void)
{
    static uint8_t RowNumber = 0;
    static uint8_t RowIndex = 1;
    uint8_t DisplayIndex = 0;
    uint8_t XCoordinate = 0;
    uint8_t YCoordinate = 0;
    uint8_t NumberOffset = 0;
    uint8_t UnitOffset = 0;
    uint8_t i = 0;

    if (AutoTurnPage)
    {
        RollCount++;
    }
    while (Display[DisplayIndex].Name[0] != 0)
    {
        DisplayIndex++;
    }
    RowNumber = (DisplayIndex + 1) / 2;

    if (RollCount > 2 && RowNumber > 4)
    {
        RollCount = 0;
        if ((RowIndex + 4) > RowNumber)
        {
            RowIndex = 1; // show from first row
        }
        else if ((RowIndex + 4 + 4) > RowNumber)
        {
            RowIndex = RowNumber - 3;
        }
        else
        {
            RowIndex = +4;
        }

        OLED_Clear();
        OLED_ShowString(20, 0, (uint8_t *)"Solar System", 12);
    }

    DisplayIndex = (RowIndex - 1) * 2;
    for (i = 0; i < 8; i++)
    {
        if (Display[DisplayIndex].Name[0] == 0)
        {
            break;
        }
        XCoordinate = (DisplayIndex % 2) * COLUMN_OFFSET;
        YCoordinate = (((i % 8) / 2) + 1) * ROW_HIGHT;

        NumberOffset = 6 * strlen(Display[DisplayIndex].Name) + XCoordinate;
        UnitOffset = 6 * Display[DisplayIndex].NumberLenth + NumberOffset;

        OLED_ShowString(XCoordinate, YCoordinate, (uint8_t *)Display[DisplayIndex].Name, 12);
        if (Display[DisplayIndex].Number < 0) // Negative
        {
            OLED_ShowString(NumberOffset, YCoordinate, (uint8_t *)"-", 12);
            OLED_ShowNum(NumberOffset + 6, YCoordinate, -Display[DisplayIndex].Number, Display[DisplayIndex].NumberLenth - 1, 12); // 显示ASCII字符的码值
        }
        else
        {
            OLED_ShowNum(NumberOffset, YCoordinate, Display[DisplayIndex].Number, Display[DisplayIndex].NumberLenth, 12); // 显示ASCII字符的码值
        }
        OLED_ShowString(UnitOffset, YCoordinate, (uint8_t *)Display[DisplayIndex].Unit, 12);
        DisplayIndex++;
    }
    OLED_Refresh_Gram(); // 更新显示到OLED
}

void TASK_1MS(void)
{
}

void TASK_10MS(void)
{
    BUTTON_Poll();
    switch (BUTTON_Handle.Funtion_Button.Touch_Type)
    {
    case TOUCH_SHORT:
        AutoTurnPage = 0;
        RollCount += 3;

        /* code */
        break;

    case TOUCH_LONG:
        AutoTurnPage = !AutoTurnPage;
        /* code */
        break;

    default: // TOUCH_NO
        break;
    }
}

void TASK_50MS(void)
{
    static char BlinkSoc = 0;
    static uint16_t SOC_DisplayCount = 0; // 电量指示闪烁

    if ((SolarSystem.Pack_Vol / SeriesNum) <= 3300)
    {
        SolarSystem.Battery_SOC = 0;
    }
    else
    {
        SolarSystem.Battery_SOC = ((SolarSystem.Pack_Vol / SeriesNum) - 3300) / 9;
        // SolarSystem.Battery_SOC = (uint16_t)SolarSystem.Battery_SOC * (uint16_t)SolarSystem.Battery_SOC * 0.01;//2次曲线
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

void TASK_AdcSample(void)
{
    static uint16_t SampleCount = 0;
    SampleCount++;
    if (SampleCount > 30)
        SampleCount = 0;

    switch (SampleCount / 5)
    {
    case 0:
        AdcRaw.ADC_CH17 = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_17, 1), AdcRaw.ADC_CH17NoOrder);
        break;

    case 1:
        AdcRaw.ADC_SolarCur = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_0, 1), AdcRaw.ADC_SolarCurNoOrder);
        break;

    case 2:
        AdcRaw.ADC_LedCur = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_1, 1), AdcRaw.ADC_LedCurNoOrder);
        break;

    case 3:
        AdcRaw.ADC_SolarVol = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_2, 1), AdcRaw.ADC_SolarVolNoOrder);
        break;

    case 4:
        AdcRaw.ADC_SystemCur = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_3, 1), AdcRaw.ADC_SystemCurNoOrder);
        break;

    case 5:
        AdcRaw.ADC_BatVol = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_6, 1), AdcRaw.ADC_BatVolNoOrder);
        break;

    default:
        AdcRaw.ADC_CH17 = SlidingMedianFilter(Adc_GetAdcAverage1(ADC_Channel_17, 1), AdcRaw.ADC_CH17NoOrder);
        break;
    }
}

void TASK_1000MS(void)
{
    static int Vref = 0; // mv

    LED_SYS = ~LED_SYS;               // 系统工作指示灯
    LED_MINI_BOARD = ~LED_MINI_BOARD; // 系统工作指示灯

    SolarSystem.RunningTime++;

    SolarSystem.Ref_Vol = 4095 * 1.2 * 1000 / (float)AdcRaw.ADC_CH17;
    Vref = 4095 * 1.2 * 1000 / (float)AdcRaw.ADC_CH17;                                                        // 典型值3300mv
    SolarSystem.Pack_Vol = DIVIDER_COEFFICENT * 1000 * 1.2 * (float)AdcRaw.ADC_BatVol / (AdcRaw.ADC_CH17);    // 单位0.01v
    SolarSystem.Solar_Vol = DIVIDER_COEFFICENT * 1000 * 1.2 * (float)AdcRaw.ADC_SolarVol / (AdcRaw.ADC_CH17); // 单位0.01v
    SolarSystem.Solar_Cur = Vref * (float)AdcRaw.ADC_SolarCur / (4095 * CUR_SAMPLE_RESISTOR_L);
    SolarSystem.Battery_Res_Vol = 1000 * 1.2 * (float)AdcRaw.ADC_SolarCur / (AdcRaw.ADC_CH17);
    SolarSystem.Bat_Cur = (12 * SolarSystem.Battery_Res_Vol - SolarSystem.Ref_Vol);
    SolarSystem.Led_Cur = Vref * (float)AdcRaw.ADC_LedCur / (4095 * CUR_SAMPLE_RESISTOR_L);
    SolarSystem.Output_Cur = Vref * (float)AdcRaw.ADC_SystemCur / (4095 * CUR_SAMPLE_RESISTOR_L);

    SolarSystem.Battery_Power = SolarSystem.Pack_Vol * SolarSystem.Bat_Cur / 1000;
    SolarSystem.Light_Power = SolarSystem.Pack_Vol * SolarSystem.Led_Cur / 1000;
    SolarSystem.Output_Power = 5000 * SolarSystem.Output_Cur / 1000;
    if (SolarSystem.Battery_Power > 0)
    {
        SolarSystem.Generation_Power = (SolarSystem.Output_Power * 1.25) + (SolarSystem.Battery_Power);
    }
    else if (SolarSystem.Output_Power > -SolarSystem.Battery_Power) // discharging while charging
    {
        SolarSystem.Generation_Power = (SolarSystem.Output_Power * 1.25) + (SolarSystem.Battery_Power); // 算法待验证
    }
    else
    {
        SolarSystem.Generation_Power = 0;
    }

    if (SolarSystem.Generation_Power >= 500)
    {
        if (SolarSystem.DailyIdleTime > 8 * 3600) // 5 hours without charging
        {                                         // the situation will be happened at morning
            SolarSystem.DailyIdleTime = 0;        // start a new cycle
            SolarSystem.DailyChargeTime = 0;
            SolarSystem.Last_Energy_wh = SolarSystem.Charge_Energy_wh;
        }
        SolarSystem.DailyChargeTime++;
    }
    else
    {
        SolarSystem.DailyIdleTime++;
    }

    SolarSystem.Charge_Energy_mws += SolarSystem.Generation_Power;
    SolarSystem.Discharge_Energy_mws += SolarSystem.Light_Power;
    SolarSystem.Discharge_Energy_mws += SolarSystem.Output_Power;

    SolarSystem.Charge_Energy_mwh = SolarSystem.Charge_Energy_mws / 3600;
    SolarSystem.Discharge_Energy_mwh = SolarSystem.Discharge_Energy_mws / 3600;

    SolarSystem.Charge_Energy_wh = SolarSystem.Charge_Energy_mwh / 1000;
    SolarSystem.Discharge_Energy_wh = SolarSystem.Discharge_Energy_mwh / 1000;
    SolarSystem.Daily_Energy_wh = SolarSystem.Charge_Energy_wh - SolarSystem.Last_Energy_wh;

    if (SolarSystem.Generation_Power > 6000 || SolarSystem.Battery_SOC > 50)
    {
        UsbState = 1;
    }
    else if (SolarSystem.Battery_SOC >= RUN_SOC && SolarSystem.Generation_Power > 1000)
    {
        UsbState = 1;
    }
    else if (SolarSystem.Battery_SOC < STOP_SOC)
    {
        UsbState = 0;
    }

    UsbState ? (MOS_G = 1) : (MOS_G = 0);

    TASK_LoadingShowValue();
    TASK_Display();

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
        TASK_AdcSample();

        TASK_50MS();
    }
    if (count_200ms >= 200)
    {
        count_200ms = 0;
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
    KEY_Init();
    Adc_PinInit();
    Adc_InitAdc1();

    OLED_IoInit();
    OLED_Init();

    OLED_ShowString(20, 0, (uint8_t *)"Solar System", 12);

    OLED_Refresh_Gram(); // 更新显示到OLED

    IWDG_Init(5, 467); // 预分频数为64,重载值为467,溢出时间为1.5s

    while (1)
    {
        CONTROL_Loop();
    }
}

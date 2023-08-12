#ifndef __MAIN_H
#define __MAIN_H
#include "sys.h"

#define FIlTER_LENTH 5

typedef enum
{
    SEND_IDEL = 0,
    SEND_READY,
    SEND_SUCCESS,
    SEND_FAULT,

} SEND_STATE_E;

typedef enum
{
    RECEIVING_MODE = 0,
    SENDING_MODE,

} NRF_MODE;

typedef struct
{
    uint16_t Pack_Vol;        // mv
    uint16_t Solar_Vol;       // mv
    uint16_t Solar_Cur;       // ma
    uint16_t Led_Cur;         // ma
    uint16_t System_Cur;         // ma
    uint32_t Charge_Power;    // mw
    uint32_t Light_Power; // mw
    uint32_t System_Power; // mw

    uint32_t Charge_Energy_mws;    // wh
    uint32_t Discharge_Energy_mws; // wh

    uint32_t Charge_Energy_mwh;    // wh
    uint32_t Discharge_Energy_mwh; // wh

    uint32_t Charge_Energy_wh;    // wh
    uint32_t Discharge_Energy_wh; // wh

    char Battery_SOC;
} SOLAR_SYSTEM_T;

typedef struct
{
    uint16_t ADC_BatVol;
    uint16_t ADC_SolarVol;
    uint16_t ADC_SolarCur;
    uint16_t ADC_LedCur;
    uint16_t ADC_SystemCur;
    uint16_t ADC_CH17;

    uint16_t ADC_BatVolNoOrder[FIlTER_LENTH];
    uint16_t ADC_SolarVolNoOrder[FIlTER_LENTH];
    uint16_t ADC_SolarCurNoOrder[FIlTER_LENTH];
    uint16_t ADC_LedCurNoOrder[FIlTER_LENTH];
    uint16_t ADC_SystemCurNoOrder[FIlTER_LENTH];
    uint16_t ADC_CH17NoOrder[FIlTER_LENTH];

} ADC_RAW_T;

typedef struct
{
    char Name[5];//最后一个元素为0
        uint8_t NameLenth;

    uint16_t Number;
    uint8_t NumberLenth;
    char Unit[4];

} DISPLAY_T;

#endif

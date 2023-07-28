#ifndef __MAIN_H
#define __MAIN_H	
#include "sys.h"

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
    union
    {
        uint8_t RockerBuff[33];
        struct
        {
            uint8_t X_Coordinate;
            uint8_t reserve0;
            uint8_t Y_Coordinate;
            uint8_t reserve1;
            uint8_t Heartbeat;

        }data;
		
    } ROCKER_DATA;

    union
    {
        uint8_t CarBuff[33];
        struct
        {
            uint8_t DutyCycle_L;
            uint8_t DutyCycle_R;
            uint16_t BatteryVol;
            uint16_t BusCurrent;//Ä¸ÏßµçÁ÷
            uint8_t CurrentYaw;
            uint8_t TargetYaw;
            uint8_t Heartbeat;

        }data;
		
    } CAR_DATA;

    
} NRF_PACKECT_T;



#endif 

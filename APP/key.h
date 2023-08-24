#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define KEY_PORT GPIOB
#define KEY1_PIN GPIO_Pin_11

typedef enum
{
    BUTTON_ACTION_RELEASE = 0,
    BUTTON_ACTION_PUSH,
} BUTTON_ACTION_E;

typedef enum
{
    TOUCH_NO,
    TOUCH_SHORT,
    TOUCH_LONG
} BUTTON_TOUCH_TYPE_E;

typedef struct
{
    // uint8_t            lock;
    BUTTON_ACTION_E Action;
    BUTTON_TOUCH_TYPE_E Touch_Type;
    uint32_t PressCount;
} button_t;

typedef struct
{
    button_t Funtion_Button;
} BUTTON_Handle_t;

extern BUTTON_Handle_t BUTTON_Handle;

void BUTTON_Poll(void);
void KEY_Init(void); // ≥ı ºªØ

#endif

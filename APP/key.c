#include "key.h"

BUTTON_Handle_t BUTTON_Handle;

void BUTTON_PressCountClear(button_t *button)
{
    if (button->Action == BUTTON_ACTION_RELEASE)
    {
        button->PressCount = 0;
    }
}

void button_GetTouchType(button_t *button) // task cycle 10ms
{
    if ((button->PressCount >= 2) && (button->PressCount <= 50)) // 20-500 ms 判断为短按
    {
        if (button->Action == BUTTON_ACTION_RELEASE)
        {
            button->Touch_Type = TOUCH_SHORT;
        }
    }
    else if (button->PressCount > 50)
    {
        if (button->Action == BUTTON_ACTION_RELEASE) // 长按松开后有效
        {
            button->Touch_Type = TOUCH_LONG;
        }
    }
    else
    {
        button->Touch_Type = TOUCH_NO;
    }

    BUTTON_PressCountClear(button);
}

void BUTTON_UpdateKeyCounter(BUTTON_Handle_t *handle)
{

    if (handle->Funtion_Button.Action == BUTTON_ACTION_PUSH)
    {
        handle->Funtion_Button.PressCount++;
    }
    else
    {
        // handle->Funtion_Button.pressCount=0;
    }

    /*按键处理的函数*/

    button_GetTouchType(&handle->Funtion_Button);
}

void BUTTON_Press(BUTTON_Handle_t *handle, uint16_t pin_no) // 更新按键动作
{
    if (pin_no == KEY1_PIN)
    {
        handle->Funtion_Button.Action = BUTTON_ACTION_PUSH;
    }
}

void BUTTON_Release(BUTTON_Handle_t *handle, uint16_t pin_no) // 更新按键动作
{
    if (pin_no == KEY1_PIN)
    {
        handle->Funtion_Button.Action = BUTTON_ACTION_RELEASE;
    }
}

static void BUTTON_event_handler(uint16_t pin_no, uint8_t button_action)
{
    if (button_action == 0)
    {
        BUTTON_Press(&BUTTON_Handle, pin_no);
    }
    else
    {
        BUTTON_Release(&BUTTON_Handle, pin_no);
    }
}

void BUTTON_Poll(void)
{
    if (GPIO_ReadInputDataBit(KEY_PORT, KEY1_PIN) == BUTTON_Handle.Funtion_Button.Action)
    {
        BUTTON_event_handler(KEY1_PIN, GPIO_ReadInputDataBit(KEY_PORT, KEY1_PIN));
    }

    BUTTON_UpdateKeyCounter(&BUTTON_Handle); // 更新按键计数
}

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能PA端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // 根据设定参数初始化GPIOA.8
    GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}

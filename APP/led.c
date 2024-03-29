#include "led.h"

void LED_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PA端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能PA端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能PA端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // 使能PA端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;        // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);            // 根据设定参数初始化GPIOA.8
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // 根据设定参数初始化GPIOA.8
    GPIO_SetBits(GPIOA, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 根据设定参数初始化GPIOA.8
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 根据设定参数初始化GPIOA.8
    GPIO_SetBits(GPIOB, GPIO_Pin_9);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);            // 根据设定参数初始化GPIOA.8
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
}

void BUZZER_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PA端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;         // LED0-->PA.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // 根据设定参数初始化GPIOA.8
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

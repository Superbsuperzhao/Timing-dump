#include "adc.h"

void Adc_PinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能ADC1通道时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入引脚
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// 初始化ADC
// 这里我们仅以规则通道为例
// 我们默认将开启通道0~3
void Adc_InitAdc1(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
    ADC_DeInit(ADC1); // 复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // 模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 // 模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 4;                             // 顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);                                 // 根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
    ADC_Cmd(ADC1, ENABLE); // 使能指定的ADC1
    ADC_ResetCalibration(ADC1); // 使能复位校准
    while (ADC_GetResetCalibrationStatus(ADC1))
        ; // 等待复位校准结束
    ADC_StartCalibration(ADC1); // 开启AD校准
    while (ADC_GetCalibrationStatus(ADC1))
        ; // 等待校准结束
}

void Adc_InitAdc2(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); // 使能ADC1通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
    ADC_DeInit(ADC2); // 复位ADC1,将外设 ADC2 的全部寄存器重设为缺省值
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // 模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 // 模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 4;                             // 顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC2, &ADC_InitStructure);                                 // 根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
    ADC_Cmd(ADC2, ENABLE); // 使能指定的ADC1
    ADC_ResetCalibration(ADC2); // 使能复位校准
    while (ADC_GetResetCalibrationStatus(ADC2))
        ; // 等待复位校准结束
    ADC_StartCalibration(ADC2); // 开启AD校准
    while (ADC_GetCalibrationStatus(ADC2))
        ; // 等待校准结束
}



// 获得ADC值
u16 Adc_GetAdc1(u8 ch)
{
    // 设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5); // ADC1,ADC通道,采样时间为239.5周期
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 使能指定的ADC1的软件转换启动功能
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ; // 等待转换结束

    return ADC_GetConversionValue(ADC1); // 返回最近一次ADC1规则组的转换结果
}

u16 Adc_GetAdc2(u8 ch)
{
    // 设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5); // ADC2,ADC通道,采样时间为239.5周期
    ADC_SoftwareStartConvCmd(ADC2, ENABLE); // 使能指定的ADC1的软件转换启动功能
    while (!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC))
        ; // 等待转换结束

    return ADC_GetConversionValue(ADC2); // 返回最近一次ADC1规则组的转换结果
}

u16 Adc_GetAdcAverage1(u8 ch, u8 times)
{
    u32 temp_val = 0;
    // u8 t;
    // for (t = 0; t < 3; t++)
    // {
    //     temp_val += Adc_GetAdc1(ch); // discard the first three data
    //     // delay_ms(5);
    // }
    temp_val = Adc_GetAdc1(ch);
    return temp_val;
}

u16 Adc_GetAdcAverage2(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;
    for (t = 0; t < 3; t++)
    {
        temp_val += Adc_GetAdc2(ch); // discard the first three data
        // delay_ms(5);
    }
    temp_val = Adc_GetAdc2(ch);
    return temp_val;
}

// u16 Adc_GetAdcAverage1(u8 ch, u8 times)
// {
//     u32 temp_val = 0;
//     u8 t;
//     for (t = 0; t < times; t++)
//     {
//         temp_val += Adc_GetAdc1(ch);
//         // delay_ms(5);
//     }
//     return temp_val / times;
// }

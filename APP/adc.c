#include "adc.h"

void Adc_PinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��ADC1ͨ��ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // ģ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// ��ʼ��ADC
// �������ǽ��Թ���ͨ��Ϊ��
// ����Ĭ�Ͻ�����ͨ��0~3
void Adc_InitAdc1(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // ʹ��ADC1ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
    ADC_DeInit(ADC1); // ��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 // ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // ת���������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 4;                             // ˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);                                 // ����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
    ADC_Cmd(ADC1, ENABLE); // ʹ��ָ����ADC1
    ADC_ResetCalibration(ADC1); // ʹ�ܸ�λУ׼
    while (ADC_GetResetCalibrationStatus(ADC1))
        ; // �ȴ���λУ׼����
    ADC_StartCalibration(ADC1); // ����ADУ׼
    while (ADC_GetCalibrationStatus(ADC1))
        ; // �ȴ�У׼����
}

void Adc_InitAdc2(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); // ʹ��ADC1ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
    ADC_DeInit(ADC2); // ��λADC1,������ ADC2 ��ȫ���Ĵ�������Ϊȱʡֵ
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 // ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // ת���������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 4;                             // ˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC2, &ADC_InitStructure);                                 // ����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
    ADC_Cmd(ADC2, ENABLE); // ʹ��ָ����ADC1
    ADC_ResetCalibration(ADC2); // ʹ�ܸ�λУ׼
    while (ADC_GetResetCalibrationStatus(ADC2))
        ; // �ȴ���λУ׼����
    ADC_StartCalibration(ADC2); // ����ADУ׼
    while (ADC_GetCalibrationStatus(ADC2))
        ; // �ȴ�У׼����
}



// ���ADCֵ
u16 Adc_GetAdc1(u8 ch)
{
    // ����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5); // ADC1,ADCͨ��,����ʱ��Ϊ239.5����
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ʹ��ָ����ADC1������ת����������
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ; // �ȴ�ת������

    return ADC_GetConversionValue(ADC1); // �������һ��ADC1�������ת�����
}

u16 Adc_GetAdc2(u8 ch)
{
    // ����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5); // ADC2,ADCͨ��,����ʱ��Ϊ239.5����
    ADC_SoftwareStartConvCmd(ADC2, ENABLE); // ʹ��ָ����ADC1������ת����������
    while (!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC))
        ; // �ȴ�ת������

    return ADC_GetConversionValue(ADC2); // �������һ��ADC1�������ת�����
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
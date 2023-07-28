#include "sys.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////
// ���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos ʹ��
#endif

//////////////////////////////////////////////////////////////////
// �������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// ��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
// ����_sys_exit()�Ա���ʹ�ð�����ģʽ
_sys_exit(int x)
{
	x = x;
}
// �ض���fputc����
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // ѭ������,ֱ���������
	USART1->DR = (u8)ch;
	return ch;
}
#endif

#if EN_USART1_RX // ���ʹ���˽���
// ����1�жϷ������
// ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.
// ����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; // ����״̬���

void uart1_Init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // ʹ��USART1��GPIOAʱ��

	// USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // �����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // ��ʼ��GPIOA.9

	// USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // ��ʼ��GPIOA.10

	// Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

	// USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;										// ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART1, &USART_InitStructure);	   // ��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // �������ڽ����ж�
	USART_Cmd(USART1, ENABLE);					   // ʹ�ܴ���1
}

void usart2_Init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��USART1��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ʹ��USART1��GPIOAʱ��

	// USART2_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // �����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // ��ʼ��GPIOA.9

	// USART2_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // ��ʼ��GPIOA.10

	// Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

	// USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;										// ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART2, &USART_InitStructure);	   // ��ʼ������1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // �������ڽ����ж�
	USART_Cmd(USART2, ENABLE);					   // ʹ�ܴ���1
}

u8 temparr[20];
u8 ch = 0;
u8 uart_A5step=0;
u8 uart_AAstep=0;
int16_t accx, accy, accz;
int16_t roll_raw, pitch_raw, yaw_raw;
int16_t yaw_afterfilter;
int16_t yaw_smooth;
void USART1_IRQHandler(void) // ����1�жϷ������
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearFlag(USART1, USART_FLAG_RXNE); // ���жϱ�־

	}
}
void USART2_IRQHandler(void) // ����2�жϷ������
{
	u8 Res;
	u8 sum;
	u8 i;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART2); // ��ȡ���յ�������
		temparr[ch++] = Res;
		if(temparr[0] == 0x5a)
		{
			switch (uart_A5step)//��ͨ��gy-21
			{
				case 0:
					uart_A5step++;
					break;

				case 1:
					if (temparr[0] == 0x5a)
					{
						uart_A5step++;
					}
					else
					{
						ch = 0;
						uart_A5step = 0;
						memset(temparr, 0, 1);
					}
					break;
				case 2:
					if (temparr[1] == 0x5a)
					{
						uart_A5step++;
					}
					else
					{
						ch = 0;
						uart_A5step = 0;
						memset(temparr, 0, 2);
					}
					break;

				case 3:
					if (ch >= 4)
					{
						if (temparr[3] == 0x0e)
							uart_A5step++;
						else
						{
							ch = 0;
							uart_A5step = 0;
							memset(temparr, 0, 4);
						}
					}
					break;

				case 4:
					if (ch >= 4 + 0x0e + 1)
					{
						sum = 0;
						for (i = 0; i < 18; i++)
						{
							sum += temparr[i];
						}
						if (temparr[ch - 1] == sum)
						{
							accx = (u16)temparr[4] << 8 | (u16)temparr[5]; // 1G=32767/2=16383.5
							accy = (u16)temparr[6] << 8 | (u16)temparr[7];
							accz = (u16)temparr[8] << 8 | (u16)temparr[9];

							roll_raw = (u16)temparr[10] << 8 | (u16)temparr[11]; // /100=du
							pitch_raw = (u16)temparr[12] << 8 | (u16)temparr[13];
							yaw_raw = (u16)temparr[14] << 8 | (u16)temparr[15];
						}
						ch = 0;
						uart_A5step = 0;
						memset(temparr, 0, sizeof(temparr));
					}
					break;

				default:
					ch = 0;
					uart_A5step = 0;
					memset(temparr, 0, sizeof(temparr));
					break;
			}
		}

		else if(temparr[0] == 0xaa)
		{
			switch (uart_AAstep)//��ͨ��gy-21
			{
				case 0:
					uart_AAstep++;
					break;

				case 1:
					if (ch >= 8)
					{
						if(temparr[7] == 0x55)
						{
							yaw_raw = (u16)temparr[1] << 8 | (u16)temparr[2];
							pitch_raw = (u16)temparr[3] << 8 | (u16)temparr[4];
							roll_raw = (u16)temparr[5] << 8 | (u16)temparr[6]; // /100=du
						}
						uart_AAstep=0;
						ch = 0;
						//memset(temparr, 0, 7);

					}
					
					break;
				
				default:
				break;
			}
		}
		if (ch > 20)
			ch = 0;
		USART_ClearFlag(USART2, USART_FLAG_RXNE); // ���жϱ�־
	}
}

#endif

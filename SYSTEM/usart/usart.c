#include "sys.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////
// 如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos 使用
#endif

//////////////////////////////////////////////////////////////////
// 加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
	x = x;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // 循环发送,直到发送完毕
	USART1->DR = (u8)ch;
	return ch;
}
#endif

#if EN_USART1_RX // 如果使能了接收
// 串口1中断服务程序
// 注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.
// 接收状态
// bit15，	接收完成标志
// bit14，	接收到0x0d
// bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0; // 接收状态标记

void uart1_Init(u32 bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // 使能USART1，GPIOA时钟

	// USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // 初始化GPIOA.9

	// USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // 初始化GPIOA.10

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器

	// USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;										// 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式

	USART_Init(USART1, &USART_InitStructure);	   // 初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启串口接受中断
	USART_Cmd(USART1, ENABLE);					   // 使能串口1
}

void usart2_Init(u32 bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能USART1，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能USART1，GPIOA时钟

	// USART2_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // 初始化GPIOA.9

	// USART2_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // 初始化GPIOA.10

	// Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器

	// USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;										// 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式

	USART_Init(USART2, &USART_InitStructure);	   // 初始化串口1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启串口接受中断
	USART_Cmd(USART2, ENABLE);					   // 使能串口1
}

u8 temparr[20];
u8 ch = 0;
u8 uart_A5step=0;
u8 uart_AAstep=0;
int16_t accx, accy, accz;
int16_t roll_raw, pitch_raw, yaw_raw;
int16_t yaw_afterfilter;
int16_t yaw_smooth;
void USART1_IRQHandler(void) // 串口1中断服务程序
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearFlag(USART1, USART_FLAG_RXNE); // 清中断标志

	}
}
void USART2_IRQHandler(void) // 串口2中断服务程序
{
	u8 Res;
	u8 sum;
	u8 i;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART2); // 读取接收到的数据
		temparr[ch++] = Res;
		if(temparr[0] == 0x5a)
		{
			switch (uart_A5step)//普通版gy-21
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
			switch (uart_AAstep)//普通版gy-21
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
		USART_ClearFlag(USART2, USART_FLAG_RXNE); // 清中断标志
	}
}

#endif

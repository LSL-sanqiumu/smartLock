#include "stm32f10x.h"                  // Device header

#include "MyDelay.h"
#include "HC06.h"

char hc06_rx_buff[BUFF_MAX_SIZE];
uint8_t hc06_rx_flag;

void HC06_SerialInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
    
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
    
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStruct);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART3, ENABLE);
}

void Serial_SendByteToHC06(uint16_t byte)
{
    USART_SendData(USART3, byte);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void Serial_SendStrToHC06(char* str)
{
    uint16_t i;
    for(i = 0; str[i] != '\0'; i++){
        Serial_SendByteToHC06(str[i]);
    }
    Serial_SendByteToHC06('\r');
    Serial_SendByteToHC06('\n');
}

uint8_t Serial_GetHC06RxFlag(void)
{
    if(hc06_rx_flag == 1){
        hc06_rx_flag = 0;
        return 1;
    }
    return 0;
}

/* @xxx\r\n */
void USART3_IRQHandler(void)
{
    static uint8_t receive_state = 0;
    static uint8_t receive_index = 0;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET){
        uint16_t data = USART_ReceiveData(USART3);
        if(receive_state == 0){
            if(data == '@' && hc06_rx_flag == 0){
                receive_index = 0;
                receive_state = 1;
            }
        }else if(receive_state == 1){
            if(data == '\r'){
                receive_state = 2;
            }else {
                hc06_rx_buff[receive_index++] = data;
            }
        }else if(receive_state == 2){
            if(data == '\n'){
                receive_state = 0;
                hc06_rx_buff[receive_index] = '\0';
                hc06_rx_flag = 1;
            }
        }
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}



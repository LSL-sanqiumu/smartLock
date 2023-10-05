#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "ESP8266.h"
#include "MyDelay.h"
#include "SerialLog.h"
#include "string.h"

extern TimerHandle_t Timer1_Handle;

char buff[512];
uint16_t buff_index;
uint8_t receive_flag;
uint8_t buff_init_flag;

void ESP8266_SerialInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART2, ENABLE);
}

void Serial_SendByteToESP(uint16_t data)
{

	USART_SendData(USART2, data);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
}

void Serial_SendStrToESP(char* str)
{
	uint16_t i;
	for(i = 0; str[i] != '\0'; i++){
		Serial_SendByteToESP(str[i]);
	}
}

void Serial_SendCmdToESP(char* cmd)
{
	Serial_SendStrToESP(cmd);
	Serial_SendByteToESP('\r');
	Serial_SendByteToESP('\n');
    MyDelay_ms(20);
}
/* AP模式 */
void ESP8266_APInit(void)
{
    ESP8266_Execution("AT+CWMODE=2", "OK", 500);
    ESP8266_Execution("AT+RST", "OK", 2000);
    ESP8266_Execution("AT+CWSAP=\"ESP8266\",\"123456789\",4,4", "OK", 200);
    ESP8266_Execution("AT+CIPMUX=1", "OK", 200);       
    ESP8266_Execution("AT+CIPSERVER=1,8080", "OK", 200);
    ESP8266_Execution("AT+CIFSR", "OK", 200);
}

uint8_t ESP8266_HasResponseCode(char* response_msg, char* response_code)
{
	if(strstr(response_msg, response_code)){
        return 1;
    }
    return 0;
}

uint8_t ESP8266_Execution(char* cmd, char* response_code, uint16_t timeout_ms)
{
    Serial_SendCmdToESP(cmd);
    if(response_code && timeout_ms){
        while(--timeout_ms){
            MyDelay_ms(1);
            if(ESP8266_GetRxFlag() == 1){
                SerialLog_SendStrMsg(buff);
                if(ESP8266_HasResponseCode(buff, response_code)){
                    return 1;
                }else {
                    return 0;
                }
            }
        }
    }
    return timeout_ms == 0;
}

uint8_t ESP8266_GetRxFlag(void)
{
    if(receive_flag == 1){
        receive_flag = 0;
        return 1;
    }
    return receive_flag;
}

void Timer1_Callback(void)
{
    receive_flag = 1;
    buff_init_flag = 1;
    buff[buff_index] = '\0';
}

void USART2_IRQHandler(void)
{
    static uint16_t index = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
        if(buff_init_flag){
            index = 0;
            buff_init_flag = 0;
        }
        if(index < 512) {
            buff[index++] = USART_ReceiveData(USART2);
            buff_index = index;
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xTimerStop(Timer1_Handle, 1000);
            /* 启动软件定时器 */
            if (xTimerStartFromISR(Timer1_Handle, &xHigherPriorityTaskWoken) != pdTRUE) {
                /* 软件定时器开启命令没有成功执行 */
                SerialLog_SendStrMsg("Timer1 start fail!");
            }
            if (xHigherPriorityTaskWoken != pdFALSE) {
                /* 执行上下文切换 */
            }
        }else {
            
        }
        
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}




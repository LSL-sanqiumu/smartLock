#include "stm32f10x.h"                  // Device header

#include "MyDelay.h"
#include "Keyboard.h"
#include "OLED.h"

char passwd_temp[PASSWD_MAX_LEN];
uint8_t pd_index = 0;

void Keyboard_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t Keyboard_Scan(void)
{
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0){
        MyDelay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0);
        MyDelay_ms(20);
        return KEY_NUM5;
    }
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0){
        MyDelay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0);
        MyDelay_ms(20);
        return KEY_CONFIRM;
    }
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0){
        MyDelay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0);
        MyDelay_ms(20);
        return KEY_NUM3;
    }
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0){
        MyDelay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);
        MyDelay_ms(20);
        return KEY_NUM1;
    }
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0){
        MyDelay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
        MyDelay_ms(20);
        return KEY_NUM2;
    }
    return KEY_NONE;
}

uint8_t WaitingPasswdEnter(void)    
{
    while(1){
        uint8_t num = Keyboard_Scan();
        if(num == KEY_NUM1) {
            passwd_temp[pd_index++] = KEY_NUM1 + 48;
            passwd_temp[pd_index] = '\0';
            OLED_ShowString(2, 1, "      ");
            OLED_ShowString(2, 1, passwd_temp);
        }
        if(num == KEY_NUM2) {
            passwd_temp[pd_index++] = KEY_NUM2 + 48;
            passwd_temp[pd_index] = '\0';
            OLED_ShowString(2, 1, "      ");
            OLED_ShowString(2, 1, passwd_temp);
        }
        if(num == KEY_NUM3) {
            passwd_temp[pd_index++] = KEY_NUM3 + 48;
            passwd_temp[pd_index] = '\0';
            OLED_ShowString(2, 1, "      ");
            OLED_ShowString(2, 1, passwd_temp);
        }
        
        if(num == KEY_CONFIRM){
            pd_index = 0;
            return 1;
        }
    }
    
} 



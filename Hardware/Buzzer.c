#include "stm32f10x.h"                  // Device header

#include "MyDelay.h"

void Buzzer_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if(GPIOx == GPIOA){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }else if(GPIOx == GPIOB){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOx, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOx, GPIO_Pin);
}

void Buzzer_Open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    
    GPIO_ResetBits(GPIOx, GPIO_Pin);
}

void Buzzer_Close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    
    GPIO_SetBits(GPIOx, GPIO_Pin);
}

void Buzzer_MakeSounds(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t num, uint16_t cycle_ms)
{
    uint8_t i;
    for(i = 0; i < num; i++){
        Buzzer_Open(GPIOx, GPIO_Pin);
        MyDelay_ms(cycle_ms);
        Buzzer_Close(GPIOx, GPIO_Pin);
        MyDelay_ms(cycle_ms);
    }
}



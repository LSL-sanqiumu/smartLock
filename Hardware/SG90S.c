#include "stm32f10x.h"                  // Device header

#include "SerialLog.h"

void SG90_TIM2Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    TIM_InternalClockConfig(TIM2);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 200 - 1;    /* 周期：20ms */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1; /* 0.1ms */
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    
    TIM_OCInitTypeDef TIM_OC1InitStruct;
    TIM_OCStructInit(&TIM_OC1InitStruct);
    TIM_OC1InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OC1InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1InitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC1InitStruct.TIM_Pulse = 0;
    
    TIM_OC1Init(TIM2, &TIM_OC1InitStruct);
    
    TIM_Cmd(TIM2, ENABLE);
    
}
/* 高电平: 0.5ms(-90) 1ms(-45) 1.5ms(0) 2ms(45) 2.5ms(90) */
/* 映射：-90(0)~90(180) */
/* (0,5) (180,25) y=k*degrees+b => b=5,k=1/9 */
void SG90_SetAngle(uint8_t degrees)
{
    TIM_SetCompare1(TIM2, (uint16_t)(degrees / 9 + 5));
}

void SG90_AngleReset(void)
{
    SG90_SetAngle(0);
}

void SG90_Rotate180(void)
{
    SG90_SetAngle(180);
}

void Door_Open(void)
{
    SG90_SetAngle(180);
}

void Door_Close(void)
{
    SG90_SetAngle(0);
}

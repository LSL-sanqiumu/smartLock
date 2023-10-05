#ifndef __BUZZER_H
#define __BUZZER_H

void Buzzer_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Buzzer_Open(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Buzzer_Close(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void Buzzer_MakeSounds(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t num, uint16_t cycle_ms);

#endif

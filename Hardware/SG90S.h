#ifndef __SG90S_H
#define __SG90S_H


void SG90_TIM2Init(void);
void SG90_SetAngle(uint8_t angle);

void SG90_AngleReset(void);
void SG90_Rotate180(void);

void Door_Open(void);
void Door_Close(void);

#endif

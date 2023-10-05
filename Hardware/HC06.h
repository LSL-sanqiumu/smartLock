#ifndef __HC06_H
#define __HC06_H

#define BUFF_MAX_SIZE 256

void HC06_SerialInit(void);

void Serial_SendStrToHC06(char* str);

uint8_t Serial_GetHC06RxFlag(void);



#endif

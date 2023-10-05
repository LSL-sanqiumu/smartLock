#ifndef __SERIAL_LOG_H
#define __SERIAL_LOG_H


void SerialLog_Init(uint32_t baud_rate);
void SerialLog_SendByte(uint16_t byte);

void SerialLog_SendStrMsg(char* str);

#endif

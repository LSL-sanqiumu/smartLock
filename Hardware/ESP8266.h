#ifndef __ESP8266_H
#define __ESP8266_H

void ESP8266_SerialInit(void);
    
void Serial_SendCmdToESP(char* cmd);

uint8_t ESP8266_GetRxFlag(void);

uint8_t ESP8266_Execution(char* cmd, char* response_code, uint16_t timeout_ms);

void ESP8266_APInit(void);

#endif

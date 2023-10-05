#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#define PASSWD_MAX_LEN 7

#define KEY_NONE 0
#define KEY_NUM1 1
#define KEY_NUM2 2
#define KEY_NUM3 3
#define KEY_CONFIRM 4
#define KEY_NUM5 5

void Keyboard_Init(void);
uint8_t Keyboard_Scan(void);
uint8_t WaitingPasswdEnter(void);

#endif

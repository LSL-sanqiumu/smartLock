#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"

#include "string.h"
#include "MyDelay.h"
#include "SerialLog.h"
#include "OLED.h"
#include "SG90S.h"
#include "HC06.h"
#include "ESP8266.h"
#include "Buzzer.h"
#include "Keyboard.h"

extern char hc06_rx_buff[BUFF_MAX_SIZE];
extern char passwd_temp[PASSWD_MAX_LEN];
extern char buff[512];

#define PasswdUnlockingEvent (0x01 << 0)
#define BTUnlockingEvent (0x01 << 1)
#define WiFiUnlockingEvent (0x01 << 2)
#define ANY_EVENT (PasswdUnlockingEvent | BTUnlockingEvent | WiFiUnlockingEvent)

EventGroupHandle_t UnlockingEvent_Handle = NULL;

SemaphoreHandle_t BuzzerSem_Handle = NULL;
SemaphoreHandle_t ESP8266InitSem_Handle = NULL;

TimerHandle_t Timer1_Handle = NULL;

void SG90Task(void);
void BuzzerTask(void);

void BTUnlockingTask(void);
void PasswdUnlockingTask(void);
void WiFiUnlockingTask(void);

TaskHandle_t ESP8266InitTask_Handle = NULL;
void ESP8266InitTask(void* pvParameters);

void Timer1_Callback(void);

void STM32_PeripheralInit(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    MyDelay_Init(72);
    
    SerialLog_Init(115200);
    OLED_Init();
    SG90_TIM2Init();
    Buzzer_Init(GPIOA, GPIO_Pin_1);
    HC06_SerialInit();
    Keyboard_Init();
    ESP8266_SerialInit();
    
    SG90_AngleReset();
    OLED_ShowString(1,1,"AC System");
    
}

int main(void){
    
	STM32_PeripheralInit();
    MyDelay_ms(3000);
    
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL();
    
    Timer1_Handle = xTimerCreate("Timer1-AutoReload", pdMS_TO_TICKS(100), pdFALSE, (void*)1,
    (TimerCallbackFunction_t)Timer1_Callback);
    
    UnlockingEvent_Handle = xEventGroupCreate();
    
    BuzzerSem_Handle = xSemaphoreCreateBinary();
    ESP8266InitSem_Handle = xSemaphoreCreateBinary();
    xSemaphoreGive(ESP8266InitSem_Handle);

    xReturn = xTaskCreate((TaskFunction_t)SG90Task, "SG90Task", 256, NULL, 4, NULL);
    xReturn = xTaskCreate((TaskFunction_t)BuzzerTask, "BuzzerTask", 256, NULL, 3, NULL);
    xReturn = xTaskCreate((TaskFunction_t)BTUnlockingTask, "BTUnlockingTask", 256, NULL, 2, NULL);
    xReturn = xTaskCreate((TaskFunction_t)PasswdUnlockingTask, "PasswdUnlockingTask", 
        256, NULL, 2, NULL);
    xReturn = xTaskCreate((TaskFunction_t)ESP8266InitTask, "ESP8266InitTask",
        256, NULL, 2, (TaskHandle_t*)&ESP8266InitTask_Handle);
    xReturn = xTaskCreate((TaskFunction_t)WiFiUnlockingTask, "WiFiUnlockingTask", 256, NULL, 2, NULL);
    
    taskEXIT_CRITICAL();
    if(xReturn == pdPASS)
        vTaskStartScheduler();
    
	while(1);
}


void SG90Task(void)
{
    EventBits_t r_event;
    while(1){

        r_event = xEventGroupWaitBits(
            UnlockingEvent_Handle, ANY_EVENT, 
            pdTRUE, pdFALSE, portMAX_DELAY);
        if((r_event | ANY_EVENT) == ANY_EVENT){
            Door_Open();
        }
        xSemaphoreGive(BuzzerSem_Handle);
        vTaskDelay(1000);
        Door_Close();
    }
}

void BuzzerTask(void)
{
    BaseType_t xReturn = pdPASS;
    while(1){
        xReturn = xSemaphoreTake(BuzzerSem_Handle, portMAX_DELAY);
        if(xReturn == pdPASS){
            Buzzer_MakeSounds(GPIOA, GPIO_Pin_1, 2, 50);
        }
        vTaskDelay(20);
    }
} 

void BTUnlockingTask(void)
{
    while(1){
        if(Serial_GetHC06RxFlag()){
            if(strstr(hc06_rx_buff, "123")){
                xEventGroupSetBits(UnlockingEvent_Handle, BTUnlockingEvent);
                OLED_ShowString(4, 1, "           ");
                OLED_ShowString(4, 1, "By BT : OK!");
            }else {
                Serial_SendStrToHC06("Passwd ERROR!");
            }
        }
        vTaskDelay(20);
    }
}

void PasswdUnlockingTask(void)
{
    uint8_t i;
    while(1){
        if(WaitingPasswdEnter() == 1){
            if(strstr(passwd_temp, "123")){
                xEventGroupSetBits(UnlockingEvent_Handle, PasswdUnlockingEvent);
                OLED_ShowString(4, 1, "         ");
                OLED_ShowString(4, 1, "Passwd OK! ");
                for(i = 0; i < strlen(passwd_temp); i++){
                    passwd_temp[i] = '0'; 
                }
            }else {
                OLED_ShowString(4, 1, "   ERROR!  ");
                Serial_SendStrToHC06("Passwd ERROR!");
            }
        }
        vTaskDelay(20);
    }
}    

void ESP8266InitTask(void* pvParameters)
{
    BaseType_t xReturn = pdPASS;
    while(1){
        xReturn = xSemaphoreTake(ESP8266InitSem_Handle, portMAX_DELAY);
        if(xReturn == pdPASS){
//            OLED_ShowString(3,1,"init");
            ESP8266_APInit();      
        }
        vTaskDelay(20);
    }
}

void WiFiUnlockingTask(void)
{
    while(1){
        /**
        if(Keyboard_Scan() == KEY_NUM5){
            ESP8266_APInit();
            MyDelay_ms(20);
        }
        if(ESP8266_GetRxFlag()){
            SerialLog_SendStrMsg(buff);
        }
        **/
        if(ESP8266_GetRxFlag()){
            SerialLog_SendStrMsg(buff);
            if(strstr(buff, "Open Sesame")){
                xEventGroupSetBits(UnlockingEvent_Handle, WiFiUnlockingEvent);
                OLED_ShowString(4, 1, "          ");
                OLED_ShowString(4, 1, "WiFi OK!  ");
            }else {
                OLED_ShowString(4, 1, "         ");
                OLED_ShowString(4, 1, "WiFi Fail! ");
            }
        }
        vTaskDelay(20);
    }
}    


#include "stm32f10x.h"  

#include "FreeRTOS.h"
#include "task.h"

static uint8_t fac_us = 0;
static uint16_t fac_ms = 0;

/* SysTick_Handler�ĺ궨�壬SysTick_Handler�Ѿ���ע�͵���,
xPortSysTickHandler����ʵ����port.c */
extern void xPortSysTickHandler(void);

/* SysTick�жϷ�����,ʹ��OSʱ�õ� */
/* ʹ��xTaskGetSchedulerState��Ҫ��FreeRTOS.h�е��������ø�Ϊ1
   #ifndef INCLUDE_xTaskGetSchedulerState
	#define INCLUDE_xTaskGetSchedulerState 1
   #endif 
*/

void SysTick_Handler(void)
{	
    /* ���ϵͳ�Ѿ�����,��ִ��xPortSysTickHandler() */
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        /* ����port.c�Ѿ�ʵ���˵�SysTick�жϺ��� */
        xPortSysTickHandler();	
    }
}    
/* ���õδ�ʱ����ʱ����Դ */
void MyDelay_Init(uint8_t SYSCLK)
{
	uint32_t reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); 
	fac_us = SYSCLK;							
	reload = SYSCLK;							/* ÿһ��ļ������� */	   
    /*����delay_ostickspersec�趨���ʱ��
	  reloadΪ24λ�Ĵ���,���ֵΪ16777216,
      ��168M��,��ʱ���ֵԼΪ99864us
      ��72MHz��,��ʱ���ֵԼΪ233015us
    */
	reload *= 1000000 / configTICK_RATE_HZ;		
	fac_ms = 1000 / configTICK_RATE_HZ;			// ����OS������ʱ�����ٵ�λ	   
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // ����SYSTICK�ж�
	SysTick->LOAD = reload; 					// ÿ1/configTICK_RATE_HZ���ж�һ��	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   // ����SYSTICK     
}	    
/* ��ʱnus,��������������� */
void MyDelay_us(uint32_t nus)
{		
	uint32_t ticks = 0;
	uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    
	uint32_t reload = SysTick->LOAD;		    //LOAD��ֵ	    	 
	ticks = nus * fac_us; 						//��Ҫ�Ľ����� 
	told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
    
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told)
                tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt += reload - tnow + told;	    
			told = tnow;
			if(tcnt >= ticks)
                break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}								    
} 

/* ��ʱnms,��������������� */
void MyDelay_ms(uint32_t nms)
{
	uint32_t i;
	for(i=0;i < nms; i++) 
        MyDelay_us(1000);
}

/* ������������� */
void MyDelayTask_ms(uint32_t nms)
{	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
	{		
		if(nms >= fac_ms)						//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			vTaskDelay(nms / fac_ms);	 		//FreeRTOS��ʱ
		}
		nms %= fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	MyDelay_us((uint32_t)(nms * 1000));				//��ͨ��ʽ��ʱ
}






#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F429������
//�ⲿ�ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//�ⲿ�жϳ�ʼ��
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();               //����GPIOHʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_3;               	//PH3
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //�ж���3-PH3
    HAL_NVIC_SetPriority(EXTI3_IRQn,6,0);       //��ռ���ȼ�Ϊ6�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //ʹ���ж���3
}

//������
extern TaskHandle_t Task2Task_Handler;

//�жϷ�����
void EXTI3_IRQHandler(void)
{
	BaseType_t YieldRequired;
	
	delay_xms(50);						//����
	if(KEY0==0)
	{
		YieldRequired=xTaskResumeFromISR(Task2Task_Handler);//�ָ�����2
		printf("�ָ�����2������!\r\n");
		if(YieldRequired==pdTRUE)
		{
			/*�������xTaskResumeFromISR()����ֵΪpdTRUE����ô˵��Ҫ�ָ������
			������������ȼ����ڻ��߸����������е�����(���жϴ�ϵ�����),������
			�˳��жϵ�ʱ��һ��Ҫ�����������л���*/
			portYIELD_FROM_ISR(YieldRequired);
		}
	}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);	//����жϱ�־λ
}
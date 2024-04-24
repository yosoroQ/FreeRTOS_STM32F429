#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"

//�������ȼ�
#define START_TASK_PRIO			1
//�����ջ��С	
#define START_STK_SIZE 			256  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define INTERRUPT_TASK_PRIO		2
//�����ջ��С	
#define INTERRUPT_STK_SIZE 		256  
//������
TaskHandle_t INTERRUPTTask_Handler;
//������
void interrupt_task(void *p_arg);

int main(void)
{
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
	delay_init(180);                //��ʼ����ʱ����
    LED_Init();                     //��ʼ��LED 
    uart_init(115200);              //��ʼ������
	TIM3_Init(10000-1,9000-1); 		//��ʼ����ʱ��3����ʱ����1S
	TIM5_Init(10000-1,9000-1); 		//��ʼ����ʱ��5����ʱ����1S
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //�����жϲ�������
    xTaskCreate((TaskFunction_t )interrupt_task,  			//������
                (const char*    )"interrupt_task", 			//��������
                (uint16_t       )INTERRUPT_STK_SIZE,		//�����ջ��С
                (void*          )NULL,						//���ݸ��������Ĳ���
                (UBaseType_t    )INTERRUPT_TASK_PRIO,		//�������ȼ�
                (TaskHandle_t*  )&INTERRUPTTask_Handler); 	//������
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�жϲ��������� 
void interrupt_task(void *pvParameters)
{
	static u32 total_num=0;
    while(1)
    {
		total_num+=1;
		if(total_num==5) 
		{
			printf("�ر��ж�.............\r\n");
			portDISABLE_INTERRUPTS();			//�ر��ж�
			delay_xms(5000);					//��ʱ5s
			printf("���ж�.............\r\n");	//���ж�
			portENABLE_INTERRUPTS();
		}
        LED0=~LED0;
        vTaskDelay(1000);
    }
}   





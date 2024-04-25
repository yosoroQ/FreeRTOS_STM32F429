#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"

//空闲任务任务堆栈
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
//空闲任务控制块
static StaticTask_t IdleTaskTCB;
//定时器服务任务堆栈
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
//定时器服务任务控制块
static StaticTask_t TimerTaskTCB;

//获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
//静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该
//有用户来提供，FreeRTOS 提供了接口函数 vApplicationGetIdleTaskMemory()
//实现此函数即可。
//ppxIdleTaskTCBBuffer:任务控制块内存
//ppxIdleTaskStackBuffer:任务堆栈内存
//pulIdleTaskStackSize:任务堆栈大小
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

//获取定时器服务任务的任务堆栈和任务控制块内存
//ppxTimerTaskTCBBuffer:任务控制块内存
//ppxTimerTaskStackBuffer:任务堆栈内存
//pulTimerTaskStackSize:任务堆栈大小
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&TimerTaskTCB;
	*ppxTimerTaskStackBuffer=TimerTaskStack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}

#define START_TASK_PRIO 1 //任务优先级
#define START_STK_SIZE 128 //任务堆栈大小
StackType_t StartTaskStack[START_STK_SIZE]; //任务堆栈 (1)
StaticTask_t StartTaskTCB; //任务控制块 (2)
TaskHandle_t StartTask_Handler; //任务句柄
void start_task(void *pvParameters); //任务函数
#define TASK1_TASK_PRIO 2 //任务优先级
#define TASK1_STK_SIZE 128 //任务堆栈大小
StackType_t Task1TaskStack[TASK1_STK_SIZE]; //任务堆栈
StaticTask_t Task1TaskTCB; //任务控制块
TaskHandle_t Task1Task_Handler; //任务句柄
void task1_task(void *pvParameters); //任务函数
#define TASK2_TASK_PRIO 3 //任务优先级
#define TASK2_STK_SIZE 128 //任务堆栈大小
StackType_t Task2TaskStack[TASK2_STK_SIZE]; //任务堆栈
StaticTask_t Task2TaskTCB; //任务控制块
TaskHandle_t Task2Task_Handler; //任务句柄
void task2_task(void *pvParameters); //任务函数

//LCD 刷屏时使用的颜色
int lcd_discolor[14]={ WHITE, BLACK, BLUE, BRED, 
                      GRED, GBLUE, RED, MAGENTA, 
                      GREEN, CYAN, YELLOW, BROWN, 
                      BRRED, GRAY };



int main(void)
{
    HAL_Init(); //初始化 HAL 库 
    Stm32_Clock_Init(360,25,2,8); //设置时钟,180Mhz
    delay_init(180); //初始化延时函数
    uart_init(115200); //初始化串口
    LED_Init(); //初始化 LED 
    KEY_Init(); //初始化按键
    SDRAM_Init(); //初始化 SDRAM
    LCD_Init(); //初始化 LCD
    POINT_COLOR = RED;
    LCD_ShowString(30,10,200,16,16,"Apollo STM32F4/F7");
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 5-2");
    LCD_ShowString(30,50,200,16,16,"Task Creat and Del");
    LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,90,200,16,16,"2024/4/24");
    //创建开始任务
    StartTask_Handler=xTaskCreateStatic((TaskFunction_t)start_task, //任务函数 (1)
                                        (const char* )"start_task", //任务名称
                                        (uint32_t )START_STK_SIZE, //任务堆栈大小
                                        (void* )NULL, //传递给任务函数的参数
                                        (UBaseType_t )START_TASK_PRIO, //任务优先级
                                        (StackType_t* )StartTaskStack, //任务堆栈 (2)
                                        (StaticTask_t* )&StartTaskTCB); //任务控制块(3) 
    vTaskStartScheduler(); //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //进入临界区
    //创建 TASK1 任务
    Task1Task_Handler=xTaskCreateStatic((TaskFunction_t )task1_task, //(1)
                                        (const char* )"task1_task",
                                        (uint32_t )TASK1_STK_SIZE,
                                        (void* )NULL,
                                        (UBaseType_t )TASK1_TASK_PRIO, 
                                        (StackType_t* )Task1TaskStack,
                                        (StaticTask_t* )&Task1TaskTCB);
    //创建 TASK2 任务
    Task2Task_Handler=xTaskCreateStatic((TaskFunction_t )task2_task, //(2)
                                        (const char* )"task2_task",
                                        (uint32_t )TASK2_STK_SIZE,
                                        (void* )NULL,
                                        (UBaseType_t )TASK2_TASK_PRIO, 
                                        (StackType_t* )Task2TaskStack,
                                        (StaticTask_t* )&Task2TaskTCB);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL(); //退出临界区
}
//task1 任务函数
void task1_task(void *pvParameters)
{
    u8 task1_num=0;
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(5,110,115,314); //画一个矩形
    LCD_DrawLine(5,130,115,130); //画线
    POINT_COLOR = BLUE;
    LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
    while(1)
    {
        task1_num++; //任务执 1 行次数加 1 注意 task1_num1 加到 255 的时候会清零！！
        LED0=!LED0;
        printf("任务 1 已经执行：%d 次\r\n",task1_num);
        if(task1_num==5) 
        {
            vTaskDelete(Task2Task_Handler);//任务 1 执行 5 次删除任务 2
            printf("任务 1 删除了任务 2!\r\n");
        }
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
        LCD_ShowxNum(86,111,task1_num,3,16,0x80); //显示任务执行次数
        vTaskDelay(1000); //延时 1s，也就是 1000 个时钟节拍
    }
}
//task2 任务函数
void task2_task(void *pvParameters)
{
    u8 task2_num=0;
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(125,110,234,314); //画一个矩形
    LCD_DrawLine(125,130,234,130); //画线
    POINT_COLOR = BLUE;
    LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
    while(1)
    {
        task2_num++; //任务 2 执行次数加 1 注意 task1_num2 加到 255 的时候会清零！！
        LED1=!LED1;
        printf("任务 2 已经执行：%d 次\r\n",task2_num);
        LCD_ShowxNum(206,111,task2_num,3,16,0x80); //显示任务执行次数
        LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
        vTaskDelay(1000); //延时 1s，也就是 1000 个时钟节拍
    }
}






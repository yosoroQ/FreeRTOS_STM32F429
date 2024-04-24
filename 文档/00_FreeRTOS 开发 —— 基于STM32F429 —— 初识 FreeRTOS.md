# FreeRTOS 开发 —— 基于STM32F429

# 开发板型号

**ALIENTEK STM32F429**

* 淘宝链接：https://my5353.com/RVMdS
* 稍贵，建议闲鱼，或者用**韦东山STM32F103最小板HAL-RTOS瑞士军刀**（https://goo.su/iGgD）来学习FreeRTOS，相对便宜点。

![image-20240423221546701](http://qny.expressisland.cn/gdou24/image-20240423221546701.png)

# 00_初识 FreeRTOS

* RTOS 全称是 Real Time Operating System。
* 中文名就是实时操作系统。
* RTOS 不是指某一个确定的系统，而是指一类系统。比如 uC/OS，FreeRTOS，RTX，RT-Thread 等这些都是 RTOS 类操作系统。

* 像 FreeRTOS 这种传统的 RTOS 类操作系统是由用户给每个任务分配一个任务优先级，任务调度器就可以根据此优先级来决定下一刻应该运行哪个任务。

## FreeRTOS 特点

​	FreeRTOS 是一个可裁剪的小型 RTOS 系统，其特点包括：

* FreeRTOS 的内核支持抢占式，合作式和时间片调度。
* 提供了一个用于低功耗的 Tickless 模式。
* 系统的组件在创建时可以选择动态或者静态的 RAM，比如任务、消息队列、信号量、软件定时器等等。
* FreeRTOS 系统简单、小巧、易用，通常情况下内核占用 4k-9k 字节的空间。
* 高可移植性，代码主要 C 语言编写。
* 支持实时任务和协程（co-routines 也有称为合作式、协同程序）。
* 任务与任务、任务与中断之间可以使用任务通知、消息队列、二值信号量、数值型信号量、递归互斥信号量和互斥信号量进行通信和同步。
* 具有优先级继承特性的互斥信号量。
* 高效的软件定时器。
* 强大的跟踪执行功能。
* 堆栈溢出检测功能。
* 任务数量不限。
* 任务优先级不限。

## FreeRTOS文档

* FreeRTOS的官网：https://www.freertos.org/zh-cn-cmn-s/index.html
* ![image-20240423200854287](http://qny.expressisland.cn/gdou24/image-20240423200854287.png)

### 查看 FreeRTOS 所支持的 MCU

* https://www.freertos.org/zh-cn-cmn-s/RTOS_ports.html

### FreeRTOS官方文档

* https://www.freertos.org/zh-cn-cmn-s/features.html
* ![image-20240423201110626](http://qny.expressisland.cn/gdou24/image-20240423201110626.png)

## FreeRTOS 源码初探

* 下载FreeRTOS源码：https://www.freertos.org/zh-cn-cmn-s/a00104.html

### FreeRTOS 文件预览

* 下载版本为`FreeRTOS 202212.01`

![image-20240423201320343](http://qny.expressisland.cn/gdou24/image-20240423201320343.png)

### 打开FreeRTOS文件夹

* 有三个文件夹，Demo、License 和 Source。
* ![image-20240423201618508](http://qny.expressisland.cn/gdou24/image-20240423201618508.png)

#### Demo 文件夹

* Demo 文件夹里面就是 FreeRTOS 的相关例程。
* FreeRTOS 针对不同的 MCU 提供了非常多的 Demo，其中就有 ST 的 F1、F4 和F7 的相关例程。
* 我们在移植的时候就会参考这些例程，虽然我们用的 STM32F429 的芯片，但是是可以用 F407 的工程的，因为他们都是 Cortem-M4F 的内核。

![image-20240423201747424](http://qny.expressisland.cn/gdou24/image-20240423201747424.png)

#### License 文件夹
* 这个文件夹里面就是相关的许可信息。

#### Source 文件夹
* FreeRTOS 的源码文件。
* `include 文件夹`是一些头文件，移植的时候是需要的，下面的这些`.C` 文件就是 FreeRTOS 的源码文件了，移植的时候肯定也是需要的。
* 软件到硬件中间必须有一个桥梁，portable 文件夹里面的东西就是FreeRTOS系统和具体的硬件之间的连接桥梁！
* 不同的编译环境，不同的 MCU，其桥梁应该是不同的，打开 portable 文件夹：

![image-20240423202115178](http://qny.expressisland.cn/gdou24/image-20240423202115178.png)

* FreeRTOS 针对不同的编译环境和 MCU 都有不同的“桥梁”。
* 我们这里就以 MDK 编译环境下的 STM32F429 为例。
* MemMang 这个文件夹是跟内存管理相关的，我们移植的时候是必须的。
* Keil 文件夹里面的东西肯定也是必须的，但是我们打开Keil文件夹以后里面只有一个文件：**See-also-the-RVDS-directory.txt**。
* 这个 txt 文件是什么鬼？意思就是参考 RVDS文件夹里面的东西。
* RVDS 文件夹针对不同的架构的 MCU 做了详细的分类，STM32F429 就参考 ARM_CM4F，打开 ARM_CM4F 文件夹：
* ARM_CM4F 有两个文件，这两个文件就是我们移植的时候所需要的！

![image-20240423202425278](http://qny.expressisland.cn/gdou24/image-20240423202425278.png)

### FreeRTOS-Plus 文件夹

* FreeRTOS-Plus 中的源码其实并不是 FreeRTOS 系统的源码，是在这个 FreeRTOS系统上另外增加的一些功能代码，比如 CLI、FAT、Trace等。
* 就系统本身而言，和 FreeRTOS里面的一模一样的，所以我们如果只是学习 FreeRTOS 这个系统的话，FreeRTOS-Plus 就没必要看了。

# 01_FreeRTOS 移植

## 准备工作

* 用基础例程中的**跑马灯实验**来作为基础工程。

## FreeRTOS 移植

### 向工程中添加相应文件

* 添加 FreeRTOS 源码
* 在基础工程中新建一个名为 FreeRTOS 的文件夹，并将将 FreeRTOS 的源码添加到这个文件夹中。
* ![image-20240423204707819](http://qny.expressisland.cn/gdou24/image-20240423204707819.png)

###  portable文件夹

* 在portable文件夹中，我们只需要留下 keil、MemMang 和RVDS这三个文件夹，其他的都可以删除掉。
* ![image-20240423204833731](http://qny.expressisland.cn/gdou24/image-20240423204833731.png)

### 修改工程名称和新建分组

![image-20240424092952997](http://qny.expressisland.cn/gdou24/image-20240424092952997.png)

![image-20240424092856920](http://qny.expressisland.cn/gdou24/image-20240424092856920.png)

### 向工程分组中添加文件

* 打开基础工程，新建分组**FreeRTOS_CORE**和**FreeRTOS_PORTABLE**，然后向这两个分组中添加文件：
* 分组FreeRTOS_CORE中的文件在FreeRTOS源码目录中。
* FreeRTOS_PORTABLE分组中的port.c 是 RVDS 文件夹下的ARM_CM4F中的文件，因为STM32F429是 Cortex-M4 内核并且带有 FPU，因此要选择 ARM_CM4F 中的 port.c 文件。
* heap_4.c 是 MemMang 文件夹中的，前面说了 MemMang 是跟内存管理相关的，里面有 5 个 c 文件：heap_1.c、heap_2.c、heap_3.c、heap_4.c 和 heap_5.c，这5 个.c文件是五种不同的内存管理方法。
* 这 5 个文件都可以用来作为 FreeRTOS 的内存管理文件，只是它们的实现原理不同，各有利弊。
* 这里我们选择heap_4.c。

![image-20240424093019250](http://qny.expressisland.cn/gdou24/image-20240424093019250.png)

### 添加相应的头文件路径

* 添加完 FreeRTOS 源码中的 C 文件以后还要添加 FreeRTOS 源码的头文件路径：
* ![image-20240424093346995](http://qny.expressisland.cn/gdou24/image-20240424093346995.png)

#### 如果报错

* 如果报错请到源码中的DEMO文件夹中找到相对应缺失的文件。
* 如果提示打不开“FreeRTOSConfig.h”这个文件，打开FreeRTOS 针对 STM32F407 的移植工程文件，文件夹是**CORTEX_M4F_STM32F407ZG-SK**，并复制到FreeRTOS 源码中的include 文件夹下。
* FreeRTOSConfig.h是 FreeRTOS 的配置文件，一般的操作系统都有裁剪、配置功能，而这些裁剪及配置都是通过一个文件来完成的，基本都是通过宏定义来完成对系统的配置和裁剪的。

### 修改SYSTEM文件

#### 修改sys.h文件
* sys.h 文件修改很简单，在 sys.h 文件里面用宏SYSTEM_SUPPORT_OS 来定义是否使用 OS，我们使用了 FreeRTOS，所以应该将宏SYSTEM_SUPPORT_OS 改为 1。

```c
//0,不支持os
//1,支持os
#define SYSTEM_SUPPORT_OS	1		//定义系统文件夹是否支持OS
```

#### 修改usart.c文件
* usart.c 文件有两部分要修改，一个是添加 FreeRTOS.h 头文件，默认是添加的 uC/OS 中的 includes.h 头文件。

```c
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os 使用	  
#endif
```

* 另外一个就是 USART1 的中断服务函数，在使用 uC/OS 的时候进出中断的时候需要添加`OSIntEnter()`和`OSIntExit()`，使用FreeRTOS 的话就不需要了，所以将这两行代码删除掉。

```c
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;

	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>maxDelay) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>maxDelay) break;	
	}
} 
```

#### 修改 delay.c 文件
* delay.c 文件因为涉及到 FreeRTOS 的系统时钟，delay.c 文件里面有4个函数，先来看一下函数 SysTick_Handler()，此函数是滴答定时器的中断服务函数。

```c
extern void xPortSysTickHandler(void);
//systick中断服务函数,使用OS时用到
void SysTick_Handler(void)
{  
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        xPortSysTickHandler();	
    }
    HAL_IncTick();
}
```

* FreeRTOS 的心跳就是由滴答定时器产生的，根据 FreeRTOS 的系统时钟节拍设置好滴答定时器的周期，这样就会周期触发滴答定时器中断了。
* 在滴答定时器中断服务函数中调用FreeRTOS 的 API 函数xPortSysTickHandler()，如果使用 HAL 库的话还需要调用 HAL 库中的函数 HAL_IncTick()！
* 因为 HAL 库中的延时是靠 HAL_IncTick()来完成的。delay_init()是用来初始化滴答定时器和延时函数。

```c
//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为AHB时钟
//SYSCLK:系统时钟频率
void delay_init(u8 SYSCLK)
{
	u32 reload;
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
	fac_us=SYSCLK;						    //不论是否使用OS,fac_us都需要使用
	reload=SYSCLK;					        //每秒钟的计数次数 单位为K	   
	reload*=1000000/configTICK_RATE_HZ;		//根据configTICK_RATE_HZ设定溢出时间
											//reload为24位寄存器,最大值:16777216,在180M下,约合0.745s左右	
	fac_ms=1000/configTICK_RATE_HZ;			//代表OS可以延时的最少单位		
    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
	SysTick->LOAD=reload; 					//每1/configTICK_RATE_HZ断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
}				
```

* 前面我们说了 FreeRTOS 的系统时钟是由滴答定时器提供的，那么肯定要根据 FreeRTOS 的系统时钟节拍来初始化滴答定时器了，delay_init()就是来完成这个功能的。
* FreeRTOS 的系统时钟节拍由宏 configTICK_RATE_HZ 来设置，这个值我们可以自由设置，但是一旦设置好以后我们就要根据这个值来初始化滴答定时器，其实就是设置滴答定时器的中断周期。
* 由于我们使用了 HAL 库，所以在设置滴答定时器的时候要照顾到 HAL 库，HAL 库里的延时函数要求滴答定时器周期为 1ms，因此FreeRTOS 的系统节拍应该设置为 1000HZ，也就是 1ms 的周期。
* 接下来的三个函数都是延时的：

```c
//延时nus
//nus:要延时的us数.	
//nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};									    
}  
	
//延时nms,会引起任务调度
//nms:要延时的ms数
//nms:0~65535
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
	{		
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(nms/fac_ms);	 		//FreeRTOS延时
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));				//普通方式延时
}

//延时nms,不会引起任务调度
//nms:要延时的ms数
void delay_xms(u32 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}
```

* delay_us()是 us 级延时函数，delay_ms 和 delay_xms()都是 ms 级的延时函数，delay_us()和delay_xms()不会导致任务切换。
* delay_ms()其实就是对 FreeRTOS 中的延时函数 vTaskDelay()的简单封装，所以在使用 delay_ms()的时候就会导致任务切换。

##### 如果编译报错“有重复定义的函数”

* 如果 port.c 和 delay.c 中有重复定义的函数：SysTick_Handler()，那么二选一。
* 很明显 delay.c 中的 SysTick_Handler()得留下来，打开FreeRTOSConfig.h 文件，找到如下一个宏定义，并屏蔽掉此宏定义。

```c
#define xPortSysTickHandler SysTick_Handler
```

## 移植验证实验

### 实验设计
* 本实验设计四个任务：`start_task()`、`led0_task ()`、`led1_task ()`和 `float_task()`。
* 这四个任务的任务功能如下：
  * `start_task()`：用来创建其他三个任务。
  * `led0_task ()`：控制 LED0 的闪烁，提示系统正在运行。
  * `led1_task ()`：控制 LED1 的闪烁。
  * `float_task()`：简单的浮点测试任务，用于测试 STM32F4 的FPU 是否工作正常。

### 实验程序（main.c）

* 测试代码中创建了 3 个任务：LED0 测试任务、LED1 测试任务和浮点测试任务。
* 它们的任务函数分别为：`led0_task()`、`led1_task()`和`float_task()`。
* `led0_task()`和 `led1_task()`任务很简单，就是让 LED0 和 LED1 周期性闪烁。
* 因为 STM32F429 支持 FPU，所以必须测试一下 FreeRTOS 是否也支持 FPU。
* `float_task()`任务就是用来完成这个功能，定义一个 float 变量，然后每隔 1s 加0.01，并且通过串口打印出来。

```c
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);

//任务优先级
#define FLOAT_TASK_PRIO		4
//任务堆栈大小	
#define FLOAT_STK_SIZE 		128
//任务句柄
TaskHandle_t FLOATTask_Handler;
//任务函数
void float_task(void *pvParameters);

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED 
    uart_init(115200);              //初始化串口
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);        
    //浮点测试任务
    xTaskCreate((TaskFunction_t )float_task,     
                (const char*    )"float_task",   
                (uint16_t       )FLOAT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )FLOAT_TASK_PRIO,
                (TaskHandle_t*  )&FLOATTask_Handler);  
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(500);
    }
}   

//LED1任务函数
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

//浮点测试任务
void float_task(void *pvParameters)
{
	static float float_num=0.00;
	while(1)
	{
		float_num+=0.01f;
		printf("float_num的值为: %.4f\r\n",float_num);
        vTaskDelay(1000);
	}
}
```

### 实验程序运行结果分析

* **请先给STM32供电！**
* **没有驱动装驱动！**

#### 选择调试器ST-Link

![image-20240424102109525](http://qny.expressisland.cn/gdou24/image-20240424102109525.png)

![image-20240424102149004](http://qny.expressisland.cn/gdou24/image-20240424102149004.png)

#### 测试运行

* 编译并下载代码到 STM32F429 开发板中，下载进去以后会看到 LED0 和 LED1 开始闪烁，LED0 均匀闪烁，那是因为我们在 LED0 的任务代码中设置好的 LED0 亮 500ms，灭 500ms。
* LED1 亮的时间短，灭的时间长，这是因为在 LED1 的任务代码中设置好的亮 200ms，灭 800ms。

![VID_20240424_104502 -middle-original](http://qny.expressisland.cn/gdou24/VID_20240424_104502%20-middle-original.gif)
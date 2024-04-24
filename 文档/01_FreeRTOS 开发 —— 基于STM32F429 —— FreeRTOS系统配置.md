# FreeRTOS 开发 —— 基于STM32F429

# 02_FreeRTOS系统配置

## FreeRTOS Config.h 文件

* FreeRTOS 的配置基本是通过在FreeRTOSConfig.h中使用“`#define`”这样的语句来定义宏定义实现的。
* 在 FreeRTOS 的官方 demo 中，每个工程都有一个 FreeRTOSConfig.h 文件，我们在使用的时候可以参考这个文件，甚至直接复制粘贴使用。

## “INCLUDE_”开始的宏

* 使用“`INCLUDE_`”开头的宏用来表示使能或除能 FreeRTOS 中相应的 API 函数，作用就是用来配置 FreeRTOS 中的可选 API 函数的。
* 比如当宏 INCLUDE_vTaskPrioritySet 设置为 0 的时候表 示不 能使用函数vTaskPrioritySet()，当 设置为1的时候就表 示可以使用函数vTaskPrioritySet()，这个功能其实就是条件编译，在文件 tasks.c 中有如下图所示的代码。

![image-20240424150819395](http://qny.expressisland.cn/gdou24/image-20240424150819395.png)

* 从图中可以看出当满足条件：`NCLUDE_vTaskPrioritySet == 1` 的时候 ，函数`vTaskPrioritySet()`才会被编译。
* FreeRTOS 中的裁剪和配置就是这种用条件编译的方法来实现的，不止 FreeRTOS 这么干，很多的协议栈、RTOS 系统和 GUI 库等都是使用条件编译的方法来完成配置和裁剪的。
* 条件编译的好处就是节省空间，不需要的功能就不用编译，这样就可以根据实际需求来减少系统占用的 ROM 和 RAM 大小，根据自己所使用的 MCU 来调整系统消耗，降低成本。

### “`INCLUDE_`”开始的都有哪些宏，它们的功能都是什么？

![image-20240424151002273](http://qny.expressisland.cn/gdou24/image-20240424151002273.png)

![image-20240424151014619](http://qny.expressisland.cn/gdou24/image-20240424151014619.png)

![image-20240424151029994](http://qny.expressisland.cn/gdou24/image-20240424151029994.png)

## “config”开始的宏

* “`config`”开始的宏和“`INCLUDE_`”开始的宏一样，都是用来完成 FreeRTOS 的配置和裁剪的。

![image-20240424151203103](http://qny.expressisland.cn/gdou24/image-20240424151203103.png)

![image-20240424151233213](http://qny.expressisland.cn/gdou24/image-20240424151233213.png)

![image-20240424151257017](http://qny.expressisland.cn/gdou24/image-20240424151257017.png)

![image-20240424151325688](http://qny.expressisland.cn/gdou24/image-20240424151325688.png)

![image-20240424151338259](http://qny.expressisland.cn/gdou24/image-20240424151338259.png)

![image-20240424151357679](http://qny.expressisland.cn/gdou24/image-20240424151357679.png)

![image-20240424151416316](http://qny.expressisland.cn/gdou24/image-20240424151416316.png)

![image-20240424151427097](http://qny.expressisland.cn/gdou24/image-20240424151427097.png)

![image-20240424151444498](http://qny.expressisland.cn/gdou24/image-20240424151444498.png)

![image-20240424151500541](http://qny.expressisland.cn/gdou24/image-20240424151500541.png)

![image-20240424151513380](http://qny.expressisland.cn/gdou24/image-20240424151513380.png)

![image-20240424151523878](http://qny.expressisland.cn/gdou24/image-20240424151523878.png)

![image-20240424151538754](http://qny.expressisland.cn/gdou24/image-20240424151538754.png)

![image-20240424151549824](http://qny.expressisland.cn/gdou24/image-20240424151549824.png)

# 03_FreeRTOS 中断配置和临界段

* FreeRTOS 的中断配置是一个很重要的内容，需要根据所使用的 MCU 来具体配置。
* 需要了解 MCU 架构中有关中断的知识，本章结合 Cortex-M 的 NVIC 来讲解 STM32平台下的FreeRTOS 中断配置。

## Cortex-M中断

### 中断简介
* 中断是微控制器一个很常见的特性，中断由硬件产生，当中断产生以后 CPU 就会中断当前的流程转而去处理中断服务，Cortex-M 内核的 MCU 提供了一个用于中断管理的嵌套向量中断控制器(NVIC)。
* Cotex-M3 和 M4 的 NVIC 最多支持 240 个IRQ(中断请求)、1 个不可屏蔽中断(NMI)、1 个Systick(滴答定时器)定时器中断和多个系统异常。

### 中断管理简介
* Cortex-M 处理器有多个用于管理中断和异常的可编程寄存器，这些寄存器大多数都在NVIC 和系统控制块(SCB)中，CMSIS 将这些寄存器定义为结构体。
* 以STM32F429为例，打开core_cm4.h，有两个结构体，NVIC_Type 和 SCB_Type，如下：

```c
typedef struct
{
 __IOM uint32_t ISER[8U]; /*!< Offset: 0x000 (R/W) Interrupt Set Enable Register */
 uint32_t RESERVED0[24U];
 __IOM uint32_t ICER[8U]; /*!< Offset: 0x080 (R/W) Interrupt Clear Enable Register */
 uint32_t RSERVED1[24U];
 __IOM uint32_t ISPR[8U]; /*!< Offset: 0x100 (R/W) Interrupt Set Pending Register */
 uint32_t RESERVED2[24U];
 __IOM uint32_t ICPR[8U]; /*!< Offset: 0x180 (R/W) Interrupt Clear Pending Register */
 uint32_t RESERVED3[24U];
 __IOM uint32_t IABR[8U]; /*!< Offset: 0x200 (R/W) Interrupt Active bit Register */
 uint32_t RESERVED4[56U];
 __IOM uint8_t IP[240U]; /*!< Offset: 0x300 (R/W) Interrupt Priority Register (8Bit wide) */
 uint32_t RESERVED5[644U];
 __OM uint32_t STIR; /*!< Offset: 0xE00 ( /W) Software Trigger Interrupt Register */
} NVIC_Type;
typedef struct
{
 __IM uint32_t CPUID; /*!< Offset: 0x000 (R/ ) CPUID Base Register */
 __IOM uint32_t ICSR; /*!< Offset: 0x004 (R/W) Interrupt Control and State Register */
 __IOM uint32_t VTOR; /*!< Offset: 0x008 (R/W) Vector Table Offset Register */
 __IOM uint32_t AIRCR; /*!< Offset: 0x00C (R/W) Application Interrupt and Reset
Control Register */
 __IOM uint32_t SCR; /*!< Offset: 0x010 (R/W) System Control Register */
 __IOM uint32_t CCR; /*!< Offset: 0x014 (R/W) Configuration Control Register */
 __IOM uint8_t SHP[12U]; /*!< Offset: 0x018 (R/W) System Handlers Priority 
Registers (4-7, 8-11, 12-15) */
 __IOM uint32_t SHCSR; /*!< Offset: 0x024 (R/W) System Handler Control and 
State Register */
     __IOM uint32_t CFSR; /*!< Offset: 0x028 (R/W) Configurable Fault Status Register */
 __IOM uint32_t HFSR; /*!< Offset: 0x02C (R/W) HardFault Status Register */
 __IOM uint32_t DFSR; /*!< Offset: 0x030 (R/W) Debug Fault Status Register */
 __IOM uint32_t MMFAR; /*!< Offset: 0x034 (R/W) MemManage Fault Address Register */
 __IOM uint32_t BFAR; /*!< Offset: 0x038 (R/W) BusFault Address Register */
 __IOM uint32_t AFSR; /*!< Offset: 0x03C (R/W) Auxiliary Fault Status Register */
 __IM uint32_t PFR[2U]; /*!< Offset: 0x040 (R/ ) Processor Feature Register */
 __IM uint32_t DFR; /*!< Offset: 0x048 (R/ ) Debug Feature Register */
 __IM uint32_t ADR; /*!< Offset: 0x04C (R/ ) Auxiliary Feature Register */
 __IM uint32_t MMFR[4U]; /*!< Offset: 0x050 (R/ ) Memory Model Feature Register */
 __IM uint32_t ISAR[5U]; /*!< Offset: 0x060 (R/ ) Instruction Set Attributes Register */
 uint32_t RESERVED0[5U];
 __IOM uint32_t CPACR; /*!< Offset: 0x088 (R/W) Coprocessor Access Control Register */
} SCB_Type;
```

* NVIC 和 SCB 都位于系统控制空间(SCS)内，SCS 的地址从 0XE000E000 开始，SCB 和NVIC的地址也在 core_cm4.h 中有定义，如下：

```c
#define SCS_BASE (0xE000E000UL) /*!< System Control Space Base Address */
#define NVIC_BASE (SCS_BASE + 0x0100UL) /*!< NVIC Base Address */
#define SCB_BASE (SCS_BASE + 0x0D00UL) /*!< System Control Block Base Address 
#define SCnSCB ((SCnSCB_Type*) SCS_BASE) /*!< System control Register not in SCB */
#define SCB ((SCB_Type *) SCB_BASE) /*!< SCB configuration struct */
#define NVIC ((NVIC_Type *) NVIC_BASE ) /*!< NVIC configuration struct */
```

* 以上的中断控制寄存器我们在移植FreeRTOS 的时候是不需要关心的。
* 我们重点关心的是是三个中断屏蔽寄存器：**PRIMASK**、**FAULTMASK** 和 **BASEPRI**。

### 优先级分组定义

* 当多个中断来临的时候处理器应该响应哪一个中断是由中断的优先级来决定的，高优先级的中断（优先级编号小）肯定是首先得到响应，而且高优先级的中断可以抢占低优先级的中断，这个就是中断嵌套。
* Cortex-M 处理器的有些中断是具有固定的优先级的，比如复位、NMI、HardFault，这些中断的优先级都是负数，优先级也是最高的。
* Cortex-M 处理器有三个固定优先级和 256 个可编程的优先级，最多有 128 个抢占等级，但是实际的优先级数量是由芯片厂商来决定的。
* 但是，绝大多数的芯片都会精简设计的，以致实际上支持的优先级数会更少，如 8 级、16 级、32 级等，比如 STM32 就只有 16 级优先级。
* 在设计芯片的时候会裁掉表达优先级的几个低端有效位，以减少优先级数，所以不管用多少位来表达优先级，都是 MSB 对齐的。

#### 使用三位来表达优先级

* Bit0~Bit4 没有实现，所以读它们总是返回零，写如它们的话则会忽略写入的值。
* 因此，对于 3 个位的情况，可是使用的优先级就是 8 个：0X00(最高优先级)、0X20、0X40、0X60、0X80、0XA0、0XC0 和 0XE0。
* 注意，这个是芯片厂商来决定的！不是我们能决定的，比如 STM32 就选择了 4 位作为优先级！

![image-20240424152931450](http://qny.expressisland.cn/gdou24/image-20240424152931450.png)

#### 优先级配置寄存器是 8 位宽的，为什么却只有128个抢占等级？8 位不应该是 256 个抢占等级吗？

* 为了使抢占机能变得更可控，Cortex-M 处理器还把 256 个优先级按位分为高低两段：
* 抢占优先级(分组优先级)和亚优先级(子优先级)，NVIC 中有一个寄存器是“应用程序中断及复位控制寄存器(AIRCR)”，AIRCR 寄存器里面有个位段名为“优先级组”。
* 表 4.1.3.1 中 PRIGROUP 就是优先级分组。

![image-20240424153135098](http://qny.expressisland.cn/gdou24/image-20240424153135098.png)

* 它把优先级分为两个位段：MSB 所在的位段（左边的）对应抢占优先级，LSB 所在的位段（右边的）对应亚优先级。

![image-20240424153246244](http://qny.expressisland.cn/gdou24/image-20240424153246244.png)

* 在看一下 STM32 的优先级分组情况，我们前面说了 STM32 使用了 4 位，因此最多有 5 组优先级分组设置，这 5 个分组在stm32f4xx_hal_cortex.h 中有定义：

```c
#define NVIC_PRIORITYGROUP_0 ((uint32_t)0x00000007) /*!< 0 bits for pre-emption priority
 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1 ((uint32_t)0x00000006) /*!< 1 bits for pre-emption priority
 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2 ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority
 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3 ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority
 1 bits for subpriority */
#define NVIC_PRIORITYGROUP_4 ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority
```

* 可以看出 STM32 有 5 个分组，但是一定要注意！STM32 中定义的分组 0 对应的值是 7！
* 如果我们选择分组 4，即NVIC_PRIORITYGROUP_4 的话，那 4 位优先级就都全是抢占优先级了，没有亚优先级，那么就有 0~15 共 16 个优先级。
* 而移植 FreeRTOS 的时候我们配置的就是
  组 4，ST 官方默认的也是组 4，优先级分组配置在 HAL_Init()中：

![image-20240424153349424](http://qny.expressisland.cn/gdou24/image-20240424153349424.png)

* 如果使用 ALIENTEK 的基础例程的话默认配置的组 2，所以在将基础例程中的外设驱动移植到 FreeRTOS 下面的时候需要修改优先级配置。
* 主要是 FreeRTOS 的中断配置没有处理亚优
  先级这种情况，所以只能配置为组 4，直接就 16 个优先级，使用起来也简单！

### 优先级设置

![image-20240424153429128](http://qny.expressisland.cn/gdou24/image-20240424153429128.png)

![image-20240424153446798](http://qny.expressisland.cn/gdou24/image-20240424153446798.png)

### 用于中断屏蔽的特殊寄存器

![image-20240424154041310](http://qny.expressisland.cn/gdou24/image-20240424154041310.png)

![image-20240424154103224](http://qny.expressisland.cn/gdou24/image-20240424154103224.png)

## FreeRTOS中断配置宏

![image-20240424154150257](http://qny.expressisland.cn/gdou24/image-20240424154150257.png)

![image-20240424154221523](http://qny.expressisland.cn/gdou24/image-20240424154221523.png)

![image-20240424154330443](http://qny.expressisland.cn/gdou24/image-20240424154330443.png)

![image-20240424154346553](http://qny.expressisland.cn/gdou24/image-20240424154346553.png)

![image-20240424154401500](http://qny.expressisland.cn/gdou24/image-20240424154401500.png)

## FreeRTOS开关中断

![image-20240424154546407](http://qny.expressisland.cn/gdou24/image-20240424154546407.png)

## 临界段代码

* 临界段代码也叫做临界区，是指那些必须完整运行，不能被打断的代码段，比如有的外设的初始化需要严格的时序，初始化过程中不能被打断。
* FreeRTOS 在进入临界段代码的时候需要关闭中断，当处理完临界段代码以后再打开中断。
* FreeRTOS 系统本身就有很多的临界段代码，这些代码都加了临界段代码保护，我们在写自己的用户程序的时候有些地方也需要添加临界段代码保护。
* FreeRTOS 与临界段代码保护有关的函数有 4 个 ： `taskENTER_CRITICAL()` 、`taskEXIT_CRITICAL()` 、`taskENTER_CRITICAL_FROM_ISR()` 和`taskEXIT_CRITICAL_FROM_ISR()`，这四个函数其实是宏定义，在 task.h 文件中有定义。
* 这四个函数的区别是前两个是任务级的临界段代码保护，后两个是中断级的临界段代码保护。

![image-20240424154723823](http://qny.expressisland.cn/gdou24/image-20240424154723823.png)

![image-20240424154749102](http://qny.expressisland.cn/gdou24/image-20240424154749102.png)

![image-20240424154801317](http://qny.expressisland.cn/gdou24/image-20240424154801317.png)

![image-20240424154813954](http://qny.expressisland.cn/gdou24/image-20240424154813954.png)

## FreeRTOS中断测试实验

* FreeRTOS 中优先级低于`configMAX_SYSCALL_INTERRUPT_PRIORITY`的中断会被屏蔽掉，高于的就不会，那么本节我们就写个简单的例程测试一下。
* 使用两个定时器，一个优先级为 4，一个优先级为 5，两个定时器每隔 1s 通过串口输出一串字符串。
* 然后在某个任务中关闭中断一段时间，查看两个定时器的输出情况。

### 实验设计

* 本实验设计了两个任务`start_task()`和 `interrupt_task()`。
* 这两个任务的任务功能如下：
  * `start_task()`：创建另外一个任务。
  * `interrupt_task()` ： 中断测试任务，任务中会调用FreeRTOS的关中断函数`portDISABLE_INTERRUPTS()`来将中断关闭一段时间。

#### 添加定时器驱动（HARDWARE/timer.c）

##### 中断初始化及处理过程

![image-20240424213651308](http://qny.expressisland.cn/gdou24/image-20240424213651308.png)

```c
#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
TIM_HandleTypeDef TIM5_Handler;      //定时器句柄 

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频系数
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}

//通用定时器5中断初始化
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIM5_Init(u32 arr,u16 psc)
{  
    TIM5_Handler.Instance=TIM5;                          //通用定时器5
    TIM5_Handler.Init.Prescaler=psc;                     //分频系数
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM5_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器5和定时器5更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,4,0);    //设置中断优先级，抢占优先级4，子优先级0
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}
	else if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM5时钟
		HAL_NVIC_SetPriority(TIM5_IRQn,5,0);    //设置中断优先级，抢占优先级5，子优先级0
		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM5中断   
	}
}


//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
        printf("TIM3输出.......\r\n");
    }
	else if(htim==(&TIM5_Handler))
	{
		printf("TIM5输出.......\r\n");
	}
}
```

### 并添加和定时器有关的hal库文件

![image-20240424213234758](http://qny.expressisland.cn/gdou24/image-20240424213234758.png)

### 代码（main.c）

![image-20240424213619483](http://qny.expressisland.cn/gdou24/image-20240424213619483.png)

```c
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"

//任务优先级
#define START_TASK_PRIO			1
//任务堆栈大小	
#define START_STK_SIZE 			256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define INTERRUPT_TASK_PRIO		2
//任务堆栈大小	
#define INTERRUPT_STK_SIZE 		256  
//任务句柄
TaskHandle_t INTERRUPTTask_Handler;
//任务函数
void interrupt_task(void *p_arg);

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED 
    uart_init(115200);              //初始化串口
	TIM3_Init(10000-1,9000-1); 		//初始化定时器3，定时周期1S
	TIM5_Init(10000-1,9000-1); 		//初始化定时器5，定时周期1S
	
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
    //创建中断测试任务
    xTaskCreate((TaskFunction_t )interrupt_task,  			//任务函数
                (const char*    )"interrupt_task", 			//任务名称
                (uint16_t       )INTERRUPT_STK_SIZE,		//任务堆栈大小
                (void*          )NULL,						//传递给任务函数的参数
                (UBaseType_t    )INTERRUPT_TASK_PRIO,		//任务优先级
                (TaskHandle_t*  )&INTERRUPTTask_Handler); 	//任务句柄
	vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//中断测试任务函数 
void interrupt_task(void *pvParameters)
{
	static u32 total_num=0;
    while(1)
    {
		total_num+=1;
		if(total_num==5) 
		{
			printf("关闭中断.............\r\n");
			portDISABLE_INTERRUPTS();			//关闭中断
			delay_xms(5000);					//延时5s
			printf("打开中断.............\r\n");	//打开中断
			portENABLE_INTERRUPTS();
		}
        LED0=~LED0;
        vTaskDelay(1000);
    }
}   
```

### 测试运行


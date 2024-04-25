# FreeRTOS 开发 —— 基于STM32F429

# 04_FreeRTOS任务基础知识

* RTOS 系统的核心就是任务管理，初步上手RTOS 系统首先必须掌握的也是任务的创建、删除、挂起和恢复等操作，由此可见任务管理的重要性。

## 什么是多任务系统

* 以前在使用 51、AVR、STM32 单片机裸机（未使用系统）的时候一般都是在main 函数里面用 `while(1)`做一个大循环来完成所有的处理，即应用程序是一个无限的循环，循环中调用相应的函数完成所需的处理。
* 有时候我们也需要中断中完成一些处理，相对于多任务系统而言，这个就是单任务系统，也称作前后台系统，中断服务函数作为前台程序，大循环`while(1)`作为后台程序。

![image-20240425100303149](http://qny.expressisland.cn/gdou24/image-20240425100303149.png)

* 前后台系统的实时性差，前后台系统各个任务都是排队等着轮流执行，不管你这个程序现在有多紧急，没轮到你就只能等着！相当于所有任务的优先级都是一样的。
* 但是前后台系统简单啊，资源消耗也少啊！在稍微大一点的嵌入式应用中前后台系统就明显力不从心了，此时就需要多任务系统出马了。
* 多任务系统会把一个大问题（应用）“分而治之”，把大问题划分成很多个小问题，逐步的把小问题解决掉，大问题也就随之解决了，这些小问题可以单独的作为一个小任务来处理。
* 这些小任务是并发处理的，注意，并不是说同一时刻一起执行很多个任务，而是由于每个任务执行的时间很短，导致看起来像是同一时刻执行了很多个任务一样。

### 多个任务带来了一个新的问题，究竟哪个任务先运行，哪个任务后运行呢？

* 完成这个功能的东西在 RTOS 系统中叫做任务调度器。
* 不同的系统其任务调度器的实现方法也不同，比如 FreeRTOS 是一个抢占式的实时多任务系统，那么其任务调度器也是抢占式的。

#### 运行过程

![image-20240425100512642](http://qny.expressisland.cn/gdou24/image-20240425100512642.png)

* 高优先级的任务可以打断低优先级任务的运行而取得 CPU 的使用权，这样就保证了那些紧急任务的运行。
* 这样我们就可以为那些对实时性要求高的任务设置一个很高的优先级，比如自动驾驶中的障碍物检测任务等。
* 高优先级的任务执行完成以后重新把 CPU 的使用权归还给低优先级的任务，这个就是抢占式多任务系统的基本原理。

## FreeRTOS 任务与协程

* FreeRTOS 中应用既可以使用**任务**，也可以使用*协程(Co-Routine)*，或者两者混合使用。
* 但是任务和协程使用不同的API函数，因此不能通过队列（或信号量）将数据从任务发送给协程，反之亦然。
* 协程是为那些资源很少的 MCU 准备的，其开销很小，但是 FreeRTOS 官方已经不打算再更新协程了，所以本文档只讲解任务。

### 任务(Task)的特性
* 在使用 RTOS 的时候一个实时应用可以作为一个独立的任务。
* 每个任务都有自己的运行环境，不依赖于系统中其他的任务或者 RTOS 调度器。
* 任何一个时间点只能有一个任务运行，具体运行哪个任务是由 RTOS 调度器来决定的，RTOS 调度器因此就会重复的开启、关闭每个任务。
* 任务不需要了解 RTOS 调度器的具体行为，RTOS 调度器的职责是确保当一个任务开始执行的时候其上下文环境（寄存器值，堆栈内容等）和任务上一次退出的时候相同。
* 为了做到这一点，每个任务都必须有个堆栈，当任务切换的时候将上下文环境保存在堆栈中，这样当任务再次执行的时候就可以从堆栈中取出上下文环境，任务恢复运行。
* 任务特性：
  * *简单。*
  * *没有使用限制。*
  * *支持抢占。*
  * *支持优先级。*
  * *每个任务都拥有堆栈导致了 RAM 使用量增大。*
  * *如果使用抢占的话的必须仔细的考虑重入的问题。*

### 协程(Co-routine)的特性

* 协程是为那些资源很少的 MCU 而做的，很少用到了！

#### 概念上协程和任务是相似的，其不同是什么呢？

![image-20240425100918530](http://qny.expressisland.cn/gdou24/image-20240425100918530.png)

## 任务状态

![image-20240425101110259](http://qny.expressisland.cn/gdou24/image-20240425101110259.png)

* **运行态**
  * 当一个任务正在运行时，那么就说这个任务处于运行态，处于运行态的任务就是当前正在使用处理器的任务。
  * 如果使用的是单核处理器的话那么不管在任何时刻永远都只有一个任务处于运行态。
* **就绪态**
  * 处于就绪态的任务是那些已经准备就绪（这些任务没有被阻塞或者挂起），可以运行的任务，但是处于就绪态的任务还没有运行，因为有一个同优先级或者更高优先级的任务正在运行！
* **阻塞态**
  * 如果一个任务当前正在等待某个外部事件的话就说它处于阻塞态，比如说如果某个任务调用了函数`vTaskDelay()`的话就会进入阻塞态，直到延时周期完成。
  * 任务在等待队列、信号量、事件组、通知或互斥信号量的时候也会进入阻塞态。
  * 任务进入阻塞态会有一个超时时间，当超过这个超时时间任务就会退出阻塞态，即使所等待的事件还没有来临！
* **挂起态**
  * 像阻塞态一样，任务进入挂起态以后也不能被调度器调用进入运行态，但是进入挂起态的任务没有超时时间。
  * 任务进入和退出挂起态通过调用函数`vTaskSuspend()`和`xTaskResume()`。

## 任务优先级

* 每 个 任 务 都 可 以 分 配 一 个 从 0~（`configMAX_PRIORITIES-1`）的 优 先 级 ，`configMAX_PRIORITIES` 在文件`FreeRTOSConfig.h`中有定义， 宏`configUSE_PORT_OPTIMISED_TASK_SELECTION`也 设 置 为 了 1 ， 那么宏`configMAX_PRIORITIES`不能超过 32！也就是优先级不能超过 32 级。
* 其他情况下宏`configMAX_PRIORITIES`可以为任意值，但是考虑到 RAM 的消耗，宏`configMAX_PRIORITIES`最好设置为一个满足应用的最小值。
* 优先级数字越低表示任务的优先级越低，0 的优先级最低，`configMAX_PRIORITIES-1` 的优先级最高，空闲任务的优先级最低，为 0。
* FreeRTOS 调度器确保处于就绪态或运行态的高优先级的任务获取处理器使用权，换句话说就是处于就绪态的最高优先级的任务才会运行。
* 当宏 `configUSE_TIME_SLICING` 定义为 1 的时候多个任务可以共用一个优先级，数量不限。
* 默认情况下宏 configUSE_TIME_SLICING 在文件 FreeRTOS.h 中已经定义为 1。
* 此时处于就绪态的优先级相同的任务就会使用时间片轮转调度器获取运行时间。

## 任务实现

* 在使用 FreeRTOS 的过程中，我们要使用函数 `xTaskCreate()`或 `xTaskCreateStatic()`来创建任务，这两个函数的第一个参数`pxTaskCode`，就是这个任务的任务函数。

### 什么是任务函数？

* 任务函数就是完成本任务工作的函数。我这个任务要干嘛？要做什么？要完成什么样的功能都是在这个任务函数中实现的。 
* 比如我要做个任务，这个任务要点个流水灯，那么这个流水灯的程序，就是任务函数中实现的。

### FreeRTOS官方给出的任务函数模板

```c
void vATaskFunction(void *pvParameters) //(1)
{
    for( ; ; ) 							//(2)
    {
        --任务应用程序-- 							//(3)
            vTaskDelay(); 								//(4)
    }
    /* 不能从任务函数中返回或者退出， 从 任 务 函 数 中 返 回 或 退 出 的 话 就 会 调 用
    configASSERT()，前提是你定义了 configASSERT()。如果一定要从任务函数中退出的话那一定 
    要调用函数 vTaskDelete(NULL)来删除此任务。*/
    vTaskDelete(NULL); 					//(5)
}
```

![image-20240425101509820](http://qny.expressisland.cn/gdou24/image-20240425101509820.png)

## 任务控制块

* FreeRTOS 的每个任务都有一些属性需要存储，FreeRTOS 把这些属性集合到一起用一个结构体来表示，这个结构体叫做任务控制块：`TCB_t`，在使用函数 xTaskCreate()创建任务的时候就会自动的给每个任务分配一个任务控制块。
* 在老版本的 FreeRTOS 中任务控制块叫做`tskTCB`，新版本重命名为 `TCB_t`，但是本质上还是 `tskTCB`，此结构体在文件 **tasks.c** 中有定义，如下：

```c
typedef struct tskTaskControlBlock
{
    volatile StackType_t *pxTopOfStack; //任务堆栈栈顶
    #if ( portUSING_MPU_WRAPPERS == 1 )
    xMPU_SETTINGSxMPUSettings; //MPU 相关设置
    #endif
    ListItem_t xStateListItem; //状态列表项
    ListItem_t xEventListItem; //事件列表项
    UBaseType_t uxPriority; //任务优先级
    StackType_t *pxStack; //任务堆栈起始地址
    char pcTaskName[ configMAX_TASK_NAME_LEN ];//任务名字
    #if ( portSTACK_GROWTH > 0 )
    StackType_t *pxEndOfStack; //任务堆栈栈底
    #endif
    #if ( portCRITICAL_NESTING_IN_TCB == 1 )
    UBaseType_t uxCriticalNesting; //临界区嵌套深度
    #endif
    #if ( configUSE_TRACE_FACILITY == 1 ) //trace 或到 debug 的时候用到
    UBaseType_t uxTCBNumber;
    UBaseType_t uxTaskNumber;
    #endif
    #if ( configUSE_MUTEXES == 1 )
    UBaseType_t uxBasePriority; //任务基础优先级,优先级反转的时候用到
    UBaseType_t uxMutexesHeld; //任务获取到的互斥信号量个数
    #endif
    #if ( configUSE_APPLICATION_TASK_TAG == 1 )
    TaskHookFunction_t pxTaskTag;
    #endif
    #if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 ) //与本地存储有关
    void 
        *pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
    #endif
    #if( configGENERATE_RUN_TIME_STATS == 1 )
    uint32_t ulRunTimeCounter; //用来记录任务运行总时间
    #endif
    #if ( configUSE_NEWLIB_REENTRANT == 1 )
    struct _reent xNewLib_reent; //定义一个 newlib 结构体变量
    #endif
    #if( configUSE_TASK_NOTIFICATIONS == 1 )//任务通知相关变量
    volatile uint32_t ulNotifiedValue; //任务通知值
    volatile uint8_t ucNotifyState; //任务通知状态
    #endif
    #if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
    //用来标记任务是动态创建的还是静态创建的，如果是静态创建的此变量就为 pdTURE，
    //如果是动态创建的就为 pdFALSE
    uint8_t ucStaticallyAllocated; 
    #endif
    #if( INCLUDE_xTaskAbortDelay == 1 )
    uint8_t ucDelayAborted;
    #endif
} tskTCB;
//新版本的 FreeRTOS 任务控制块重命名为 TCB_t，但是本质上还是 tskTCB，主要是为了兼容
//旧版本的应用。
typedef tskTCB TCB_t;
```

## 任务堆栈

* FreeRTOS 之所以能正确的恢复一个任务的运行就是因为有任务堆栈在保驾护航，任务调度器在进行任务切换的时候会将当前任务的现场（CPU 寄存器值等）保存在此任务的任务堆栈中，等到此任务下次运行的时候就会先用堆栈中保存的值来恢复现场，恢复现场以后任务就会接着从上次中断的地方开始运行。
* 创建任务的时候需要给任务指定堆栈，如果使用的函数`xTaskCreate()`创建任务（动态方法）的话那么任务堆栈就会由函数`xTaskCreate()`自动创建。
* 如果使用函数`xTaskCreateStatic()`创建任务（静态方法）的话就需要程序员自行定义任务堆栈，然后堆栈首地址作为函数的参数`puxStackBuffer`传递给函数。

```c
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                               const char * const pcName,
                               const uint32_t ulStackDepth,
                               void * const pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t * const puxStackBuffer, //任务堆栈，需要用户定义，然后将堆栈首地址传递给这个参数。
                               StaticTask_t * const pxTaskBuffer ) 
```

### 堆栈大小
* 我们不管是使用函数`xTaskCreate()`还是`xTaskCreateStatic()`创建任务都需要指定任务堆栈大小。
* 任务堆栈的数据类型为`StackType_t`，`StackType_t`本质上是`uint32_t`，在**portmacro.h** 中有定义，如下：

```c
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE long
typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
```

* 可以看出`StackType_t`类型的变量为 4 个字节，那么任务的实际堆栈大小就应该是我们所定义的 4 倍。

# 05_FreeRTOS任务相关API 函数

## 任务创建和删除 API 函数

![image-20240425102602501](http://qny.expressisland.cn/gdou24/image-20240425102602501.png)

### 函数`xTaxkCreate()`

* 此函数用来创建一个任务，任务需要 RAM 来保存与任务有关的状态信息（任务控制块），任务也需要一定的 RAM 来作为任务堆栈。
* 如果使用函数`xTaskCreate()`来创建任务的话那么这些所需的 RAM 就会自动的从FreeRTOS 的堆中分配，因此必须提供内存管理文件，默认我们使用**heap_4.c**这个内存管理文件，而且宏 `configSUPPORT_DYNAMIC_ALLOCATION`必须为 1。
* 如果使用函数`xTaskCreateStatic()`创建的话这些 RAM 就需要用户来提供了。
* 新创建的任务默认就是就绪态的，如果当前没有比它更高优先级的任务运行那么此任务就会立即进入运行态开始运行，不管在任务调度器启动前还是启动后，都可以创建任务。

#### 函数原型

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const uint16_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask )
```

#### 参数和返回值

![image-20240425102936881](http://qny.expressisland.cn/gdou24/image-20240425102936881.png)

### 函数`xTaskCreateStatic()`

* 此函数和`xTaskCreate()`的功能相同，也是用来创建任务的，但是使用此函数创建的任务所需的RAM需要用户来提供 。
* 如果要使用此函数的话需要将宏`configSUPPORT_STATIC_ALLOCATION`定义为 1。

#### 函数原型

```c
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                               const char * const pcName,
                               const uint32_t ulStackDepth,
                               void * const pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t * const puxStackBuffer,
                               StaticTask_t * const pxTaskBuffer )
```

#### 参数和返回值

![image-20240425103230056](http://qny.expressisland.cn/gdou24/image-20240425103230056.png)

### 函数`xTaskCreateRestricted()`
* 此函数也是用来创建任务的，只不过此函数要求所使用的MCU有MPU（内存保护单元），用此函数创建的任务会受到MPU的保护。
* 其他的功能和函数`xTaxkCreate()`一样。

#### 函数原型

```c
BaseType_t xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, 
                                 TaskHandle_t * pxCreatedTask )
```

#### 参数和返回值

![image-20240425103429894](http://qny.expressisland.cn/gdou24/image-20240425103429894.png)

### 函数`vTaskDelete()`

* 删除一个用函数`xTaskCreate()`或者`xTaskCreateStatic()`创建的任务，被删除了的任务不再存在，也就是说再也不会进入运行态。
* 任务被删除以后就不能再使用此任务的句柄！如果此任务是使用动态方法创建的，也就是使用函数`xTaskCreate()`创建的，那么在此任务被删除以后此任务之前申请的堆栈和控制块内存会在空闲任务中被释放掉，因此当调用函数`vTaskDelete()`删除任务以后必须给空闲任务一定的运行时间。
* 只有那些由内核分配给任务的内存才会在任务被删除以后自动的释放掉，用户分配给任务的内存需要用户自行释放掉，比如某个任务中用户调用函数`pvPortMalloc()`分配了 500 字节的内存，那么在此任务被删除以后用户也必须调用函数vPortFree()将这 500 字节的内存释放掉，否则会导致内存泄露。

#### 函数原型

```c
vTaskDelete( TaskHandle_t xTaskToDelete )
```

#### 参数和返回值

* `xTaskToDelete`： 要删除的任务的任务句柄。
* 返回值：无

## 任务创建和删除实验（动态方法）

* 学习`xTaskCreate()`和 `vTaskDelete()`这两个函数的使用。

### 实验设计

* 本实验设计三个任务：**start_task、task1_task 和 task2_task** ，这三个任务功能如下：
* **start_task**：用来创建其他两个任务。
* **task1_task** ：当此任务运行 5 次以后就会调用函数`vTaskDelete()`删除任务task2_task，此任务也会控制 LED0 的闪烁，并且周期性的刷新 LCD 指定区域的背景颜色。
* **task2_task** ：此任务普通的应用任务，此任务也会控制 LED1 的闪烁，并且周期性的刷新LCD 指定区域的背景颜色。

### 分项详解

#### 任务设置

```c
//任务优先级
#define START_TASK_PRIO		1
```

* 任务优先级：start_task任务的任务优先级，此处用宏来表示，以后所有的任务优先级都用宏来表示。
* 创建任务设置优先级的时候就用这个宏，当然了也可以直接在创建任务的时候指定任务优先级。

#### main()函数 

* 调用函数`xTaskCreate()`创建 tart_task 任务，函数中的各个参数就是上面的任务设置中定义的，其他任务的创建也用这种方法。
* 调用函数`vTaskStartScheduler()`开启 FreeRTOS 的任务调度器，FreeRTOS 开始运行。

```c
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄                
    vTaskStartScheduler();          //开启任务调度
}
```

#### 任务函数

![image-20240425161610882](http://qny.expressisland.cn/gdou24/image-20240425161610882.png)

```c
//开始任务任务函数
void start_task(void *pvParameters) //(1)
    
// -----省略符
    
    vTaskDelete(StartTask_Handler); //删除开始任务 (2)
taskEXIT_CRITICAL(); //退出临界区
}

// -----省略符

//task1 任务函数
void task1_task(void *pvParameters) //(3)
    
// -----省略符
    
    if(task1_num==5) 
    {
        vTaskDelete(Task2Task_Handler);//任务 1 执行 5 次删除任务 2 
        //(4)
        printf("任务 1 删除了任务 2!\r\n");
    }  

// -----省略符

//task2 任务函数
void task2_task(void *pvParameters) //(5)
```

### 代码（main.c）

```c
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
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
#define TASK1_TASK_PRIO		2
//任务堆栈大小	
#define TASK1_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		3
//任务堆栈大小	
#define TASK2_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
void task2_task(void *pvParameters);

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };
						
int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
		delay_init(180);                //初始化延时函数
		uart_init(115200);              //初始化串口
    LED_Init();                     //初始化LED 
		KEY_Init();						//初始化按键
		SDRAM_Init();					//初始化SDRAM
		LCD_Init();						//初始化LCD
	
    POINT_COLOR = RED;
		LCD_ShowString(30,10,200,16,16,"Apollo STM32F4/F7");	
		LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 6-1");
		LCD_ShowString(30,50,200,16,16,"Task Creat and Del");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2024/4/24");
	
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
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	u8 task1_num=0;
	
	POINT_COLOR = BLACK;

	LCD_DrawRectangle(5,110,115,314); 	//画一个矩形	
	LCD_DrawLine(5,130,115,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
	while(1)
	{
		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
		LED0=!LED0;
		printf("任务1已经执行：%d次\r\n",task1_num);
		if(task1_num==5) 
		{
			if(Task2Task_Handler != NULL)		//任务2是否存在？	
			{
				vTaskDelete(Task2Task_Handler);	//任务1执行5次删除任务2
				Task2Task_Handler=NULL;			//任务句柄清零
				printf("任务1删除了任务2!\r\n");
			}
		}
		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
        vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	u8 task2_num=0;
	
	POINT_COLOR = BLACK;

	LCD_DrawRectangle(125,110,234,314); //画一个矩形	
	LCD_DrawLine(125,130,234,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
	while(1)
	{
		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
        LED1=!LED1;
		printf("任务2已经执行：%d次\r\n",task2_num);
		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
        vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}
```

### 测试运行

#### 实机调试

* 任务1不断运行，但任务2运行了5次就停止了。

![VID_20240425_155408 -middle-original](http://qny.expressisland.cn/gdou24/VID_20240425_155408%20-middle-original.gif)

#### 串口调试

* 一开始任务 1 和任务 2 是同时运行的，由于任务 2 的优先级比任务 1 的优先级高，所以任务 2 先输出信息。
* 当任务 1 运行了 5 次以后任务 1 就删除了任务 2，最后只剩下了任务 1 在运行了。

![image-20240425155750171](http://qny.expressisland.cn/gdou24/image-20240425155750171.png)

## 任务创建和删除实验（静态方法）

* 本节在上一小节的基础上做简单的修改，使用函数`xTaskCreateStatic()`来创建任务，也就是静态方法。
* 任务的堆栈、任务控制块就需要由用户来指定了。

### 系统设置
* 使用静态方法创建任务的时候需要文件FreeRTOSConfig.h 中将宏`configSUPPORT_STATIC_ALLOCATION` 设置为 1。

```c
#define configSUPPORT_STATIC_ALLOCATION 1 //静态内存
```

![image-20240425162815831](http://qny.expressisland.cn/gdou24/image-20240425162815831.png)

### 两个函数未定义？
* 宏`configSUPPORT_STATIC_ALLOCATION`定义为 1 以后编译一次，会提示我们有两函
  数未定义。
* 如果使用静态方法的话需要用户实现两个函 数`vApplicationGetIdleTaskMemory()` 和`vApplicationGetTimerTaskMemory()`。
* 通过这两个函数来给空闲任务和定时器服务任务的任务堆栈和任务控制块分配内存，这两个函数我们在main.c中定义：

```c
#include "FreeRTOS.h"
#include "task.h"
// -----以上无关，非完整代码

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

// -----以下无关，非完整代码
int main(void)
{
```

* 这两个函数很简单，用户定义静态的任务堆栈和任务控制块内存，然后将这些内存传递给函数参数。
* 最后创建空闲任务和定时器服务任务的 API 函数会调用`vApplicationGetIdleTaskMemory()`和`vApplicationGetTimerTaskMemory()`来获取这些内存。

### 任务设置

* 静态创建任务需要用户提供任务堆栈，这里定义一个数组作为任务堆栈，堆栈数组为`StackType_t`类型。
* 定义任务控制块，注意任务控制块类型要用`StaticTask_t`，而不是`TCB_t`或`tskTCB`！

```c
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
```

### main()函数 

![image-20240425164325920](http://qny.expressisland.cn/gdou24/image-20240425164325920.png)

```c
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

```

### 任务函数

* 使用静态任务创建函数`xTaskCreateStatic()`来创建任务`task1_task`。
* 使用静态任务创建函数`xTaskCreateStatic()`来创建任务`task2_task`。

```c
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
```

### 完整代码（main.c）

```c
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
```

### 测试运行

* 和前一节效果是一样的，略。

![image-20240425165152969](http://qny.expressisland.cn/gdou24/image-20240425165152969.png)

## 任务挂起和恢复API函数

P93

## 任务挂起和恢复实验

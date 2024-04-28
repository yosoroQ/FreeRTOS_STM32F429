# FreeRTOS 开发 —— 基于STM32F429

# 06_FreeRTOS列表和列表项

* 要想看懂 FreeRTOS 源码并学习其原理，有一个东西绝对跑不了，那就是 FreeRTOS 的列表和列表项。
* 列表和列表项是 FreeRTOS 的一个数据结构，FreeRTOS 大量使用到了列表和列表项， 它是 FreeRTOS 的基石。

## 什么是列表和列表项

### 列表
* 列表是 FreeRTOS 中的一个数据结构，概念上和链表有点类似，列表被用来跟踪 FreeRTOS 中的任务。
* 与列表相关的全部东西都在文件 list.c 和 list.h 中，在 list.h 中定义了一个叫 `List_t` 的 结构体：

![image-20240426170057818](http://qny.expressisland.cn/gdou24/image-20240426170057818.png)

### 列表项

* 列表项就是存放在列表中的项目，FreeRTOS 提供了两种列表项：**列表项**和**迷你列表项**。这两个都在文件list.h中有定义：

![image-20240427200716628](http://qny.expressisland.cn/gdou24/image-20240427200716628.png)

#### 列表项结构示意图

* 图中并未列出用于列表项完整性检查的成员变量！

![image-20240427200759036](http://qny.expressisland.cn/gdou24/image-20240427200759036.png)

### 迷你列表项

* 迷你列表项在文件 list.h 中有定义：

![image-20240427200844698](http://qny.expressisland.cn/gdou24/image-20240427200844698.png)



#### 迷你列表项结构示意图

* 注意！图中并未列出用于迷你列表项完整性检查的成员变量！

![image-20240427200914654](http://qny.expressisland.cn/gdou24/image-20240427200914654.png)

## 列表和列表项的初始化

### 列表初始化

![image-20240427201018212](http://qny.expressisland.cn/gdou24/image-20240427201018212.png)

#### 列表初始化图

![image-20240427201055917](http://qny.expressisland.cn/gdou24/image-20240427201055917.png)

### 列表项初始化

![image-20240427201121763](http://qny.expressisland.cn/gdou24/image-20240427201121763.png)

## 列表项的插入

### 列表项插入函数分析

![image-20240427201421048](http://qny.expressisland.cn/gdou24/image-20240427201421048.png)

![image-20240427201442889](http://qny.expressisland.cn/gdou24/image-20240427201442889.png)

### 列表项插入过程图示

* 上一节我们分析了列表项插入函数`vListInsert()`，本小节我们就通过图片来演示一下这个插入过程，本小节我们会向一个空的列表中插入三个列表项，这三个列表项的值分别为40，60和 50。

#### 插入值为 40 的列表项

![image-20240427201552279](http://qny.expressisland.cn/gdou24/image-20240427201552279.png)

#### 插入值为 60 的列表项

![image-20240427201609732](http://qny.expressisland.cn/gdou24/image-20240427201609732.png)

#### 插入值为 50 的列表项

![image-20240427201628553](http://qny.expressisland.cn/gdou24/image-20240427201628553.png)

## 列表项末尾插入

### 列表项末尾插入函数分析

![image-20240427201714987](http://qny.expressisland.cn/gdou24/image-20240427201714987.png)

### 列表项末尾插入图示

* 跟函数`vListInsert()`一样，我们也用图片来看一下函数`vListInsertEnd()`的插入过程。

#### 默认列表

* 在插入列表项之前我们先准备一个默认列表：

* 注意图中列表的`pxIndex`所指向的列表项，这里为`ListItem1`，不再是`xListEnd`了。

![image-20240427202009033](http://qny.expressisland.cn/gdou24/image-20240427202009033.png)

#### 插入值为 50 的列表项
* 在上面的列表中插入一个值为 50 的列表项`ListItem3`，插入完成以后如图所示：

* 列表`List`的`pxIndex`指向列表项 `ListItem1`，因此调用函数`vListInsertEnd()`插入`ListItem3`的话就会在`ListItem1`的前面插入。

![image-20240427202018201](http://qny.expressisland.cn/gdou24/image-20240427202018201.png)

## 列表项的删除

* 有列表项的插入，那么必然有列表项的删除，列表项的删除通过函数`uxListRemove()`来完成，函数原型如下：

```c
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
```

![image-20240427202135709](http://qny.expressisland.cn/gdou24/image-20240427202135709.png)

## 列表项的遍历

![image-20240427202229248](http://qny.expressisland.cn/gdou24/image-20240427202229248.png)

## 列表项的插入和删除实验

### 实验设计

![image-20240427202314679](http://qny.expressisland.cn/gdou24/image-20240427202314679.png)

### 程序代码（main.c）

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
#define LIST_TASK_PRIO		3
//任务堆栈大小	
#define LIST_STK_SIZE 		128  
//任务句柄
TaskHandle_t ListTask_Handler;
//任务函数
void list_task(void *pvParameters);

//定义一个测试用的列表和3个列表项
List_t TestList;		//测试用列表
ListItem_t ListItem1;	//测试用列表项1
ListItem_t ListItem2;	//测试用列表项2
ListItem_t ListItem3;	//测试用列表项3

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
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 7-1");
	LCD_ShowString(30,50,200,16,16,"list and listItem");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2024/4/27");
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
    //创建LIST任务
    xTaskCreate((TaskFunction_t )list_task,     
                (const char*    )"list_task",   
                (uint16_t       )LIST_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&ListTask_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	while(1)
	{
		LED0=!LED0;
        vTaskDelay(500);                           //延时500ms，也就是500个时钟节拍	
	}
}

//list任务函数
void list_task(void *pvParameters)
{
	//第一步：初始化列表和列表项
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue=40;			//ListItem1列表项值为40
	ListItem2.xItemValue=60;			//ListItem2列表项值为60
	ListItem3.xItemValue=50;			//ListItem3列表项值为50
	
	//第二步：打印列表和其他列表项的地址
	printf("/*******************列表和列表项地址*******************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList                          %#x					\r\n",(int)&TestList);
	printf("TestList->pxIndex                 %#x					\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd                %#x					\r\n",(int)(&TestList.xListEnd));
	printf("ListItem1                         %#x					\r\n",(int)&ListItem1);
	printf("ListItem2                         %#x					\r\n",(int)&ListItem2);
	printf("ListItem3                         %#x					\r\n",(int)&ListItem3);
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第三步：向列表TestList添加列表项ListItem1，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem1);		//插入列表项ListItem1
	printf("/******************添加列表项ListItem1*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第四步：向列表TestList添加列表项ListItem2，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem2);	//插入列表项ListItem2
	printf("/******************添加列表项ListItem2*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第五步：向列表TestList添加列表项ListItem3，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem3);	//插入列表项ListItem3
	printf("/******************添加列表项ListItem3*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第六步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	uxListRemove(&ListItem2);						//删除ListItem2
	printf("/******************删除列表项ListItem2*****************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下
	
	//第七步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	TestList.pxIndex=TestList.pxIndex->pxNext;			//pxIndex向后移一项，这样pxIndex就会指向ListItem1。
	vListInsertEnd(&TestList,&ListItem2);				//列表末尾添加列表项ListItem2
	printf("/***************在末尾添加列表项ListItem2***************/\r\n");
	printf("项目                              地址				    \r\n");
	printf("TestList->pxIndex                 %#x					\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd->pxNext        %#x					\r\n",(int)(TestList.xListEnd.pxNext));
	printf("ListItem2->pxNext                 %#x					\r\n",(int)(ListItem2.pxNext));
	printf("ListItem1->pxNext                 %#x					\r\n",(int)(ListItem1.pxNext));
	printf("ListItem3->pxNext                 %#x					\r\n",(int)(ListItem3.pxNext));
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	printf("ListItem2->pxPrevious             %#x					\r\n",(int)(ListItem2.pxPrevious));
	printf("ListItem1->pxPrevious             %#x					\r\n",(int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious             %#x					\r\n",(int)(ListItem3.pxPrevious));
	printf("/************************结束**************************/\r\n\r\n\r\n");
	while(1)
	{
		LED1=!LED1;
        vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}
```
### 调试结果

```c
'-?LCD ID:9341
    /*******************列表和列表项地址*******************/
    项目                              地址				    
    TestList                          0x200000bc					
    TestList->pxIndex                 0x200000c4					
    TestList->xListEnd                0x200000c4					
    ListItem1                         0x200000d0					
    ListItem2                         0x200000e4					
    ListItem3                         0x200000f8					
    /************************结束**************************/
    按下KEY_UP键继续!


    /******************添加列表项ListItem1*****************/
    项目                              地址				    
    TestList->xListEnd->pxNext        0x200000d0					
    ListItem1->pxNext                 0x200000c4					
    /*******************前后向连接分割线********************/
    TestList->xListEnd->pxPrevious    0x200000d0					
    ListItem1->pxPrevious             0x200000c4					
    /************************结束**************************/
    按下KEY_UP键继续!


    /******************添加列表项ListItem2*****************/
    项目                              地址				    
    TestList->xListEnd->pxNext        0x200000d0					
    ListItem1->pxNext                 0x200000e4					
    ListItem2->pxNext                 0x200000c4					
    /*******************前后向连接分割线********************/
    TestList->xListEnd->pxPrevious    0x200000e4					
    ListItem1->pxPrevious             0x200000c4					
    ListItem2->pxPrevious             0x200000d0					
    /************************结束**************************/
    按下KEY_UP键继续!


    /******************添加列表项ListItem3*****************/
    项目                              地址				    
    TestList->xListEnd->pxNext        0x200000d0					
    ListItem1->pxNext                 0x200000f8					
    ListItem3->pxNext                 0x200000e4					
    ListItem2->pxNext                 0x200000c4					
    /*******************前后向连接分割线********************/
    TestList->xListEnd->pxPrevious    0x200000e4					
    ListItem1->pxPrevious             0x200000c4					
    ListItem3->pxPrevious             0x200000d0					
    ListItem2->pxPrevious             0x200000f8					
    /************************结束**************************/
    按下KEY_UP键继续!


    /******************删除列表项ListItem2*****************/
    项目                              地址				    
    TestList->xListEnd->pxNext        0x200000d0					
    ListItem1->pxNext                 0x200000f8					
    ListItem3->pxNext                 0x200000c4					
    /*******************前后向连接分割线********************/
    TestList->xListEnd->pxPrevious    0x200000f8					
    ListItem1->pxPrevious             0x200000c4					
    ListItem3->pxPrevious             0x200000d0					
    /************************结束**************************/
    按下KEY_UP键继??


    /***************在末尾添加列表项ListItem2***************/
    项目                              地址				    
    TestList->pxIndex                 0x200000d0					
    TestList->xListEnd->pxNext        0x200000e4					
    ListItem2->pxNext                 0x200000d0					
    ListItem1->pxNext                 0x200000f8					
    ListItem3->pxNext                 0x200000c4					
    /*******************前后向连接分割线********************/
    TestList->xListEnd->pxPrevious    0x200000f8					
    ListItem2->pxPrevious             0x200000c4					
    ListItem1->pxPrevious             0x200000e4					
    ListItem3->pxPrevious             0x200000d0					
    /************************结束**************************/
```

#### 第一步和第二步

* 第一步和第二步是用来初始化列表和列表项的，并且通过串口输出列表和列表项的地址，这一步是开发板复位后默认运行的，串口调试助手信息如下所示：

![image-20240427203414446](http://qny.expressisland.cn/gdou24/image-20240427203414446.png)

#### 第三步

![image-20240427203449757](http://qny.expressisland.cn/gdou24/image-20240427203449757.png)

#### 第四步

![image-20240427203521637](http://qny.expressisland.cn/gdou24/image-20240427203521637.png)

#### 第五步

![image-20240427203558772](http://qny.expressisland.cn/gdou24/image-20240427203558772.png)

#### 第六步和第七步

* 这两步是观察函数`uxListRemove()`和`vListInsertEnd()`的运行过程的，分析过程和前五步一样。

# 07_FreeRTOS调度器开启和任务相关函数详解

* 略，看看文档得了。
* 涉及到ARM的汇编指令，有关涉及到的 ARM 指令的详细使用情况请参考《权威指南》的“第 5 章 指令集”。

# 08_FreeRTOS任务切换

* RTOS 系统的核心是任务管理，而任务管理的核心是任务切换，任务切换决定了任务的执行顺序，任务切换效率的高低也决定了一款系统的性能，尤其是对于实时操作系统。
## PendSV 异常

* PendSV（可挂起的系统调用）异常对 OS 操作非常重要，其优先级可以通过编程设置。
* 可以通过将中断控制和壮态寄存器 ICSR 的 bit28，也就是 PendSV 的挂起位置 1 来触发 PendSV中断。
* 与 SVC 异常不同，它是不精确的，因此它的挂起壮态可在更高优先级异常处理内设置，且 会在高优先级处理完成后执行。
* 利用该特性，若将 PendSV 设置为最低的异常优先级，可以让 PendSV 异常处理在所有其他中断处理完成后执行，这对于上下文切换非常有用，也是各种 OS 设计中的关键。

### 上下文切换简单实例

* 在具有嵌入式 OS 的典型系统中，处理时间被划分为了多个时间片。
* 若系统中只有两个任务，这两个任务会交替执行：

![image-20240428100722230](http://qny.expressisland.cn/gdou24/image-20240428100722230.png)

![image-20240428100752447](http://qny.expressisland.cn/gdou24/image-20240428100752447.png)

### ISR执行期间的上下文切换会延迟中断服务

* 若中断请求(IRQ)在SysTick异常前产生，则SysTick异常可能会抢占 IRQ 的处理，在这种情况下，OS不应该执行上下文切换，否则中断请求 IRQ 处理就会被延迟，而且在真实系统中延迟时间还往往不可预知，任何有一丁点实时要求的系统都决不能容忍这种事。
* 对于 CortexM3 和 Cortex-M4 处理器，当存在活跃的异常服务时，设计默认不允许返回到线程模式，若存在活跃中断服务，且 OS 试图返回到线程模式，则将触发用法**fault**：

![image-20240428100924511](http://qny.expressisland.cn/gdou24/image-20240428100924511.png)

### PendSV上下文切换

* 在一些 OS 设计中，要解决这个问题，可以在运行中断服务时不执行上下文切换，此时可以检查栈帧中的压栈 xPSR 或 NVIC 中的中断活跃壮态寄存器。
* 不过，系统的性能可能会受到影响，特别时当中断源在 SysTick 中断前后持续产生请求时，这样上下文切换可能就没有执行的机会了。
* 为了解决这个问题，PendSV 异常将上下文切换请求延迟到所有其他 IRQ 处理都已经完成后，此时需要将 PendSV 设置为最低优先级。
* 若 OS 需要执行上下文切换，他会设置 PendSV 的挂起壮态，并在 PendSV 异常内执行上下文切换：

![image-20240428101041481](http://qny.expressisland.cn/gdou24/image-20240428101041481.png)

#### 图中事件的流水账记录如下：

![image-20240428101052446](http://qny.expressisland.cn/gdou24/image-20240428101052446.png)

## FreeRTOS 任务切换场合

* 可以执行一个系统调用。
* 系统滴答定时器(SysTick)中断。

### 执行系统调用

* 执行系统调用就是执行 FreeRTOS 系统提供的相关 API 函数，比如任务切换函数`taskYIELD()`，FreeRTOS 有些 API 函数也会调用函数`taskYIELD()`，这些 API 函数都会导致任务切换，这些 API 函数和任务切换函数`taskYIELD()`都统称为系统调用。
* 函数`taskYIELD()`其实就是个宏，在文件 **task.h**中有如下定义：

```c
#define taskYIELD() portYIELD()
```

* 函数`portYIELD()`也是个宏，在文件 **portmacro.h**中有如下定义：

![image-20240428101326983](http://qny.expressisland.cn/gdou24/image-20240428101326983.png)

* 中断级的任务切换函数为`portYIELD_FROM_ISR()`，定义如下：

```c
#define portEND_SWITCHING_ISR( xSwitchRequired ) 
if( xSwitchRequired != pdFALSE ) portYIELD()
#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
```

* 可以看出`portYIELD_FROM_ISR()`最终也是通过调用函数`portYIELD()`来完成任务切换的。

### 系统滴答定时器(SysTick)中断
* FreeRTOS 中滴答定时器(SysTick)中断服务函数中也会进行任务切换，滴答定时器中断服务函数如下：

```c
void SysTick_Handler(void)
{ 
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        xPortSysTickHandler();
    }
    HAL_IncTick();
}
```

* 在滴答定时器中断服务函数中调用了 FreeRTOS 的 API 函数`xPortSysTickHandler()`。

#### API函数`xPortSysTickHandler()`函数源码

![image-20240428101526011](http://qny.expressisland.cn/gdou24/image-20240428101526011.png)

## PendSV 中断服务函数

* PendSV 中断服务函数本应该为`PendSV_Handler()`，但是 FreeRTOS 使用`#define`重定义了，如下：

```c
#define xPortPendSVHandler PendSV_Handler
```

### 函数`xPortPendSVHandler()`源码

![image-20240428101723130](http://qny.expressisland.cn/gdou24/image-20240428101723130.png)

![image-20240428101750110](http://qny.expressisland.cn/gdou24/image-20240428101750110.png)

## 查找下一个要运行的任务

* 在 PendSV 中断服务程序中有调用函数`vTaskSwitchContext()`来获取下一个要运行的任务，也就是查找已经就绪了的优先级最高的任务。

### 缩减后（去掉条件编译）的函数源码

![image-20240428101924044](http://qny.expressisland.cn/gdou24/image-20240428101924044.png)

### 通用方法
* 所有的处理器都可以用的方法。

![image-20240428102031330](http://qny.expressisland.cn/gdou24/image-20240428102031330.png)

### 硬件方法

* 硬件方法就是使用处理器自带的硬件指令来实现的，比如 Cortex-M 处理器就带有的计算前导 0 个数指令：`CLZ`。
* 硬件方法借助一个指令就可以快速的获取处于就绪态的最高优先级，但是会限制任务的优先级数。
* 比如 STM32 只能有 32 个优先级，不过 32 个优先级已经完全够用了。
* 要知道FreeRTOS 是支持时间片的，每个优先级可以支持无限多个任务。

![image-20240428102311147](http://qny.expressisland.cn/gdou24/image-20240428102311147.png)

## FreeRTOS 时间片调度
* 在 FreeRTOS 中允许一个任务运行一个时间片（一个时钟节拍的长度）后让出 CPU 的使用权，让拥有同优先级的下一个任务运行，至于下一个要运行哪个任务？
* FreeRTOS 中的这种调度方法就是时间片调度。

### 时间片调度

* 运行在同一优先级下的执行时间图，在优先级 N 下有 3 个就绪的任务。

![image-20240428102451141](http://qny.expressisland.cn/gdou24/image-20240428102451141.png)

### 任务调度是有条件的，函数`xPortSysTickHandler()`

* 要使用时间片调度的话宏`configUSE_PREEMPTION`和宏`configUSE_TIME_SLICING` 必须为 1。
* 时间片的长度由宏`configTICK_RATE_HZ`来确定，一个时间片的长度就是滴答定时器的中断周期。
* 比如本教程中`configTICK_RATE_HZ`为 1000，那么一个时间片的长度就是 1ms。
* 时间片调度发生在滴答定时器的中断服务函数中，在中断服务函数`SysTick_Handler()`中会调用 FreeRTOS 的 API 函数`xPortSysTickHandler()`，而函数`xPortSysTickHandler()`会引发任务调度 ， 但是这个任务调度是有条件的，函数`xPortSysTickHandler()`如下：

![image-20240428102728668](http://qny.expressisland.cn/gdou24/image-20240428102728668.png)

* 上述代码中红色部分表明只有函数 `xTaskIncrementTick()`的返回值不为 **pdFALSE**的时候就会进行任务调度！

#### 查看函数`xTaskIncrementTick()`会发现有如下条件编译语句：

![image-20240428102840090](http://qny.expressisland.cn/gdou24/image-20240428102840090.png)

## 时间片调度实验

### 实验设计

![image-20240428102903473](http://qny.expressisland.cn/gdou24/image-20240428102903473.png)

### 完整代码（main.c）

``` c
#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用OS,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用	 
#include "task.h"
#endif

static u32 fac_us=0;							//us延时倍乘数

#if SYSTEM_SUPPORT_OS		
    static u16 fac_ms=0;				        //ms延时倍乘数,在os下,代表每个节拍的ms数
#endif

 
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

![image-20240428104559605](http://qny.expressisland.cn/gdou24/image-20240428104559605.png)

### 运行测试

![image-20240428105021092](http://qny.expressisland.cn/gdou24/image-20240428105021092.png)

* 不管是task1_task还是task2_task都是连续执行 4、5 次，和前面程序设计的一样，说明在一个时间片内一直在运行一个任务，当时间片用完后就切换到下一个任务运行。


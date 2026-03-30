
#include "FreeRtos_demo.h"
#include "FreeRTOS.h"
#include "USBVirtualSerial.h"
#include "diskio.h"
#include "ff.h"
#include "key.h"
#include "task.h"





#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief  重定向标准输出到串口1（适用于GCC）
 */
int _write(int file, char *ptr, int len)
{
    if (file == STDOUT_FILENO || file == STDERR_FILENO)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
        return len;
    }

    errno = EBADF;
    return -1;
}



/**
 * @brief 栈溢出钩子函数（configCHECK_FOR_STACK_OVERFLOW>0 时必须实现）
 * @param pxTask 发生栈溢出的任务句柄
 * @param pcTaskName 发生栈溢出的任务名
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    /* 栈溢出处理逻辑，比如打印任务名、触发硬件看门狗、记录日志等 */
    ( void ) pxTask;
    ( void ) pcTaskName;
    /* 死循环（可选，便于调试定位问题） */
    for( ;; )
	{
		usbprintf("StackOverflow!");
		vTaskDelay(1000); // 延时1秒，避免打印过快导致串口堵塞
	}
}








/* 进入低功耗前所需要执行的操作 */
void PRE_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
}
/* 退出低功耗后所需要执行的操作 */
void POST_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}





/*启动任务的配置*/
#define START_TASK_STACK 130
#define START_TASK_PRIORITY 1
TaskHandle_t start_TaskHandle;
void start_Task(void * pvParameters);


/*任务1的配置*/
#define TASK1_STACK 4096
#define TASK1_PRIORITY 2
TaskHandle_t task1_TaskHandle;
void task1(void * pvParameters);

/*任务2的配置*/
#define TASK2_STACK 130
#define TASK2_PRIORITY 3
TaskHandle_t task2_TaskHandle;
void task2(void * pvParameters);

/*任务3的配置*/
#define TASK3_STACK 130
#define TASK3_PRIORITY 4
TaskHandle_t task3_TaskHandle;
void task3(void * pvParameters);



void timer1_callback(TimerHandle_t xTimer);
TimerHandle_t timer1Handle;



void timer2_callback(TimerHandle_t xTimer);
TimerHandle_t timer2Handle;




void freertos_start(void)
{

	/*创建软件定时器*/
	timer1Handle = xTimerCreate((char *) "Timer1", //定时器名称
		(TickType_t) 500, //定时器周期（单位：tick）
		(BaseType_t) pdFALSE, //是否自动重载
		(void *) 1, //定时器ID
		(TimerCallbackFunction_t )timer1_callback //定时器回调函数
	);

	if(timer1Handle != NULL)
	{
		usbprintf("单次timer1创建成功!\r\n");
	}

	/*创建软件定时器*/
	timer2Handle = xTimerCreate((char *) "Timer2", //定时器名称
		(TickType_t) 1000, //定时器周期（单位：tick）
		(BaseType_t) pdTRUE, //是否自动重载
		(void *) 2, //定时器ID
		(TimerCallbackFunction_t )timer2_callback //定时器回调函数
	);

	if(timer2Handle != NULL)
	{
		usbprintf("重复timer2创建成功!\r\n");
	}


	
	/*创建启动任务*/
	xTaskCreate((TaskFunction_t) start_Task,//任务函数的地址
		 (char *) "start_Task",//任务名字
		 (configSTACK_DEPTH_TYPE) START_TASK_STACK,//任务堆栈大小
		 (void *) NULL,//传递给任务函数的参数
		 (UBaseType_t) START_TASK_PRIORITY,//任务优先级
		 (TaskHandle_t *) &start_TaskHandle//任务句柄的地址
		);

	
	/*启动调度器*/
	vTaskStartScheduler();

	

}


/**
 * @brief  启动任务函数，创建其他任务并删除自身
 * @param  pvParameters: 传递给任务函数的参数（此处未使用）
 */

FATFS SD_fs;
FIL file1;
FILINFO file_info;
DIR dir;





void start_Task(void * pvParameters)
{



	taskENTER_CRITICAL(); // 进入临界区，防止任务切换干扰任务创建过程
	/*创建启动任务*/
	xTaskCreate((TaskFunction_t) task1,//任务函数的地址
		 (char *) "task1",//任务名字
		 (configSTACK_DEPTH_TYPE) TASK1_STACK,//任务堆栈大小
		 (void *) NULL,//传递给任务函数的参数
		 (UBaseType_t) TASK1_PRIORITY,//任务优先级
		 (TaskHandle_t *) &task1_TaskHandle//任务句柄的地址

		);

	xTaskCreate((TaskFunction_t) task2,//任务函数的地址
		 (char *) "task2",//任务名字
		 (configSTACK_DEPTH_TYPE) TASK2_STACK,//任务堆栈大小
		 (void *) NULL,//传递给任务函数的参数
		 (UBaseType_t) TASK2_PRIORITY,//任务优先级
		 (TaskHandle_t *) &task2_TaskHandle//任务句柄的地址

		);
	xTaskCreate((TaskFunction_t) task3,//任务函数的地址
		 (char *) "task3",//任务名字
		 (configSTACK_DEPTH_TYPE) TASK3_STACK,//任务堆栈大小
		 (void *) NULL,//传递给任务函数的参数
		 (UBaseType_t) TASK3_PRIORITY,//任务优先级
		 (TaskHandle_t *) &task3_TaskHandle//任务句柄的地址

		);
	
	/*启动任务执行一次，删除启动任务*/
	vTaskDelete(start_TaskHandle);

	taskEXIT_CRITICAL(); // 退出临界区，允许任务切换


}


/**

FRESULT f_open (
	FIL* fp,			
	const TCHAR* path,	
	BYTE mode			
)



 @param  fp：指向打开的文件对象结构的指针。
 @param  path：待打开的文件的名称路径。
 @param  mode：模式标志，指定对文件的访问类型和打开方法。它由以下标志的组合指定。

 @brief FA_READ：指定对文件的读取访问权限。可以从文件中读取数据。
 @brief FA_WRITE：指定对文件的写入访问权限。可以向文件写入数据。与 FA_READ 结合可实现读写访问。

 @brief FA_OPEN_EXISTING：打开已经存在的文件。如果文件不存在，函数将失败。（默认）
 @brief FA_OPEN_ALWAYS：如果文件存在，则打开文件。如果不存在，将创建一个新文件。
 @brief FA_CREATE_NEW：创建新文件。如果文件已存在，函数将以 FR_EXIST 错误失败。
 @brief FA_CREATE_ALWAYS：创建新文件。如果文件已存在，将截断并覆盖它。
 @brief FA_OPEN_APPEND：与 FA_OPEN_ALWAYS 相同，只是读 / 写指针设置到文件的末尾。

 @retval FRESULT
*/






void task1(void *pvParameters) {
    FRESULT res;
    FIL file;          // 文件对象（必须定义）
    UINT bw;           // 实际写入的字节数

    while (1) {
        usbprintf("任务1：执行...\r\n");
        if (Key_Check(0, KEY_SINGLE)) {
            usbprintf("按键按下，开始操作\r\n");

            // 1. 挂载SD卡
            res = f_mount(&SD_fs, "1:", 1);
            if(res == FR_OK) {
                usbprintf("SD挂载成功\r\n");
            } else {
                usbprintf("SD挂载失败,错误码: %d\r\n", res);
                // 挂载失败直接跳过后续操作
                continue;
            }

            // 2. 切换驱动器
            res = f_chdrive("1:");
            if(res == FR_OK) {
                usbprintf("驱动器更改成功\r\n");
            } else {
                usbprintf("驱动器更改失败，错误码 %d\r\n", res);
                continue;
            }

            // ====================== 新增：打开/创建文件并写入 ======================
            // 打开 test.txt 文件，不存在则创建，存在则覆盖写入
            res = f_open(&file, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
            if(res == FR_OK) {
                usbprintf("文件打开/创建成功\r\n");

                // 写入字符串 "hello,world"
                const char *write_data = "hello,world\r\n";
                res = f_write(&file, write_data, strlen(write_data), &bw);

                if(res == FR_OK) {
                    usbprintf("文件写入成功，实际写入 %u 字节\r\n", bw);
                } else {
                    usbprintf("文件写入失败，错误码: %d\r\n", res);
                }

                // 必须关闭文件！否则数据不会真正写入SD卡
                f_close(&file);
                usbprintf("文件已关闭\r\n");
            } else {
                usbprintf("文件打开失败，错误码: %d\r\n", res);
            }
            
        }

        // 打印栈和堆信息
        UBaseType_t stack = uxTaskGetStackHighWaterMark(NULL);
        usbprintf("任务1 最小剩余栈: %u 字, 堆空闲: %u 字节, 历史最小: %u 字节\r\n",
                  stack, xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
        vTaskDelay(500);
    }
}



void task2(void * pvParameters)
{

    while (1) {
        usbprintf("任务2:执行....\r\n");
        
        vTaskDelay(500);
    }
}







void task3(void *pvParameters)
{
	

   
    while (1) {
		usbprintf("任务3:执行...\r\n");


		
		
		for(uint8_t i=0;i<KEY_COUNT;i++)
		{
			

			if(Key_Check(i,KEY_DOUBLE))
			{
				if(i==0)
				{
					

					
					
					
				
				}

				
			}

			if(Key_Check(i,KEY_LONG))
			{
				if(i == 0)
				{
					

					
					

				}
			}
		}

		
        vTaskDelay(500); 
    }
	

}




void timer1_callback(TimerHandle_t xTimer)
{
	static uint16_t count = 0;
	count++;
	usbprintf("单次timer1回调执行！count=%d\r\n",count);
}

void timer2_callback(TimerHandle_t xTimer)
{
	static uint16_t count = 0;
	count++;
	usbprintf("重复timer2回调执行！count=%d\r\n",count);
}


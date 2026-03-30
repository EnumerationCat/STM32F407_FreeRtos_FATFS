
#ifndef _USBVirtualSerial_H
#define _USBVirtualSerial_H


#ifdef __cplusplus
extern "C" {
#endif
    /*****************C*****************/
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>       // 可变参数处理头文件
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdio.h>

void usbRecData(void);
int usbprintf(const char *format, ...);


#ifdef __cplusplus
}
/**********************C++*************************/

#endif

#endif 

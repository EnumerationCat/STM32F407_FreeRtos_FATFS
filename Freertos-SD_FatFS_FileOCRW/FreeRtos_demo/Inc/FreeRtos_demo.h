
#ifndef _FREERTOS_DEMO_H
#define _FREERTOS_DEMO_H


#ifdef __cplusplus
extern "C" {
#endif
    /*****************C*****************/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "usbd_cdc_if.h"
#include "key.h"
#include "USBVirtualSerial.h"
#include "SDCard.h"
#include "ff.h"
#include "diskio.h"





void PRE_SLEEP_PROCESSING(void);
void POST_SLEEP_PROCESSING(void);

void freertos_start(void);


#ifdef __cplusplus
}
/**********************C++*************************/

#endif

#endif 



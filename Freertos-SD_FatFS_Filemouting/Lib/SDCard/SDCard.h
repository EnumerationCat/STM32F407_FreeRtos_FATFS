
#ifndef _SDCard_H
#define _SDCard_H


#ifdef __cplusplus
extern "C" {
#endif
    /*****************C*****************/
#include "stm32f4xx_hal.h"
#include "USBVirtualSerial.h"
#include "sdio.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>


void SDCard_ShowInfo(void);
void SDCard_EraseBlocks(void);


#ifdef __cplusplus
}
/**********************C++*************************/

#endif

#endif 

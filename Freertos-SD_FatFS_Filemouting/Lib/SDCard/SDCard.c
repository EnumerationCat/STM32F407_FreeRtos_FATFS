#include "SDCard.h"







void SDCard_ShowInfo(void)
{
	HAL_SD_CardInfoTypeDef cardInfo;

	HAL_StatusTypeDef res = HAL_SD_GetCardInfo(&hsd, &cardInfo);

	if(res != HAL_OK)
	{
		usbprintf("Failed to get SD card info.\r\n");
		return;
	}
	char temp_str[128];
	usbprintf("SD Card Info:\r\n");
	sprintf(temp_str, "Card Type: %" PRIu32 "\r\n", cardInfo.CardType);
	usbprintf(temp_str);
	sprintf(temp_str, "Card Version: %" PRIu32 "\r\n", cardInfo.CardVersion);
	usbprintf(temp_str);
	sprintf(temp_str, "Class: %" PRIu32 "\r\n", cardInfo.Class);
	usbprintf(temp_str);
	sprintf(temp_str, "RelCardAdd: %" PRIu32 "\r\n", cardInfo.RelCardAdd);
	usbprintf(temp_str);
	sprintf(temp_str, "BlockNbr: %" PRIu32 "\r\n", cardInfo.BlockNbr);
	usbprintf(temp_str);
	sprintf(temp_str, "BlockSize: %" PRIu32 "\r\n", cardInfo.BlockSize);
	usbprintf(temp_str);

	
}



void SDCard_EraseBlocks(void)
{
	uint32_t BlockStartAdd = 0;
	uint32_t BlockEndAdd = 10; // 擦除前10个块（每块512字节）

	HAL_StatusTypeDef res = HAL_SD_Erase(&hsd, BlockStartAdd, BlockEndAdd);
	if(res != HAL_OK)
	{
		usbprintf("Failed to erase SD card blocks.\r\n");
		return;
	}

	HAL_SD_CardStateTypeDef cardState;
	cardState = HAL_SD_GetCardState(&hsd);
	char temp_str[128];
	sprintf(temp_str, "Card State: %" PRIu32 "\r\n", cardState);

	while(cardState != HAL_SD_CARD_TRANSFER)
	{
		HAL_Delay(1);
		cardState = HAL_SD_GetCardState(&hsd);
	}
	usbprintf("Blocks erased successfully.\r\n");

}






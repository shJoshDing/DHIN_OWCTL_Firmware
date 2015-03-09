// SDP_i2c.c

#include "SDP_std_include.h"
#include "SDP_twi_low_level.h"
#include "SDP_i2c_pseudo_low_level.h"
//#include "SDP_gpio_i2c.h"
#include "SDP_i2c.h"
#include <stdio.h>

extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

//-----------------------------------------------------------------------------
void processI2cCmd(SDP_USB_HEADER *pUsbHeader)
{
	SDP_TWI_STRUCT twiParam;
	TWI_ERROR nack;
	u32 i;
	
	switch (pUsbHeader->cmd)
	{
/*		case ADI_SDP_CMD_GPIO_I2C:
			//todo Mage
			break;
*/		case ADI_SDP_CMD_I2C_TRANSFER:
		
			//int i
	/*		printf("reg00 = 0x%2X \r\n", gGeneralDataBuf->u8[0]);
			printf("reg01 = 0x%2X \r\n", gGeneralDataBuf->u8[1]);
			printf("reg02 = 0x%2X \r\n", gGeneralDataBuf->u8[2]);
			printf("reg03 = 0x%2X \r\n", gGeneralDataBuf->u8[3]);
			printf("reg04 = 0x%2X \r\n", gGeneralDataBuf->u8[4]);
			printf("reg05 = 0x%2X \r\n", gGeneralDataBuf->u8[5]);
	*/			
			twiParam.connector = pUsbHeader->paramArray[0];
			twiParam.bus = pUsbHeader->paramArray[1];
			twiParam.slaveAddress = pUsbHeader->paramArray[2];
			twiParam.clkFrequency = pUsbHeader->paramArray[3];
			twiParam.clkDutyCycle = pUsbHeader->paramArray[4];
			twiParam.repeatStartEnable = pUsbHeader->paramArray[5];
			twiParam.writeByteCount = pUsbHeader->downByteCount;
			twiParam.readByteCount = pUsbHeader->upByteCount;
			twiParam.pWriteData = gGeneralDataBuf[0].u8;
			twiParam.pReadData = gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF/2].u8;
		
			if (twiParam.writeByteCount)
			{
				usbGetBulkData( twiParam.pWriteData,
				                twiParam.writeByteCount,
				                true );
			}
		
			printf("reg00 = 0x%2X \r\n", *(twiParam.pWriteData));
			printf("reg01 = 0x%2X \r\n", *(twiParam.pWriteData+1));
			printf("reg02 = 0x%2X \r\n", *(twiParam.pWriteData+2));
			printf("reg03 = 0x%2X \r\n", *(twiParam.pWriteData+3));
			printf("reg04 = 0x%2X \r\n", *(twiParam.pWriteData+4));
			//printf("reg05 = 0x%2X \r\n", gGeneralDataBuf->u8[5]);
		
			if (twiParam.bus)
			{
				//flashLed();
				nack = i2cPseudoTransfer(&twiParam);
			}
			else
			{
				flashLed();
				nack = twiTransfer(&twiParam);
			}
		
			// if just writing data and not reading
			if (twiParam.readByteCount == 0)
			{
				twiParam.pWriteData[0] = nack;
				usbSendBulkData( twiParam.pWriteData,
								 1,
								 true);
			}
			else // if there is a read then should be sending data back anyways
			{
				// if there was a problem with communication then blank the readData
				if (nack != TWI_ERROR_OK)
				{
					for (i=0; i < twiParam.readByteCount; i++)
					{
						twiParam.pReadData[i] = 0;
					}
				}
			
				twiParam.pReadData[twiParam.readByteCount] = nack; // Put status in last byte of array
				usbSendBulkData( twiParam.pReadData,
				                 twiParam.readByteCount + 1,
				                 true );
			}
			
			break;		
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

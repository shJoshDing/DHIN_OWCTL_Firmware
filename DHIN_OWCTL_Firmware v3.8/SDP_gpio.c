// SDP_gpio.c

#include "SDP_std_include.h"
#include "SDP_gpio.h"

extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

//-----------------------------------------------------------------------------
void processGpioCmd(SDP_USB_HEADER *pUsbHeader)
{
	u16 bitReg;
	u16 bitMask;
	
	switch (pUsbHeader->paramArray[0])
	{
		case CONNECTOR_A:
			// PH[7:0]
			bitReg = pUsbHeader->paramArray[1] & 0x000000FF;
			bitMask = 0x000000FF;
			break;
			
		case CONNECTOR_B: 
			// { PH[7:6], PH[15:10] }
			bitReg = (pUsbHeader->paramArray[1] << 10) & 0x0000FC00;
			bitReg |= pUsbHeader->paramArray[1] & 0x000000C0;
			bitMask = 0x0000FCC0;
			break;
			
		default:
			bitReg = 0;
			bitMask = 0;
			break;
	}
	
	switch (pUsbHeader->cmd)
	{
		case ADI_SDP_CMD_GPIO_CONFIG_OUTPUT:
			*pPORTH_FER &= ~bitReg;
			*pPORTHIO_INEN &= ~bitReg;                        // disable input
			*pPORTHIO_DIR |= bitReg;                          // enable output
			break;
			
		case ADI_SDP_CMD_GPIO_CONFIG_INPUT:
			*pPORTH_FER &= ~bitReg;
			*pPORTHIO_DIR &= ~bitReg;                         // disable output
			*pPORTHIO_INEN |= bitReg;                         // enable input
			break;
			
		case ADI_SDP_CMD_GPIO_BIT_SET:
			*pPORTHIO_SET = bitReg;
			break;
			
		case ADI_SDP_CMD_GPIO_BIT_CLEAR:
			*pPORTHIO_CLEAR = bitReg;
			break;
			
		case ADI_SDP_CMD_GPIO_BIT_TOGGLE:
			*pPORTHIO_TOGGLE = bitReg;
			break;
			
		case ADI_SDP_CMD_GPIO_DATA_WRITE:
			*pPORTHIO = (*pPORTHIO & ~bitMask) | bitReg;
			break;
			
		case ADI_SDP_CMD_GPIO_DATA_READ:
			switch (pUsbHeader->paramArray[0])
			{
				case CONNECTOR_A:
					bitReg = *pPORTHIO & 0x000000FF;          // PH[7:0]
					break;
			
				case CONNECTOR_B: 
					bitReg = (*pPORTHIO & 0x0000FC00) >> 10;  // PH[15:10]
					bitReg |= *pPORTHIO & 0x000000C0;         // PH[7:6]
					break;
			
				default:
					bitReg = 0;
					break;
			}
			gGeneralDataBuf[0].u8[0] = bitReg;
			usbSendBulkData(gGeneralDataBuf, pUsbHeader->upByteCount, true);
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

// SDP_user.c

// Edit this file and adi_sdp_user_cmd.h to implement custom commands. It is
// best not to alter any of the other files in the project as it should be
// possible to limit changes to these two files and any other files you wish to
// add. User command values #defines are located in adi_sdp_user_cmd.h and must
// be greater than 0xF8000000 and less than 0xF8000100. It is important to
// change the GUID below to one specific to your project.

#include "SDP_std_include.h"
#include "SDP_user.h"

// SSL HANDLES
// These handles are saved on initialization and are available for use with
// system servers drivers. Do not modify their contents.
extern ADI_DMA_MANAGER_HANDLE ghDmaManager;
extern ADI_DEV_MANAGER_HANDLE ghDevManager;

// GLOABAL GENERAL DATA BUFFER
// This data buffer is available for general use. It is defined globally to
// allow easy reuse of the same data area across files It is used by most
// commands so its structure should not be changed. No assumption should be
// made about the contents of this buffer before the command function is
// called. Also no assumptions be made about data being preserved or removed
// between calls to command functions. It should not be used to pass data
// between files however it is acceptable to pass a pointer to a sub function
// so long as this function does not expect the contents to be preserved when
// execution continues after it returns. The buffer is 4MB structured as an
// array of 64 (SDP_NUM_GEN_DATA_BUF defined in SDP_std_include.h) 64KB (65536
// SDP_DATA_BUF_SIZE defined in SDP_std_include.h) buffers of type 
// SDP_GENERAL_DATA_BUF (also defined in SDP_std_include.h). Each buffer is a
// union of types s8, u8, s16, u16, s32, u32, s64 and u64 to allow for use with
// different sizes of data.
extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

// COMPULSORY GUID (Globally Unique IDentifier)
// To allow your software identify your firmware from others you must
// generate an new GUID and replace the one below then remove the #error.
//#error "IMPORTANT generate an new GUID and replace the one below"
// Use one of the many tools to generate a new GUID and replace the one below.
// e.g. {2F2B0DCF-6CF8-4ae7-B9B1-E760B1B78631} is defined as follows:
static GUID gUserGUID = 
	{0x2f2b0dcf,0x6cf8,0x4ae7,{0xb9,0xb1,0xe7,0x60,0xb1,0xb7,0x86,0x31}};

static void exampleUserCmdNoData(SDP_USB_HEADER *pUsbHeader);
static void exampleUserCmdDataDown(SDP_USB_HEADER *pUsbHeader);
static void exampleUserCmdDataUp(SDP_USB_HEADER *pUsbHeader);
static void exampleUserCmdDataDownUp(SDP_USB_HEADER *pUsbHeader);

//-----------------------------------------------------------------------------
void processUserCmd(SDP_USB_HEADER *pUsbHeader)
{
	switch (pUsbHeader->cmd)
	{
		// compulsory command - DO NOT REMOVE
		case ADI_SDP_CMD_USER_GET_GUID:
			if(pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount != 16)
			{
				invalidTransferCounts(pUsbHeader);
			}
			else
			{
				usbSendBulkData(&gUserGUID, sizeof(gUserGUID), true);
			}
			break;
		
		// example commands - add, edit or remove as necessary
		
		case ADI_SDP_CMD_USER_EXAMPLE_GET_USER_VERSION:
			// check the transfer counts
			if (pUsbHeader->downByteCount != 0 || 
				pUsbHeader->upByteCount != 3 * sizeof(u32))
			{
				invalidTransferCounts(pUsbHeader);
			}
			else
			{
				// transfer the version data
				gGeneralDataBuf[0].u32[0] = 0;     // major
				gGeneralDataBuf[0].u32[1] = 1;     // minor
				gGeneralDataBuf[0].u32[2] = 567;   // build
				
				usbSendBulkData(&gGeneralDataBuf[0], 3 * sizeof(u32), true);
			}
			break;
			
		case ADI_SDP_CMD_USER_EXAMPLE_NO_DATA:
			// check the transfer counts
			if (pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount != 0)
			{
				invalidTransferCounts(pUsbHeader);
			}
			else
			{
				exampleUserCmdNoData(pUsbHeader);
			}
			break;
			
		case ADI_SDP_CMD_USER_EXAMPLE_DATA_DOWN:
			// check the transfer counts
			if (pUsbHeader->downByteCount == 0 || pUsbHeader->upByteCount != 0)
			{
				invalidTransferCounts(pUsbHeader);
			}
			else
			{
				exampleUserCmdDataDown(pUsbHeader);
			}
			break;
			
		case ADI_SDP_CMD_USER_EXAMPLE_DATA_UP:
			// check the transfer counts
			if (pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount == 0)
			{
				invalidTransferCounts(pUsbHeader);
			}
			else
			{
				exampleUserCmdDataUp(pUsbHeader);
			}
			break;
			
		case ADI_SDP_CMD_USER_EXAMPLE_DATA_DOWN_UP:
			// check the transfer counts
			if (pUsbHeader->downByteCount == 0 || pUsbHeader->upByteCount == 0)
			{
				invalidTransferCounts(pUsbHeader);
			}
			else
			{
				exampleUserCmdDataDownUp(pUsbHeader);
			}
			break;
		
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

//-----------------------------------------------------------------------------
static void exampleUserCmdNoData(SDP_USB_HEADER *pUsbHeader)
{
	// This example flashes LED1 if parameter[0] is even
	
	if (!(pUsbHeader->paramArray[0] & 1))
	{
		flashLed();
	}
}

//-----------------------------------------------------------------------------
static void exampleUserCmdDataDown(SDP_USB_HEADER *pUsbHeader)
{
	// This example flashes LED1 if the sum of the data received is between
	// parameter[0] and parameter[1]. The source data is u8 but the sum and
	// comparison are at 32 bits.
	u32 i;
	u32 sum;
	
	usbGetBulkData(&gGeneralDataBuf[0], pUsbHeader->downByteCount, true);
	
	sum = 0;
	for (i=0; i<pUsbHeader->downByteCount; i++)
	{
		sum += gGeneralDataBuf[0].u8[i];
	}
	if (sum > pUsbHeader->paramArray[0] && sum < pUsbHeader->paramArray[1])
	{
		flashLed();
	}
}

//-----------------------------------------------------------------------------
static void exampleUserCmdDataUp(SDP_USB_HEADER *pUsbHeader)
{
	// This example counts in increments of parameter[1] form parameter[0] and 
	// sends the values to the PC. It operates with u16 data.
	u32 i;
	u16 increment;
	
	gGeneralDataBuf[0].u16[0] = (u16)pUsbHeader->paramArray[0];
	increment = (u16)pUsbHeader->paramArray[1];
	for (i=1; i<pUsbHeader->upByteCount; i++)
	{
		gGeneralDataBuf[0].u16[i] = gGeneralDataBuf[0].u16[i-1] + increment;
	}

	usbSendBulkData(&gGeneralDataBuf[0], pUsbHeader->upByteCount, true);
}

//-----------------------------------------------------------------------------
static void exampleUserCmdDataDownUp(SDP_USB_HEADER *pUsbHeader)
{
	// This example adds parameter[0] to all the data values and sends them
	// back to the PC. It operates with u32 data.
	SDP_GENERAL_DATA_BUF *downData = &gGeneralDataBuf[0];
	SDP_GENERAL_DATA_BUF *upData = &gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF/2];
	u32 i;
	u32 loopStop;
	u32 increment;
	
	// In this example we use the bottom half of gGeneralDataBuf to store the
	// data received from the PC and the upper half to store the data before
	// upload to the PC.
	downData = &gGeneralDataBuf[0];
	upData = &gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF/2];
	
	usbGetBulkData(downData, pUsbHeader->downByteCount, true);
	
	if (pUsbHeader->downByteCount > pUsbHeader->upByteCount)
	{
		loopStop = pUsbHeader->upByteCount;
	}
	else
	{
		loopStop = pUsbHeader->downByteCount;
	}
	increment = pUsbHeader->paramArray[0];
	for (i=0; i<loopStop; i++)
	{
		upData->u32[i] = downData->u32[i] + increment;
	}
	
	usbSendBulkData(upData, pUsbHeader->upByteCount, true);
}

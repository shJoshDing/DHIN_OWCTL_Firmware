// SDP_ppi.c

#include "SDP_std_include.h"
#include <drivers/ppi/adi_ppi.h>
#include "SDP_ppi.h"

// streaming parameters
#define NUM_STREAM_BUF (SDP_NUM_GEN_DATA_BUF)
#define STREAM_OVERFLOW_GAP 10

#define TRANSFER_ENABLE_PIN 0x0020

extern ADI_DMA_MANAGER_HANDLE ghDmaManager;
extern ADI_DEV_MANAGER_HANDLE ghDevManager;
extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];
extern ADI_DEV_1D_BUFFER gGeneral1DBufArray[];
extern u8 latchValue;

static ADI_DEV_DEVICE_HANDLE gPpiDriverHandle;

static void ppiWrite(u32 byteCount, u32 parameters[]);
static void ppiRead(u32 byteCount, u32 parameters[]);
static void ppiStreamFrom(u32 parameters[]);
static void ppiCallback(void *AppHandle, u32 Event, void *pArg);

//-----------------------------------------------------------------------------
void processPpiCmd(SDP_USB_HEADER *pUsbHeader)
{
	switch (pUsbHeader->cmd)
	{
		case ADI_SDP_CMD_PPI_WRITE:
			ppiWrite(pUsbHeader->downByteCount, pUsbHeader->paramArray);
			break;
			
		case ADI_SDP_CMD_PPI_READ:
			ppiRead(pUsbHeader->upByteCount, pUsbHeader->paramArray);
			break;
			
		case ADI_SDP_CMD_PPI_STREAM_FROM:
			ppiStreamFrom(pUsbHeader->paramArray);
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

//-----------------------------------------------------------------------------
static void ppiWrite(u32 byteCount, u32 parameters[])
{
	u32 result;
	bool loopMode;
	ADI_DEV_1D_BUFFER write1dBuf;
	ADI_DEV_CMD_VALUE_PAIR configTable[] = {
		{ ADI_PPI_CMD_SET_CONTROL_REG,          (void *)0xF80E },
		{ ADI_PPI_CMD_SET_DELAY_COUNT_REG,      (void *)0 },
		{ ADI_PPI_CMD_SET_TRANSFER_COUNT_REG,   (void *)(byteCount/2 - 1) },
		{ ADI_DEV_CMD_END, 0 }
	};
	
	loopMode = parameters[1];
	
	usbGetBulkData(&gGeneralDataBuf[0], byteCount, true);
	
	write1dBuf.Data = &gGeneralDataBuf[0];
	write1dBuf.ElementWidth = sizeof(u16);
	write1dBuf.ElementCount = byteCount / sizeof(u16);
	write1dBuf.CallbackParameter = &write1dBuf;
	write1dBuf.ProcessedFlag = FALSE;
	write1dBuf.ProcessedElementCount = 0;
	write1dBuf.pNext = NULL;
	write1dBuf.pAdditionalInfo = NULL;
	
	result = adi_dev_Open( 	
		ghDevManager,                    // device manager handle
		&ADIPPIEntryPoint,               // ppi entry point
		0,                               // device number
		NULL,                            // client handle
		&gPpiDriverHandle,               // location for new device handle
		ADI_DEV_DIRECTION_OUTBOUND,      // data direction
		ghDmaManager,                    // handle to the DMA manager
		NULL,
		ppiCallback );                   // client callback function
	
	if (loopMode)
	{
		result = adi_dev_Control( gPpiDriverHandle,
		                          ADI_DEV_CMD_SET_DATAFLOW_METHOD,
		                          (void *)ADI_DEV_MODE_CHAINED_LOOPBACK );
		SDP_ASSERT(result)
		
		result = adi_dev_Control( gPpiDriverHandle,
		                          ADI_DEV_CMD_SET_STREAMING,
		                          (void *)TRUE );
		SDP_ASSERT(result)
	}
	else
	{
		result = adi_dev_Control( gPpiDriverHandle,
		                          ADI_DEV_CMD_SET_DATAFLOW_METHOD,
		                          (void *)ADI_DEV_MODE_CHAINED );
		SDP_ASSERT(result)
	}
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_TABLE,
	                          (void *)configTable );
	SDP_ASSERT(result)
	
	result = adi_dev_Write( gPpiDriverHandle,
	                        ADI_DEV_1D,
	                        (ADI_DEV_BUFFER *)&write1dBuf );
	SDP_ASSERT(result)
	
	switch (parameters[0])
	{
		case CONNECTOR_A:
			latchValue |= 0x30;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x30);
			break;
		case CONNECTOR_B:
			latchValue |= 0xC0;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0xC0);
			break;
	}
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)TRUE );
	SDP_ASSERT(result)
	
	if (loopMode)
	{
		while (!usbCheckNextUsbHeaderAvailable());
	}
	else
	{
		while (write1dBuf.ProcessedFlag == FALSE);
	}
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)FALSE );
	SDP_ASSERT(result)
	
	latchValue &= 0x0F;
	writeToSdpLatch(latchValue);
	//writeToSdpLatch(0x00);

	result = adi_dev_Close(gPpiDriverHandle);
	SDP_ASSERT(result)
}

//-----------------------------------------------------------------------------
static void ppiRead(u32 byteCount, u32 parameters[])
{
	u32 result;
	ADI_DEV_1D_BUFFER read1dBuf;
	ADI_DEV_CMD_VALUE_PAIR configTable[] = {
		{ ADI_DEV_CMD_SET_DATAFLOW_METHOD,      (void *)ADI_DEV_MODE_CHAINED },
		{ ADI_PPI_CMD_SET_CONTROL_REG,          (void *)0xF80C },
		{ ADI_PPI_CMD_SET_DELAY_COUNT_REG,      (void *)0 },
		{ ADI_PPI_CMD_SET_TRANSFER_COUNT_REG,   (void *)(byteCount/2 - 1) },
		{ ADI_DEV_CMD_END, 0 }
	};
	
	read1dBuf.Data = &gGeneralDataBuf[0];
	read1dBuf.ElementWidth = sizeof(u16);
	read1dBuf.ElementCount = byteCount / sizeof(u16);
	read1dBuf.CallbackParameter = &read1dBuf;
	read1dBuf.ProcessedFlag = FALSE;
	read1dBuf.ProcessedElementCount = 0;
	read1dBuf.pNext = NULL;
	read1dBuf.pAdditionalInfo = NULL;
	
	result = adi_dev_Open(
		ghDevManager,                    // device manager handle
		&ADIPPIEntryPoint,               // ppi entry point
		0,                               // device number
		NULL,                            // client handle
		&gPpiDriverHandle,               // location for new device handle
		ADI_DEV_DIRECTION_INBOUND,       // data direction
		ghDmaManager,                    // handle to the DMA manager
		NULL,
		ppiCallback );                   // client callback function
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_TABLE,
	                          (void *)configTable );
	SDP_ASSERT(result)
	
	result = adi_dev_Read( gPpiDriverHandle,
	                       ADI_DEV_1D,
	                       (ADI_DEV_BUFFER *)&read1dBuf );
	SDP_ASSERT(result)
	
	switch (parameters[0])
	{
		case CONNECTOR_A:
			latchValue |= 0x10;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x10);
			break;
		case CONNECTOR_B:
			latchValue |= 0x40;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x40);
			break;
	}
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)TRUE );
	SDP_ASSERT(result)
	
	while (read1dBuf.ProcessedFlag == FALSE);
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)FALSE );
	SDP_ASSERT(result)
	
	latchValue &= 0x0F;
	writeToSdpLatch(latchValue);
	//writeToSdpLatch(0x00);

	result = adi_dev_Close(gPpiDriverHandle);
	SDP_ASSERT(result)
	
	usbSendBulkData(&gGeneralDataBuf[0], byteCount, true);
}

//-----------------------------------------------------------------------------
static void ppiStreamFrom(u32 parameters[])
{
	u16 i;
	u32 result;
	s32 currentUploadBuffer;
	s32 overflowCheckBuffer;
	u32 transfersPerBuffer = parameters[1] / 2;
	bool transferPinEnable = parameters[2];
	ADI_DEV_CMD_VALUE_PAIR configTable[] = {
		{ ADI_DEV_CMD_SET_DATAFLOW_METHOD,
			(void *)ADI_DEV_MODE_CHAINED_LOOPBACK },
		{ ADI_DEV_CMD_SET_STREAMING,
			(void *)TRUE },
		{ ADI_PPI_CMD_SET_CONTROL_REG,
			(void *)0xF80C },
		{ ADI_PPI_CMD_SET_DELAY_COUNT_REG,
			(void *)0 },
		{ ADI_PPI_CMD_SET_TRANSFER_COUNT_REG,
			(void *)(transfersPerBuffer - 1) },
		{ ADI_DEV_CMD_END, 0 }
	};
	
	for (i=0; i < NUM_STREAM_BUF; i++)
	{
		gGeneral1DBufArray[i].Data = &gGeneralDataBuf[i];
		gGeneral1DBufArray[i].ElementWidth = sizeof(u16);
		gGeneral1DBufArray[i].ElementCount = transfersPerBuffer;
		gGeneral1DBufArray[i].CallbackParameter = &gGeneral1DBufArray[i];
		gGeneral1DBufArray[i].ProcessedFlag = FALSE;
		gGeneral1DBufArray[i].pNext = &gGeneral1DBufArray[i+1];
		gGeneral1DBufArray[i].pAdditionalInfo = NULL;
	}
	gGeneral1DBufArray[NUM_STREAM_BUF - 1].pNext = NULL;
	
	result = adi_dev_Open(
		ghDevManager,                    // device manager handle
		&ADIPPIEntryPoint,               // ppi entry point
		0,                               // device number
		NULL,                            // client handle
		&gPpiDriverHandle,               // location for new device handle
		ADI_DEV_DIRECTION_INBOUND,       // data direction
		ghDmaManager,                    // handle to the DMA manager
		NULL,
		ppiCallback );                   // client callback function
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_TABLE,
	                          (void *)configTable );
	SDP_ASSERT(result)
	
	result = adi_dev_Read( gPpiDriverHandle,
	                       ADI_DEV_1D,
	                       (ADI_DEV_BUFFER *)&gGeneral1DBufArray[0] );
	SDP_ASSERT(result)
	
	currentUploadBuffer = 0;
	overflowCheckBuffer = NUM_STREAM_BUF - STREAM_OVERFLOW_GAP - 2;
	
	switch (parameters[0])
	{
		case CONNECTOR_A:
			latchValue |= 0x10;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x10);
			break;
		case CONNECTOR_B:
			latchValue |= 0x40;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x40);
			break;
	}
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)TRUE );
	SDP_ASSERT(result)
	
	if(transferPinEnable)
	{
		*pPORTG_FER &= ~TRANSFER_ENABLE_PIN;               // enable GPIO
		*pPORTGIO_INEN &= ~TRANSFER_ENABLE_PIN;            // disable input
		*pPORTGIO_SET = TRANSFER_ENABLE_PIN;               // set
		*pPORTGIO_DIR |= TRANSFER_ENABLE_PIN;              // enable output
	}
	
	do							
	{
		// check if there is a buffer ready to send
		if (gGeneral1DBufArray[currentUploadBuffer].ProcessedFlag == TRUE)
		{
			gGeneral1DBufArray[currentUploadBuffer].ProcessedFlag = FALSE;
			
			usbSendBulkData( gGeneral1DBufArray[currentUploadBuffer].Data,
			                 transfersPerBuffer * sizeof(u16),
			                 true);
									
			if (currentUploadBuffer == NUM_STREAM_BUF - 1)
			{
				currentUploadBuffer = 0;
			}
			else
			{
				currentUploadBuffer++;
			}
			
			if (overflowCheckBuffer == NUM_STREAM_BUF - 1)
			{
				overflowCheckBuffer = 0;
			}
			else
			{
				overflowCheckBuffer++;
			}
		}
		
		// check for near buffer overflow
		if (gGeneral1DBufArray[overflowCheckBuffer].ProcessedFlag)
		{
			if(transferPinEnable)
			{
				*pPORTGIO_CLEAR = TRANSFER_ENABLE_PIN;
			}
			else
			{
				break;
			}
		}
		else
		{
			if(transferPinEnable)
			{
				*pPORTGIO_SET = TRANSFER_ENABLE_PIN;
			}
		}
		
		// exit if stop packet received
	} while (!usbCheckNextUsbHeaderAvailable());
	
	if(transferPinEnable)
	{
		*pPORTGIO_CLEAR = TRANSFER_ENABLE_PIN;
	}
	
	result = adi_dev_Control( gPpiDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)FALSE );
	SDP_ASSERT(result)
	
	latchValue &= 0x0F;
	writeToSdpLatch(latchValue);
	//writeToSdpLatch(0x00);

	result = adi_dev_Close(gPpiDriverHandle);
	SDP_ASSERT(result)
}

//-----------------------------------------------------------------------------
static void ppiCallback(void *AppHandle, u32 Event, void *pArg)
{
}

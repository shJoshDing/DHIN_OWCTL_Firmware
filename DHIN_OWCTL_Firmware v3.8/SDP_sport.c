// SDP_sport.c

#include "SDP_std_include.h"
#include <drivers/sport/adi_sport.h>
#include "SDP_sport.h"
#include "SDP_pdm.h"

// streaming parameters
#define NUM_STREAM_BUF (SDP_NUM_GEN_DATA_BUF-1)
#define STREAM_OVERFLOW_GAP 6

extern ADI_DMA_MANAGER_HANDLE ghDmaManager;
extern ADI_DEV_MANAGER_HANDLE ghDevManager;
extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];
extern ADI_DEV_1D_BUFFER gGeneral1DBufArray[];
ADI_DEV_1D_BUFFER write1dBufArray[32];
ADI_DEV_1D_BUFFER read1dBufArray[32];
static ADI_DEV_DEVICE_HANDLE gSportDriverHandle;

static void sportTransfer( u32 downByteCount,
                           u32 upByteCount,
                           u32 parameters[] );
static void sportStreamFrom(u32 parameters[]);
static void sportDriverCheckAndConfig( bool streamMode,
                                       u32 connector,
                                       u32 configVals[] );
static void sportCallback(void *AppHandle, u32 Event, void *pArg);

//-----------------------------------------------------------------------------
void processSportCmd(SDP_USB_HEADER *pUsbHeader)
{
	switch (pUsbHeader->cmd)
	{
/*		case ADI_SDP_CMD_SPORT_PDM:
			//todo MyPDM, Mage
			
			PDMEntry();
			break;
		case ADI_SDP_CMD_SPORT_TestRead:
			sendDatatoPC();
			break;
		case ADI_SDP_CMD_SPORT_PDMStop:
			PDMClose();
			break;
*/		case ADI_SDP_CMD_SPORT_TRANSFER:
			if (pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount != 0)
			{
				sportTransfer( pUsbHeader->downByteCount,
				               pUsbHeader->upByteCount,
				               pUsbHeader->paramArray );
			}
			break; 
			
		case ADI_SDP_CMD_SPORT_STREAM_FROM:
			sportStreamFrom(pUsbHeader->paramArray);
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

//-----------------------------------------------------------------------------
static void sportTransfer( u32 downByteCount,
                           u32 upByteCount,
                           u32 parameters[] )
{
	u32 dummyData;
	s32 i;
	s32 loopStop;
	u32 elementWidth;
	u32 elementCount;
	ADI_DEV_1D_BUFFER write1dBuf;
	ADI_DEV_1D_BUFFER read1dBuf;
	bool loopMode;
	u32 result;
	u32 xferCount;
	u32 remainder;
	u32 totalElementCount; 
	
	if ((downByteCount <65536) && (upByteCount < 65536))
	{

		loopMode = (bool)parameters[31];
	
		sportDriverCheckAndConfig(loopMode, parameters[0], &parameters[1]);
	
		elementWidth = (parameters[12]<8)? 1 : (parameters[12]<16)? 2 : 4;
	
		if (downByteCount == 0)
		{
			dummyData = parameters[30] ? 0xFFFFFFFF : 0x00000000;
			loopStop = (upByteCount/4) + (upByteCount%4);
			for (i=0; i<loopStop; i++)
			{
				gGeneralDataBuf[0].u32[i] = dummyData;
			}
			elementCount = upByteCount / elementWidth;
		}
		else
		{
			usbGetBulkData(&gGeneralDataBuf[0], downByteCount, true);
			if(upByteCount == 0 || downByteCount < upByteCount)
				elementCount = downByteCount / elementWidth;
			else
				elementCount = upByteCount / elementWidth;
		}
	
		write1dBuf.Data = &gGeneralDataBuf[0];
		write1dBuf.ElementWidth = elementWidth;
		write1dBuf.ElementCount = elementCount;		
		write1dBuf.CallbackParameter = NULL;
		write1dBuf.ProcessedFlag = FALSE;
		write1dBuf.ProcessedElementCount = 0;
		write1dBuf.pNext = NULL;
		write1dBuf.pAdditionalInfo = NULL;
	
		read1dBuf.Data = &gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF/2];
		read1dBuf.ElementWidth = elementWidth;
		read1dBuf.ElementCount = elementCount;
		read1dBuf.CallbackParameter = &read1dBuf;
		read1dBuf.ProcessedFlag = FALSE;
		read1dBuf.pNext = NULL;
		read1dBuf.pAdditionalInfo = NULL;
	
	// give buffers to the write & read functions the and set data flow
		result = adi_dev_Read( gSportDriverHandle,
	                       ADI_DEV_1D,
	                       (ADI_DEV_BUFFER *)&read1dBuf );
		SDP_ASSERT(result)
	
		result = adi_dev_Write( gSportDriverHandle,
	                        ADI_DEV_1D,
	                        (ADI_DEV_BUFFER *)&write1dBuf );
		SDP_ASSERT(result)
	
		result = adi_dev_Control( gSportDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)TRUE );
		SDP_ASSERT(result)
	
		if (loopMode)
		{
			while (!usbCheckNextUsbHeaderAvailable());
		}
		else
		{
			while (read1dBuf.ProcessedFlag == FALSE);
		}
	
		result = adi_dev_Control( gSportDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)FALSE );
		SDP_ASSERT(result)
	
		if (upByteCount)
		{
			usbSendBulkData(read1dBuf.Data, upByteCount, true);
		}
	}
	else
	{
		sportDriverCheckAndConfig(true, parameters[0], &parameters[1]);
		elementWidth = (parameters[12]<8)? 1 : (parameters[12]<16)? 2 : 4;

		if (downByteCount == 0)
		{
			dummyData = parameters[30] ? 0xFFFFFFFF : 0x00000000;
			loopStop = (upByteCount/4) + (upByteCount%4);
			for (i=0; i<loopStop; i++)
			{
		//	*(gGeneralDataBuf[0].u32 +i) = i;
				gGeneralDataBuf[0].u32[i] = dummyData;
			}
			remainder = upByteCount % 65536;
			if (remainder == 0)
				xferCount = upByteCount/65536;
			else
				xferCount = upByteCount/65536 +1;
			totalElementCount = upByteCount/elementWidth;
		}
		else //downByteCount >0
		{
			usbGetBulkData(&gGeneralDataBuf[0], downByteCount, true);
			if(upByteCount == 0 || downByteCount < upByteCount)
			{
				remainder= downByteCount%65536;
				if (remainder == 0)
					xferCount = downByteCount/65536;
				else
					xferCount = downByteCount/65536 +1;
				totalElementCount = downByteCount / elementWidth;
			}
			else
			{
				remainder= downByteCount%65536;
				if (remainder == 0)
					xferCount = downByteCount/65536;
				else
					xferCount = downByteCount/65536 +1;
				totalElementCount = upByteCount/elementWidth;
			}
		}

		for (i = 0; i<xferCount; i++)
		{
			if((i == xferCount -1) && (totalElementCount%65536>0))			
				elementCount = totalElementCount% 65536;
			else
				elementCount = 65536;
				
			write1dBufArray[i].Data = &gGeneralDataBuf[i];
			write1dBufArray[i].ElementWidth = elementWidth;
			write1dBufArray[i].ElementCount = elementCount;		
			write1dBufArray[i].CallbackParameter = NULL;
			write1dBufArray[i].ProcessedFlag = FALSE;
			write1dBufArray[i].ProcessedElementCount = 0;
			write1dBufArray[i].pNext = &write1dBufArray[i+1];
			write1dBufArray[i].pAdditionalInfo = NULL;
	

			read1dBufArray[i].Data = &gGeneralDataBuf[32+i];
			read1dBufArray[i].ElementWidth = elementWidth;
			read1dBufArray[i].ElementCount = elementCount;
			read1dBufArray[i].CallbackParameter = &read1dBufArray[i];
			read1dBufArray[i].ProcessedFlag = FALSE;
			read1dBufArray[i].pNext = &read1dBufArray[i+1];
			read1dBufArray[i].pAdditionalInfo = NULL;
		}
		write1dBufArray[xferCount - 1].pNext = NULL;
		read1dBufArray[xferCount - 1].pNext = NULL;
	
	// give buffers to the write & read functions the and set data flow
		result = adi_dev_Read( gSportDriverHandle,
	                       		ADI_DEV_1D,
	                       		(ADI_DEV_BUFFER *)read1dBufArray);
		SDP_ASSERT(result)
	
		result = adi_dev_Write(	gSportDriverHandle,
	                        ADI_DEV_1D,
	                        (ADI_DEV_BUFFER *)write1dBufArray);
		SDP_ASSERT(result)

		result = adi_dev_Control( gSportDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)TRUE );
	    SDP_ASSERT(result)
	
		while (read1dBufArray[xferCount -1].ProcessedFlag == FALSE);
	   
		result = adi_dev_Control( gSportDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                         (void *)FALSE); 
	    SDP_ASSERT(result)
		    
		if (upByteCount)
		{
			usbSendBulkData(read1dBufArray[0].Data, upByteCount, true);
		}
		
	}
	}

//-----------------------------------------------------------------------------
static void sportStreamFrom(u32 parameters[])
{
	u32 dummyData;
	s32 i;
	s32 bufferSize;
	ADI_DEV_1D_BUFFER write1dBuf;
	s32 elementWidth;
	s32 elementCount;
	s32 currentUploadBuffer;
	s32 overflowCheckBuffer;
	u32 result;

	bufferSize = parameters[31];
	
	sportDriverCheckAndConfig(true, parameters[0], &parameters[1]);
	
	//fill the dummy txbuffer
	dummyData = parameters[30] ? 0xFFFFFFFF : 0x00000000;
	for (i=0; i<bufferSize/sizeof(u32); i++)
	{
		gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF-1].u32[i] = dummyData;
	}
	
	if (parameters[12] < 8)
	{
		elementWidth = 1;
	}
	else if (parameters[12] < 16)
	{
		elementWidth = 2;
	}
	else 
	{
		elementWidth = 4;
	}
	elementCount = bufferSize / elementWidth;
	
	write1dBuf.Data = &gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF-1];
	write1dBuf.ElementWidth = elementWidth;
	write1dBuf.ElementCount = elementCount;		
	write1dBuf.CallbackParameter = NULL;
	write1dBuf.ProcessedFlag = FALSE;
	write1dBuf.ProcessedElementCount = 0;
	write1dBuf.pNext = NULL;
	write1dBuf.pAdditionalInfo = NULL;
	
	for (i=0; i < NUM_STREAM_BUF; i++)
	{
		gGeneral1DBufArray[i].Data = &gGeneralDataBuf[i];
		gGeneral1DBufArray[i].ElementWidth = elementWidth;
		gGeneral1DBufArray[i].ElementCount = elementCount;
		gGeneral1DBufArray[i].CallbackParameter = &gGeneral1DBufArray[i];
		gGeneral1DBufArray[i].ProcessedFlag = FALSE;
		gGeneral1DBufArray[i].pNext = &gGeneral1DBufArray[i+1];
		gGeneral1DBufArray[i].pAdditionalInfo = NULL;
	}
	gGeneral1DBufArray[NUM_STREAM_BUF - 1].pNext = NULL;
	
	// give buffers to the write & read functions the and set data flow
	result = adi_dev_Read( gSportDriverHandle,
	                       ADI_DEV_1D,
	                       (ADI_DEV_BUFFER *)&gGeneral1DBufArray[0] );
	SDP_ASSERT(result)
	
	result = adi_dev_Write(	gSportDriverHandle,
	                        ADI_DEV_1D,
	                        (ADI_DEV_BUFFER *)&write1dBuf);
	SDP_ASSERT(result)
	
	currentUploadBuffer = 0;
	overflowCheckBuffer = NUM_STREAM_BUF - STREAM_OVERFLOW_GAP - 2;
	
	result = adi_dev_Control( gSportDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)TRUE );
	SDP_ASSERT(result)
	
	do							
	{
		// check if there is a buffer ready to send
		if (gGeneral1DBufArray[currentUploadBuffer].ProcessedFlag == TRUE)
		{
			gGeneral1DBufArray[currentUploadBuffer].ProcessedFlag = FALSE;
			
			usbSendBulkData( gGeneral1DBufArray[currentUploadBuffer].Data,
			                 bufferSize,
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
		
		// exit if looped buffer is near overflow or stop packet received
	} while (!gGeneral1DBufArray[overflowCheckBuffer].ProcessedFlag
	          && !usbCheckNextUsbHeaderAvailable());
	
	result = adi_dev_Control( gSportDriverHandle,
	                          ADI_DEV_CMD_SET_DATAFLOW,
	                          (void *)FALSE );
	SDP_ASSERT(result)
}

//-----------------------------------------------------------------------------
static void sportDriverCheckAndConfig( bool streamMode,
                                       u32 connector,
                                       u32 configVals[])
{
	static const u32 driverCommandList[29] = {
		ADI_SPORT_CMD_SET_TX_CLOCK_FREQ,
		ADI_SPORT_CMD_SET_TX_FRAME_SYNC_FREQ,
		ADI_SPORT_CMD_SET_TX_CLOCK_SOURCE,
		ADI_SPORT_CMD_SET_TX_DATA_FORMAT,
		ADI_SPORT_CMD_SET_TX_BIT_ORDER,
		ADI_SPORT_CMD_SET_TX_FS_SOURCE,
		ADI_SPORT_CMD_SET_TX_FS_REQUIREMENT,
		ADI_SPORT_CMD_SET_TX_FS_DATA_GEN,
		ADI_SPORT_CMD_SET_TX_FS_POLARITY,
		ADI_SPORT_CMD_SET_TX_FS_TIMING,
		ADI_SPORT_CMD_SET_TX_EDGE_SELECT,
		ADI_SPORT_CMD_SET_TX_WORD_LENGTH,
		ADI_SPORT_CMD_SET_TX_SECONDARY_ENABLE,
		ADI_SPORT_CMD_SET_TX_STEREO_FS_ENABLE,
		ADI_SPORT_CMD_SET_TX_LEFT_RIGHT_ORDER,
		ADI_SPORT_CMD_SET_RX_CLOCK_FREQ,
		ADI_SPORT_CMD_SET_RX_FRAME_SYNC_FREQ,
		ADI_SPORT_CMD_SET_RX_CLOCK_SOURCE,
		ADI_SPORT_CMD_SET_RX_DATA_FORMAT,
		ADI_SPORT_CMD_SET_RX_BIT_ORDER,
		ADI_SPORT_CMD_SET_RX_FS_SOURCE,
		ADI_SPORT_CMD_SET_RX_FS_REQUIREMENT,
		ADI_SPORT_CMD_SET_RX_FS_POLARITY,
		ADI_SPORT_CMD_SET_RX_FS_TIMING,
		ADI_SPORT_CMD_SET_RX_EDGE_SELECT,
		ADI_SPORT_CMD_SET_RX_WORD_LENGTH,
		ADI_SPORT_CMD_SET_RX_SECONDARY_ENABLE,
		ADI_SPORT_CMD_SET_RX_STEREO_FS_ENABLE,
		ADI_SPORT_CMD_SET_RX_LEFT_RIGHT_ORDER };
	static bool driverOpen = false;
	static u32 currentConnector;
	static u32 currentStreamMode;
	static u32 currentConfig[29];
	s32 i;
	bool reOpenDriver;
	u32 result;
	
	if (driverOpen)
	{
		reOpenDriver = false;
		for (i=0; i<29 && !reOpenDriver; i++)
		{
			reOpenDriver = currentConfig[i] != configVals[i];
		}
		if (currentConnector != connector || currentStreamMode != streamMode)
		{
			reOpenDriver = true;
		}
		if (reOpenDriver)
		{
			result = adi_dev_Close(gSportDriverHandle);
			SDP_ASSERT(result)
			driverOpen = false;
		}
	}
	
	if (!driverOpen)
	{
		result = adi_dev_Open( 	
			ghDevManager,                    // device manager handle
			&ADISPORTEntryPoint,             // sport entry point
			connector,                       // device number
			NULL,                            // client handle
			&gSportDriverHandle,             // location for new device handle
			ADI_DEV_DIRECTION_BIDIRECTIONAL, // data direction
			ghDmaManager,                    // handle to the DMA manager
			NULL,
			sportCallback);                  // client callback function
		SDP_ASSERT(result)
		driverOpen = true;
		currentConnector = connector;
		
		if (streamMode)
		{
			// stream mode / loop mode
			result = adi_dev_Control( gSportDriverHandle,
			                          ADI_DEV_CMD_SET_DATAFLOW_METHOD,
			                         (void *)ADI_DEV_MODE_CHAINED_LOOPBACK);
			
			
			SDP_ASSERT(result)
			result = adi_dev_Control( gSportDriverHandle,
			                          ADI_DEV_CMD_SET_STREAMING,
			                          (void *)TRUE );
			SDP_ASSERT(result)
			currentStreamMode = true;
		}
		else
		{
			// single transfer mode
			result = adi_dev_Control( gSportDriverHandle,
			                          ADI_DEV_CMD_SET_DATAFLOW_METHOD,
			                          (void *)ADI_DEV_MODE_CHAINED);
			                          

			SDP_ASSERT(result)
			currentStreamMode = false;
		}
		
		for (i=0; i<29; i++)
		{
			currentConfig[i] = configVals[i];
			result = adi_dev_Control( gSportDriverHandle,
			                          driverCommandList[i],
			                          (void*)currentConfig[i] );
			SDP_ASSERT(result)
		}
	}
}

//-----------------------------------------------------------------------------
static void sportCallback(void *AppHandle, u32 Event, void *pArg)
{
}

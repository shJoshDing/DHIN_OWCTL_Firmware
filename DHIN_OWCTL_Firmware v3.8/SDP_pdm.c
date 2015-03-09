/*****************************************************************************
 * TDM.c
 *****************************************************************************/
#include <bfrom.h>
#include <services/services.h>     // system service includes
#include <drivers/adi_dev.h>       // device manager includes
#include "SDP_ssl_init.h"
#include "SDP_adi_int.h"
#include <cdefBF527.h>
#include <sys/exception.h>
#include <stdio.h>
#include <ccblkfn.h>
#include <drivers/sport/adi_sport.h>
#include <string.h>
#include "SDP_std_include.h"
#include "SDP_usb.h"
//#include <gpio_i2c.h>
#include "SDP_gpio_i2c.h"

#define CONFIG_LENGTH 9
#define DATA_BUF_SIZE 	1024*32*8
//#define I2S_BUF_SIZE   	(DATA_BUF_SIZE/4)
#define PDM_BUF_SIZE 	(DATA_BUF_SIZE/4)

//u8 gRxDataBuf[DATA_BUF_SIZE * 2];
//u8 gTxDataBuf[DATA_BUF_SIZE * 2];
//u32 gI2SDataBuf[I2S_BUF_SIZE/2];
u32 gPDMDataBuf[PDM_BUF_SIZE / 2];
u32 sendPDMDataBuf[PDM_BUF_SIZE / 4];
u32 TestBuf[128];
volatile u32 CountFlag = 0;
 
extern ADI_DMA_MANAGER_HANDLE ghDmaManager;   // handle to the DMA manager
extern ADI_DEV_MANAGER_HANDLE ghDevManager;   // handle to the device manager
/* Handle to the vendor specific Bulk USB class driver */
static ADI_DEV_DEVICE_HANDLE    DevHandle;

ADI_DEV_DEVICE_HANDLE gSport0Handle;
//ADI_DEV_DEVICE_HANDLE gSport1Handle;

//ADI_DEV_1D_BUFFER write1dBufPing, write1dBufPang;
//ADI_DEV_1D_BUFFER read1dBufPing, read1dBufPang;
//ADI_DEV_1D_BUFFER I2S1dBufPing, I2S1dBufPang;
ADI_DEV_1D_BUFFER PDMBufPing, PDMBufPang, SendBuf;

static void audioCallback(void *AppHandle, u32 Event, void *pArg);
static void I2SCallback(void *AppHandle, u32 Event, void *pArg);
static void PDMCallback(void *AppHandle, u32 Event, void *pArg);
u32 send_PDMData(ADI_DEV_DEVICE_HANDLE DevHandle, u32 *p32Address, u32 u32Count);
u32 usb_Send(ADI_DEV_DEVICE_HANDLE 	DeviceHandle,			/* DM handle         */
			ADI_DEV_BUFFER_TYPE		BufferType,				/* buffer type       */
			ADI_DEV_BUFFER			*pBuffer,				/* pointer to buffer */
			bool					bWaitForCompletion);		/* completion flag   */


void PDMEntry( void )
{
	u8 i = 0;
	int Result = 0;
	
	//-----------------------------------------------------------------
	static const u32 driverCommandList[CONFIG_LENGTH] = {	
/*		ADI_SPORT_CMD_SET_TX_CLOCK_SOURCE,
		ADI_SPORT_CMD_SET_TX_FS_SOURCE,
		ADI_SPORT_CMD_SET_TX_FS_REQUIREMENT,
		ADI_SPORT_CMD_SET_TX_FS_TIMING,
		ADI_SPORT_CMD_SET_TX_EDGE_SELECT,
		ADI_SPORT_CMD_SET_TX_WORD_LENGTH,
		ADI_SPORT_CMD_SET_TX_STEREO_FS_ENABLE,
		ADI_SPORT_CMD_SET_TCLKDIV,
		ADI_SPORT_CMD_SET_TFSDIV,
*/		
		//ADI_SPORT_CMD_SET_RX_FRAME_SYNC_FREQ,
		ADI_SPORT_CMD_SET_RX_CLOCK_SOURCE,
		ADI_SPORT_CMD_SET_RX_FS_SOURCE,
		ADI_SPORT_CMD_SET_RX_FS_REQUIREMENT,
		ADI_SPORT_CMD_SET_RX_FS_TIMING,
		ADI_SPORT_CMD_SET_RX_EDGE_SELECT,
		ADI_SPORT_CMD_SET_RX_WORD_LENGTH,
		ADI_SPORT_CMD_SET_RX_STEREO_FS_ENABLE,
		ADI_SPORT_CMD_SET_RCLKDIV,
		ADI_SPORT_CMD_SET_RFSDIV };
		
	static const u32 currentConfig[CONFIG_LENGTH] = {
	/*	0x00,	//1:internal clock, 0: external clock
		0x00,	//1:internal fs, 0: external fs
		0x01,	//fs each word
		0x01,	//late fs
		0x00,	//edge select
		31,		//word length
		0x01,	//stereo fs
		0x13,	//clk div
		0x1F,	//fsdiv
	*/	
		0x00,	//1:internal clock, 0: external clock
		0x01,	//1:internal fs, 0: external fs
		0x00,	//0:no receive frame sync with each word, 1:receive frame sync for each word
		0x00,	//late fs
		0x00,	//edge select, Internal clk:(1: falling edge. 0: rising edge) External clk: (1: rising 0: falling)
		31,		//word length
		0x01,	//1:stereo fs 0:normal mode
		0x1,	//clk div
		0x1F	//fsdiv
		};
	
	
	//adiSslInit(0);
	//sdpSslUpdateSdramPll();
		
	//sport0	
	//------------------------------------------------------------------------------	
	adi_dev_Open(ghDevManager,
	              &ADISPORTEntryPoint,
	              0,
	              NULL,
	              &gSport0Handle,
	              //ADI_DEV_DIRECTION_BIDIRECTIONAL,
	              ADI_DEV_DIRECTION_INBOUND,
	              ghDmaManager,
	              NULL,
	              PDMCallback);

    if (adi_dev_Control(gSport0Handle, ADI_DEV_CMD_SET_DATAFLOW_METHOD,(void *)ADI_DEV_MODE_CHAINED_LOOPBACK)!=0)
    	printf("open failed!\r\n");
	
    //Config parameters
/*    *pSPORT0_RFSDIV = 0x1F;
    *pSPORT0_RCR2 = 0x1F;
    *pSPORT0_RCR2 |= RSFSE;
    *pSPORT0_RCR1 = TFSR;
    *pSPORT0_RCR1 |= IRFS;
*/    
    for (i=0; i< CONFIG_LENGTH; i++)
	{
		if (adi_dev_Control( gSport0Handle, driverCommandList[i], (void*)currentConfig[i] )!=0)
			printf("config failed!\r\n");
	}
		
	PDMBufPing.Data = (u32 *)&gPDMDataBuf[0];
	PDMBufPing.ElementWidth = 4;
	PDMBufPing.ElementCount = PDM_BUF_SIZE/4;
	PDMBufPing.CallbackParameter = &PDMBufPing;
	PDMBufPing.ProcessedFlag = FALSE;
	PDMBufPing.pNext = &PDMBufPang;
	
	PDMBufPang.Data = (u32 *)&gPDMDataBuf[PDM_BUF_SIZE/4];
	PDMBufPang.ElementWidth = 4;
	PDMBufPang.ElementCount = PDM_BUF_SIZE/4;
	PDMBufPang.CallbackParameter = &PDMBufPang;
	PDMBufPang.ProcessedFlag = FALSE;
	PDMBufPang.pNext = NULL;
		
	//ADMP521T_CLK_OE(false);	
	CountFlag = 0;
	if(adi_dev_Read( gSport0Handle, ADI_DEV_1D, (ADI_DEV_BUFFER *)&PDMBufPing) != 0)
		printf("read failed!\r\n");	
	
//	else
//		printf("Read successfully\r\n");
	//start SPORT interface loopback
	//--------------------------------------------------------------------
	if (adi_dev_Control( gSport0Handle,ADI_DEV_CMD_SET_DATAFLOW,(void *)TRUE) != 0)
		printf("Start error!\r\n");
		
}

void PDMClose( void )
{
	if (adi_dev_Control( gSport0Handle,ADI_DEV_CMD_SET_DATAFLOW,(void *)FALSE) != 0)
		printf("Close error!\r\n");
	
	if(adi_dev_Close(gSport0Handle) != ADI_DEV_RESULT_SUCCESS)
		printf("close failed!!\r\n");
	else
		printf("close sucessfully!!\r\n");

}

//-----------------------------------------------------------------------------
static void audioCallback(void *AppHandle, u32 Event, void *pArg)
{
/*	u8  	*pSrc = NULL;
    u8 		*pDest = NULL;
    
    u32      i;
    u32      j;
    // Pointer to processed buffer 
    ADI_DEV_1D_BUFFER *pBuffer;
    
     // CASEOF (Event) 
    switch (Event)
    {
     // CASE (Event buffer processed) 
		case (ADI_DEV_EVENT_BUFFER_PROCESSED):
			// Get the address of 1D buffer that was processed 
			pBuffer = (ADI_DEV_1D_BUFFER*)pArg;

			// get the source & destination data buffer addresses 
			if ( pBuffer == &PDMBufPing )
			{
				flashLed();
				pSrc = (u32 *) &gRxDataBuf[0];
				pDest = (u32 *) &gPDMDataBuf[0];
			}
			else if (pBuffer == &PDMBufPang)
			{
				pSrc = (u8 *) &gRxDataBuf[DATA_BUF_SIZE];
				pDest = (u8 *) &gPDMDataBuf[PDM_BUF_SIZE/4];	
			}
		
			for(i = 0; i < (I2S_BUF_SIZE/8) ; i++)
			{
				for( j = 0; j<4; j++ )
				{
					*(pDest + i*8 + 3 - j) = *(pSrc + i*32 + j) ;
				
					*(pDest + i*8 + 7 - j) = *(pSrc + i*32 + 4+ j) ;
				}			
			}
			break;
		default:
			break;
    }
    */
    return;
}


//-----------------------------------------------------------------------------
static void I2SCallback(void *AppHandle, u32 Event, void *pArg)
{
	return;
}

//-----------------------------------------------------------------------------
u32      iii = 0 ;
static void PDMCallback(void *AppHandle, u32 Event, void *pArg)
{
	u8  	*pSrc = NULL;
    u8 		*pDest = NULL;
    
    
    u32      j;
        
    //flashLed();
    //printf("Enter PDM call back\r\n");
    /* Pointer to processed buffer */
    ADI_DEV_1D_BUFFER *pBuffer;
    
     /* CASEOF (Event) */
    switch (Event)
    {
     /* CASE (Event buffer processed) */
		case (ADI_DEV_EVENT_BUFFER_PROCESSED):
			/* Get the address of 1D buffer that was processed */
			pBuffer = (ADI_DEV_1D_BUFFER*)pArg;

			/* get the source & destination data buffer addresses */
			if ( pBuffer == &PDMBufPing )
			{
				//flashLed();
				memcpy((void *)&sendPDMDataBuf[0], (void *)&gPDMDataBuf[0], PDM_BUF_SIZE );
			}
			else if (pBuffer == &PDMBufPang)
			{
				memcpy((void *)&sendPDMDataBuf[0], (void *)&gPDMDataBuf[PDM_BUF_SIZE/4], PDM_BUF_SIZE); //PDM_BUF_SIZE / 4
			}
			//usbSendBulkData(
			CountFlag++;
			//printf("PDM call back \r\n");
			usbSendBulkData( &sendPDMDataBuf[0],
			                 PDM_BUF_SIZE,
			                 false);
			//send_PDMData(DevHandle, (u32 *)&sendPDMDataBuf[0], PDM_BUF_SIZE/4);		
			break;
		default:
			break;
    }
	return;
}

//-----------------------------------------------------------------------------
u32 usb_Send(ADI_DEV_DEVICE_HANDLE 	DeviceHandle,			/* DM handle         */
			ADI_DEV_BUFFER_TYPE		BufferType,				/* buffer type       */
			ADI_DEV_BUFFER			*pBuffer,				/* pointer to buffer */
			bool					bWaitForCompletion)		/* completion flag   */
{
	u32 Result;// = ADI_DEV_RESULT_SUCCESS;
    ADI_DEV_1D_BUFFER			*p1DBuff;
    p1DBuff = (ADI_DEV_1D_BUFFER*)pBuffer;

    // Place the Endpoint ID 
    //p1DBuff->Reserved[BUFFER_RSVD_EP_ADDRESS] = g_WriteEpID;

 	// if the user wants to wait until the operation is complete 
/*    if (bWaitForCompletion)
    {
        // clear the tx flag and call read 
   		g_bTxFlag = FALSE;
		Result = adi_dev_Write(DeviceHandle, BufferType, pBuffer);

		// wait for the tx flag to be set 
		while (!g_bTxFlag)
		{
		   // make sure we are still configured, if not we should fail
			if ( !g_bUsbConfigured )
				return ADI_DEV_RESULT_FAILED;
		}

		return Result;
    }
    
    //else they do not want to wait for completion
    else*/
    {
        return (adi_dev_Write(DeviceHandle, BufferType, pBuffer));
    }
	
}

//-----------------------------------------------------------------------------
u32 send_PDMData(ADI_DEV_DEVICE_HANDLE DevHandle, u32 *p32Address, u32 u32Count)
{
	//Set the buffer formate, which will send to PC
	SendBuf.Data = (u32 *)p32Address;
	SendBuf.ElementCount = u32Count;
	SendBuf.ElementWidth = 4;
	SendBuf.CallbackParameter = NULL;
	SendBuf.ProcessedFlag = FALSE;
	SendBuf.ProcessedElementCount = 0;
	SendBuf.pNext = NULL;
	
	//Send the data to PC by USB
	return usb_Send(DevHandle, ADI_DEV_1D, (ADI_DEV_BUFFER *)&SendBuf, FALSE);
}

//----------------------------------------------------------------------------
void sendDatatoPC(void)
{	
	u8 i = 0;
	
	for(i = 0;i < 128; i++)
		TestBuf[i] = i;
	
	TestBuf[0] = CountFlag;
	usbSendBulkData((u8 *)&TestBuf[0], 512, false);
	
	//usbSendBulkData((u8 *)usbTestDataBuf, pUsbHeader->upByteCount, false);
	//*pPORTHIO_SET = PH2;
	
	
}


// SDP.c

#include "SDP_std_include.h"
#include <bfrom.h>
#include "SDP_ssl_init.h"
#include "SDP_id_eeprom.h"
#include "SDP_twi_low_level.h"

// optional modules - remove include of header to remove module from build
#include "SDP_i2c.h"
#include "SDP_gpio.h"
#include "SDP_sport.h"
#include "SDP_spi.h"
#include "SDP_async_parallel.h"
#include "SDP_ppi.h"
#include "SDP_user.h"
#include "SDP_ADMP521T.h"
#include "SDP_timer.h"
#include "SDP_OneWire.h"

//Base Commands (to be implemented in every Blackfin App)
#define ADI_SDP_CMD_GROUP_BASE				0xCA000000
#define ADI_SDP_CMD_FLASH_LED	 			0xCA000001
#define ADI_SDP_CMD_GET_FW_VERSION			0xCA000002
#define ADI_SDP_CMD_SDRAM_PROGRAM_BOOT		0xCA000003
#define ADI_SDP_CMD_READ_ID_EEPROMS			0xCA000004
#define ADI_SDP_CMD_RESET_BOARD				0xCA000005
#define ADI_SDP_CMD_READ_MAC_ADDRESS		0xCA000006
#define ADI_SDP_CMD_STOP_STREAM             0xCA000007

//#define SDRAM_BASE_ADDRESS ((u8*)0x1800000) 
#define SDRAM_BASE_ADDRESS ((u8*)0x010)

// global memory
ADI_DMA_MANAGER_HANDLE ghDmaManager;
ADI_DEV_MANAGER_HANDLE ghDevManager;
SDP_GENERAL_DATA_BUF gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF];
ADI_DEV_1D_BUFFER gGeneral1DBufArray[SDP_NUM_GEN_DATA_BUF];

u64 boardInitOtpWord;					   // Better option to make global than call OtpRead so many times
u8  latchValue;

static SDP_VERSION version = {	0,         // major rev
								1,         // minor rev
								570,       // host software rev
								293,       // blackfin software rev		
								__DATE__,  // date of build
								__TIME__,  // time of build
								0,         // reserved
								0 };       // flags
static void mainInit(void);
void closeMainInit(void);
static void processCommand(SDP_USB_HEADER *pUsbHeader);
static void processBaseCmd(SDP_USB_HEADER *pUsbHeader);

extern volatile bool g_bSuspendFlag;
static SDP_USB_HEADER usbHeader;
//-----------------------------------------------------------------------------
int main(void)
{
	
	unsigned long usbStatus;
	
	mainInit();
	
	//Init ADMP441
	//InitADMP441(false);
	
	//processOneWireCmd();
	
	flashLed();
	
	while(1)
	{
		usbGetUsbHeader(&usbHeader);
		processCommand(&usbHeader);		
		asm("nop;");
	}
	

}

//-----------------------------------------------------------------------------
static void mainInit(void)
{
	unsigned int result;
//	u64 boardInitOtpWord;

	*pPORTG_FER = 0x0000;			// To provide compatibility with previous releases
	
	// enable all async banks
	*pEBIU_AMGCTL = 0xFF;			// when ssl ebiu init not in use
	
	// read SDP board type for init from OTP
	result = bfrom_OtpRead( OTP_MAC_ADDRESS_PAGE,
	                        OTP_UPPER_HALF,
	                        &boardInitOtpWord );
	SDP_ASSERT(result)
	
	// initialise all system services, managers etc
	adiSslInit(boardInitOtpWord);
	
	usbInit(boardInitOtpWord);
	
	// enable PG0 as an output driving high (enable peripherals, no flash) or USB configured
	enablePG0();
	
	latchValue = 0x04; // USB Configured
	writeToSdpLatch(latchValue);
	
	twiInit();
		
	// Once the USB have been configured 
	sdpSslUpdateSdramPll();
	
	//*************************************************//
	//Initialise 4 GPIOs for OWCI CONTROL BOARD
	//OPEN
	*pPORTH_FER &= ~(PH1);
	*pPORTH_FER &= ~(PH2);
	*pPORTH_FER &= ~(PH3);
	*pPORTH_FER &= ~(PH6);
	
	//DISABLE INPUT
	*pPORTHIO_INEN &= ~PH1;
	*pPORTHIO_INEN &= ~PH2;
	*pPORTHIO_INEN &= ~PH3;
	*pPORTHIO_INEN &= ~PH6;
	
	//first set IO data to 1, and then switch it to GPO. 
	//*pPORTHIO_SET = PH1;
	//*pPORTHIO_SET = PH2;
	//*pPORTHIO_SET = PH3;
	//*pPORTHIO_SET = PH6;
	
	*pPORTHIO_CLEAR = PH1;		//SET LR AS OWCI PIN
	*pPORTHIO_SET = PH2;		//DATA BUFFERED
	*pPORTHIO_SET = PH3;		//CLK BUFFERED
	*pPORTHIO_SET = PH6;		//SET LR TO HIGH
	
	//*pPORTHIO_CLEAR = PH2;		//BYPASS DATA BUFFER
	//*pPORTHIO_CLEAR = PH3;		//BYPASS CLK BUFFER
	//*pPORTHIO_CLEAR = PH6;		//SET LR TO LOW
	
	//ENABLE OUTPUT
 	*pPORTHIO_DIR |= PH1;
	*pPORTHIO_DIR |= PH2;
	*pPORTHIO_DIR |= PH3;
	*pPORTHIO_DIR |= PH6;
	//***************************************************//
	
}

//-----------------------------------------------------------------------------
void closeMainInit(void)
{
	usbClose();
	adiSslTerminate();
}

//-----------------------------------------------------------------------------
static void processCommand(SDP_USB_HEADER *pUsbHeader)
{
	switch (pUsbHeader->cmd & 0xFFFFFF00)
	{
		case ADI_SDP_CMD_GROUP_ONEWIRE:					//For one wire main
			processOneWireCmd(pUsbHeader);
			break;
			
		case ADI_SDP_CMD_GROUP_ADMP521T:
			process521TCmd(pUsbHeader);		
			break;
			
		case ADI_SDP_CMD_GROUP_BASE:
			processBaseCmd(pUsbHeader);
			break;
			
		#ifdef ADI_SDP_CMD_GROUP_I2C
		case ADI_SDP_CMD_GROUP_I2C:
			processI2cCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_GPIO
		case ADI_SDP_CMD_GROUP_GPIO:
			processGpioCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_SPORT
		case ADI_SDP_CMD_GROUP_SPORT:
			processSportCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_SPI
		case ADI_SDP_CMD_GROUP_SPI:
			processSpiCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_ASYNC_PAR
		case ADI_SDP_CMD_GROUP_ASYNC_PAR:
			processAsyncParCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_PPI
		case ADI_SDP_CMD_GROUP_PPI:
			processPpiCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_USER
		case ADI_SDP_CMD_GROUP_USER:
			processUserCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_MANUFACTURE
		case ADI_SDP_CMD_GROUP_MANUFACTURE:
			processManufactureCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_RESERVED
		case ADI_SDP_CMD_GROUP_RESERVED:
			processReservedCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_CYCLONE
		case ADI_SDP_CMD_GROUP_CYCLONE:
			processFpgaCmd(pUsbHeader);
			break;
		#endif
		
		#ifdef ADI_SDP_CMD_GROUP_TIMER
		case ADI_SDP_CMD_GROUP_TIMER:
			processTimerCmd(pUsbHeader);
			break;
		#endif
		
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

//-----------------------------------------------------------------------------
static void processBaseCmd(SDP_USB_HEADER *pUsbHeader)
{
	int i;
	unsigned int result;
	
	switch (pUsbHeader->cmd)
	{
		case ADI_SDP_CMD_FLASH_LED:
			flashLed();
			break;
			
		case ADI_SDP_CMD_GET_FW_VERSION:
			#ifdef PSA_RELEASE_BUILD
				version.flags &= ~0x1;
			#else
				version.flags |= 0x1;
			#endif
			for (i=0; i<sizeof(version); i++)
			{
				gGeneralDataBuf[0].u8[i] = ((u8 *)&version)[i];
			}
			usbSendBulkData( &gGeneralDataBuf[0],
			                 pUsbHeader->upByteCount,
			                 true);
			break;
			
		case ADI_SDP_CMD_SDRAM_PROGRAM_BOOT:
			//usbGetBulkData( SDRAM_BASE_ADDRESS + pUsbHeader->paramArray[0],
			usbGetBulkData( SDRAM_BASE_ADDRESS + pUsbHeader->paramArray[0],
			                pUsbHeader->downByteCount,
			                true);
			// reboot from SDRAM if reboot flag (bit 1) is set
			if (pUsbHeader->paramArray[1] & 0x02)
			{
				closeMainInit();
				
				flashLed();
							
				// disable instruction cache
				*pIMEM_CONTROL = 0x00000001;
				ssync();
				
				// Info needed for initcode booting
				*pSWRST = 0x0005;
				
				// boot
				bfrom_MemBoot(SDRAM_BASE_ADDRESS,0,0,NULL);
			}	
			break;
			
		case ADI_SDP_CMD_READ_ID_EEPROMS:
			readIdEeproms();
			break;
			
		case ADI_SDP_CMD_RESET_BOARD:
			closeMainInit();
			// disable instruction cache
			*pIMEM_CONTROL = 0x00000001;
			ssync();
			// software reset
			*pSWRST = 0x0007;
			ssync();
			*pSWRST = 0x0000;
			ssync();
			asm("RAISE 1;");
			break;
			
		case ADI_SDP_CMD_READ_MAC_ADDRESS:
			result = bfrom_OtpRead( OTP_MAC_ADDRESS_PAGE,
			                        OTP_LOWER_HALF,
			                        &gGeneralDataBuf[0].u64[0] );
			SDP_ASSERT(result)
			usbSendBulkData( &gGeneralDataBuf[0],
			                 pUsbHeader->upByteCount,
			                 true );
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

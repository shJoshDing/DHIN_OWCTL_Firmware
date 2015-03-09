// SDP_spi_pseudo.c

#include "SDP_std_include.h"
#include "SDP_spi.h"

#define PIN_PG2_SCLK 	 0x0004
#define PIN_PG3_MISO_RDY 0x0008 
#define RXS				 0x0020
#define SPIF			 0x0001	
#define LSB_FIRST		 0x0200
#define CPHA			 0x0400
#define CPOL			 0x0800

extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

SDP_GENERAL_DATA_BUF *pOutData;
SDP_GENERAL_DATA_BUF *pInData;

static int spiWords;
static u8 wordOrder;
static u8 nBytesShift;
static u8 wordOrderInit;
static u16 spiCtlReg;
static u32 indexCount;
static u32 mosiState;
static u32 periodSel;
static u32 periodSelInit;
static u32 numBasicTransfers;
static bool spiDone;
static bool pDownByteCount;
static bool csEnableBetweenSamples;
static bool extendedWord;

	
volatile unsigned short *pSelPortSet;
volatile unsigned short *pSelPortClear;
static short selPin;

void initSelLine(u8 spiSel);
void waitRDY_BUSY(void);
void configSpi(u8 spiSel, u16 wordLength, bool cpha, bool cpol, u32 sclkFrequency, bool lsbFirst, bool spiMosi, u32 downByteCount, u32 upByteCount);

static void spiInit(u32 downByteCount, u32 upByteCount, u32 parameters[]);
static void spiExtendedTransfer(bool write, u32 tx_index, u32 rx_index, u32 bytes_to_transfer, bool longSize);
static void spiTransfer(int upByteCount, u16 wordLength);

void spiBusyRead(int numSamples, int numBytesSample);
void spiSingleConv(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesConfig, int numBytesSample);
void spiContConv(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesSample);
void spiContConv16Ad7175(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesSample);
void spiContRead(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesConfig, int numBytesSample);
void spiGenConv(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesConfig, int numBytesSample);

//EX_INTERRUPT_HANDLER(spi_status);
EX_INTERRUPT_HANDLER(spi_status_32bits);
EX_INTERRUPT_HANDLER(spi_status_16bits);
EX_INTERRUPT_HANDLER(spi_status_16bitsAD7175);
EX_INTERRUPT_HANDLER(spi_status_24bits);
EX_INTERRUPT_HANDLER(spi_status_8bits);
EX_INTERRUPT_HANDLER(spi_status_extended);

static u8 *dOut8;
static u8 *dIn8;
static u8 *dOutBase8;
static u8 *dInBase8;

u16 *dOut16;
u16 *dIn16;
u16 *dOutBase16;
u16 *dInBase16;

//-----------------------------------------------------------------------------
void processSpiCmd(SDP_USB_HEADER *pUsbHeader)
{
	int numSamples;
	bool cmdRepeat;
	int numReadCmd;
	int numBytesConfig;
	int numBytesSample;
	
	csEnableBetweenSamples = pUsbHeader->paramArray[16] ? true : false;
	
	switch (pUsbHeader->cmd)
	{
		case ADI_SDP_CMD_SPI_TRANSFER:
			if (pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount != 0)
			{
				extendedWord = false;	
				
				spiInit( pUsbHeader->downByteCount,
				         pUsbHeader->upByteCount,
				         pUsbHeader->paramArray );
				             
				spiTransfer(pUsbHeader->upByteCount, (u16)pUsbHeader->paramArray[2]);  				           
			}
			break;
		
		case ADI_SDP_CMD_SPI_EXTENDED_TRANSFER:
		
			if (pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount != 0)
			{
				spiInit( pUsbHeader->downByteCount,
				         pUsbHeader->upByteCount,
				         pUsbHeader->paramArray );
				             
				numSamples = (pUsbHeader->upByteCount/pUsbHeader->paramArray[14]);
				numBytesConfig = pUsbHeader->paramArray[12];
				numReadCmd = pUsbHeader->paramArray[13];
				numBytesSample = pUsbHeader->paramArray[14];
				cmdRepeat = pUsbHeader->paramArray[15];
							         
				switch(pUsbHeader->paramArray[11])		// What interface type is to be executed
				{
					case 0:								// Standard busy read
						extendedWord = false;	
						spiBusyRead(numSamples, numBytesSample);
						break;
						
					case 1:								// Single conversion mode
						extendedWord = true;	
						spiSingleConv(numSamples, cmdRepeat, numReadCmd, numBytesConfig, numBytesSample);
						break;

					case 2:								// Continuous conversion mode
						// AD7190 compatible
						extendedWord = false;	
						spiContConv(numSamples, cmdRepeat, numReadCmd, numBytesSample);												
						break;
						
					case 3:								// Continuous read mode
						extendedWord = false;					
						spiContRead(numSamples, cmdRepeat, numReadCmd, numBytesConfig, numBytesSample);
						break;	
					
					case 4: 
						extendedWord = true;			// Generic case was default case before
						spiGenConv(numSamples, cmdRepeat, numReadCmd, numBytesConfig, numBytesSample);
						break;
						
					case 5:								// Continuous conv mode 16 bits
						extendedWord = false;
						spiContConv16Ad7175(numSamples, cmdRepeat, numReadCmd, numBytesSample);						
						break;								
						
					default:
						unknownCommand(pUsbHeader);
						break;
				}
			}					
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

void spiBusyRead(int numSamples, int numBytesSample)
{
	int i = 0;
	int tx_index = 0;
	int rx_index = 0;

	register_handler(ik_ivg11, spi_status_8bits);	
	
	while(i < numSamples)
	{
		*pSelPortClear = selPin;
	
		waitRDY_BUSY();
	
		spiExtendedTransfer(false, 0, rx_index, numBytesSample,false);			// number de bytes por sample + status + crc
		
		*pSelPortSet = selPin;
		
		rx_index += numBytesSample;		
		i++;
	}
			
	usbSendBulkData(pInData, (numSamples * numBytesSample), true);
}


void spiSingleConv(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesConfig, int numBytesSample)
{
	int i = 0;
	int tx_index = 0;
	int rx_index = 0;
	
	register_handler(ik_ivg11, spi_status_8bits);	
		
	
	while(i < numSamples)
	{
		*pSelPortClear = selPin;
		
		if (numBytesConfig != 0)	// possible need for optimize
		{
			spiExtendedTransfer(true,tx_index,rx_index,numBytesConfig,false);		// bytes to configure
			tx_index += numBytesConfig;
		}
			
		waitRDY_BUSY();
		
		// command to read
	//	if(cmdRepeat | ((i == 0) & !cmdRepeat))									// By definiton the single conversion always repeat command after CS 
		if(cmdRepeat)									// By definiton the single conversion always repeat command after CS 
			spiExtendedTransfer(true,tx_index,rx_index,numReadCmd,false);		// number of commands to read
		
		// read 
		spiExtendedTransfer(false, 0, rx_index, numBytesSample, false);			// number de bytes por sample + status + crc
				
		*pSelPortSet = selPin;
		
		rx_index += numBytesSample;	
		tx_index = 0;	
		i++;		
	}
	
	usbSendBulkData(pInData, (numSamples * numBytesSample), true);
}			

void spiContRead(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesConfig, int numBytesSample)
{
	int i = 0;
	int tx_index = 0;
	int rx_index = 0;
	
	register_handler(ik_ivg11, spi_status_8bits);		
	
	*pSelPortClear = selPin;
		
	if (numBytesConfig != 0)
		spiExtendedTransfer(true,tx_index,rx_index,numBytesConfig, false);			// bytes to configure
			
	while(i < numSamples)
	{
		waitRDY_BUSY();
		
		// read 
		spiExtendedTransfer(false, 0, rx_index, numBytesSample,false);			// number de bytes por sample + status + crc
		
		rx_index += numBytesSample;				
		i++;
	}
	
	*pSelPortSet = selPin;
		
	usbSendBulkData(pInData, (numSamples * numBytesSample), true);
}

void spiContConv(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesSample)	// num samples is read lenght (numero de samples)
{
	int i = 0;
	int tx_index = 0;
	int rx_index = 0;

	register_handler(ik_ivg11, spi_status_8bits);	
	
	*pSelPortClear = selPin;
	
	while(i < numSamples)
	{
		waitRDY_BUSY();
	
		// command to read
		if(cmdRepeat | ((i == 0) & !cmdRepeat))						
			spiExtendedTransfer(true,tx_index,rx_index,numReadCmd,false);			// number of commands to read
		
		// read 
		spiExtendedTransfer(false, 0, rx_index, numBytesSample, false);			// number de bytes por sample + status + crc
		
		rx_index += numBytesSample;		
		i++;
	}
	
	*pSelPortSet = selPin;
			
	usbSendBulkData(pInData, (numSamples * numBytesSample), true);
}

void spiGenConv(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesConfig, int numBytesSample)
{
	int i = 0;
	int tx_index = 0;
	int rx_index = 0;
	u8 cur;
	u8 nd;
		
	register_handler(ik_ivg11, spi_status_8bits);
	
	while(i < numSamples)
	{
		
		*pSelPortClear = selPin;
		
		if ((numBytesConfig != 0) && (i == 0) ||
		    (numBytesConfig != 0) && (csEnableBetweenSamples == false))
		{
			spiExtendedTransfer(true,tx_index,rx_index,numBytesConfig,false);		// bytes to configure
			tx_index += numBytesConfig;
		}
			
		waitRDY_BUSY();
		
		if(numReadCmd != 0)
		{
			if ( cmdRepeat | ((i == 0) & !cmdRepeat) ) 						
				spiExtendedTransfer(true,tx_index,rx_index,numReadCmd,false);		// number of commands to read
		}
		
		// read 
		spiExtendedTransfer(false, 0, rx_index, numBytesSample,false);			// number de bytes por sample + status + crc
		
		if (!csEnableBetweenSamples) 
		{
			*pSelPortSet = selPin;		
			
			// Adding a few ns delay to keep the CS high			
			nd = 6;	// 140 ns
			cur = 0;
			while (cur < nd)
				cur++;
			
		}
		
		rx_index += numBytesSample;	
		tx_index = 0;	
		i++;
				
	}
	
	*pSelPortSet = selPin;
	
	usbSendBulkData(pInData, (numSamples * numBytesSample), true);
}


void calculateFrameRate(u32 frameFreq, u16 wordLength)
{
	if(frameFreq == 0)
	{
		periodSel = 0;
		periodSelInit = 0;
	}
	else
	{
		periodSel = (CORE_CLK_HZ/frameFreq) - 30;	
		periodSelInit = (wordLength == 24) ? periodSel - 130 : ((wordLength == 32) ? periodSel - 170 : periodSel);
	}
	
//	Not sure about periodSel for 40 and 48
}



static void spiInit(u32 downByteCount, u32 upByteCount, u32 parameters[])
{
	u32 i;  
	u16 j;
	volatile unsigned short *pSelPortToggle;
	u32 frameCount;
	u8 spiSel;
	u32 frameFreq; 
	u16 wordLength;
		
	bool cpha = (bool)parameters[4];		
	bool cpol = (bool)parameters[5];		
	bool lsbFirst = (bool) parameters[3];	
	u32 sclkFrequency = parameters[0];		
   	
	spiSel = parameters[1];
	wordLength = (u16)parameters[2];
	frameFreq = parameters[9];
	csEnableBetweenSamples = parameters[16] ? true : false;   
	
	spiDone = false;
	spiWords = 0;
		
	if (cpol)
	{
		// Clock manually tight high (PG2 as a SPI_SCLK)
		*pPORTG_FER &= ~PIN_PG2_SCLK;
		*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
		*pPORTGIO_SET = PIN_PG2_SCLK;
		*pPORTGIO_DIR |= PIN_PG2_SCLK;		
	}						
			
	pOutData = &gGeneralDataBuf[0];
	pInData = &gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF/2];

	*pTSCALE = 0;
	
	// Calculate of Frame Rate
	calculateFrameRate(frameFreq, wordLength);	
	
	//Config SPI registers
	configSpi(spiSel,wordLength,cpha,cpol,sclkFrequency,lsbFirst,parameters[10],downByteCount,upByteCount);	
}

static void spiExtendedTransfer(bool write, u32 tx_index, u32 rx_index, u32 bytes_to_transfer,bool longSize)
{
	u32 numWords;
	
	spiWords = 0;
	spiDone = false;
	
	if (!write)
		pDownByteCount = false;
	else
		pDownByteCount = true;									// It has been added the 1st July to fix the Single Conv
	
	*pPORTG_FER |= PIN_PG2_SCLK;								// Enable peripheral pin for SPI: MISO,MOSI,CLK
	*pSPI_CTL = spiCtlReg;
	
	if (longSize)											// Only used in the case of AD7175	(full performance)
	{
		numWords = bytes_to_transfer/2;
		numBasicTransfers = numWords;
	
		// Pointers ready to access to data
		dOut16 = &pOutData->u16[tx_index];	
		dIn16 = &pInData->u16[rx_index];
		dOutBase16 = &pOutData->u16[0];
		dInBase16 = &pInData->u16[0];
		
		*pSPI_TDBR = write ? *dOut16 : mosiState;
		*pSelPortClear = selPin;									// Optimize timing		
		*pTPERIOD = periodSelInit;
		*pTCNTL = CORE_TIMER_ENABLE_BITS;
		*dIn16 = *pSPI_RDBR;
	}
	else
	{
		numWords = bytes_to_transfer;
		numBasicTransfers = numWords;
	
		// Pointers ready to access to data
		dOut8 = &pOutData->u8[tx_index];
		dIn8 = &pInData->u8[rx_index];
		dOutBase8 = &pOutData->u8[0];
		dInBase8 = &pInData->u8[0];
		
		*pSPI_TDBR = write ? *dOut8 : mosiState;
		*pSelPortClear = selPin;									// Optimize timing		
		*pTPERIOD = periodSelInit;
		*pTCNTL = CORE_TIMER_ENABLE_BITS;
		*dIn8 = *pSPI_RDBR;
	}
	
	while (!spiDone);											// Wait until SPI is finished
		
}

static void spiTransfer(int upByteCount, u16 wordLength)
{
//	ver como eliminar variables globales en la medida de lo posible
	
	if((wordLength==16) | (wordLength==32))
	{
		// Pointers ready to access to data
		dOut16 = &pOutData->u16[wordOrder];
		dIn16 = &pInData->u16[wordOrder];							
		dOutBase16 = &pOutData->u16[0];			
		dInBase16 = &pInData->u16[0];
		
		*pSPI_TDBR = pDownByteCount ? *dOut16 : mosiState;
		*pSelPortClear = selPin;								// Optimize timing	
		*pTPERIOD = periodSelInit;
		*pTCNTL = CORE_TIMER_ENABLE_BITS;						// enable timer for frame rate
		*dIn16 = *pSPI_RDBR;		
		
		if(wordLength == 16)
			register_handler(ik_ivg11, spi_status_16bits);	
		else
		{
			nBytesShift = 2;
			register_handler(ik_ivg11, spi_status_32bits);		
		}
	}
	else														// wordLength==8,24, 40,48	
	{
		// Pointers ready to access to data
		dOut8 = &pOutData->u8[wordOrder];
		dIn8 = &pInData->u8[wordOrder];
		dOutBase8 = &pOutData->u8[0];
		dInBase8 = &pInData->u8[0];
		
		*pSPI_TDBR = pDownByteCount ? *dOut8 : mosiState;
		*pSelPortClear = selPin;								// Optimize timing		
		*pTPERIOD = periodSelInit;
		*pTCNTL = CORE_TIMER_ENABLE_BITS;
		*dIn8 = *pSPI_RDBR;
	
			
		if(wordLength == 8)
			register_handler(ik_ivg11, spi_status_8bits);	
		else if(wordLength == 24)
		{
			nBytesShift = 4;
			register_handler(ik_ivg11, spi_status_24bits);
		}
		else // 40 and 48? Read in base of 8?
		{
			wordOrder = wordLength/8;							// wordOrder will have a different functionality for 40 and 48 bits
			wordOrderInit = wordOrder;
			nBytesShift = 1;
			register_handler(ik_ivg11, spi_status_extended);
		}
				
	}
	
	while (!spiDone);											// Wait until SPI is finished

	if (upByteCount != 0)
		usbSendBulkData(pInData, upByteCount, true);
	
}


// ISR for the 8 bits SPI
EX_INTERRUPT_HANDLER(spi_status_8bits)
{
	
	if (*pSPI_STAT & SPIF)			
	{
		while((*pSPI_STAT & RXS) != RXS);
		
		if(!csEnableBetweenSamples & !extendedWord)
			*pSelPortSet = selPin;								// Set spi sel pin
				
		spiWords++;
							
		if ( spiWords == numBasicTransfers )
		{
			if(*pSPI_CTL & CPOL)				
			{
				*pPORTG_FER &= ~PIN_PG2_SCLK;
				*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
				*pPORTGIO_SET = PIN_PG2_SCLK;
				*pPORTGIO_DIR |= PIN_PG2_SCLK;		
			}
			
			if(!csEnableBetweenSamples & !extendedWord)
				*pSelPortSet = selPin;							// Set spi sel pin
					
			*pSPI_CTL = 0;									
//			*pSIC_IMASK0 &= 0xFFDFFFFF;
			spiDone = true;
			*dIn8 = *pSPI_RDBR;			
		}
		else
		{
			dOut8++;
			*pSPI_TDBR = pDownByteCount ? *dOut8 : mosiState;
						
			if(!csEnableBetweenSamples & !extendedWord)			// Keep an eye, in cnv mode with cs not enabled
			{													// we might have an issue, change it!
				while (*pTCOUNT > 0 );
				
				*pSelPortClear = selPin;						// Clear spi sel pin
				
				*pTPERIOD = periodSel;							
				*pTCNTL = CORE_TIMER_ENABLE_BITS;				
			}
			
			*dIn8 = *pSPI_RDBR;
			dIn8++;
			
			while((*pSPI_STAT & RXS) != 0x0);
		}
	}
}


// ISR for the 16 bits SPI
EX_INTERRUPT_HANDLER(spi_status_16bits)
{
	
	if (*pSPI_STAT & SPIF)			
	{
		while((*pSPI_STAT & RXS) != RXS);
		
		*pSelPortSet = selPin;									// Set spi sel pin
		spiWords++;
							
		if ( spiWords == numBasicTransfers )
		{
			if(*pSPI_CTL & CPOL)				
			{
				*pPORTG_FER &= ~PIN_PG2_SCLK;
				*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
				*pPORTGIO_SET = PIN_PG2_SCLK;
				*pPORTGIO_DIR |= PIN_PG2_SCLK;		
			}
			
			*pSPI_CTL = 0;
			*pSIC_IMASK0 &= 0xFFDFFFFF;
			spiDone = true;
			*dIn16 = *pSPI_RDBR;
		}
		else
		{
			dOut16++;
			*pSPI_TDBR = pDownByteCount ? *dOut16 : mosiState;
			while (*pTCOUNT > 0 );
			*pSelPortClear = selPin;							// Clear spi sel pin
			*pTPERIOD = periodSel;
			*pTCNTL = CORE_TIMER_ENABLE_BITS;
			*dIn16 = *pSPI_RDBR;
			dIn16++;
			
			while((*pSPI_STAT & RXS) != 0x0);
		}
	}

}

// ISR for the 24 bits SPI
EX_INTERRUPT_HANDLER(spi_status_24bits)
{
	
	if (*pSPI_STAT & SPIF)			
	{	
		
		while((*pSPI_STAT & RXS) != RXS);
		
		dIn8 = dInBase8 + wordOrder + indexCount;
						
		if (wordOrder == 0)
		{
			wordOrder = wordOrderInit;
			indexCount += nBytesShift;
			
			*pSelPortSet = selPin;								// Set spi sel pin
				
			while (*pTCOUNT > 0 );
			*pTPERIOD = periodSel;
			*pTCNTL = CORE_TIMER_ENABLE_BITS;					
		}
		else
			wordOrder--; 			
		
		spiWords++;
							
		if ( spiWords == numBasicTransfers )
		{
			if(*pSPI_CTL & CPOL)				
			{
				*pPORTG_FER &= ~PIN_PG2_SCLK;
				*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
				*pPORTGIO_SET = PIN_PG2_SCLK;
				*pPORTGIO_DIR |= PIN_PG2_SCLK;		
			}
			*pSPI_CTL = 0;
			*pSIC_IMASK0 &= 0xFFDFFFFF;
			spiDone = true;
			*dIn8 = *pSPI_RDBR;
		}
		else
		{
			dOut8 = dOutBase8 + indexCount + wordOrder;
			*pSPI_TDBR = pDownByteCount ? *dOut8 : mosiState;
			*pSelPortClear = selPin;							// Clear spi sel pin
			ssync();
			*dIn8 = *pSPI_RDBR;
			
			while((*pSPI_STAT & RXS) != 0x0);
		}
	}

}

// ISR for the 32 bits SPI
EX_INTERRUPT_HANDLER(spi_status_32bits)
{
	
	if (*pSPI_STAT & SPIF)			
	{
		while((*pSPI_STAT & RXS) != RXS);
		
		dIn16 = dInBase16 + wordOrder + indexCount;
				
		if (wordOrder == 0)
		{
			*pSelPortSet = selPin;								// Set spi sel pin
			wordOrder = wordOrderInit;
			indexCount += nBytesShift;
			
			while (*pTCOUNT > 0 );
			*pTPERIOD = periodSel;
			*pTCNTL = CORE_TIMER_ENABLE_BITS;
		}
		else
			wordOrder--; 			
		
		spiWords++;
							
		if ( spiWords == numBasicTransfers )
		{
			if(*pSPI_CTL & CPOL)				
			{
				*pPORTG_FER &= ~PIN_PG2_SCLK;
				*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
				*pPORTGIO_SET = PIN_PG2_SCLK;
				*pPORTGIO_DIR |= PIN_PG2_SCLK;		
			}
			
			*pSPI_CTL = 0;
			*pSIC_IMASK0 &= 0xFFDFFFFF;
			spiDone = true;
			*dIn16 = *pSPI_RDBR;
		}
		else
		{
			dOut16 = dOutBase16 + indexCount + wordOrder;
			*pSPI_TDBR = pDownByteCount ? *dOut16 : mosiState;
			*pSelPortClear = selPin;							// Clear spi sel pin
			*dIn16 = *pSPI_RDBR;
			
			while((*pSPI_STAT & RXS) != 0x0);
		}
	}

}


// ISR for the 40 and 48 bits SPI
EX_INTERRUPT_HANDLER(spi_status_extended)
{
	
	if (*pSPI_STAT & SPIF)			
	{	
		
		while((*pSPI_STAT & RXS) != RXS);
		
		if (wordOrder == 1) 
		{
			*pSelPortSet = selPin;								// Set spi sel pin
			
			wordOrder = wordOrderInit;
			indexCount += nBytesShift;
				
			while (*pTCOUNT > 0 );
			*pTPERIOD = periodSel;
			*pTCNTL = CORE_TIMER_ENABLE_BITS;					
		}
		else
			wordOrder--; 			
		
		spiWords++;
							
		if ( spiWords == numBasicTransfers )
		{
			if(*pSPI_CTL & CPOL)				
			{
				*pPORTG_FER &= ~PIN_PG2_SCLK;
				*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
				*pPORTGIO_SET = PIN_PG2_SCLK;
				*pPORTGIO_DIR |= PIN_PG2_SCLK;		
			}
			
			*pSPI_CTL = 0;
			*pSIC_IMASK0 &= 0xFFDFFFFF;
			spiDone = true;
			*dIn8 = *pSPI_RDBR;
		}
		else
		{
			dOut8++;
			*pSPI_TDBR = pDownByteCount ? *dOut8 : mosiState;
			*pSelPortClear = selPin;							// Clear spi sel pin
			ssync();
			*dIn8 = *pSPI_RDBR;
			dIn8++;
			
			while((*pSPI_STAT & RXS) != 0x0);
		}
	}

}

void waitRDY_BUSY(void)
{
	// Need to wait for busy
	*pPORTG_FER &= ~PIN_PG3_MISO_RDY; 							// PG3 (MISO) not a peripheral pin
	*pPORTGIO_DIR &= ~PIN_PG3_MISO_RDY;
	*pPORTGIO_INEN |= PIN_PG3_MISO_RDY;
	ssync();
							
	while((*pPORTGIO & PIN_PG3_MISO_RDY)) {;}					// wait until busy or rdy is gone
			
	*pPORTG_FER |= PIN_PG3_MISO_RDY; 							// PG3 (MISO) is a peripheral pin
}


void configSpi(u8 spiSel, u16 wordLength, bool cpha, bool cpol, u32 sclkFrequency, bool lsbFirst, bool spiMosi, u32 downByteCount, u32 upByteCount )
{
	u32 numWords;
	
	initSelLine(spiSel);
	
	// SPI Bit Rate
	*pSPI_BAUD = SYSTEM_CLK_HZ/(2 * sclkFrequency);				// SCLK/2 xSPI_BAUD (Max frecuency)
	
	// SPI_CTL configuration
	if ((wordLength == 16) || (wordLength == 32))
		*pSPI_CTL |= 0x0100;
	else														// 8|24|40|48
		*pSPI_CTL &= 0xFEFF;
	
	if (lsbFirst)
		*pSPI_CTL |= LSB_FIRST;
	else
		*pSPI_CTL &= ~LSB_FIRST;				
	 
	if (cpha)
		*pSPI_CTL |= CPHA;
	else
		*pSPI_CTL &= ~CPHA;						
		
	if (cpol)	
		*pSPI_CTL |= CPOL;
	else		
		*pSPI_CTL &= ~CPOL;						
		
	*pPORTG_FER |= 0x001C;										// Enable peripheral pin for SPI: MISO,MOSI,CLK
	*pPORTG_MUX	|= 0x0002;										// SSEL1 but not SS
				
	*pSPI_CTL |= 0x5028;	
	spiCtlReg = *pSPI_CTL;
	
	// Default value for the MOSI line	
	mosiState = spiMosi ? 0xFFFFFFFFU : 0x00000000U;
	
	// Initial setup for counting the SPI word order transaction
	wordOrder = (wordLength == 24) ? 2 : ((wordLength == 32) ? 1 : 0);	// Memory location
	wordOrderInit = wordOrder;
	indexCount = 0;	
	
	// Info to manage the SPI transactions
	// numWords is according to the parameters Bytes and wordLength
	// numBasicTransfers are the transfers of 8 or 16 bits needed
	if (downByteCount == 0)
	{
		pDownByteCount = false;
		numWords = (wordLength == 24) ? (8 * upByteCount / 32): (8 * upByteCount / wordLength);
		numBasicTransfers = (wordLength == 24) ? (3 * numWords) : ((wordLength == 32) ? (2 * numWords) : (((wordLength == 8) | (wordLength == 16)) ? numWords : (numWords * wordLength / 8)));
	}
	else
	{
		pDownByteCount = true;
		usbGetBulkData(pOutData, downByteCount, true);
		numWords = (wordLength == 24) ? (8 * downByteCount / 32): (8 * downByteCount / wordLength);
		numBasicTransfers = (wordLength == 24) ? (3 * numWords) : ((wordLength == 32) ? (2 * numWords) : (((wordLength == 8) | (wordLength == 16)) ? numWords : (numWords * wordLength / 8)));
	}		
	
	// Interrupt assignment
	// SPI-DMA7
	*pSIC_IAR2 = 0x33432222;									// move to ivg11	
	*pSIC_IMASK0 |= 0x00200000;									// SPI status interrupt
		
}


void initSelLine(u8 spiSel)
{
	// All the CS lines are enabled
	
	// SPISEL1 (PG1)
	selPin = 0x0002;
	*pPORTG_FER &= ~selPin;
	*pPORTGIO_INEN &= ~selPin;
	*pPORTGIO_SET = selPin;
	*pPORTGIO_DIR |= selPin;
	
	// SPISEL2 (ConA-C, PF12)
	selPin = 0x1000;
	*pPORTF_FER &= ~selPin;
	*pPORTFIO_INEN &= ~selPin;
	*pPORTFIO_SET = selPin;
	*pPORTFIO_DIR |= selPin;

	// SPISEL3 (ConB-C, PF13)
	selPin = 0x2000;
	*pPORTF_FER &= ~selPin;
	*pPORTFIO_INEN &= ~selPin;
	*pPORTFIO_SET = selPin;
	*pPORTFIO_DIR |= selPin;
	
	// SPISEL4 (ConA-A, PH8)
	selPin = 0x0100;
	*pPORTH_FER &= ~selPin;
	*pPORTHIO_INEN &= ~selPin;
	*pPORTHIO_SET = selPin;
	*pPORTHIO_DIR |= selPin;
	
	// SPISEL5 (ConB-A, PH9)
	selPin = 0x0200;
	*pPORTH_FER &= ~selPin;
	*pPORTHIO_INEN &= ~selPin;
	*pPORTHIO_SET = selPin;
	*pPORTHIO_DIR |= selPin;
	
	// SPISEL6 (ConA-B, PF9)
	selPin = 0x0200;
	*pPORTF_FER &= ~selPin;
	*pPORTFIO_INEN &= ~selPin;
	*pPORTFIO_SET = selPin;
	*pPORTFIO_DIR |= selPin;
	
	// SPISEL7 (ConB-B, PF10)
	selPin = 0x0400;
	*pPORTF_FER &= ~selPin;
	*pPORTFIO_INEN &= ~selPin;
	*pPORTFIO_SET = selPin;
	*pPORTFIO_DIR |= selPin;
	
	
	switch (spiSel)
	{
		case 0x00000002: // SPISEL1 (PG1)
			selPin = 0x0002;
			pSelPortSet = pPORTGIO_SET;
			pSelPortClear = pPORTGIO_CLEAR;
			break;

		case 0x00000004: // SPISEL2 (ConA-C, PF12)
			selPin = 0x1000;
			pSelPortSet = pPORTFIO_SET;
			pSelPortClear = pPORTFIO_CLEAR;
			break;

		case 0x00000008: // SPISEL3 (ConB-C, PF13)
			selPin = 0x2000;
			pSelPortSet = pPORTFIO_SET;
			pSelPortClear = pPORTFIO_CLEAR;
			break;

		case 0x00000010: // SPISEL4 (ConA-A, PH8)
			selPin = 0x0100;
			pSelPortSet = pPORTHIO_SET;
			pSelPortClear = pPORTHIO_CLEAR;
			break;

		case 0x00000020: // SPISEL5 (ConB-A, PH9)
			selPin = 0x0200;
			pSelPortSet = pPORTHIO_SET;
			pSelPortClear = pPORTHIO_CLEAR;
			break;

		case 0x00000040: // SPISEL6 (ConA-B, PF9)
			selPin = 0x0200;
			pSelPortSet = pPORTFIO_SET;
			pSelPortClear = pPORTFIO_CLEAR;
			break;

		case 0x00000080: // SPISEL7 (ConB-B, PF10)
			selPin = 0x0400;
			pSelPortSet = pPORTFIO_SET;
			pSelPortClear = pPORTFIO_CLEAR;
			break;
	}			
}


/*--------------------------------------------------
	CUSTOMIZATION FOR THE AD7175 SILICON 
	WORKING AT THE HIGHEST PERFORMANCE 
--------------------------------------------------*/

void spiContConv16Ad7175(int numSamples, bool cmdRepeat, int numReadCmd, int numBytesSample)	// num samples is read lenght (numero de samples)
{
	int i,x;
	int tx_index = 0;
	int rx_index = 0;
	int total_num_to_reverse = 0;
	u8 dataAuxByte;
	SDP_GENERAL_DATA_BUF *dataAux;
	
	dataAux = &gGeneralDataBuf[0];
		
	register_handler(ik_ivg11, spi_status_16bitsAD7175);
	
	total_num_to_reverse = numReadCmd + numBytesSample;
	
	i = 0;
	// need to swap 8 bits to send 16 bits.	
	while(i < total_num_to_reverse)
	{
		dataAuxByte = (u8)(dataAux->u8[i]);	
		dataAux->u8[i] = dataAux->u8[i+1];
		dataAux->u8[i+1] = dataAuxByte;
	
		i += 2; 	
	}
	i = 0;
		
	*pSelPortClear = selPin;
	
	while(i < numSamples)
	{
		waitRDY_BUSY();
	
		// read 
		spiExtendedTransfer(true, 0, rx_index, numReadCmd + numBytesSample,true);			// number de bytes por sample + status + crc
		
		rx_index += (numBytesSample + numReadCmd)/2;		
		i++;
	}
	
	*pSelPortSet = selPin;
	
	i = 0;
	x = 1;
	
	// need to swap 16 bits to send 8 bits.	
	dataAux = &gGeneralDataBuf[32];
	
	while(i < numSamples)
	{
		dataAux->u8[i] = dataAux->u8[i+x-1];
		dataAux->u8[i+1] = dataAux->u8[i+x+2];
		dataAux->u8[i+2] = dataAux->u8[i+x+1];
		
		i += 3;	
		x += 1;	
	}
				
	usbSendBulkData(pInData, (numSamples * numBytesSample), true);
}


// ISR for the 16 bits SPI
EX_INTERRUPT_HANDLER(spi_status_16bitsAD7175)
{
	
	if (*pSPI_STAT & SPIF)			
	{
		while((*pSPI_STAT & RXS) != RXS);
		
		spiWords++;
							
		if ( spiWords == numBasicTransfers )
		{
			if(*pSPI_CTL & CPOL)				
			{
				*pPORTG_FER &= ~PIN_PG2_SCLK;
				*pPORTGIO_INEN &= ~PIN_PG2_SCLK;
				*pPORTGIO_SET = PIN_PG2_SCLK;
				*pPORTGIO_DIR |= PIN_PG2_SCLK;		
			}
			
			*pSPI_CTL = 0;
			spiDone = true;
			*dIn16 = *pSPI_RDBR;
		}
		else
		{
			dOut16++;
			*pSPI_TDBR = pDownByteCount ? *dOut16 : mosiState;
			*dIn16 = *pSPI_RDBR;
			dIn16++;
			
			while((*pSPI_STAT & RXS) != 0x0);
		}
	}

}


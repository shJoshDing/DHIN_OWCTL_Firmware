// SDP_spi_pseudo.c

#include "SDP_std_include.h"
#include "SDP_spi.h"

#define SPI_CLK_PIN 0x0004
#define SPI_MOSI_PIN 0x0010
#define SPI_MISO_PIN 0x0008

extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

static void spiTransfer(u32 downByteCount, u32 upByteCount, u32 parameters[]);

//-----------------------------------------------------------------------------
void processSpiCmd(SDP_USB_HEADER *pUsbHeader)
{
	switch (pUsbHeader->cmd)
	{
		case ADI_SDP_CMD_SPI_TRANSFER:
			if (pUsbHeader->downByteCount != 0 || pUsbHeader->upByteCount != 0)
			{
				spiTransfer( pUsbHeader->downByteCount,
				             pUsbHeader->upByteCount,
				             pUsbHeader->paramArray );
			}
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

//-----------------------------------------------------------------------------
static void spiTransfer(u32 downByteCount, u32 upByteCount, u32 parameters[])
{
	u32 i;
	u16 j;
	volatile unsigned short *pSelPortToggle;
	short selPin;
	SDP_GENERAL_DATA_BUF *pOutData;
	SDP_GENERAL_DATA_BUF *pInData;
	u8 inDataWordByteSize;
	u8 outDataWordByteSize;
	u32 mosiState;
	u32 msbMask;
	u32 validDataMask;
	u32 numTransfers;
	s32 sclkCount;
	u32 frameCount;
	u32 dataReg;
	
	u32 sclkFrequency = parameters[0];
   	u8 spiSel = parameters[1];
	u16 wordLength = (u16)parameters[2];
	bool cpha = (bool)parameters[4];
	bool cpol = (bool)parameters[5];
	
	// lsbFirst = parameters[3];
	// openDrainMaster = parameters[6];
	// littleEndian = parameters[7];
	// wordsPerFrame = parameters[8];
	// frameFreq = parameters[9];
	
	pOutData = &gGeneralDataBuf[0];
	pInData = &gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF/2];
	
	// clk and mosi as outputs and miso as input
	*pPORTG_FER &= ~(SPI_CLK_PIN | SPI_MOSI_PIN | SPI_MISO_PIN);
	*pPORTGIO_INEN &= ~(SPI_CLK_PIN | SPI_MOSI_PIN);
	*pPORTGIO_DIR |= SPI_CLK_PIN | SPI_MOSI_PIN;
	*pPORTGIO_DIR &= ~SPI_MISO_PIN;
	*pPORTGIO_INEN |= SPI_MISO_PIN;
	
	switch (spiSel)
	{
		case 0x00000002: // SPISEL1 (PG1)
			selPin = 0x0002;
			*pPORTG_FER &= ~selPin;
			*pPORTGIO_INEN &= ~selPin;
			*pPORTGIO_SET = selPin;
			*pPORTGIO_DIR |= selPin;
			pSelPortToggle = pPORTGIO_TOGGLE;
			break;

		case 0x00000004: // SPISEL2 (ConA-C, PF12)
			selPin = 0x1000;
			*pPORTF_FER &= ~selPin;
			*pPORTFIO_INEN &= ~selPin;
			*pPORTFIO_SET = selPin;
			*pPORTFIO_DIR |= selPin;
			pSelPortToggle = pPORTFIO_TOGGLE;
			break;

		case 0x00000008: // SPISEL3 (ConB-C, PF13)
			selPin = 0x2000;
			*pPORTF_FER &= ~selPin;
			*pPORTFIO_INEN &= ~selPin;
			*pPORTFIO_SET = selPin;
			*pPORTFIO_DIR |= selPin;
			pSelPortToggle = pPORTFIO_TOGGLE;
			break;

		case 0x00000010: // SPISEL4 (ConA-A, PH8)
			selPin = 0x0100;
			*pPORTH_FER &= ~selPin;
			*pPORTHIO_INEN &= ~selPin;
			*pPORTHIO_SET = selPin;
			*pPORTHIO_DIR |= selPin;
			pSelPortToggle = pPORTHIO_TOGGLE;
			break;

		case 0x00000020: // SPISEL5 (ConB-A, PH9)
			selPin = 0x0200;
			*pPORTH_FER &= ~selPin;
			*pPORTHIO_INEN &= ~selPin;
			*pPORTHIO_SET = selPin;
			*pPORTHIO_DIR |= selPin;
			pSelPortToggle = pPORTHIO_TOGGLE;
			break;

		case 0x00000040: // SPISEL6 (ConA-B, PF9)
			selPin = 0x0200;
			*pPORTF_FER &= ~selPin;
			*pPORTFIO_INEN &= ~selPin;
			*pPORTFIO_SET = selPin;
			*pPORTFIO_DIR |= selPin;
			pSelPortToggle = pPORTFIO_TOGGLE;
			break;

		case 0x00000080: // SPISEL7 (ConB-B, PF10)
			selPin = 0x0400;
			*pPORTF_FER &= ~selPin;
			*pPORTFIO_INEN &= ~selPin;
			*pPORTFIO_SET = selPin;
			*pPORTFIO_DIR |= selPin;
			pSelPortToggle = pPORTFIO_TOGGLE;
			break;
	}
	
	if (cpol)
		*pPORTGIO_SET = SPI_CLK_PIN;
	else
		*pPORTGIO_CLEAR = SPI_CLK_PIN;
	
	inDataWordByteSize = (wordLength<=8)? 1 : (wordLength<=16)? 2 : 4;
	
	msbMask = 1 << (wordLength-1);
	validDataMask = ~( (~1U) << (wordLength-1) );
	
	if (downByteCount == 0)
	{
		mosiState = parameters[10] ? 0xFFFFFFFFU : 0x00000000U;
		outDataWordByteSize = 0;
		numTransfers = upByteCount / inDataWordByteSize;
	}
	else
	{
		usbGetBulkData(pOutData, downByteCount, true);
		outDataWordByteSize = inDataWordByteSize;
		if(upByteCount == 0 || downByteCount < upByteCount)
			numTransfers = downByteCount / inDataWordByteSize;
		else
			numTransfers = upByteCount / inDataWordByteSize;
	}
	
	*pTSCALE = 0x0;
	sclkCount = (CORE_CLK_HZ/sclkFrequency)-36;
	if (sclkCount < 20)
		sclkCount = 20;
	frameCount = sclkCount;
	
	if (cpha)
	{
		for (i=0; i<numTransfers; i++)
		{
			// LOAD DATA (laod Tx word)
			switch(outDataWordByteSize)
			{
				case 0: dataReg = mosiState; break;
				case 1: dataReg = pOutData->u8[i]; break;
				case 2: dataReg = pOutData->u16[i]; break;
				default: dataReg = pOutData->u32[i]; break;
			}
			
			// SEL (toggle spi sel pin)
			*pSelPortToggle = selPin;
			
			for (j=0; j<wordLength; j++)
			{
				*pTPERIOD = sclkCount;
				*pTCNTL = CORE_TIMER_ENABLE_BITS;
				
				// WAIT 1
				while (*pTCOUNT > ((sclkCount/2) + 10)); 
				// (Needed tweak factor to ensure 50% duty cycle)
				
				// CLK 1 (toggle spi clk pin)
				*pPORTGIO_TOGGLE = SPI_CLK_PIN;
				
				// DATA OUT (output tx bit on mosi pin)
				if (dataReg & msbMask)
					*pPORTGIO_SET = SPI_MOSI_PIN;
				else
					*pPORTGIO_CLEAR = SPI_MOSI_PIN;
				// shift data reg
				dataReg <<= 1;
				
				// WAIT 2
				while (*pTCOUNT > 0 );
				
				// DATA IN (sample rx bit on miso pin)
				if (*pPORTGIO & SPI_MISO_PIN)
					dataReg |= 0x0001;
				
				// CLK 2 (toggle spi clk pin)
				*pPORTGIO_TOGGLE = SPI_CLK_PIN;
			}
			
			// WAIT
			*pTPERIOD = sclkCount/2; 
			*pTCNTL = CORE_TIMER_ENABLE_BITS;
			while (*pTCOUNT > 0 );
			
			// SEL (toggle spi sel pin)
			*pSelPortToggle = selPin;
			
			// SAVE DATA (clear bits outside word size and save Rx word)
			dataReg &= validDataMask;
			switch (inDataWordByteSize)
			{
				case 1: pInData->u8[i] = dataReg;
				case 2: pInData->u16[i] = dataReg;
				default: pInData->u32[i] = dataReg;
			}
			
			// FRAME WAIT
			*pTPERIOD = frameCount;
			*pTCNTL = CORE_TIMER_ENABLE_BITS;
			while (*pTCOUNT > 0 );
		}
	}
	else
	{
		for (i=0; i<numTransfers; i++)
		{
			// LOAD DATA (laod Tx word)
			switch(outDataWordByteSize)
			{
				case 0: dataReg = mosiState; break;
				case 1: dataReg = pOutData->u8[i]; break;
				case 2: dataReg = pOutData->u16[i]; break;
				default: dataReg = pOutData->u32[i]; break;
			}
			
			// SEL (toggle spi sel pin)
			*pSelPortToggle = selPin;
			
			for (j=0; j<wordLength; j++)
			{
				*pTPERIOD = sclkCount;
				*pTCNTL = CORE_TIMER_ENABLE_BITS;
				
				// DATA OUT (output tx bit on mosi pin)
				if (dataReg & msbMask)
					*pPORTGIO_SET = SPI_MOSI_PIN;
				else
					*pPORTGIO_CLEAR = SPI_MOSI_PIN;
				// shift data reg
				dataReg <<= 1;
				
				// WAIT 1
				while (*pTCOUNT > ((sclkCount/2) + 10)); 
				// (Needed tweak factor to ensure 50% duty cycle)
				
				// CLK 1 (toggle spi clk pin)
				*pPORTGIO_TOGGLE = SPI_CLK_PIN;
				
				// DATA IN (sample rx bit on miso pin)
				if (*pPORTGIO & SPI_MISO_PIN)
					dataReg |= 0x0001;
				
				// WAIT 2
				while (*pTCOUNT > 0 );
				
				// CLK 2 (toggle spi clk pin)
				*pPORTGIO_TOGGLE = SPI_CLK_PIN;
			}
			
			// WAIT
			*pTPERIOD = sclkCount/2; 
			*pTCNTL = CORE_TIMER_ENABLE_BITS;
			while (*pTCOUNT > 0 );
			
			// SEL (toggle spi sel pin)
			*pSelPortToggle = selPin;
			
			// SAVE DATA (clear bits outside word size and save Rx word)
			dataReg &= validDataMask;
			switch (inDataWordByteSize)
			{
				case 1: pInData->u8[i] = dataReg;
				case 2: pInData->u16[i] = dataReg;
				default: pInData->u32[i] = dataReg;
			}
			
			// FRAME WAIT
			*pTPERIOD = frameCount;
			*pTCNTL = CORE_TIMER_ENABLE_BITS;
			while (*pTCOUNT > 0 );
		}
	}
	
	if (upByteCount != 0)
	{
		usbSendBulkData(pInData, upByteCount, true);
	}
}

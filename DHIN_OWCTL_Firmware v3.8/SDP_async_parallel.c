// SDP_async_parallel.c

#include "SDP_std_include.h"
#include "SDP_async_parallel.h"

extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

extern u8 latchValue;

u32 nSamples;
u32 nChannels;
bool samplesComplete;
u32 asyncAddress;
bool nCnvstEnable;
bool intEnable;
u32 nCnvstPeriod;
u32 nCnvstWidth;
u32 waitPeriod;
u32 connector;
bool firstRun;
u32 readDataPointer;

u32 addressIncr;
u32 i;

u32 tCnvWait;
bool cnvstPol;

EX_INTERRUPT_HANDLER(async_timer_isr);


//-----------------------------------------------------------------------------
void processAsyncParCmd(SDP_USB_HEADER *pUsbHeader)
{
	u32 j;
	
	peripheralAccessSdpRev0(true);
	
	// make sure PPI buffers are disabled
	latchValue &= 0x0F;
	writeToSdpLatch(latchValue);

	samplesComplete = false;
	readDataPointer = 0;
	
	firstRun = true;
	
		
	switch (pUsbHeader->cmd)
	{
		
		case ADI_SDP_CMD_ASYNC_PAR_WRITE:
		
			usbGetBulkData(gGeneralDataBuf, pUsbHeader->downByteCount, true);
			
			generateFlowControl(pUsbHeader);
		
			if (intEnable | nCnvstEnable)
			{
				while (!samplesComplete) { ; } 
			}
			else
			{
				// waitPeriod -= 111;
				waitPeriod -= 15;
			
				*pTSCALE = 0x0;
				*pTPERIOD = waitPeriod;
				*pTCNTL = CORE_TIMER_ENABLE_BITS;

				for (i = 0; i < nSamples; i++)
				{
					*(u16*)asyncAddress = gGeneralDataBuf[0].u16[i];  
					
					while (*pTCOUNT > 0 );				
					*pTPERIOD = waitPeriod;
					*pTCNTL = CORE_TIMER_ENABLE_BITS;
					
					asyncAddress += addressIncr;
				}
			}
			
			break;

			
		case ADI_SDP_CMD_ASYNC_PAR_READ:
		
			generateFlowControl(pUsbHeader);
		
			if (intEnable | nCnvstEnable)
			{
				while (!samplesComplete) { ; } 
			}
			else
			{
				waitPeriod -= 15;
			
				*pTSCALE = 0x0;
				*pTPERIOD = waitPeriod;
				*pTCNTL = CORE_TIMER_ENABLE_BITS;
								
				for (i = 0; i < nSamples; i++)
				{
					gGeneralDataBuf[0].u16[i] = *(u16*)asyncAddress;
					asyncAddress += addressIncr;
					
					while (*pTCOUNT > 0 );
				
					*pTPERIOD = waitPeriod;
					*pTCNTL = CORE_TIMER_ENABLE_BITS;
				}
				
			
			}
						
			usbSendBulkData(&gGeneralDataBuf[0], pUsbHeader->upByteCount, true);
			
			break;	
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}


void generateFlowControl(SDP_USB_HEADER *pUsbHeader)
{

	u32 bankAccessTime;
	u32 accessTimeCalc;	
	u32 holdSetupTime;
	u32 holdSetupCalc;
	
	// parameter0 - connector
	connector = pUsbHeader->paramArray[0];	
	asyncAddress = (0x20000000 | (pUsbHeader->paramArray[0] << 20));
	// parameter1 - address bits
	asyncAddress |= (pUsbHeader->paramArray[1] << 1);
	// parameter2 - frequency
	waitPeriod = CORE_CLK_HZ/pUsbHeader->paramArray[2];				
	// we might need to re-shape it (frequency and cnvst period might be the same)

	// Total Samples
	if (pUsbHeader->upByteCount == 0)										// write
		nSamples  = pUsbHeader->downByteCount/sizeof(u16);
	else
		nSamples = pUsbHeader->upByteCount/sizeof(u16);
	
	holdSetupCalc = pUsbHeader->paramArray[3] / 8.33;
	accessTimeCalc = pUsbHeader->paramArray[4] / 8.33;
	nCnvstEnable = pUsbHeader->paramArray[5]; 
	nCnvstPeriod = waitPeriod/5;											// Stand as period for converters (5 is CCLK/SCLK)
	nCnvstWidth = pUsbHeader->paramArray[6]/8.33;
	intEnable = pUsbHeader->paramArray[7];
	nChannels = (intEnable | nCnvstEnable) ? pUsbHeader->paramArray[8] : 1;
	addressIncr = 2 * pUsbHeader->paramArray[9];							// 16 bit alignment		
	cnvstPol = pUsbHeader->paramArray[11];
	
	if(pUsbHeader->paramArray[10] <= (pUsbHeader->paramArray[6] + 125))
		tCnvWait = pUsbHeader->paramArray[6]/1.67;
	else
		tCnvWait = (pUsbHeader->paramArray[6] + pUsbHeader->paramArray[10] - pUsbHeader->paramArray[6] - 125)/1.67;
	
		
	holdSetupCalc -= accessTimeCalc;
	
	// considerar hacer esto en API level
	if (holdSetupCalc == 0)
		holdSetupCalc = 0x5;
	else if (holdSetupCalc >= 6)
		holdSetupCalc = 0xF;
	else
		holdSetupCalc = ((holdSetupCalc/2) << 2) | (holdSetupCalc/2);
	
		
	if(intEnable | nCnvstEnable)  // aun  por definir la interruption
		register_handler(ik_ivg12, async_timer_isr);			
	
							
	*pEBIU_AMGCTL |= 0x0100;											
	
	switch(connector)
	{
		case CONNECTOR_A:															// connector A is bank 0
		
			bankAccessTime = (accessTimeCalc << 12) | (accessTimeCalc << 8);		
			holdSetupTime = (holdSetupCalc << 4);
			*pEBIU_AMBCTL0 = 0x00040004 | bankAccessTime | holdSetupTime;
			
			if(intEnable)	
			{
				*pPORTG_FER &= ~(0x0200);								// PG9 as GPIO
				*pPORTGIO_DIR &= ~(0x0200);								// Disable PG9 Output
				*pPORTGIO_CLEAR = 0x0200;								// Clear PG9
				*pPORTGIO_INEN |= 0x0200;								// PG9 as an input
				*pPORTGIO_EDGE |= 0x0200;								// Set as edge-sensitive input
				*pPORTGIO_POLAR |= 0x0200;								// falling edge
				*pPORTGIO_MASKA_SET = 0x0200;							// Enables PG9 onto IntA (IRQ40)
								
				*pSIC_IMASK1 |= 0x00000100; 							// Enable PortG Int_A onto IVG12
				
				*pPORTG_FER |= 0x0080;									// PG7 as a peripheral function
				*pPORTG_MUX &= ~(0x0030);								// PG7 as PWM_OUT
				
				*pTIMER3_CONFIG = cnvstPol ? 0x000D : 0x0009;			// pwm, pulse high, period count, no enable IRQ
				
				*pTIMER3_PERIOD = nCnvstPeriod;
				*pTIMER3_WIDTH = nCnvstWidth;
				
				*pTIMER_STATUS = 0x000F000F;							// clears all interrupts
				*pTIMER_ENABLE = 0x00000008;							// Timer 3 enable
				
			}
			else if(nCnvstEnable)
			{
				*pSIC_IMASK1 |= 0x00000008; 							// Enable Timer 3 interrupt
				
				*pPORTG_FER |= 0x0080;									// PG7 as a peripheral function
				*pPORTG_MUX &= ~(0x0030);								// PG7 as PWM_OUT
				
				*pTIMER3_CONFIG = cnvstPol ? 0x001D : 0x0019;			// pwm, pulse high, period count, enable IRQ
				
				*pTIMER3_PERIOD = nCnvstPeriod;
				*pTIMER3_WIDTH = nCnvstWidth;
				
				*pTIMER_STATUS = 0x000F000F;							// clears all interrupts
				*pTIMER_ENABLE = 0x00000008;							// Timer 3 enable
				
			}
			
			
			break;
		
						
		case CONNECTOR_B:															// connector B is bank 1
		
			bankAccessTime = (accessTimeCalc << 28) | (accessTimeCalc << 24);		
			holdSetupTime = (holdSetupCalc << 20);
			*pEBIU_AMBCTL0 = 0x00040004 | bankAccessTime | holdSetupTime;
			
			if(intEnable)	
			{
				*pPORTG_FER &= ~(0x2000);								// PG13 as GPIO
				*pPORTGIO_DIR &= ~(0x2000);								// Disable PG13 Output
				*pPORTGIO_CLEAR = 0x2000;								// Clear PG13
				*pPORTGIO_INEN |= 0x2000;								// PG13 as an input
				*pPORTGIO_EDGE |= 0x2000;								// Set as edge-sensitive input
				*pPORTGIO_POLAR |= 0x2000;								// falling edge
				*pPORTGIO_MASKA_SET = 0x2000;							// Enables PG13 onto IntA (IRQ40)
				
				*pSIC_IMASK1 |= 0x00000100; 							// Enable PortG Int_A onto IVG12
				
				*pPORTG_FER |= 0x0100;									// PG8 as a peripheral function
				*pPORTG_MUX &= ~(0x0030);								// PG8 as PWM_OUT
				
				*pTIMER4_CONFIG = cnvstPol ? 0x000D : 0x0009;			// pwm, pulse high, period count, no enable IRQ
				
				*pTIMER4_PERIOD = nCnvstPeriod;
				*pTIMER4_WIDTH = nCnvstWidth;
				*pTIMER_STATUS = 0x000F000F;							// clears all interrupts
				*pTIMER_ENABLE = 0x0010;		
				
			}
			else if(nCnvstEnable)
			{
				*pSIC_IMASK1 |= 0x00000010; 							// Enable Timer 4 interrupt
				
				*pPORTG_FER |= 0x0100;									// PG8 as a peripheral function
				*pPORTG_MUX &= ~(0x0030);								// PG8 as PWM_OUT
				
				*pTIMER4_CONFIG = cnvstPol ? 0x001D : 0x0019;			// pwm, pulse high, period count, enable IRQ
				*pTIMER4_PERIOD = nCnvstPeriod;
				*pTIMER4_WIDTH = nCnvstWidth;
				*pTIMER_STATUS = 0x000F000F;							// clears all interrupts
				*pTIMER_ENABLE = 0x0010;								// Timer 4 enable
				
			}
					
			break;
			
		default:
			bankAccessTime = (accessTimeCalc << 28) | (accessTimeCalc << 24) | (accessTimeCalc << 12) | (accessTimeCalc << 8);		
			holdSetupTime = (holdSetupCalc << 20) | (holdSetupCalc << 4);
			*pEBIU_AMBCTL0 = 0x00040004 | bankAccessTime | holdSetupTime;
			*pEBIU_AMBCTL1 = 0x00040004 | bankAccessTime | holdSetupTime;
			
			break;		
	}
	
}


// ISR for Converters						// estudiar to move algo de esto to the main function
EX_INTERRUPT_HANDLER(async_timer_isr)
{
			
	if (connector == CONNECTOR_A)
	{
		if(intEnable)
		{
			*pPORTGIO_CLEAR = 0x0200; 			// clears PG9 sticky bit
		}
		else
			*pTIMER_STATUS = 0x00000008;		// clears int for Timer 3
	}
	else
	{
		if(intEnable)
		{
			*pPORTGIO_CLEAR = 0x2000; 			// clears PG13 sticky bit
		}
		else
			*pTIMER_STATUS = 0x00010000;		// clears int for Timer 4
	}
	
	// Add delay here							// Don't like too much doing it here
	*pTSCALE = 0x0;
	*pTPERIOD = tCnvWait;						// Time before starting to capture data
	*pTCNTL = CORE_TIMER_ENABLE_BITS;
								
	while (*pTCOUNT > 0 );
	// Finish delay here			
	

	if (firstRun)
	{
		// prevents false first read due to timer idling low.
		firstRun = false;
	}
	else
	{
		for (i = 0; i < nChannels; i++)	
		{
			
			gGeneralDataBuf[0].u16[readDataPointer++] = *(u16*)(asyncAddress);
			asyncAddress += addressIncr;
		}
				
	}
	
	if(readDataPointer >= (nSamples) )	// * nChannels
	{
		if (connector == CONNECTOR_A)
		{
			if (intEnable)	
			{
				*pPORTGIO_MASKA_CLEAR = 0x0200;		// Disables PG9 on IntA
				*pSIC_IMASK1 &= ~(0x00000100);		// Disable PortG Int_A	
				
			}
			else
				*pSIC_IMASK1 &= ~(0x00000008);		// Disable Timer3 interrupt
			
			
			*pTIMER_DISABLE = 0x0008;				// Disable Timer3
		}
		else
		{
			if (intEnable)
			{
				*pPORTGIO_MASKA_CLEAR = 0x2000;		// Disables PG13 on IntA
				*pSIC_IMASK1 &= ~(0x00000100);		// Disable PortG Int_A	
			}
			else
				*pSIC_IMASK1 &= ~(0x00000010);		// Disable Timer4 interrupt
				
			*pTIMER_DISABLE = 0x0010;				// Disable Timer4	
		}
				
		samplesComplete = true;
	}

}




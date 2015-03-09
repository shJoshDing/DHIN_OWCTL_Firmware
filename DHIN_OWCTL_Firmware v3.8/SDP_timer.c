// SDP_timer.c

#include "SDP_std_include.h"
#include "SDP_timer.h"


//-----------------------------------------------------------------------------
void processTimerCmd(SDP_USB_HEADER *pUsbHeader)
{
	u32 i;
	
	switch (pUsbHeader->cmd)
	{
		case ADI_SDP_CMD_TIMER_ENABLE:

			//Timer Pins : pUsbHeader->paramArray[0]
			enableTimer(pUsbHeader->paramArray[0]);
			break;
		
		case ADI_SDP_CMD_TIMER_DISABLE:
			disableTimer(pUsbHeader->paramArray[0]);
			break;
			
		case ADI_SDP_CMD_TIMER_CONFIG:
		
			// Period: pUsbHeader->paramArray[0]
			// Pulse duration: pUsbHeader->paramArray[1]
			// Timer Pin: pUsbHeader->paramArray[2]
			// Polarity: pUsbHeader->paramArray[3] 
			
			generateTimer(pUsbHeader->paramArray[0], pUsbHeader->paramArray[1], pUsbHeader->paramArray[2], pUsbHeader->paramArray[3]);
			break;
			
		case ADI_SDP_CMD_TIMER_UPDATE_PULSE:
		
			// Pulse duration: pUsbHeader->paramArray[0]
			// Timer Pin: pUsbHeader->paramArray[1]
			updateTimerDutyCycle(pUsbHeader->paramArray[0],pUsbHeader->paramArray[1]);
			break;
			
		case ADI_SDP_CMD_TIMER_UPDATE_PERIOD:
		
			// period duration: pUsbHeader->paramArray[0]
			// Timer Pin: pUsbHeader->paramArray[1]
			updateTimerPeriod(pUsbHeader->paramArray[0],pUsbHeader->paramArray[1]);
			break;
			
		default:
			unknownCommand(pUsbHeader);
			break;
	}
}

//***********************************************************************************************************//

void updateTimerDutyCycle(u32 pulse_duration, u8 timerPin)
{
	u32 pulse_counter; 
	pulse_counter = pulse_duration/8.33; 
	
	switch(timerPin)
	{
		case 0:
		*pTIMER3_WIDTH = pulse_counter;
		break;
		
		case 1:
		*pTIMER1_WIDTH = pulse_counter;
		break;
		
		case 2:
		*pTIMER0_WIDTH = pulse_counter;	
		break;
		
		case 3:
		*pTIMER4_WIDTH = pulse_counter;	
		break;
		
		case 4:
		*pTIMER2_WIDTH = pulse_counter;	
		break;
		
		default:
		break;
	}
}

void updateTimerPeriod(u32 period, u8 timerPin)
{
	u32 period_counter;
	period_counter = period/8.33;

		switch(timerPin)
	{
		case 0:
		*pTIMER3_PERIOD = period_counter;
		break;
		
		case 1:
		*pTIMER1_PERIOD = period_counter;
		break;
		
		case 2:
		*pTIMER0_PERIOD = period_counter;
		break;
		
		case 3:
		*pTIMER4_PERIOD = period_counter;	
		break;
		
		case 4:
		*pTIMER2_PERIOD = period_counter;
		break;
		
		default:
		break;
	}
}

void generateTimer(u32 period, u32 pulse_duration, u8 timerPin, bool polarity)
{
	// Counters using SCLK, could be changed if needed
	// pulse_duration and period are given in nanoseconds
	
	u32 pulse_counter; 
	u32 period_counter;
	
//	pulse_counter = pulse_duration*(1000/120); 		// SCLK 120MHz
//	period_counter = period*(1000/120);
	pulse_counter = pulse_duration/8.33; 
	period_counter = period/8.33;

	
	switch(timerPin)
	{
		case 0:									// timer A (PG7) Connector A
			
			*pPORTG_FER |= 0x0080;				// PG7 as a peripheral function
			*pPORTG_MUX &= ~(0x0030);			// PG7 as PWM_OUT
					
			if (polarity)
				*pTIMER3_CONFIG = 0x000D;    	// positive action pulse
			else
				*pTIMER3_CONFIG = 0x0009;    	// negative action pulse
				
			*pTIMER3_WIDTH = pulse_counter;		
			*pTIMER3_PERIOD = period_counter;	
			
//			*pTIMER_ENABLE |= 0x0008;			// Timer3 enable
						
			break;
			
		case 1:									// timer B (PG5) Connector A
			
			*pPORTG_FER |= 0x0020;				// PG5 as a peripheral function
			*pPORTG_MUX &= ~(0x000C);			// PG5 as PWM_OUT
					
			if (polarity)
				*pTIMER1_CONFIG = 0x000D;    	// positive action pulse
			else
				*pTIMER1_CONFIG = 0x0009;    	// negative action pulse
				
			*pTIMER1_WIDTH = pulse_counter;		
			*pTIMER1_PERIOD = period_counter;	
			
	//		*pTIMER_ENABLE |= 0x0002;			// Timer1 enable
			
			break;
		
		case 2:									// timer D (PJ0): Shared across the two connectors (A and B)
			
			if (polarity)
				*pTIMER0_CONFIG = 0x000D;    	// positive action pulse
			else
				*pTIMER0_CONFIG = 0x0009;    	// negative action pulse
				
			*pTIMER0_WIDTH = pulse_counter;		
			*pTIMER0_PERIOD = period_counter;	
			
	//		*pTIMER_ENABLE |= 0x0001;			// Timer0 enable
		
			break;
			
		case 3:									// timer A (PG8) Connector B
		
			*pPORTG_FER |= 0x0100;				// PG8 as a peripheral function
			*pPORTG_MUX &= ~(0x0030);			// PG8 as PWM_OUT
			
			if (polarity)
				*pTIMER4_CONFIG = 0x000D;    	// positive action pulse
			else
				*pTIMER4_CONFIG = 0x0009;    	// negative action pulse
				
			*pTIMER4_WIDTH = pulse_counter;		
			*pTIMER4_PERIOD = period_counter;	
			
		//	*pTIMER_ENABLE |= 0x0010;			// Timer4 enable
		
			break;
			
		case 4:									// timer B (PG6) Connector B
		
			*pPORTG_FER |= 0x0040;							// PG6 as a peripheral function
			*pPORTG_MUX = (*pPORTG_MUX & 0xFFF3) | 0x0004;	// PG6 as PWM_OUT
		
			if (polarity)
				*pTIMER2_CONFIG = 0x000D;    	// positive action pulse
			else
				*pTIMER2_CONFIG = 0x0009;    	// negative action pulse
				
			*pTIMER2_WIDTH = pulse_counter;		
			*pTIMER2_PERIOD = period_counter;	
			
		//	*pTIMER_ENABLE |= 0x0004;			// Timer2 enable
		
			break;		
			
		default:								// No valid timer
			
			
			break;
	}

}

void disableTimer(u8 timerPin)
{
	*pTIMER_DISABLE &= timerPin;
}

void enableTimer(u8 timerPin)
{
	*pTIMER_ENABLE |= timerPin;
}


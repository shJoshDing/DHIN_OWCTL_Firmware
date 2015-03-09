// SDP_i2c_pseudo_low_level.c

#include "SDP_std_include.h"
#include "SDP_twi_low_level.h"
#include "SDP_i2c_pseudo_low_level.h"

#define SCL_PIN 0x1000
#define CONNECTOR_A_SDA_PIN 0x0400
#define CONNECTOR_B_SDA_PIN 0x8000  

extern u64 boardInitOtpWord;

static void i2cStart(u16 sdaPin, u32 timerSclPeriod);
static bool i2cSendByte(u8 data, u16 sdaPin, u32 timerSclPeriod);
static void i2cReceiveByte(u8 *data, bool ack, u16 sdaPin, u32 timerSclPeriod);
static void i2cStop(u16 sdaPin, u32 timerSclPeriod, bool prepForRepeatStart);

//-----------------------------------------------------------------------------
TWI_ERROR i2cPseudoTransfer(SDP_TWI_STRUCT *twiParam)
{
	u32 i;
	u16 sdaPin;
	u32 timerSclPeriod;
	bool nack;

	nack = false;
	switch(twiParam->connector)
	{
		case CONNECTOR_A: 
		
			// Rev 1.3 assign PG15 to SDA while previous SDP assign PG10
			if(boardInitOtpWord == 0x000100000000)	
				sdaPin = CONNECTOR_B_SDA_PIN; 
			else
				sdaPin = CONNECTOR_A_SDA_PIN; 
				
			break;
			
		case CONNECTOR_B: 
		
			sdaPin = CONNECTOR_B_SDA_PIN; 
			break;
			
		default: sdaPin = 0;
	}
	timerSclPeriod = (CORE_CLK_HZ/1000/twiParam->clkFrequency) - 0x38;
	*pTSCALE = 0x0;
	
	if (twiParam->writeByteCount)
	{
		i2cStart(sdaPin, timerSclPeriod);
		
		nack = i2cSendByte( twiParam->slaveAddress << 1,
		                    sdaPin,
		                    timerSclPeriod );
		
		// Check if the slave ACKed the address
		if (nack)
		{	// If nack == true then slave did not ACK address
			i2cStop( sdaPin,
		         timerSclPeriod,
		         twiParam->repeatStartEnable && twiParam->readByteCount );
			return TWI_ERROR_ADDR_NACK;
		}
		                    
		for (i=0; !nack && i<twiParam->writeByteCount; i++)
		{
			nack = i2cSendByte( twiParam->pWriteData[i],
			                    sdaPin,
			                    timerSclPeriod );
			                    
			// Check if the slave ACKed the data byte
			if (nack)
			{	// If nack == true then slave did not ACK data byte
				i2cStop( sdaPin,
		         timerSclPeriod,
		         twiParam->repeatStartEnable && twiParam->readByteCount );
		         
				return TWI_ERROR_DATA_NACK;
			}	
		}
		
		i2cStop( sdaPin,
		         timerSclPeriod,
		         twiParam->repeatStartEnable && twiParam->readByteCount );
	}
	if (twiParam->readByteCount)
	{
		i2cStart(sdaPin, timerSclPeriod);
		nack = i2cSendByte( (twiParam->slaveAddress << 1) | 1,
		                    sdaPin,
		                    timerSclPeriod );
		// Check if the slave ACKed the address
		if (nack)
		{	// If nack == true then slave did not ACK address
			i2cStop( sdaPin,
		         timerSclPeriod,
		         twiParam->repeatStartEnable && twiParam->readByteCount );
			return TWI_ERROR_ADDR_NACK;
		}
		
		//if (!nack)
		//{
			for (i=0; i<twiParam->readByteCount-1; i++)
			{
				i2cReceiveByte( &twiParam->pReadData[i],
				                true,
		                        sdaPin,
		                        timerSclPeriod );
			}
			i2cReceiveByte( &twiParam->pReadData[i],
			                false,
	                        sdaPin,
	                        timerSclPeriod );
		//}
		i2cStop(sdaPin, timerSclPeriod, false);
	}
	return TWI_ERROR_OK;
}

//-----------------------------------------------------------------------------
static void i2cStart(u16 sdaPin, u32 timerSclPeriod)
{
	u16 bothI2cPins = SCL_PIN | sdaPin;

	*pPORTG_FER &= ~bothI2cPins;            	// Enable GPIO mode for SCL & SDA
	*pPORTGIO_INEN &= ~bothI2cPins;         	// SCL & SDA disable input and
	*pPORTGIO_DIR |= bothI2cPins;   	        // SCL & SDA = 1 (make output)

	*pPORTGIO_SET = bothI2cPins;        	   	// SCL & SDA = 1 (disable output) 
	
	
	*pTPERIOD = timerSclPeriod;             	// start timer
	*pTCNTL = CORE_TIMER_ENABLE_BITS;
	
	while (*pTCOUNT > 0); 
	
	*pPORTGIO_CLEAR = sdaPin;           		// cleared for when output enabled

	*pTPERIOD = timerSclPeriod;              	// start timer
	*pTCNTL = CORE_TIMER_ENABLE_BITS;
	
	while (*pTCOUNT > 0 ) {;}
}

//-----------------------------------------------------------------------------
static bool i2cSendByte(u8 data, u16 sdaPin, u32 timerSclPeriod)
{
	bool nack;
	u8 bitMask;
	
	while (*pTCOUNT > 0);                    	// wait for start of SCL peroid
	
	*pPORTGIO_CLEAR = SCL_PIN;                	// SCL = 0 (enable output)
	*pTPERIOD = timerSclPeriod;              	// start timer
	*pTCNTL = CORE_TIMER_ENABLE_BITS;
	for (bitMask=0x80; bitMask; bitMask>>=1)
	{
		while (*pTCOUNT > 3*timerSclPeriod/4); 	// wait for middle of SCL peroid
		
		if (data & bitMask)                  	// SDA = data bit
			*pPORTGIO_SET = sdaPin;        		// (disable output)
		else
			*pPORTGIO_CLEAR = sdaPin;         	// (enable output)
			
		while (*pTCOUNT > timerSclPeriod/2); 	// wait for middle of SCL peroid
		
		*pPORTGIO_SET = SCL_PIN;           		// SCL = 1 (disable output)
		while (*pTCOUNT > 0);                	// wait for start of SCL peroid
		
		*pPORTGIO_CLEAR = SCL_PIN;            	// SCL = 0 (enable output) // david: should be 1
	
		*pTPERIOD = timerSclPeriod;          	// start timer
		*pTCNTL = CORE_TIMER_ENABLE_BITS;
	}
		
	while (*pTCOUNT > 3*timerSclPeriod/4);   	// wait a quarter an SCL peroid

	*pPORTGIO_INEN |= sdaPin;
 	*pPORTGIO_DIR &= ~sdaPin;                	// SDA disable output

	while (*pTCOUNT > timerSclPeriod/2);     	// wait for middle of SCL peroid
	
	*pPORTGIO_SET = SCL_PIN;              		// SCL = 1 (disable output)
	
	nack = *pPORTGIO & sdaPin;               	// test acknowledge bit (SDA)
	*pPORTGIO_INEN &= ~sdaPin;               	// SDA disable input and
	*pPORTGIO_DIR |= sdaPin;                	// SDA disable output
	
	return nack;
}

//-----------------------------------------------------------------------------
static void i2cReceiveByte(u8 *data, bool ack, u16 sdaPin, u32 timerSclPeriod)
{
	u8 bitMask;
	u8 dataReg;

	while (*pTCOUNT > 0);                    	// wait for start of SCL peroid
	
	*pPORTGIO_CLEAR = SCL_PIN;                	// SCL = 0 (enable output)
	*pTPERIOD = timerSclPeriod;              	// start timer
	*pTCNTL = CORE_TIMER_ENABLE_BITS;
	while (*pTCOUNT > timerSclPeriod*3/4);	   	// wait a quarter an SCL peroid

	*pPORTGIO_DIR &= ~sdaPin;               	// SDA disable output
	*pPORTGIO_INEN |= sdaPin;                	// SDA enable input	
	
	dataReg = 0;
	for (bitMask=0x80; bitMask; bitMask>>=1)
	{
		while (*pTCOUNT > timerSclPeriod/2); 	// wait for middle of SCL peroid
		
		*pPORTGIO_SET = SCL_PIN;           		// SCL = 1 (disable output)
		if (*pPORTGIO & sdaPin)             	// read data bit on SDA
			dataReg |= bitMask;
		while (*pTCOUNT > 0);                	// wait for start of SCL peroid
		
		*pPORTGIO_CLEAR = SCL_PIN;            	// SCL = 0 (enable output)
		*pTPERIOD = timerSclPeriod;          	// start timer
		*pTCNTL = CORE_TIMER_ENABLE_BITS;
	}
	*pPORTGIO_INEN &= ~sdaPin;               	// SDA disable input and
	*pPORTGIO_CLEAR = sdaPin;                	// clear for when output enabled	
	
	if (ack)
	{                                 			// acknowledge if parameter is set
		*pPORTGIO_CLEAR = sdaPin;             	// SDA = 0 (enable output)
		*pPORTGIO_DIR |= sdaPin;             	// SDA = 0 (enable output)
	}
	
	while (*pTCOUNT > timerSclPeriod/2);     	// wait for middle of SCL peroid
	
	*pPORTGIO_SET = SCL_PIN;               		// SCL = 1 (disable output)
	*data = dataReg;                         	// save received data byte
}

//-----------------------------------------------------------------------------
static void i2cStop(u16 sdaPin, u32 timerSclPeriod, bool prepForRepeatStart)
{
	while (*pTCOUNT > 0);                    	// wait for start of SCL peroid
	
	*pPORTGIO_CLEAR = SCL_PIN;                	// SCL = 0 (enable output)
	*pTPERIOD = timerSclPeriod;              	// start timer
	*pTCNTL = CORE_TIMER_ENABLE_BITS;
	
	*pPORTGIO_DIR |= sdaPin;  
	
	if (prepForRepeatStart)
		*pPORTGIO_SET = sdaPin;		            // SDA = 1 (disable output)
	else
		*pPORTGIO_CLEAR = sdaPin;    	        // SDA = 0 (enable output)

	while (*pTCOUNT > timerSclPeriod/2);	    // wait for middle of SCL peroid
	
	*pPORTGIO_SET = SCL_PIN;               		// SCL = 1 (disable output)

	while (*pTCOUNT > 0);                   	// wait half an SCL peroid
	
	*pPORTGIO_SET = sdaPin;                		// SDA = 1 (disable output)
}

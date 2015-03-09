// SDP_twi_low_level.c

#include "SDP_std_include.h"
#include <drivers/twi/adi_twi.h>
#include "SDP_twi_low_level.h"
#include "SDP_my_TWI.h"

extern ADI_DEV_MANAGER_HANDLE ghDevManager;
extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];
extern u8 latchValue;

static ADI_DEV_DEVICE_HANDLE gTwiDriverHandle;
static volatile s32 gTwiCallbackCountDown;
static u32 gTwiCallbackEvent;

static void twiCallback(void *AppHandle, u32 Event, void *pArg);

ADI_DEV_SEQ_1D_BUFFER outBuf;
ADI_DEV_SEQ_1D_BUFFER inBuf;

//-----------------------------------------------------------------------------
void twiInit(void)
{
	u32 result;
	adi_twi_bit_rate rate = { 100, 50 };
	ADI_DEV_CMD_VALUE_PAIR twiConfigurationTable[] = {
		{ ADI_TWI_CMD_SET_HARDWARE,        (void *) ADI_INT_TWI },
		{ ADI_DEV_CMD_SET_DATAFLOW_METHOD, (void *) ADI_DEV_MODE_SEQ_CHAINED },
		{ ADI_TWI_CMD_SET_FIFO,            (void *) 0x0000 },
		{ ADI_TWI_CMD_SET_RATE,                     &rate },
		{ ADI_TWI_CMD_SET_LOSTARB,         (void *) 1 },
		{ ADI_TWI_CMD_SET_ANAK,            (void *) 1 },
		{ ADI_TWI_CMD_SET_DNAK,            (void *) 1 },
		{ ADI_DEV_CMD_SET_DATAFLOW,        (void *) TRUE },
		{ ADI_DEV_CMD_END,                          NULL }
	};

	result = adi_dev_Open( ghDevManager,
	                       &ADITWIEntryPoint,
	                       0,
	                       NULL,
	                       &gTwiDriverHandle,
	                       ADI_DEV_DIRECTION_BIDIRECTIONAL,
	                       NULL,
	                       NULL,
	                       twiCallback );
	SDP_ASSERT(result)
	
	result = adi_dev_Control( gTwiDriverHandle,
	                          ADI_DEV_CMD_TABLE,
	                          twiConfigurationTable );
	SDP_ASSERT(result)
}

//-----------------------------------------------------------------------------
TWI_ERROR twiTransfer(SDP_TWI_STRUCT *twiParam)
{
	//ADI_DEV_SEQ_1D_BUFFER outBuf;
	//ADI_DEV_SEQ_1D_BUFFER inBuf;
	adi_twi_bit_rate rate;
	u32 result;
	TWI_ERROR errorRetval = TWI_ERROR_ADDR_NACK;
	
	switch (twiParam->connector)
	{
		case CONNECTOR_A:
			latchValue &= 0xFC;
			latchValue |= 0x01;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x01);
			break;
		case CONNECTOR_B:
			latchValue &= 0xFC;
			latchValue |= 0x02;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x02);
			break;
		default:
			latchValue &= 0xFC;
			latchValue |= 0x03;
			writeToSdpLatch(latchValue);
			//writeToSdpLatch(0x03);
			break;
	}
	
	rate.frequency = (u16)twiParam->clkFrequency;
	rate.duty_cycle = (u16)twiParam->clkDutyCycle;
	result = adi_dev_Control(gTwiDriverHandle, ADI_TWI_CMD_SET_RATE, &rate);
	SDP_ASSERT(result)
	
	if (twiParam->writeByteCount)
	{
		outBuf.Buffer.Data = twiParam->pWriteData;
		outBuf.Buffer.ElementCount = twiParam->writeByteCount;
		outBuf.Buffer.ElementWidth = 1;
		outBuf.Buffer.CallbackParameter = &outBuf;
		outBuf.Buffer.ProcessedFlag = 0;
		outBuf.Buffer.ProcessedElementCount = 0;
		outBuf.Buffer.pNext = NULL;
		outBuf.Buffer.pAdditionalInfo = (void *)twiParam->slaveAddress;
		outBuf.Direction = ADI_DEV_DIRECTION_OUTBOUND;
	}
	
	if (twiParam->readByteCount)
	{
		inBuf.Buffer.Data = twiParam->pReadData;
		inBuf.Buffer.ElementCount = twiParam->readByteCount;
		inBuf.Buffer.ElementWidth = 1;
		inBuf.Buffer.CallbackParameter = &inBuf;
		inBuf.Buffer.ProcessedFlag = 0;
		inBuf.Buffer.ProcessedElementCount = 0;
		inBuf.Buffer.pNext = NULL;
		inBuf.Buffer.pAdditionalInfo = (void *)twiParam->slaveAddress;
		inBuf.Direction = ADI_DEV_DIRECTION_INBOUND;
		
		if (twiParam->writeByteCount)
		{
			outBuf.Buffer.pNext = &inBuf.Buffer;
			if (twiParam->repeatStartEnable)
			{
				inBuf.Buffer.pAdditionalInfo = 
					(void *)(twiParam->slaveAddress | ADI_TWI_RSTART);
			}
			result = adi_dev_SequentialIO( gTwiDriverHandle,
			                               ADI_DEV_SEQ_1D,
			                               (ADI_DEV_BUFFER *)&outBuf);
			SDP_ASSERT(result)
			gTwiCallbackCountDown = 2;
		}
		else
		{
			result = adi_dev_SequentialIO( gTwiDriverHandle,
			                               ADI_DEV_SEQ_1D,
			                               (ADI_DEV_BUFFER *)&inBuf);
			SDP_ASSERT(result)
			gTwiCallbackCountDown = 1;
		}
	}
	else
	{
		if (twiParam->writeByteCount)
		{
			result = adi_dev_SequentialIO( gTwiDriverHandle,
			                               ADI_DEV_SEQ_1D,
			                               (ADI_DEV_BUFFER *)&outBuf);
			SDP_ASSERT(result)
			gTwiCallbackCountDown = 1;
		}
	}
	
	while(gTwiCallbackCountDown > 0);
	
	switch (gTwiCallbackEvent)
	{
		case ADI_DEV_EVENT_BUFFER_PROCESSED:
			errorRetval = TWI_ERROR_OK; // Passed
			break;
		case ADI_TWI_EVENT_ANAK:
			errorRetval = TWI_ERROR_ADDR_NACK;
			break;
		case ADI_TWI_EVENT_DNAK:
			errorRetval = TWI_ERROR_DATA_NACK;
			break;
	}
	
	return errorRetval;
}

//-----------------------------------------------------------------------------
static void twiCallback(void *AppHandle, u32 Event, void *pArg)
{
	gTwiCallbackCountDown--;
	gTwiCallbackEvent = Event;
}


//-----------------------------------------------------------------------------
void InitADMP441_regs_twi( bool rw )
{
	u8 regs[512];
	u8 regs_read[512];
	SDP_TWI_STRUCT twiParam;
	
	
	u8 i = 0;
	
	if(rw)		//ture : read
	{
/*		regs[0] = 0x00;
		regs[1] = 0x00;

		regs[2] = 0x01;
		regs[3] = 0x00;

		regs[4] = 0x02;
		regs[5] = 0;

		regs[6] = 0x03;
		regs[7] = 0;
		
		regs[8] = 0x04;
		regs[9] = 0x00;

		regs[10] = 0x05;
		regs[11] = 0x00;

		regs[12] = 0x06;
		regs[13] = 0;

		regs[14] = 0x07;
		regs[15] = 0;
		
		regs[16] = 0x08;
		regs[17] = 0x00;

		regs[18] = 0x09;
		regs[19] = 0x00;
*/
		regs[0] = 0xF5;
		regs[1] = 0x00;

		regs[2] = 0x0A;
		regs[3] = 0x00;

		regs[4] = 0x0B;
		regs[5] = 0x00;

		regs[6] = 0x0C;
		regs[7] = 0x00;
	
		regs_read[0] = 0xF5;
		regs_read[1] = 0x00;
		regs_read[2] = 0x0A;
		regs_read[3] = 0x00;
		regs_read[4] = 0x0B;
		regs_read[5] = 0x00;
		regs_read[6] = 0x0C;
		regs_read[7] = 0x00;
		
		twiParam.connector = 0x00;
		twiParam.bus = 0x00;
		twiParam.slaveAddress = 0x28;
		twiParam.clkFrequency = 0x64;
		twiParam.clkDutyCycle = 0x32;
		twiParam.repeatStartEnable = 0x00;
		twiParam.writeByteCount = 0x01;
		twiParam.readByteCount = 0x01;
		twiParam.pWriteData = (u8 *)regs;
		twiParam.pReadData = (u8 *)regs_read;
		
		for(i=0 ; i< 4;i++)
		{
			twiParam.pWriteData = (u8 *)(regs+i*2);
			twiTransfer(&twiParam);
		}

	}
	else
	{
		regs[0] = 0xAA;
		regs[1] = 0x41;

		regs[2] = 0xF5;
		regs[3] = 0x10;

		regs[4] = 0x0A;
		regs[5] = 0x1A;

		regs[6] = 0x0B;
		regs[7] = 0x11;
		
		regs[8] = 0x0C;
		regs[9] = 0x01;
		
		twiParam.connector = 0x00;
		twiParam.bus = 0x00;
		twiParam.slaveAddress = 0x28;
		twiParam.clkFrequency = 0x64;
		twiParam.clkDutyCycle = 0x32;
		twiParam.repeatStartEnable = 0x00;
		twiParam.writeByteCount = 0x02;
		twiParam.readByteCount = 0x00;
		twiParam.pWriteData = (u8 *)regs;
		//twiParam.pReadData = (u8 *)regs_read;
	
		//twiTransfer(&twiParam);
		for(i=0 ; i< 5;i++)
		{
			twiParam.pWriteData = (u8 *)(regs+i*2);
			twiTransfer(&twiParam);
		}
	}
}


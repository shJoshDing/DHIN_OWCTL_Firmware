// SDP_id_eeprom.c

#include "SDP_std_include.h"
#include "SDP_twi_low_level.h"
#include "SDP_id_eeprom.h"

extern SDP_GENERAL_DATA_BUF gGeneralDataBuf[];

//-----------------------------------------------------------------------------
void readIdEeproms(void)
{
	s32 numBoards;
	s32 i;
	SDP_TWI_STRUCT twiParam;
	u8 header[10];
	bool noEeprom;
	u16 descriptorLength;
	u8 regAddress[2];
	
	numBoards = 0;
	i = 0;
	
	twiParam.bus = 0;
	twiParam.clkFrequency = 100;
	twiParam.clkDutyCycle = 50;
	twiParam.repeatStartEnable = false;
	twiParam.writeByteCount = 2;
	twiParam.pWriteData = regAddress;
	
	for (twiParam.connector = 0; twiParam.connector < 2; twiParam.connector++)
	{
		for (twiParam.slaveAddress = 0x50;
		     twiParam.slaveAddress < 0x58;
		     twiParam.slaveAddress++ )
		{
			regAddress[0] = 0;
			regAddress[1] = 0;
			twiParam.pReadData = header;
			twiParam.readByteCount = sizeof(header);
			noEeprom = twiTransfer(&twiParam);
			if (!noEeprom)
			{
				if (header[0] == 'A' &&
					header[1] == 'D' &&
					header[2] == 'I' &&
					header[3] == 'S' &&
					header[4] == 'D' &&
					header[5] == 'P' )
				{
					numBoards++;
					
					descriptorLength = *(u16*)(header+8);
					descriptorLength -= sizeof(header);
					
					gGeneralDataBuf[0].u8[i++] = (u8)twiParam.connector;
					gGeneralDataBuf[0].u8[i++] = (u8)twiParam.slaveAddress;
					gGeneralDataBuf[0].u8[i++] = header[6]; // descriptor rev
					gGeneralDataBuf[0].u8[i++] = header[7]; // descriptor rev
					gGeneralDataBuf[0].u8[i++] = (u8)descriptorLength;
					gGeneralDataBuf[0].u8[i++] = (u8)(descriptorLength>>8);
					
					regAddress[0] = (u8)( ((u16)( sizeof(header) )) >> 8);
					regAddress[1] = (u8)sizeof(header);
					twiParam.pReadData = &gGeneralDataBuf[0].u8[i];
					twiParam.readByteCount = descriptorLength;
					twiTransfer(&twiParam);
					i += descriptorLength;
				}
			}
		}
	}
	gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF-1].s32[0] = 0;
	gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF-1].s32[1] = i;
	gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF-1].s32[2] = numBoards;
	usbSendBulkData(&gGeneralDataBuf[SDP_NUM_GEN_DATA_BUF-1], 512, true);
	if (i > 0)
	{
		usbSendBulkData(&gGeneralDataBuf[0], i, true);
	}
}

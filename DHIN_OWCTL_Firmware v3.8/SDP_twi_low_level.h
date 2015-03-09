// SDP_twi_low_level.h

#ifndef _SDP_TWI_LOW_LEVEL_H_
#define _SDP_TWI_LOW_LEVEL_H_

typedef struct _SDP_TWI_STRUCT
{
	u32 connector;
	u32 bus;
	u32 slaveAddress;
	u32 clkFrequency;
	u32 clkDutyCycle;
	u32 writeByteCount;
	u32 readByteCount;
	u8 *pWriteData;
	u8 *pReadData;
	bool repeatStartEnable;
} SDP_TWI_STRUCT, *pSDP_TWI_STRUCT;

typedef enum
{
	TWI_ERROR_OK,				// 
	TWI_ERROR_ADDR_NACK,
	TWI_ERROR_DATA_NACK,
} TWI_ERROR;

void twiInit(void);
void InitADMP441_regs_twi( bool rw );
TWI_ERROR twiTransfer(SDP_TWI_STRUCT *twiParam);

#endif

#ifndef _GPIO_I2C_H_
#define _GPIO_I2C_H_

#include <bfrom.h>
#include <stdio.h>

//#define SCL PF4
//#define SDA PF5

// Proto definition
/*
void i2cStart(void);
void i2cStop (void);
bool i2cGetAck(void);
void i2cPutAck(bool isACK);
void i2cSendByte(u8 data);
u8   i2cGetByte (void);
bool i2c_Open(void);
void i2c_Close(void);
bool i2c_SendData(u8 Addr, u8 *dataBuffer, u8 Size);
bool i2c_GetData(u8 Addr, u8 *dataBuffer, u8 Size);
void i2cDelay(u16);
*/

#define PORTF 1
#define PORTG 2
#define PORTH 3
#define PORTFH	4

#define FUSE_OE				 PH0
#define CLK_OE				 PH1
#define DIR_H				 PH5
#define DATA_DIR_H			 PH4
#define CLK_SEL_H			 PH2		//v2.0:CLK_I2C/PDM
#define MUX_SEL_H			 PH7		//v2.0:CLK_AP/SDP
#define SDP_LR0_H			 PH3
#define SDP_LR1_G			 PG5

void TestClock(void);
bool I2CWrite(u8 chipAddr, u8 *dataBuffer, u8 writeNum, u8 portx);
bool I2CRead(u8 chipAddr, u8 *dataBuffer, u8 readNum, u8 portx);
void SetLR(bool high, u32 channel, bool ifNomalMode);
void ADMP521T_CLK_OE(bool enable);
void ADMP521T_FUSE_OE(bool enable);
void FuseChip(u16 _fuseVal, u32 _pulse_halfDelay);
void SetClkPinDir(bool ifIn);
void SetPin_F(u16 pinDefine, bool high);
void SetPin_G(u16 pinDefine, bool high);
void SetPin_H(u16 pinDefine, bool high);



//void waitHalfMilliSec(u32 msec);
//void waitHalfMicroSec(u32 msec);


#endif // I2C_H




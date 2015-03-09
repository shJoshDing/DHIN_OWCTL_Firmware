// SDP_i2c.h

#ifndef _SDP_I2C_H_
#define _SDP_I2C_H_

// I2C Commands
#define ADI_SDP_CMD_GROUP_I2C               0xCA000300
#define ADI_SDP_CMD_I2C_TRANSFER            0xCA000301
//#define ADI_SDP_CMD_GPIO_I2C				0xCA000302

void processI2cCmd(SDP_USB_HEADER *pUsbHeader);

#endif

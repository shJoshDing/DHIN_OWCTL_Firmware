// SDP_gpio.h

#ifndef _SDP_GPIO_H_
#define _SDP_GPIO_H_

// GPIO Commands
#define ADI_SDP_CMD_GROUP_GPIO              0xCA000500
#define ADI_SDP_CMD_GPIO_CONFIG_OUTPUT      0xCA000501
#define ADI_SDP_CMD_GPIO_CONFIG_INPUT       0xCA000502
#define ADI_SDP_CMD_GPIO_BIT_SET            0xCA000503
#define ADI_SDP_CMD_GPIO_BIT_CLEAR          0xCA000504
#define ADI_SDP_CMD_GPIO_BIT_TOGGLE         0xCA000505
#define ADI_SDP_CMD_GPIO_DATA_WRITE         0xCA000506
#define ADI_SDP_CMD_GPIO_DATA_READ          0xCA000507

void processGpioCmd(SDP_USB_HEADER *pUsbHeader);

#endif

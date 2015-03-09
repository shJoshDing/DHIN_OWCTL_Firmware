// SDP_spi.h

#ifndef _SDP_SPI_H_
#define _SDP_SPI_H_

// SPI Commands
#define ADI_SDP_CMD_GROUP_SPI               0xCA000200
#define ADI_SDP_CMD_SPI_TRANSFER            0xCA000210
#define ADI_SDP_CMD_SPI_EXTENDED_TRANSFER	0xCA000211

void processSpiCmd(SDP_USB_HEADER *pUsbHeader);

#endif

// SDP_ppi.h

#ifndef _SDP_PPI_H_
#define _SDP_PPI_H_

// PPI Commands
#define ADI_SDP_CMD_GROUP_PPI               0xCA000600
#define ADI_SDP_CMD_PPI_WRITE               0xCA000601
#define ADI_SDP_CMD_PPI_READ                0xCA000602
#define ADI_SDP_CMD_PPI_STREAM_FROM         0xCA000603

void processPpiCmd(SDP_USB_HEADER *pUsbHeader);

#endif

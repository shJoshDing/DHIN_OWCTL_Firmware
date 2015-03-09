// SDP_sport.h

#ifndef _SDP_SPORT_H_
#define _SDP_SPORT_H_

// SPORT Commands
#define ADI_SDP_CMD_GROUP_SPORT             0xCA000400
#define ADI_SDP_CMD_SPORT_TRANSFER          0xCA00040A
#define ADI_SDP_CMD_SPORT_STREAM_FROM       0xCA00040B
#define ADI_SDP_CMD_SPORT_PDM				0xCA00040C
#define ADI_SDP_CMD_SPORT_TestRead			0xCA00040D
#define ADI_SDP_CMD_SPORT_PDMStop			0xCA00040E

void processSportCmd(SDP_USB_HEADER *pUsbHeader);

#endif

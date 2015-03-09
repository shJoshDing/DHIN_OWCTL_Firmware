// SDP_async_parallel.h

#ifndef _SDP_ASYNC_PARALLEL_H_
#define _SDP_ASYNC_PARALLEL_H_

#define ADI_SDP_CMD_GROUP_ASYNC_PAR         0xCA000700
#define ADI_SDP_CMD_ASYNC_PAR_WRITE         0xCA000701
#define ADI_SDP_CMD_ASYNC_PAR_READ          0xCA000702
#define ADI_SDP_CMD_ASYNC_PAR_WRITE_CNVT	0xCA000703
#define ADI_SDP_CMD_ASYNC_PAR_READ_CNVT     0xCA000704


void processAsyncParCmd(SDP_USB_HEADER *pUsbHeader);
void generateFlowControl(SDP_USB_HEADER *pUsbHeader);

#endif

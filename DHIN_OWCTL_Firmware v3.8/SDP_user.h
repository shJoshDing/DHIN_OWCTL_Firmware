// SDP_user.h

// Refer to SDP_user.c for instructions on adding user commands.

#ifndef _SDP_USER_H_
#define _SDP_USER_H_

// USER COMMANDS
// The fist is not a command and should not be removed or renamed. The second
// is a compulsory and should not be removed. The rest are examples and can be
// renamed or removed. Further commands can be added. Values must be greater
// than 0xF8000000 and less than 0xF8000100.
#define ADI_SDP_CMD_GROUP_USER                     0xF8000000 // not a command
#define ADI_SDP_CMD_USER_GET_GUID                  0xF8000001 // compulsory
#define ADI_SDP_CMD_USER_EXAMPLE_GET_USER_VERSION  0xF8000002
#define ADI_SDP_CMD_USER_EXAMPLE_NO_DATA           0xF8000003
#define ADI_SDP_CMD_USER_EXAMPLE_DATA_DOWN         0xF8000004
#define ADI_SDP_CMD_USER_EXAMPLE_DATA_UP           0xF8000005
#define ADI_SDP_CMD_USER_EXAMPLE_DATA_DOWN_UP      0xF8000006

// Entry function called when user command is received. Internal function
// prototype should go at the top of SDP_user.c
void processUserCmd(SDP_USB_HEADER *pUsbHeader);

#endif

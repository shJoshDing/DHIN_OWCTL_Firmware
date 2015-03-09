//SDP_ADMP521T.h

#ifndef _SDP_ADMP521T_
#define _SDP_ADMP521T_

// 521T Commands
#define ADI_SDP_CMD_GROUP_ADMP521T        		    0x51000000
#define ADI_SDP_CMD_ADMP521T_SportPDM_Start			0x51000001	
#define ADI_SDP_CMD_ADMP521T_SportPDM_Stop			0x51000002
#define ADI_SDP_CMD_ADMP521T_GPIO_I2C_Read			0x51000004
#define ADI_SDP_CMD_ADMP521T_Mode_Normal			0x51000005
#define ADI_SDP_CMD_ADMP521T_Mode_Test				0x51000006
#define ADI_SDP_CMD_ADMP521T_Mode_NormalTest		0x51000007	
#define ADI_SDP_CMD_ADMP521T_Mode_Fuse				0x51000008
#define ADI_SDP_CMD_ADMP521T_FW_Version				0x51000009
#define ADI_SDP_CMD_ADMP521T_SETLR					0x5100000A
#define ADI_SDP_CMD_ADMP521T_POSTTRIM				0x5100000B
#define ADI_SDP_CMD_ADMP521T_OtherCtrl              0x5100000C      //Other controls.

#define ADI_SDP_CMD_ADMP521T_TEST					0x5100000F

//-------------------other ctrl--------------------------------------
#define ADMP521_OTHER_CTRL_CLKSWTICH				0x01
#define ADMP521_OTHER_CTRL_SetPostTrim				0x02
#define ADMP521_OTHER_CTRL_SelectSDPorAP			0x03
//-------------------------------------------------------------------


#define ADMP521T_REG_NUM 9
#define ADMP521T_ChipAdd  0x28
//Register value define
#define Norm_en 	0x40
#define LR_sel		0x20
#define Norm_T		0x10
#define ANA_T		0x08
#define Fuse_en 	0x02
#define Trim_load	0x04
#define Trim_Sel	0x01


//Define register struct
typedef struct _REG
{
	u32 Address;
	u32 Value;
}REG,*PREG;

typedef struct _REGMAP
{
	REG reg[ADMP521T_REG_NUM];
}REGMAP;






void Initi_ADMP521T(void);
void InitADMP441(bool rw);
void process521TCmd(SDP_USB_HEADER *pUsbHeader);
void TempInit(void);

#endif



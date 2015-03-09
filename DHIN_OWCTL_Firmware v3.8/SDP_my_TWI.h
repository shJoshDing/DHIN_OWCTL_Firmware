//SDP_my_TWI.h
#ifndef _MY_TWI_H_
#define _MY_TWI_H_

/* defines for TWI */
#define PRESCALE_VALUE 8		/* PRESCALE = 80/10 = 8 */
#define RESET_TWI 0				/* RESET_TWI value for controller */
#define CLKDIV_HI 66			/* SCL high period */
#define CLKDIV_LO 67			/* SCL low period */

void Reset_TWI(void);
void TWI_MasterMode_Write(unsigned short DeviceAddr, unsigned char *TWI_Data_Pointer, unsigned short Count, unsigned short TWI_Length);
void TWI_MasterMode_Read(unsigned short DeviceAddr, unsigned char *TWI_Data_Pointer, unsigned short Count);

//void Config2602Regs( void );

#endif

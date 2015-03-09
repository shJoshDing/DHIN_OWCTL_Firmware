//SDP_my_TWI.c
#include <cdefBF527.h>
#include <ccblkfn.h>
#include "SDP_my_TWI.h"

/*******************************************************************
*   Function:    Reset_TWI
*   Description: reset the TWI interface
*******************************************************************/
void Reset_TWI(void)
{
	/* reset TWI controller */
	*pTWI_CONTROL = RESET_TWI;
	ssync();

	/* clear errors before enabling TWI */
	*pTWI_MASTER_STAT = BUFWRERR | BUFRDERR | LOSTARB | ANAK | DNAK;
	ssync();

	/* clear interrupts before enabling TWI */
	*pTWI_INT_STAT = SINIT | SCOMP | SERR | SOVF | MCOMP | MERR | XMTSERV | RCVSERV;
	ssync();

	/* flush rx and tx fifos */
	*pTWI_FIFO_CTL = XMTFLUSH | RCVFLUSH;
	ssync();
}  


/*******************************************************************
*   Function:    TWI_MasterMode_Write
*   Description: do a master mode write
*******************************************************************/
void TWI_MasterMode_Write(unsigned short DeviceAddr, unsigned char *TWI_Data_Pointer, unsigned short Count, unsigned short TWI_Length)
{
	int i, j, timeout;
	
    *pTWI_FIFO_CTL = 0;									/* clear the bit manually */
	*pTWI_CONTROL = TWI_ENA | PRESCALE_VALUE;			/* PRESCALE = fsclk/10MHz */
	*pTWI_CLKDIV = ((CLKDIV_HI) << 8) | (CLKDIV_LO);	/* CLKDIV = (1/SCL)/(1/10MHz) */
	*pTWI_MASTER_ADDR = DeviceAddr;						/* target address (7-bits plus the read/write bit) */

	for (i = 0; i < Count; i++)
	{
	    /* # of configurations to send */
		*pTWI_XMT_DATA8 = *TWI_Data_Pointer++;	/* pointer to data */
		ssync();
		
		*pTWI_MASTER_CTL = (TWI_Length<<6) | MEN /*| FAST*/;	/* start transmission */

		timeout = 0x1000;	// TAR37913
		
		for (j = 0; j < (TWI_Length-1); j++)
		{
		    /* # of transfers before stop condition */
			while ((*pTWI_FIFO_STAT == XMTSTAT) && --timeout)	/* wait to load the next sample into the TX FIFO */ 	// TAR37913
			{
				asm("nop;");									// TAR37913
				asm("nop;");
				asm("nop;");
				ssync();
			}
			if(!timeout)	// TAR37913
				return;

			*pTWI_XMT_DATA8 = *TWI_Data_Pointer++;		/* load the next sample into the TX FIFO */
			ssync();
		}

		while ((*pTWI_INT_STAT & MCOMP) == 0)			/* wait until transmission complete and MCOMP is set */
			ssync();

		*pTWI_INT_STAT = XMTSERV | MCOMP;				/* service TWI for next transmission */
	}

	asm("nop;");
	
	asm("nop;");
	
	asm("nop;");
	
}

/*******************************************************************
*   Function:    TWI_MasterMode_Read
*   Description: do a master mode read
*******************************************************************/
void TWI_MasterMode_Read(unsigned short DeviceAddr, unsigned char *TWI_Data_Pointer, unsigned short Count)
{
	int i, j;
	
    *pTWI_FIFO_CTL = 0;									/* clear the bit manually */
	*pTWI_CONTROL = TWI_ENA | PRESCALE_VALUE;			/* PRESCALE = fsclk/10MHz */
	*pTWI_CLKDIV = ((CLKDIV_HI) << 8) | (CLKDIV_LO);	/* CLKDIV = (1/SCL)/(1/10MHz) */
	*pTWI_MASTER_ADDR = DeviceAddr;						/* target address (7-bits plus the read/write bit) */
		
	*pTWI_MASTER_CTL = (Count<<6) | MEN | MDIR /*| FAST*/;	/* start transmission */
		
	/* for each item */
	for (i = 0; i < Count; i++)
	{ 
		while (*pTWI_FIFO_STAT == RCV_EMPTY)		/* wait for data to be in FIFO */
			ssync();

		*TWI_Data_Pointer++ = *pTWI_RCV_DATA8;		/* read the data */
		ssync();
	}
	
	while ((*pTWI_INT_STAT & MCOMP) == 0)			/* wait until transmission complete and MCOMP is set */
			ssync();

		*pTWI_INT_STAT = RCVSERV | MCOMP;			/* service TWI for next transmission */

	asm("nop;");
	asm("nop;");
	asm("nop;");
}



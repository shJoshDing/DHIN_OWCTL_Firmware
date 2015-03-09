#include "SDP_gpio_i2c.h"
//#include "SDP_general_functions.h"
#include <stdio.h>
#include <cdefBF527.h>
#include <ccblkfn.h>
#include <sys\exception.h>
#include <bfrom.h>
#include "SDP_std_include.h"
#include <services/services.h>          /* system service includes      */ 
//#include <drivers/adi_dev.h>            /* device manager includes      */

//Params & functions definition ---------------------------------------------------------------
/*
u32 I2C_SDA;
u32 I2C_SCL;
u32 I2C_BUS;
*/
// Delay Time (Max I2C = 400 KHz)
// 20 ---> 195 KHz
// 10 ---> 355 KHz
#define DELAY				 250
u16 mytest = 0;
//#define SCL PG12
//#define SDA PG15

/*
#define FUSE_OE				 PH0
#define OE	 PH1
#define DIR_H				 PH5
#define DATA_DIR_H			 PH4
#define CLK_SEL_H			 PH2
#define MUX_SEL_H			 PH7
#define SDP_LR0_H			 PH3
#define SDP_LR1_G			 PG5
*/

//Port F
#define SCL_F				 PF2
#define SDA_F 				 PF0

void i2cStart_F(void);
void i2cStop_F (void);
bool i2cGetAck_F(void);
void i2cPutAck_F(bool isACK);
void i2cSendByte_F(u8 data);
u8   i2cGetByte_F(void);
bool i2c_Open_F(void);
void i2c_Close_F(void);
bool i2c_SendData_F(u8 Addr, u8 *dataBuffer, u8 Size);
bool i2c_GetData_F(u8 Addr, u8 *dataBuffer, u8 Size);

//Port G
#define SCL_G PG12
#define SDA_G PG15

void i2cStart_G(void);
void i2cStop_G(void);
bool i2cGetAck_G(void);
void i2cPutAck_G(bool isACK);
void i2cSendByte_G(u8 data);
u8   i2cGetByte_G(void);
bool i2c_Open_G(void);
void i2c_Close_G(void);
bool i2c_SendData_G(u8 Addr, u8 *dataBuffer, u8 Size);
bool i2c_GetData_G(u8 Addr, u8 *dataBuffer, u8 Size);

//Port H
#define SCL_H PH7
#define SDA_H PH8

void i2cStart_H(void);
void i2cStop_H(void);
bool i2cGetAck_H(void);
void i2cPutAck_H(bool isACK);
void i2cSendByte_H(u8 data);
u8   i2cGetByte_H(void);
bool i2c_Open_H(void);
void i2c_Close_H(void);
bool i2c_SendData_H(u8 Addr, u8 *dataBuffer, u8 Size);
bool i2c_GetData_H(u8 Addr, u8 *dataBuffer, u8 Size);

//PortFH for posttrim mode
void i2cStart_FH(void);
void i2cStop_FH (void);
bool i2cGetAck_FH(void);
void i2cPutAck_FH(bool isACK);
void i2cSendByte_FH(u8 data);
u8   i2cGetByte_FH(void);
bool i2c_Open_FH(void);
void i2c_Close_FH(void);
bool i2c_SendData_FH(u8 Addr, u8 *dataBuffer, u8 Size);
bool i2c_GetData_FH(u8 Addr, u8 *dataBuffer, u8 Size);


void i2cDelay(u16);

#pragma optimize_off
inline void FuseDelay(u16 cnt);


//---------------------------------------------------------------------------------------------


void i2cDelay(u16 cnt)
{
    u16 i;
    for (i=0;i<cnt;i++);
}

inline void FuseDelay(u16 cnt)
{
	//flashLed();
	//while(--cnt > 0);
	

    //u16 i;
    //for (i=0;i<cnt;i++);
    
    //Test----------------------
    if(cnt == 0);   	
	else if (cnt == 1) 
	{
//		NOP();
	}
	else if (cnt == 2) 
	{
//		NOP();
		NOP();
	}
	else if (cnt == 3)
	{
//		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 4) 
	{
//		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 5) 
	{
//		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 6) 
	{
		NOP();
//		NOP();
		NOP();
		NOP();
		NOP();
		NOP();

	}
	else if (cnt == 7) 
	{
		NOP();
//		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 8)
	{
		NOP();
//		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 9)
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 10) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 12) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 13) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 14) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 15) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 16) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 17) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 18) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 19) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 20) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 21) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 22) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 23) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 24) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 25) 
	{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
	}
	else if (cnt == 26) 
	{
		NOP();
	}
	else if (cnt == 27) 
	{
		NOP();
	}
	else if (cnt == 28) 
	{
		NOP();
	}
	else if (cnt == 29) 
	{
		NOP();
	}
	else NOP();
		
	
    	
}

void TestClock(void)
{
/*	if(!i2c_Open())
	{
		printf("Test failed!\r\n");
		return;
	}
*/
 	*pPORTF_FER &= ~(SCL_F);
	*pPORTF_FER &= ~(SDA_F);
	
	*pPORTFIO_INEN &= ~SCL_F;
	*pPORTFIO_INEN &= ~SDA_F;
	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	*pPORTFIO_SET = SCL_F;
	*pPORTFIO_SET = SDA_F;
 	*pPORTFIO_DIR |= SCL_F;
	*pPORTFIO_DIR |= SDA_F;
			
	//printf("Test clock start\r\n");
//	*pPORTFIO_INEN &= ~(SDA_F);
//    *pPORTFIO_DIR |= SDA_F;
//    *pPORTFIO_INEN &= ~SCL_F;
//    *pPORTFIO_DIR |= SCL_F;
    
    u32 i = 0;
    u32 j = 600000;
    //for( j =0 ; j < 60000; j++)
    //{
	    //for(i = 0; i < 600; i++)
	    while(1)
	    {
	    	*pPORTFIO_TOGGLE = SCL_F;
	    	*pPORTFIO_TOGGLE = SDA_F;
	    	//*pPORTFIO_TOGGLE = SCL_F;
    	
	    	//waitHalfMilliSec(1);
	    	waitHalfMicroSec(1);
	    	
	    	//printf("11111\r\n");
	    	//for (i=0;i< 600;i++)
	    	//	i++;
//	    	for(j = 0 ; j < 100; j++)
//	    	{
//	    		i2cDelay(DELAY);
//	    	}
    	
/*
	    	*PORTFIO_TOGGLE = SDA_F;
	    	*PORTFIO_TOGGLE = SCL_F;
	    	for(j = 0 ; j < 100; j++)
	    	{
	    		i2cDelay(DELAY);
	    	}
*/
	  	 }
    //}
	
}

//-----------------------------------Port G interface-------------------------
/* -------------------------------------------------------------------------*/
/* Function   : i2c_Open                                                    */
/* Description: Judge if defines GPIO as I2C bus and if the GPIO used right.*/
/* Parameters : None                                                        */
/* Return     : TRUE if OK.                                                 */
/*              FALSE if Wrong.                                             */
/* -------------------------------------------------------------------------*/
bool i2c_Open_G(void)
{
    *pPORTG_FER &= ~(SCL_G);
	*pPORTG_FER &= ~(SDA_G);
	
	*pPORTGIO_INEN &= ~SCL_G;
	*pPORTGIO_INEN &= ~SDA_G;
	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	*pPORTGIO_SET = SCL_G;
	*pPORTGIO_SET = SDA_G;
 	*pPORTGIO_DIR |= SCL_G;
	*pPORTGIO_DIR |= SDA_G;
	
	//printf("Open I2C Bus.\n");
	
	i2cDelay(4*DELAY);
	
	//*pPORTGIO_CLEAR = SCL_G;
	//*pPORTGIO_CLEAR = SDA_G;
	
	//printf("Open I2C Bus.\n");
   
     return(TRUE);
    //}
}

/* -------------------------------------------------------------------------*/
/* Function   : i2c_Close                                                   */
/* Description: Close gpio I2C Bus to idle.                                 */
/* Parameters : None                                                        */
/* Return     : None                                                        */
/* -------------------------------------------------------------------------*/
void i2c_Close_G(void)
{

    //*pPORTGIO_DIR = 0x00;
	//*pPORTGIO_DIR = 0x00;
	
	*pPORTGIO_CLEAR |= SCL_G;
	*pPORTGIO_CLEAR |= SDA_G;
	
	i2cDelay(DELAY);
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_SendData                                                *
 * Description: Send I2C data format.                                       *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be sent                       *
 *              Size       -- Data size will be sent                        *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_SendData_G(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8  bufSize = Size;
   
/*
    i2cStart();
    i2cSendByte(Addr<<1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
*/  
    do
    {
		i2cStart_G();
		i2cSendByte_G(Addr<<1);
		if (!i2cGetAck_G())
		{
	    	i2cStop_G();
	    	return FALSE;
		}
    		
        i2cSendByte_G(*dataBuffer++);
        if (!i2cGetAck_G())
        {
         	i2cStop_G();
         	return FALSE;
        }
        
        i2cSendByte_G(*dataBuffer++);
        if (!i2cGetAck_G())
        {
         	i2cStop_G();
         	return FALSE;
        }
        i2cStop_G();
    }
    while((--bufSize)!=0);
   
    //Set to In
    *pPORTGIO_INEN |= SDA_G;
  	*pPORTGIO_DIR &= ~SDA_G;
  	
  	*pPORTGIO_INEN |= SCL_G;
  	*pPORTGIO_DIR &= ~SCL_G;
    
    //i2cStop();
    return TRUE;
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_GetData                                                 *
 * Description: Get I2C data from I2C Slave.                                *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be stored                     *
 *              Size       -- Data size will be stored                      *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_GetData_G(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8 bufSize = Size;
   
  /*  
    i2cStart();
    i2cSendByte((Addr<<1)|0x1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
  */ 
    //i2cSendByte( *(dataBuffer));
    
    u8 i = 0;
   
    do
    {
		i2cStart_G();
		i2cSendByte_G(Addr<<1);
		if (!i2cGetAck_G())
		{
			i2cStop_G();
			return FALSE;
		}
		i2cSendByte_G( *(dataBuffer+i*2));
		if (!i2cGetAck_G())
		{
			i2cStop_G();
			return FALSE;
		}
		i2cStop_G();

		i2cDelay(DELAY);

		i2cStart_G();
		i2cSendByte_G((Addr<<1)|0x1);
		if (!i2cGetAck_G())
		{
			i2cStop_G();
			return FALSE;
		}

	    *(dataBuffer+i*2+1) = i2cGetByte_G();
	    
	    i2cPutAck_G(TRUE);
     
	    i++;
     
	    i2cStop_G();
	    i2cDelay(2*DELAY);
    }
    while((--bufSize)!=0);
   
    //*dataBuffer = i2cGetByte();
    //i2cPutAck(FALSE);
   
    //i2cStop();
    return (TRUE);
}

// I2C Internal API

/* -------------------------------------------------------------------------*
 * Function   : i2cStart                                                    *
 * Description: SDA_G change from high to low when SCL_G keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStart_G(void)
{
  //*pPORTGIO_DIR = SCL_G;
	//*pPORTGIO_DIR = SDA_G;
  //*pPORTGIO_SET |= SCL_G;
  *pPORTGIO_INEN &= ~(SDA_G);
  *pPORTGIO_DIR |= SDA_G;
  //*pPORTGIO_SET = SCL_G;
  i2cDelay(DELAY/2);
  *pPORTGIO_SET = SDA_G;
  i2cDelay(DELAY/2);
  *pPORTGIO_SET = SCL_G;
  
  i2cDelay(DELAY);
	
  *pPORTGIO_CLEAR = SDA_G;
  
  i2cDelay(DELAY/2);
	
  *pPORTGIO_CLEAR = SCL_G;
  
   i2cDelay(DELAY/2);
  
}

/* -------------------------------------------------------------------------*
 * Function   : i2cStop                                                     *
 * Description: SDA_G change from low to high when SCL_G keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStop_G(void)
{
    // After STOP, I2C_BUS goes into IDLE state.
    *pPORTGIO_INEN &= ~(SDA_G);
    *pPORTGIO_DIR |= SDA_G;
    //*pPORTGIO_DIR = SCL_G;
    if(*pPORTGIO & SDA_G)
	{
		//i2cDelay(DELAY/2);
		*pPORTGIO_CLEAR = SCL_G;	
		i2cDelay(DELAY/2);
		*pPORTGIO_CLEAR = SDA_G;
		i2cDelay(DELAY/2);
		*pPORTGIO_SET = SCL_G;
		i2cDelay(DELAY/2);
		*pPORTGIO_SET = SDA_G;
		i2cDelay(DELAY);
	}
	else
	{
	    //*pPORTGIO_CLEAR = SDA_G;
	    i2cDelay(DELAY/2);
	    *pPORTGIO_SET = SCL_G;
	    i2cDelay(DELAY/2);
		*pPORTGIO_SET = SDA_G;
		i2cDelay(DELAY);
	}
}

/* -------------------------------------------------------------------------*
 * Function   : i2cSendByte                                                 *
 * Description: Send the byte output of gpio, MSB first.                    *
 * Parameters : data to be shifted.                                         *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cSendByte_G(u8 data)
{
    u8 cnt = 8;
    //hal_gpio_SetOut(I2C_SDA_G);
    *pPORTGIO_INEN &= ~(SDA_G);
    *pPORTGIO_DIR |= SDA_G;
    *pPORTGIO_DIR |= SCL_G;
   
    do
    {
        if (data & 0x80)
        {
            *pPORTGIO_SET = SDA_G;
        }
        else
        {
            //hal_gpio_ClrBit(I2C_SDA_G);
            *pPORTGIO_CLEAR = SDA_G;
        }
        data <<= 1;
        i2cDelay(DELAY/2);
        //hal_gpio_SetBit(I2C_SCL_G);
        *pPORTGIO_SET = SCL_G;
        i2cDelay(DELAY);
        //*pPORTGIO_CLEAR = SDA_G;
        //i2cDelay(DELAY/2);
        //hal_gpio_ClrBit(I2C_SCL_G);
        *pPORTGIO_CLEAR = SCL_G;
        i2cDelay(DELAY/4);
    }
    while (--cnt != 0);
    //*pPORTGIO_CLEAR = SDA_G;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetByte                                                  *
 * Description: Get the Data from slave device                              *
 * Parameters : None                                                        *
 * Return     : Received data                                               *
 * -------------------------------------------------------------------------*/
u8 i2cGetByte_G(void)
{
    u8 cnt = 8;
    u8 data = 0;
   
//    hal_gpio_SetOut(I2C_SDA_G);
//    hal_gpio_SetBit(I2C_SDA_G); // Should pull up SDA_G before read.
//    i2cDelay(DELAY);


    //*pPORTGIO_DIR |= SDA_G;
    //*pPORTGIO_INEN &= ~(SDA_G);
		//*pPORTGIO_SET = SDA_G;
		//i2cDelay(DELAY);
   
    // // Set GPIO as input
    //*pPORTGIO_INEN |= SDA_G;
    *pPORTGIO_SET = SDA_G;
    *pPORTGIO_DIR &= ~(SDA_G);
    *pPORTGIO_INEN |= SDA_G;
    //*pPORTGIO_SET = SDA_G;
    //i2cDelay(DELAY/2);
   
    do
    {
	     i2cDelay(DELAY/2);
	     *pPORTGIO_SET = SCL_G;
	     i2cDelay(DELAY/2);
     
	     data <<= 1;
	     if (*pPORTGIO & SDA_G)
	     		data++; // Get input data bit
     
	     i2cDelay(DELAY/2);
	     *pPORTGIO_CLEAR = SCL_G;
	     i2cDelay(DELAY/2);
    }
    while (--cnt != 0);
   
    return(data);
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetAck                                                   *
 * Description: During SCL_G high, when SDA_G holds low is ACK otherwise NACK.  *
 * Parameters : No                                                          *
 * Return     : TRUE is ACK, FALSE is NACK.                                 *
 * Notes      : This function gets the ACK signal from the slave.           *
 * -------------------------------------------------------------------------*/
bool i2cGetAck_G(void)
{
    bool isACK;
   
    //*pPORTGIO_INEN = SDA_G;
    //*pPORTGIO_CLEAR = SDA_G;
    *pPORTGIO_SET = SDA_G;
    *pPORTGIO_DIR &= ~(SDA_G);
    *pPORTGIO_INEN = SDA_G;
    *pPORTGIO_CLEAR = SCL_G;
    
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SCL_G);
    *pPORTGIO_SET = SCL_G;
    
    i2cDelay(DELAY/2);
   
    // Read Line at the middle of input, it is stable.
    //*pPORTGIO_INEN |= SDA_G;
    if (*pPORTGIO & SDA_G) 
    	isACK = FALSE;
    else                          
    	isACK = TRUE;
   
    i2cDelay(DELAY/2);
    //hal_gpio_ClrBit(I2C_SCL_G);
    *pPORTGIO_CLEAR = SCL_G;
    i2cDelay(DELAY/2);
   
    return isACK;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cPutAck                                                   *
 * Description: During SCL_G high, when SDA_G holds low is ACK otherwise NACK.  *
 * Parameters : TRUE is ACK, FALSE is NACK                                  *
 * Return     :                                                             *
 * Notes      : This function send ACK or NACK to slave device              *
 * -------------------------------------------------------------------------*/
void i2cPutAck_G(bool isACK)
{
    //hal_gpio_SetOut(I2C_SDA_G);
    *pPORTGIO_INEN &= ~(SDA_G);
    *pPORTGIO_DIR |= SDA_G;
   
    if (!isACK) 
    	*pPORTGIO_CLEAR = SDA_G;
    else      
    	*pPORTGIO_SET = SDA_G;
   
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SCL_G);
    *pPORTGIO_SET = SCL_G;
    i2cDelay(DELAY);
    //hal_gpio_ClrBit(I2C_SCL_G);
    *pPORTGIO_CLEAR = SCL_G;
    i2cDelay(DELAY/2);
    *pPORTGIO_CLEAR = SDA_G;
}

/*/-----------------------------------------------------------------------------
void waitHalfMilliSec(u32 msec)
{
	volatile u64 cur;
	volatile u64 nd;
	
	_GET_CYCLE_COUNT(cur);
	//original is nd = cur + ((__PROCESSOR_SPEED__/1000)* msec); and modified by test on oscilloscope.
	nd = cur + ((__PROCESSOR_SPEED__/668)* msec);
	//nd = cur + (__PROCESSOR_SPEED__ * msec);
	while (cur < nd)
	{
		_GET_CYCLE_COUNT(cur);
	}
}

void waitHalfMicroSec(u32 msec)
{
	volatile u64 cur;
	volatile u64 nd;
	
	_GET_CYCLE_COUNT(cur);
	nd = cur + ((__PROCESSOR_SPEED__/(668 * 1000))* msec);
	//nd = cur + (__PROCESSOR_SPEED__ * msec);
	while (cur < nd)
	{
		_GET_CYCLE_COUNT(cur);
	}
}
//
*/     
//-----------------------------------Port F interface-------------------------
/* -------------------------------------------------------------------------*/
/* Function   : i2c_Open                                                    */
/* Description: Judge if defines GPIO as I2C bus and if the GPIO used right.*/
/* Parameters : None                                                        */
/* Return     : TRUE if OK.                                                 */
/*              FALSE if Wrong.                                             */
/* -------------------------------------------------------------------------*/
bool i2c_Open_F(void)
{
    *pPORTF_FER &= ~(SCL_F);
	*pPORTF_FER &= ~(SDA_F);
	
	*pPORTFIO_INEN &= ~SCL_F;
	*pPORTFIO_INEN &= ~SDA_F;
	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	*pPORTFIO_SET = SCL_F;
	*pPORTFIO_SET = SDA_F;
 	*pPORTFIO_DIR |= SCL_F;
	*pPORTFIO_DIR |= SDA_F;
	
	//printf("Open I2C Bus.\n");
	
	i2cDelay(10*DELAY);
	
	//*pPORTFIO_CLEAR = SCL_F;
	//*pPORTFIO_CLEAR = SDA_F;
	
	//printf("Open I2C Bus.\n");
   
     return(TRUE);
    //}
}

/* -------------------------------------------------------------------------*/
/* Function   : i2c_Close                                                   */
/* Description: Close gpio I2C Bus to idle.                                 */
/* Parameters : None                                                        */
/* Return     : None                                                        */
/* -------------------------------------------------------------------------*/
void i2c_Close_F(void)
{

    //*pPORTFIO_DIR = 0x00;
	//*pPORTFIO_DIR = 0x00;
	
	*pPORTFIO_CLEAR |= SCL_F;
	*pPORTFIO_CLEAR |= SDA_F;
	
	i2cDelay(DELAY);
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_SendData                                                *
 * Description: Send I2C data format.                                       *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be sent                       *
 *              Size       -- Data size will be sent                        *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_SendData_F(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8  bufSize = Size;
   
/*
    i2cStart();
    i2cSendByte(Addr<<1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
*/     
	//SetPin_H(DATA_DIR_H, true);		//Set Level Shift hardware, make sure it work on output status.
    do
    {
		i2cStart_F();
		i2cSendByte_F(Addr<<1);
		if (!i2cGetAck_F())
		{
	    	i2cStop_F();
	    	return FALSE;
		}
    		
        i2cSendByte_F(*dataBuffer++);
        if (!i2cGetAck_F())
        {
         	i2cStop_F();
         	return FALSE;
        }
        
        i2cSendByte_F(*dataBuffer++);
        if (!i2cGetAck_F())
        {
         	i2cStop_F();
         	return FALSE;
        }
        i2cStop_F();
    }
    while((--bufSize)!=0);
    
    //i2cStop();
    return TRUE;
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_GetData                                                 *
 * Description: Get I2C data from I2C Slave.                                *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be stored                     *
 *              Size       -- Data size will be stored                      *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_GetData_F(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8 bufSize = Size;
   
  /*  
    i2cStart();
    i2cSendByte((Addr<<1)|0x1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
  */ 
    //i2cSendByte( *(dataBuffer));
    //SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    u8 i = 0;
   
    do
    {
		i2cStart_F();
		i2cSendByte_F(Addr<<1);
		if (!i2cGetAck_F())
		{
			i2cStop_F();
			return FALSE;
		}
		i2cSendByte_F( *(dataBuffer+i*2));
		if (!i2cGetAck_F())
		{
			i2cStop_F();
			return FALSE;
		}
		i2cStop_F();

		i2cDelay(DELAY);

		i2cStart_F();
		i2cSendByte_F((Addr<<1)|0x1);
		if (!i2cGetAck_F())
		//if(0)
		{
			i2cStop_F();
			return FALSE;
		}

	    *(dataBuffer+i*2+1) = i2cGetByte_F();
	    
	    i2cPutAck_F(TRUE);
     
	    i++;
     
	    i2cStop_F();
	    i2cDelay(2*DELAY);
    }
    while((--bufSize)!=0);
   
    //*dataBuffer = i2cGetByte();
    //i2cPutAck(FALSE);
   
    //i2cStop();
	//SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    return (TRUE);
}

// I2C Internal API

/* -------------------------------------------------------------------------*
 * Function   : i2cStart                                                    *
 * Description: SDA_F change from high to low when SCL_F keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStart_F(void)
{
  //*pPORTFIO_DIR = SCL_F;
	//*pPORTFIO_DIR = SDA_F;
  //*pPORTFIO_SET |= SCL_F;
  *pPORTFIO_INEN &= ~(SDA_F);
  *pPORTFIO_DIR |= SDA_F;
  //*pPORTFIO_SET = SCL_F;
  i2cDelay(DELAY/2);
  *pPORTFIO_SET = SDA_F;
  i2cDelay(DELAY/2);
  *pPORTFIO_SET = SCL_F;
  
  i2cDelay(DELAY);
	
  *pPORTFIO_CLEAR = SDA_F;
  
  i2cDelay(DELAY/2);
	
  *pPORTFIO_CLEAR = SCL_F;
  
   i2cDelay(DELAY/2);
  
}

/* -------------------------------------------------------------------------*
 * Function   : i2cStop                                                     *
 * Description: SDA_F change from low to high when SCL_F keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStop_F(void)
{
    // After STOP, I2C_BUS goes into IDLE state.
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
    //*pPORTFIO_DIR = SCL_F;
    if(*pPORTFIO & SDA_F)
	{
		//i2cDelay(DELAY/2);
		*pPORTFIO_CLEAR = SCL_F;	
		i2cDelay(DELAY/2);
		*pPORTFIO_CLEAR = SDA_F;
		i2cDelay(DELAY/2);
		*pPORTFIO_SET = SCL_F;
		i2cDelay(DELAY/2);
		*pPORTFIO_SET = SDA_F;
		i2cDelay(DELAY);
	}
	else
	{
	    //*pPORTFIO_CLEAR = SDA_F;
	    i2cDelay(DELAY/2);
	    *pPORTFIO_SET = SCL_F;
	    i2cDelay(DELAY/2);
		*pPORTFIO_SET = SDA_F;
		i2cDelay(DELAY);
	}
}

/* -------------------------------------------------------------------------*
 * Function   : i2cSendByte                                                 *
 * Description: Send the byte output of gpio, MSB first.                    *
 * Parameters : data to be shifted.                                         *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cSendByte_F(u8 data)
{
    u8 cnt = 8;
    //hal_gpio_SetOut(I2C_SDA_F);
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
    *pPORTFIO_DIR |= SCL_F;
   
    do
    {
        if (data & 0x80)
        {
            *pPORTFIO_SET = SDA_F;
        }
        else
        {
            //hal_gpio_ClrBit(I2C_SDA_F);
            *pPORTFIO_CLEAR = SDA_F;
        }
        data <<= 1;
        i2cDelay(DELAY/2);
        //hal_gpio_SetBit(I2C_SCL_F);
        *pPORTFIO_SET = SCL_F;
        i2cDelay(DELAY);
        //*pPORTFIO_CLEAR = SDA_F;
        //i2cDelay(DELAY/2);
        //hal_gpio_ClrBit(I2C_SCL_F);
        *pPORTFIO_CLEAR = SCL_F;
        i2cDelay(DELAY/4);
    }
    while (--cnt != 0);
    //*pPORTFIO_CLEAR = SDA_F;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetByte                                                  *
 * Description: Get the Data from slave device                              *
 * Parameters : None                                                        *
 * Return     : Received data                                               *
 * -------------------------------------------------------------------------*/
u8 i2cGetByte_F(void)
{
    u8 cnt = 8;
    u8 data = 0;
   
//    hal_gpio_SetOut(I2C_SDA_F);
//    hal_gpio_SetBit(I2C_SDA_F); // Should pull up SDA_F before read.
//    i2cDelay(DELAY);


    //*pPORTFIO_DIR |= SDA_F;
    //*pPORTFIO_INEN &= ~(SDA_F);
		//*pPORTFIO_SET = SDA_F;
		//i2cDelay(DELAY);
   
    // // Set GPIO as input
    //*pPORTFIO_INEN |= SDA_F;
    
    //--------------------------------------------------
	SetPin_H(DATA_DIR_H, false);		//Set Level Shift hardware, make sure it work on GPI status.
	//ADMP521T_CLK_OE(true);
    *pPORTFIO_SET = SDA_F;
    *pPORTFIO_DIR &= ~(SDA_F);
    *pPORTFIO_INEN |= SDA_F;
    //*pPORTFIO_SET = SDA_F;
    //i2cDelay(DELAY/2);
   
    do
    {
	     i2cDelay(DELAY/2);
	     *pPORTFIO_SET = SCL_F;
	     i2cDelay(DELAY/2);
     
	     data <<= 1;
	     if (*pPORTFIO & SDA_F)
	     	data++; // Get input data bit
     
	     i2cDelay(DELAY/2);
	     *pPORTFIO_CLEAR = SCL_F;
	     i2cDelay(DELAY/2);
    }
    while (--cnt != 0);
    
    SetPin_H(DATA_DIR_H, true);		//Set Level Shift hardware
    //ADMP521T_CLK_OE(false);
    return(data);
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetAck                                                   *
 * Description: During SCL_F high, when SDA_F holds low is ACK otherwise NACK.  *
 * Parameters : No                                                          *
 * Return     : TRUE is ACK, FALSE is NACK.                                 *
 * Notes      : This function gets the ACK signal from the slave.           *
 * -------------------------------------------------------------------------*/
bool i2cGetAck_F(void)
{
    bool isACK;
   
    //*pPORTFIO_INEN = SDA_F;
    //*pPORTFIO_CLEAR = SDA_F;
    //*pPORTFIO_SET = SDA_F;
    SetPin_H(DATA_DIR_H, false);		//Set Level Shift hardware
    *pPORTFIO_DIR &= ~(SDA_F);
    *pPORTFIO_INEN = SDA_F;
    *pPORTFIO_CLEAR = SCL_F;
    
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SCL_F);
    *pPORTFIO_SET = SCL_F;
    
    i2cDelay(DELAY/2);
   
    // Read Line at the middle of input, it is stable.
    //*pPORTFIO_INEN |= SDA_F;
    if (*pPORTFIO & SDA_F) 
    	isACK = FALSE;
    else                          
    	isACK = TRUE;
   
    i2cDelay(DELAY/2);
    SetPin_H(DATA_DIR_H, true);		//Set Level Shift hardware
    
    //hal_gpio_ClrBit(I2C_SCL_F);
    *pPORTFIO_CLEAR = SCL_F;
    
/*  i2cDelay(DELAY/4);
    //Test use-----------------------------------------
    *pPORTFIO_SET = SDA_F;			//This is Just test
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
    *pPORTFIO_SET = SDA_F;			//This is Just test
*/

    i2cDelay(DELAY/2);
    
    return isACK;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cPutAck                                                   *
 * Description: During SCL_F high, when SDA_F holds low is ACK otherwise NACK.  *
 * Parameters : TRUE is ACK, FALSE is NACK                                  *
 * Return     :                                                             *
 * Notes      : This function send ACK or NACK to slave device              *
 * -------------------------------------------------------------------------*/
void i2cPutAck_F(bool isACK)
{
    //hal_gpio_SetOut(I2C_SDA_F);
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
   
    if (!isACK) 
    		*pPORTFIO_CLEAR = SDA_F;
    else      
    		*pPORTFIO_SET = SDA_F;
   
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SCL_F);
    *pPORTFIO_SET = SCL_F;
    i2cDelay(DELAY);
    //hal_gpio_ClrBit(I2C_SCL_F);
    *pPORTFIO_CLEAR = SCL_F;
    i2cDelay(DELAY/2);
    *pPORTFIO_CLEAR = SDA_F;
}

//-----------------------------------Port H interface-------------------------
/* -------------------------------------------------------------------------*/
/* Function   : i2c_Open                                                    */
/* Description: Judge if defines GPIO as I2C bus and if the GPIO used right.*/
/* Parameters : None                                                        */
/* Return     : TRUE if OK.                                                 */
/*              FALSE if Wrong.                                             */
/* -------------------------------------------------------------------------*/
bool i2c_Open_H(void)
{
    *pPORTH_FER &= ~(SCL_H);
	*pPORTH_FER &= ~(SDA_H);
	
	*pPORTHIO_INEN &= ~SCL_H;
	*pPORTHIO_INEN &= ~SDA_H;
	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	*pPORTHIO_SET = SCL_H;
	*pPORTHIO_SET = SDA_H;
 	*pPORTHIO_DIR |= SCL_H;
	*pPORTHIO_DIR |= SDA_H;
	
	//printf("Open I2C Bus.\n");
	
	i2cDelay(4*DELAY);
	
	//*pPORTHIO_CLEAR = SCL_H;
	//*pPORTHIO_CLEAR = SDA_H;
	
	//printf("Open I2C Bus.\n");
   
     return(TRUE);
    //}
}

/* -------------------------------------------------------------------------*/
/* Function   : i2c_Close                                                   */
/* Description: Close gpio I2C Bus to idle.                                 */
/* Parameters : None                                                        */
/* Return     : None                                                        */
/* -------------------------------------------------------------------------*/
void i2c_Close_H(void)
{

    //*pPORTHIO_DIR = 0x00;
	//*pPORTHIO_DIR = 0x00;
	
	*pPORTHIO_CLEAR |= SCL_H;
	*pPORTHIO_CLEAR |= SDA_H;
	
	i2cDelay(DELAY);
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_SendData                                                *
 * Description: Send I2C data format.                                       *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be sent                       *
 *              Size       -- Data size will be sent                        *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_SendData_H(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8  bufSize = Size;
   
/*
    i2cStart();
    i2cSendByte(Addr<<1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
*/  
    do
    {
		i2cStart_H();
		i2cSendByte_H(Addr<<1);
		if (!i2cGetAck_H())
		{
	    	i2cStop_H();
	    	return FALSE;
		}
    		
        i2cSendByte_H(*dataBuffer++);
        if (!i2cGetAck_H())
        {
         	i2cStop_H();
         	return FALSE;
        }
        
        i2cSendByte_H(*dataBuffer++);
        if (!i2cGetAck_H())
        {
         	i2cStop_H();
         	return FALSE;
        }
        i2cStop_H();
    }
    while((--bufSize)!=0);
   
    //i2cStop();
    return TRUE;
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_GetData                                                 *
 * Description: Get I2C data from I2C Slave.                                *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be stored                     *
 *              Size       -- Data size will be stored                      *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_GetData_H(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8 bufSize = Size;
   
  /*  
    i2cStart();
    i2cSendByte((Addr<<1)|0x1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
  */ 
    //i2cSendByte( *(dataBuffer));
    
    u8 i = 0;
   
    do
    {
		i2cStart_H();
		i2cSendByte_H(Addr<<1);
		if (!i2cGetAck_H())
		{
			i2cStop_H();
			return FALSE;
		}
		i2cSendByte_H( *(dataBuffer+i*2));
		if (!i2cGetAck_H())
		{
			i2cStop_H();
			return FALSE;
		}
		i2cStop_H();

		i2cDelay(DELAY);

		i2cStart_H();
		i2cSendByte_H((Addr<<1)|0x1);
		if (!i2cGetAck_H())
		{
			i2cStop_H();
			return FALSE;
		}

	    *(dataBuffer+i*2+1) = i2cGetByte_H();
	    
	    i2cPutAck_H(TRUE);
     
	    i++;
     
	    i2cStop_H();
	    i2cDelay(2*DELAY);
    }
    while((--bufSize)!=0);
   
    //*dataBuffer = i2cGetByte();
    //i2cPutAck(FALSE);
   
    //i2cStop();
    return (TRUE);
}

// I2C Internal API

/* -------------------------------------------------------------------------*
 * Function   : i2cStart                                                    *
 * Description: SDA_H change from high to low when SCL_H keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStart_H(void)
{
  //*pPORTHIO_DIR = SCL_H;
	//*pPORTHIO_DIR = SDA_H;
  //*pPORTHIO_SET |= SCL_H;
  *pPORTHIO_INEN &= ~(SDA_H);
  *pPORTHIO_DIR |= SDA_H;
  //*pPORTHIO_SET = SCL_H;
  i2cDelay(DELAY/2);
  *pPORTHIO_SET = SDA_H;
  i2cDelay(DELAY/2);
  *pPORTHIO_SET = SCL_H;
  
  i2cDelay(DELAY);
	
  *pPORTHIO_CLEAR = SDA_H;
  
  i2cDelay(DELAY/2);
	
  *pPORTHIO_CLEAR = SCL_H;
  
	i2cDelay(DELAY/2);
  
}

/* -------------------------------------------------------------------------*
 * Function   : i2cStop                                                     *
 * Description: SDA_H change from low to high when SCL_H keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStop_H(void)
{
    // After STOP, I2C_BUS goes into IDLE state.
    *pPORTHIO_INEN &= ~(SDA_H);
    *pPORTHIO_DIR |= SDA_H;
    //*pPORTHIO_DIR = SCL_H;
    if(*pPORTHIO & SDA_H)
	{
		//i2cDelay(DELAY/2);
		*pPORTHIO_CLEAR = SCL_H;	
		i2cDelay(DELAY/2);
		*pPORTHIO_CLEAR = SDA_H;
		i2cDelay(DELAY/2);
		*pPORTHIO_SET = SCL_H;
		i2cDelay(DELAY/2);
		*pPORTHIO_SET = SDA_H;
		i2cDelay(DELAY);
	}
	else
	{
	    //*pPORTHIO_CLEAR = SDA_H;
	    i2cDelay(DELAY/2);
	    *pPORTHIO_SET = SCL_H;
	    i2cDelay(DELAY/2);
		*pPORTHIO_SET = SDA_H;
		i2cDelay(DELAY);
	}
}

/* -------------------------------------------------------------------------*
 * Function   : i2cSendByte                                                 *
 * Description: Send the byte output of gpio, MSB first.                    *
 * Parameters : data to be shifted.                                         *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cSendByte_H(u8 data)
{
    u8 cnt = 8;
    //hal_gpio_SetOut(I2C_SDA_H);
    *pPORTHIO_INEN &= ~(SDA_H);
    *pPORTHIO_DIR |= SDA_H;
    *pPORTHIO_DIR |= SCL_H;
   
    do
    {
        if (data & 0x80)
        {
            *pPORTHIO_SET = SDA_H;
        }
        else
        {
            //hal_gpio_ClrBit(I2C_SDA_H);
            *pPORTHIO_CLEAR = SDA_H;
        }
        data <<= 1;
        i2cDelay(DELAY/2);
        //hal_gpio_SetBit(I2C_SCL_H);
        *pPORTHIO_SET = SCL_H;
        i2cDelay(DELAY);
        //*pPORTHIO_CLEAR = SDA_H;
        //i2cDelay(DELAY/2);
        //hal_gpio_ClrBit(I2C_SCL_H);
        *pPORTHIO_CLEAR = SCL_H;
        i2cDelay(DELAY/4);
    }
    while (--cnt != 0);
    //*pPORTHIO_CLEAR = SDA_H;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetByte                                                  *
 * Description: Get the Data from slave device                              *
 * Parameters : None                                                        *
 * Return     : Received data                                               *
 * -------------------------------------------------------------------------*/
u8 i2cGetByte_H(void)
{
    u8 cnt = 8;
    u8 data = 0;
   
//    hal_gpio_SetOut(I2C_SDA_H);
//    hal_gpio_SetBit(I2C_SDA_H); // Should pull up SDA_H before read.
//    i2cDelay(DELAY);


    //*pPORTHIO_DIR |= SDA_H;
    //*pPORTHIO_INEN &= ~(SDA_H);
		//*pPORTHIO_SET = SDA_H;
		//i2cDelay(DELAY);
   
    // // Set GPIO as input
    //*pPORTHIO_INEN |= SDA_H;
    *pPORTHIO_SET = SDA_H;
    *pPORTHIO_DIR &= ~(SDA_H);
    *pPORTHIO_INEN |= SDA_H;
    //*pPORTHIO_SET = SDA_H;
    //i2cDelay(DELAY/2);
   
    do
    {
	     i2cDelay(DELAY/2);
	     *pPORTHIO_SET = SCL_H;
	     i2cDelay(DELAY/2);
     
	     data <<= 1;
	     if (*pPORTHIO & SDA_H)
	     	data++; // Get input data bit
     
	     i2cDelay(DELAY/2);
	     *pPORTHIO_CLEAR = SCL_H;
	     i2cDelay(DELAY/2);
    }
    while (--cnt != 0);
   
    return(data);
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetAck                                                   *
 * Description: During SCL_H high, when SDA_H holds low is ACK otherwise NACK.  *
 * Parameters : No                                                          *
 * Return     : TRUE is ACK, FALSE is NACK.                                 *
 * Notes      : This function gets the ACK signal from the slave.           *
 * -------------------------------------------------------------------------*/
bool i2cGetAck_H(void)
{
    bool isACK;
   
    //*pPORTHIO_INEN = SDA_H;
    //*pPORTHIO_CLEAR = SDA_H;
    *pPORTHIO_SET = SDA_H;
    *pPORTHIO_DIR &= ~(SDA_H);
    *pPORTHIO_INEN = SDA_H;
    *pPORTHIO_CLEAR = SCL_H;
    
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SCL_H);
    *pPORTHIO_SET = SCL_H;
    
    i2cDelay(DELAY/2);
   
    // Read Line at the middle of input, it is stable.
    //*pPORTHIO_INEN |= SDA_H;
    if (*pPORTHIO & SDA_H) 
    	isACK = FALSE;
    else                          
    	isACK = TRUE;
   
    i2cDelay(DELAY/2);
    //hal_gpio_ClrBit(I2C_SCL_H);
    *pPORTHIO_CLEAR = SCL_H;
    i2cDelay(DELAY/2);
   
    return isACK;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cPutAck                                                   *
 * Description: During SCL_H high, when SDA_H holds low is ACK otherwise NACK.  *
 * Parameters : TRUE is ACK, FALSE is NACK                                  *
 * Return     :                                                             *
 * Notes      : This function send ACK or NACK to slave device              *
 * -------------------------------------------------------------------------*/
void i2cPutAck_H(bool isACK)
{
    //hal_gpio_SetOut(I2C_SDA_H);
    *pPORTHIO_INEN &= ~(SDA_H);
    *pPORTHIO_DIR |= SDA_H;
   
    if (!isACK) 
    	*pPORTHIO_CLEAR = SDA_H;
    else      
    	*pPORTHIO_SET = SDA_H;
   
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SCL_H);
    *pPORTHIO_SET = SCL_H;
    i2cDelay(DELAY);
    //hal_gpio_ClrBit(I2C_SCL_H);
    *pPORTHIO_CLEAR = SCL_H;
    i2cDelay(DELAY/2);
    *pPORTHIO_CLEAR = SDA_H;
}

//-----------------------------------------------------------------------------------------


//-------- Port F and H interface(For post trim mode).SDP_LR0_H is clk and SDA_F is SDA------------
/* -------------------------------------------------------------------------*/
/* Function   : i2c_Open                                                    */
/* Description: Judge if defines GPIO as I2C bus and if the GPIO used right.*/
/* Parameters : None                                                        */
/* Return     : TRUE if OK.                                                 */
/*              FALSE if Wrong.                                             */
/* -------------------------------------------------------------------------*/
bool i2c_Open_FH(void)
{
    *pPORTH_FER &= ~(SDP_LR0_H);
	*pPORTF_FER &= ~(SDA_F);
	
	*pPORTHIO_INEN &= ~SDP_LR0_H;
	*pPORTFIO_INEN &= ~SDA_F;
	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	*pPORTHIO_SET = SDP_LR0_H;
	*pPORTFIO_SET = SDA_F;
 	*pPORTHIO_DIR |= SDP_LR0_H;
	*pPORTFIO_DIR |= SDA_F;
	
	//printf("Open I2C Bus.\n");
	
	i2cDelay(4*DELAY);
	
	//*pPORTHIO_CLEAR = SDP_LR0_H;
	//*pPORTFIO_CLEAR = SDA_F;
	
	//printf("Open I2C Bus.\n");
   
     return(TRUE);
    //}
}

/* -------------------------------------------------------------------------*/
/* Function   : i2c_Close                                                   */
/* Description: Close gpio I2C Bus to idle.                                 */
/* Parameters : None                                                        */
/* Return     : None                                                        */
/* -------------------------------------------------------------------------*/
void i2c_Close_FH(void)
{

    //*pPORTHIO_DIR = 0x00;
	//*pPORTFIO_DIR = 0x00;
	
	*pPORTHIO_CLEAR |= SDP_LR0_H;
	*pPORTFIO_CLEAR |= SDA_F;
	
	i2cDelay(DELAY);
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_SendData                                                *
 * Description: Send I2C data format.                                       *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be sent                       *
 *              Size       -- Data size will be sent                        *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_SendData_FH(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8  bufSize = Size;
   
/*
    i2cStart();
    i2cSendByte(Addr<<1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
*/  
	SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    do
    {
		i2cStart_FH();
		i2cSendByte_FH(Addr<<1);
		if (!i2cGetAck_FH())
		{
	    	i2cStop_FH();
	    	return FALSE;
		}
    		
        i2cSendByte_FH(*dataBuffer++);
        if (!i2cGetAck_FH())
        {
         	i2cStop_FH();
         	return FALSE;
        }
        
        i2cSendByte_FH(*dataBuffer++);
        if (!i2cGetAck_FH())
        {
         	i2cStop_FH();
         	return FALSE;
        }
        i2cStop_FH();
    }
    while((--bufSize)!=0);
    SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    //i2cStop();
    return TRUE;
}

/* -------------------------------------------------------------------------*
 * Function   : i2c_GetData                                                 *
 * Description: Get I2C data from I2C Slave.                                *
 * Parameters : Addr       -- Slave Address                                 *
 *              dataBuffer -- Data pointer to be stored                     *
 *              Size       -- Data size will be stored                      *
 * Return     : TURE is sending to slave OK, FALSE is sending fail.         *
 * -------------------------------------------------------------------------*/
bool i2c_GetData_FH(u8 Addr, u8 *dataBuffer, u8 Size)
{
    u8 bufSize = Size;
   
  /*  
    i2cStart();
    i2cSendByte((Addr<<1)|0x1);
    if (!i2cGetAck())
    {
        i2cStop();
        return FALSE;
    }
  */ 
    //i2cSendByte( *(dataBuffer));
    
    u8 i = 0;
    SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    //ADMP521T_CLK_OE(true);
    do
    {
		i2cStart_FH();
		i2cSendByte_FH(Addr<<1);
		if (!i2cGetAck_FH())
		{
			i2cStop_FH();
			return FALSE;
		}
		i2cSendByte_FH( *(dataBuffer+i*2));
		if (!i2cGetAck_FH())
		{
			i2cStop_FH();
			return FALSE;
		}
		i2cStop_FH();

		i2cDelay(DELAY);

		i2cStart_FH();
		i2cSendByte_FH((Addr<<1)|0x1);
		if (!i2cGetAck_FH())
		{
			i2cStop_FH();
			return FALSE;
		}

	    *(dataBuffer+i*2+1) = i2cGetByte_FH();
	    
	    i2cPutAck_FH(TRUE);
     
	    i++;
     
	    i2cStop_FH();
	    i2cDelay(2*DELAY);
    }
    while((--bufSize)!=0);
    SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    //ADMP521T_CLK_OE(false);
    //*dataBuffer = i2cGetByte();
    //i2cPutAck(FALSE);
   
    //i2cStop();
    return (TRUE);
}

// I2C Internal API

/* -------------------------------------------------------------------------*
 * Function   : i2cStart                                                    *
 * Description: SDA_F change from high to low when SDP_LR0_H keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStart_FH(void)
{
  //*pPORTHIO_DIR = SDP_LR0_H;
  //*pPORTFIO_DIR = SDA_F;
  //*pPORTHIO_SET |= SDP_LR0_H;
  *pPORTFIO_INEN &= ~(SDA_F);
  *pPORTFIO_DIR |= SDA_F;
  //*pPORTHIO_SET = SDP_LR0_H;
  i2cDelay(DELAY/2);
  *pPORTFIO_SET = SDA_F;
  i2cDelay(DELAY/2);
  *pPORTHIO_SET = SDP_LR0_H;
  
  i2cDelay(DELAY);
	
  *pPORTFIO_CLEAR = SDA_F;
  
  i2cDelay(DELAY/2);
	
  *pPORTHIO_CLEAR = SDP_LR0_H;
  
	i2cDelay(DELAY/2);
  
}

/* -------------------------------------------------------------------------*
 * Function   : i2cStop                                                     *
 * Description: SDA_F change from low to high when SDP_LR0_H keep in high.          *
 * Parameters : None                                                        *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cStop_FH(void)
{
    // After STOP, I2C_BUS goes into IDLE state.
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
    //*pPORTHIO_DIR = SDP_LR0_H;
    if(*pPORTFIO & SDA_F)
	{
		//i2cDelay(DELAY/2);
		*pPORTHIO_CLEAR = SDP_LR0_H;	
		i2cDelay(DELAY/2);
		*pPORTFIO_CLEAR = SDA_F;
		i2cDelay(DELAY/2);
		*pPORTHIO_SET = SDP_LR0_H;
		i2cDelay(DELAY/2);
		*pPORTFIO_SET = SDA_F;
		i2cDelay(DELAY);
	}
	else
	{
	    //*pPORTFIO_CLEAR = SDA_F;
	    i2cDelay(DELAY/2);
	    *pPORTHIO_SET = SDP_LR0_H;
	    i2cDelay(DELAY/2);
		*pPORTFIO_SET = SDA_F;
		i2cDelay(DELAY);
	}
}

/* -------------------------------------------------------------------------*
 * Function   : i2cSendByte                                                 *
 * Description: Send the byte output of gpio, MSB first.                    *
 * Parameters : data to be shifted.                                         *
 * Return     : None                                                        *
 * -------------------------------------------------------------------------*/
void i2cSendByte_FH(u8 data)
{
    u8 cnt = 8;
    //hal_gpio_SetOut(I2C_SDA_F);
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
    *pPORTHIO_DIR |= SDP_LR0_H;
   
    do
    {
        if (data & 0x80)
        {
            *pPORTFIO_SET = SDA_F;
        }
        else
        {
            //hal_gpio_ClrBit(I2C_SDA_F);
            *pPORTFIO_CLEAR = SDA_F;
        }
        data <<= 1;
        i2cDelay(DELAY/2);
        //hal_gpio_SetBit(I2C_SDP_LR0_H);
        *pPORTHIO_SET = SDP_LR0_H;
        i2cDelay(DELAY);
        //*pPORTFIO_CLEAR = SDA_F;
        //i2cDelay(DELAY/2);
        //hal_gpio_ClrBit(I2C_SDP_LR0_H);
        *pPORTHIO_CLEAR = SDP_LR0_H;
        i2cDelay(DELAY/4);
    }
    while (--cnt != 0);
    //*pPORTFIO_CLEAR = SDA_F;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetByte                                                  *
 * Description: Get the Data from slave device                              *
 * Parameters : None                                                        *
 * Return     : Received data                                               *
 * -------------------------------------------------------------------------*/
u8 i2cGetByte_FH(void)
{
    u8 cnt = 8;
    u8 data = 0;
   
//    hal_gpio_SetOut(I2C_SDA_F);
//    hal_gpio_SetBit(I2C_SDA_F); // Should pull up SDA_F before read.
//    i2cDelay(DELAY);


    //*pPORTFIO_DIR |= SDA_F;
    //*pPORTFIO_INEN &= ~(SDA_F);
		//*pPORTFIO_SET = SDA_F;
		//i2cDelay(DELAY);
   
    // // Set GPIO as input
    //*pPORTFIO_INEN |= SDA_F;
    *pPORTFIO_SET = SDA_F;
    *pPORTFIO_DIR &= ~(SDA_F);
    *pPORTFIO_INEN |= SDA_F;
    //*pPORTFIO_SET = SDA_F;
    //i2cDelay(DELAY/2);
    SetPin_H(DATA_DIR_H, false);		//make sure it work on input status.
	//ADMP521T_CLK_OE(true);
    do
    {
	     i2cDelay(DELAY/2);
	     *pPORTHIO_SET = SDP_LR0_H;
	     i2cDelay(DELAY/2);
     
	     data <<= 1;
	     if (*pPORTFIO & SDA_F)
	     	data++; // Get input data bit
     
	     i2cDelay(DELAY/2);
	     *pPORTHIO_CLEAR = SDP_LR0_H;
	     i2cDelay(DELAY/2);
    }
    while (--cnt != 0);
    SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    //ADMP521T_CLK_OE(false);
    return(data);
}

/* -------------------------------------------------------------------------*
 * Function   : i2cGetAck                                                   *
 * Description: During SDP_LR0_H high, when SDA_F holds low is ACK otherwise NACK.  *
 * Parameters : No                                                          *
 * Return     : TRUE is ACK, FALSE is NACK.                                 *
 * Notes      : This function gets the ACK signal from the slave.           *
 * -------------------------------------------------------------------------*/
bool i2cGetAck_FH(void)
{
    bool isACK;
   
    //*pPORTFIO_INEN = SDA_F;
    //*pPORTFIO_CLEAR = SDA_F;
    SetPin_H(DATA_DIR_H, false);		//make sure it work on input status.
    *pPORTFIO_SET = SDA_F;
    *pPORTFIO_DIR &= ~(SDA_F);
    *pPORTFIO_INEN = SDA_F;
    *pPORTHIO_CLEAR = SDP_LR0_H;
    
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SDP_LR0_H);
    *pPORTHIO_SET = SDP_LR0_H;
    
    i2cDelay(DELAY/2);
   
    // Read Line at the middle of input, it is stable.
    //*pPORTFIO_INEN |= SDA_F;
    if (*pPORTFIO & SDA_F) 
    	isACK = FALSE;
    else                          
    	isACK = TRUE;
   
    i2cDelay(DELAY/2);
    //hal_gpio_ClrBit(I2C_SDP_LR0_H);
    *pPORTHIO_CLEAR = SDP_LR0_H;
    i2cDelay(DELAY/2);
    SetPin_H(DATA_DIR_H, true);		//make sure it work on output status.
    return isACK;
}

/* -------------------------------------------------------------------------*
 * Function   : i2cPutAck                                                   *
 * Description: During SDP_LR0_H high, when SDA_F holds low is ACK otherwise NACK.  *
 * Parameters : TRUE is ACK, FALSE is NACK                                  *
 * Return     :                                                             *
 * Notes      : This function send ACK or NACK to slave device              *
 * -------------------------------------------------------------------------*/
void i2cPutAck_FH(bool isACK)
{
    //hal_gpio_SetOut(I2C_SDA_F);
    *pPORTFIO_INEN &= ~(SDA_F);
    *pPORTFIO_DIR |= SDA_F;
   
    if (!isACK) 
    	*pPORTFIO_CLEAR = SDA_F;
    else      
    	*pPORTFIO_SET = SDA_F;
   
    i2cDelay(DELAY/2);
    //hal_gpio_SetBit(I2C_SDP_LR0_H);
    *pPORTHIO_SET = SDP_LR0_H;
    i2cDelay(DELAY);
    //hal_gpio_ClrBit(I2C_SDP_LR0_H);
    *pPORTHIO_CLEAR = SDP_LR0_H;
    i2cDelay(DELAY/2);
    *pPORTFIO_CLEAR = SDA_F;
}

//-----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
bool I2CWrite(u8 chipAddr, u8 *dataBuffer, u8 writeNum, u8 portx)
{
/*	u8 _chipAddr = (u8)chipAddr;
	u8 _writeNum = (u8) writeNum;
	u8 _dataBuffer[2 * writeNum];
	
	bool result;
	u8 index;
	for(index = 0; index < writeNum; index++)
	{
		_dataBuffer[2 * index] =(u8)*(dataBuffer + 2 * index);
		_dataBuffer[2 * index + 1] =(u8)*(dataBuffer + 2 * index + 1);
	}
*/	
	bool result;
	switch(portx)
	{
		case PORTF:
			i2c_Open_F();
			SetPin_H(DATA_DIR_H, true);		//make sure the scl and sda pin work on output status.	
			result = i2c_SendData_F(chipAddr, dataBuffer, writeNum);
			SetPin_H(DATA_DIR_H, true);		//make sure the scl and sda pin work on output status.
			//i2c_Close_F();
			break;
		
		case PORTG:
			i2c_Open_G();	
			result = i2c_SendData_G(chipAddr, dataBuffer, writeNum);
			//i2c_Close_G();
			break;
		
		case PORTH:
			i2c_Open_H();	
			result = i2c_SendData_H(chipAddr, dataBuffer, writeNum);
			//i2c_Close_H();
			break;
		
		case PORTFH:
			i2c_Open_FH();
			result = i2c_SendData_FH(chipAddr, dataBuffer, writeNum);
			//i2c_Close_FH();
			break;
		
		default:
			result = false;
			break;
	}
	return result;
}


//----------------------------------------------------------------------------
bool I2CRead(u8 chipAddr, u8 *dataBuffer, u8 readNum, u8 portx)
{
	bool result;
	switch(portx)
	{
		case PORTF:
			i2c_Open_F();
			SetPin_H(DATA_DIR_H, true);		//make sure the scl and sda pin work on output status.	
			result = i2c_GetData_F(chipAddr, dataBuffer, readNum);
			SetPin_H(DATA_DIR_H, true);		//make sure the scl and sda pin work on output status.
			//i2c_Close_F();
			break;
		
		case PORTG:
			i2c_Open_G();	
			result = i2c_GetData_G(chipAddr, dataBuffer, readNum);
			//i2c_Close_G();
			break;
		
		case PORTH:
			i2c_Open_H();	
			result = i2c_GetData_H(chipAddr, dataBuffer, readNum);
			//i2c_Close_H();
			break;
		
		case PORTFH:
			i2c_Open_FH();
			result = i2c_GetData_FH(chipAddr, dataBuffer, readNum);
			//i2c_Close_FH();
			break;
		
		default:
			result = false;
			break;
	}

	return result;
}


//----------------------------------------------------------------------------
void SetLR(bool high, u32 channel, bool ifNomalMode)
{
	//flashLed();
	if(ifNomalMode)
	{
		//part0
		*pPORTH_FER &= ~(SDP_LR0_H);	
		*pPORTHIO_INEN &= ~SDP_LR0_H;	
		if(high)
		{
			*pPORTHIO_SET = SDP_LR0_H;				
		}
		else
		{
			*pPORTHIO_CLEAR = SDP_LR0_H;
		}
		*pPORTHIO_DIR |= SDP_LR0_H;
		//part1
		*pPORTG_FER &= ~(SDP_LR1_G);	
		*pPORTGIO_INEN &= ~SDP_LR1_G;	
		if(!high)
		{
			*pPORTGIO_SET = SDP_LR1_G;				
		}
		else
		{
			*pPORTGIO_CLEAR = SDP_LR1_G;
		}
		*pPORTGIO_DIR |= SDP_LR1_G;
		return;
	}
	switch(channel)
	{
		case 0:	
			*pPORTH_FER &= ~(SDP_LR0_H);	
			*pPORTHIO_INEN &= ~SDP_LR0_H;	
			if(high)
			{
				*pPORTHIO_SET = SDP_LR0_H;				
			}
			else
			{
				*pPORTHIO_CLEAR = SDP_LR0_H;
			}
			*pPORTHIO_DIR |= SDP_LR0_H;
			break;
		case 1:
			*pPORTG_FER &= ~(SDP_LR1_G);	
			*pPORTGIO_INEN &= ~SDP_LR1_G;	
			if(high)
			{
				*pPORTGIO_SET = SDP_LR1_G;				
			}
			else
			{
				*pPORTGIO_CLEAR = SDP_LR1_G;
			}
			*pPORTGIO_DIR |= SDP_LR1_G;
			break;
		case 2:
			//part0
			*pPORTH_FER &= ~(SDP_LR0_H);	
			*pPORTHIO_INEN &= ~SDP_LR0_H;	
			if(high)
			{
				*pPORTHIO_SET = SDP_LR0_H;				
			}
			else
			{
				*pPORTHIO_CLEAR = SDP_LR0_H;
			}
			*pPORTHIO_DIR |= SDP_LR0_H;
			//part1
			*pPORTG_FER &= ~(SDP_LR1_G);	
			*pPORTGIO_INEN &= ~SDP_LR1_G;	
			if(high)
			{
				*pPORTGIO_SET = SDP_LR1_G;				
			}
			else
			{
				*pPORTGIO_CLEAR = SDP_LR1_G;
			}
			*pPORTGIO_DIR |= SDP_LR1_G;
			break;
		default:
			flashLed();
			break;
	}
}

//----------------------------------------------------------------------------
void ADMP521T_CLK_OE(bool enable)			//enable->true:chip enable,false:disable. It is inverse of OE
{
	//flashLed();
	*pPORTH_FER &= ~(CLK_OE);
	
	*pPORTHIO_INEN &= ~CLK_OE;
	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	if(enable)
	{
		*pPORTHIO_SET = CLK_OE;				
	}
	else
	{
		*pPORTHIO_CLEAR = CLK_OE;
	}
	*pPORTHIO_DIR |= CLK_OE;
	
}

//----------------------------------------------------------------------------
void ADMP521T_FUSE_OE(bool enable)			//enable->true:chip enable,false:disable. It is inverse of OE
{
	//flashLed();
	*pPORTH_FER &= ~(FUSE_OE);		
	*pPORTHIO_INEN &= ~FUSE_OE;	
	//first set IO data to 1, and then switch it to GPO. If not, it may generate
	if(enable)
	{
		*pPORTHIO_SET = FUSE_OE;				
	}
	else
	{
		*pPORTHIO_CLEAR = FUSE_OE;
	}
	*pPORTHIO_DIR |= FUSE_OE;
	
	
//	*pPORTH_FER &= ~(PH4);	
//	*pPORTHIO_INEN &= ~PH4;
//	*pPORTHIO_SET = PH4;
//	*pPORTHIO_DIR |= PH4;	
}

//----------------------------------------------------------------------------
void SetClkPinDir(bool ifIn)
{
	*pPORTF_FER &= ~SCL_F;	
	*pPORTFIO_INEN |= SCL_F;
	*pPORTFIO_DIR &= ~SCL_F;
	
	*pPORTF_FER &= ~SDA_F;	
	*pPORTFIO_INEN |= SDA_F;
	*pPORTFIO_DIR &= ~SDA_F;

/*	
	if(ifIn)
	{
		*pPORTF_FER &= ~SCL_F;	
		*pPORTFIO_INEN |= SCL_F;
		*pPORTFIO_DIR &= ~SCL_F;
	}
	else
	{
		*pPORTF_FER &= ~SCL_F;	
		*pPORTFIO_INEN &= ~SCL_F;
		*pPORTFIO_DIR |= SCL_F;
	}
*/	
}

//----------------------------------------------------------------------------
void SetPin_F(u16 pinDefine, bool high)
{
	*pPORTF_FER &= ~pinDefine;	
	*pPORTFIO_INEN &= ~pinDefine;
	if(high)
	{
		*pPORTFIO_SET = pinDefine;
	}
	else
	{
		*pPORTFIO_CLEAR = pinDefine;		
	}
	//switch the IO port direction
 	*pPORTFIO_DIR |= pinDefine;
}

void SetPin_G(u16 pinDefine, bool high)
{
	*pPORTG_FER &= ~pinDefine;	
	*pPORTGIO_INEN &= ~pinDefine;
	if(high)
	{
		*pPORTGIO_SET = pinDefine;
	}
	else
	{
		*pPORTGIO_CLEAR = pinDefine;		
	}
	//switch the IO port direction
 	*pPORTGIO_DIR |= pinDefine;
}

void SetPin_H(u16 pinDefine, bool high)
{
	*pPORTH_FER &= ~pinDefine;	
	*pPORTHIO_INEN &= ~pinDefine;
	if(high)
	{
		*pPORTHIO_SET = pinDefine;
	}
	else
	{
		*pPORTHIO_CLEAR = pinDefine;		
	}
	//switch the IO port direction
 	*pPORTHIO_DIR |= pinDefine;
}

//----------------------------------------------------------------------------
void FuseChip(u16 _fuseVal, u32 _pulse_halfDelay)
{
	//SetPin_H(DATA_DIR_H, true);		//Set Level Shift hardware
	
	*pPORTF_FER &= ~(SCL_F);
	*pPORTF_FER &= ~(SDA_F);
	
	*pPORTFIO_INEN &= ~SCL_F;
	*pPORTFIO_INEN &= ~SDA_F;
	
	//first set IO data to 0, and then switch it to GPO. (If not, it may generate a negative pluse)
	*pPORTFIO_CLEAR = SCL_F;
	*pPORTFIO_CLEAR = SDA_F;
	//switch the IO port direction
 	*pPORTFIO_DIR |= SCL_F;
	*pPORTFIO_DIR |= SDA_F;			
   
    u16 fuseValue = _fuseVal;
    u32 pulse_delay = 2 * _pulse_halfDelay;
    i2cDelay(2 * pulse_delay);
    
  	SetLR(true, 2,false);			//Set LR to high, enter fuse mode internal.
  	
    i2cDelay(4 * pulse_delay);		//Another delay for hard ware enter to fuse mode.
    //printf("fuse value is : %x",fuseValue);
    mytest = fuseValue;
    //One more pulse before first fuse bit.
    *pPORTFIO_SET = SCL_F;
    i2cDelay(pulse_delay);
    *pPORTFIO_CLEAR = SCL_F;
    i2cDelay(_pulse_halfDelay);
    
  	u8 cnt = 16;			//fuse value has 16 bits.   
    do
    {
    	i2cDelay(_pulse_halfDelay);
    	*pPORTFIO_SET = SCL_F;
		i2cDelay(_pulse_halfDelay);
		
        if ((bool)(fuseValue & 0x0001))
        {
            *pPORTFIO_SET = SDA_F;
            //flashLed();
        }
        else
        {
            *pPORTFIO_CLEAR = SDA_F;
            
        }
        
        fuseValue >>= 1;			//fuseValue shift 1 bit right, prepare to fuse next bit.
        
        i2cDelay(_pulse_halfDelay);
        *pPORTFIO_CLEAR = SCL_F;
        i2cDelay(_pulse_halfDelay);
        *pPORTFIO_CLEAR = SDA_F;        
    }
    while (--cnt != 0);
    
    SetLR(false, 2,false);			//Set LR to low, exit fuse mode internal.
    //*pPORTFIO_CLEAR = SDA_F;
}








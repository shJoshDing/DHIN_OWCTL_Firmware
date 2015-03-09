/*****************************************************************************
 * TDM.c
 *****************************************************************************/
#include <bfrom.h>
#include <services/services.h>     // system service includes
#include <drivers/adi_dev.h>       // device manager includes
//#include "SDP_ssl_init.h"
//#include "adi_int.h"
#include <cdefBF527.h>
#include <sys/exception.h>
#include <stdio.h>
#include <ccblkfn.h>
#include <drivers/sport/adi_sport.h>

#define CONFIG_LENGTH 18
#define DATA_BUF_SIZE 	1024*32
#define I2S_BUF_SIZE   	(DATA_BUF_SIZE/4)

//static void audioCallback(void *AppHandle, u32 Event, void *pArg);
//static void I2SCallback(void *AppHandle, u32 Event, void *pArg);
//static void PDMCallback(void *AppHandle, u32 Event, void *pArg)

 
void PDMEntry(void) ;
void PDMClose(void) ;
void sendDatatoPC(void);

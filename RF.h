/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_IR_to_RF_MICRF113_test'.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef RF_H
#define	RF_H

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "user.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define f 3
#define FAIL 0
#define PASS 1
#define SENDING 2
#define RF 55
#define EndofRFcode 99

/******************************************************************************/
/* NumRfConfigs
 *
 * This contains the number of available RF configs.
/******************************************************************************/
#define NumRfConfigs 2

/******************************************************************************/
/* Rf Codes for RFConfig = 1
 *
 * This contains the RF codes that are sent from the system.
/******************************************************************************/
// RF bit Timing for PT2260a

/* channel D */
const unsigned char Conf1_ChannelD[] = {f,f,f,f,f,f,0,0,1,0,0,0};//(A1 A0 x x x x x x)
const unsigned char Conf1_ChannelD_STR[] = "Conf1_ChannelD";

/* channel E */
const unsigned char Conf1_ChannelE[] = {0,f,f,f,f,f,0,0,1,0,0,0};//(A1 A0 x x x x x x)
const unsigned char Conf1_ChannelE_STR[] = "Conf1_ChannelE";

/* channel F */
const unsigned char Conf1_ChannelF[] = {f,0,f,f,f,f,0,0,1,0,0,0};//(A1 A0 x x x x x x)
const unsigned char Conf1_ChannelF_STR[] = "Conf1_ChannelF";

/******************************************************************************/
/* Rf Codes for RFConfig = 2
 *
 * This contains the RF codes that are sent from the system.
/******************************************************************************/
// RF bit Timing for AUT980202-A-Pn or AUT980202-B
// These have different timing then the one above
// zero is 650uS high folowed by 1.75mS low
// full pulse is 2.42mS
// one is 1.75mS high followed by 650uS low
// sync is 650uS high followed by 11.89 mS low

/* channel B */
const unsigned char Conf2_ChannelB_ON[] =  {0,1,1,0,1,0,0,0,0,0,1,0,0,1,0,0};
const unsigned char Conf2_ChannelB_ON_STR[] = "Conf2_ChannelB_ON";
const unsigned char Conf2_ChannelB_OFF[] = {0,1,1,0,1,0,0,0,0,0,0,1,0,1,0,0};
const unsigned char Conf2_ChannelB_OFF_STR[] = "Conf2_ChannelB_OFF";

/* channel D */
const unsigned char Conf2_ChannelD_ON[] =  {0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,1};
const unsigned char Conf2_ChannelD_ON_STR[] = "Conf2_ChannelD_ON";
const unsigned char Conf2_ChannelD_OFF[] = {0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,1};
const unsigned char Conf2_ChannelD_OFF_STR[] = "Conf2_ChannelD_OFF";

/* channel H */
const unsigned char Conf2_ChannelH_1_ON[] =  {0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_1_ON_STR[] = "Conf2_ChannelH_1_ON";
const unsigned char Conf2_ChannelH_1_OFF[] = {0,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_1_OFF_STR[] = "Conf2_ChannelH_1_OFF";
const unsigned char Conf2_ChannelH_2_ON[] =  {0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0};
const unsigned char Conf2_ChannelH_2_ON_STR[] = "Conf2_ChannelH_2_ON";
const unsigned char Conf2_ChannelH_2_OFF[] = {0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,0};
const unsigned char Conf2_ChannelH_2_OFF_STR[] = "Conf2_ChannelH_2_OFF";
const unsigned char Conf2_ChannelH_3_ON[] =  {0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_3_ON_STR[] = "Conf2_ChannelH_3_ON";
const unsigned char Conf2_ChannelH_3_OFF[] = {0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_3_OFF_STR[] = "Conf2_ChannelH_3_OFF";

/******************************************************************************/
/* RF bit Timing for RFConfig = 1
 *
 * Settings:
 * Timer clock = FOSC/4 = 16MHz
 * Prescaler = 16
 * Postscaler = 15 * RF_Postscaler
 * RF_Postscaler = 2
/******************************************************************************/
/* Short bit/pause */
/* Nominal 128 cycles or about 240uS*/
#define Conf1_Short 8

/* Long bit/pause */
/* Nominal 384 cycles or about 720uS*/
#define Conf1_Long 24

/* Sync bit/pause */
/* Nominal 4096 cycles or about 7.680mS*/
#define Conf1_Sync 250

/******************************************************************************/
/* RF bit Timing for RFConfig = 2
 *
 * Settings:
 * Timer clock = FOSC/4 = 16MHz
 * Prescaler = 16
 * Postscaler = 15 * RF_Postscaler
 * RF_Postscaler = 4
/******************************************************************************/
/* Short bit/pause */
/* 650uS*/
#define Conf2_Short 11 //~660uS

/* Long bit/pause */
/* 1.75mS*/
#define Conf2_Long 29 // ~1740uS

/* Sync bit/pause */
/* 11.89mS*/
#define Conf2_Sync 187 //11220 + 660 = 11880uS

/******************************************************************************/
/* RFmaxSize
 *
 * This is the size of the RF send buffer. A code can not be more bytes than
 *  this value.
/******************************************************************************/
#define RFmaxSize 20

/******************************************************************************/
/* RFsendWaitTime
 *
 * This is the amount of delay that the system waits before it allows another
 *  RF code to send. This is dependant upon the toggle speed of the end system.
/******************************************************************************/
#define RFsendWaitTime 6

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
extern unsigned char RFsendCode[RFmaxSize];
extern volatile unsigned char RFsendFlag;
extern unsigned char RFcodePlace;
extern unsigned char RFcodeBit;
extern volatile unsigned char Sendcount;
extern unsigned char RFConfig;
volatile unsigned char Sent;
extern volatile unsigned char RF_IR_Postscaler;
extern unsigned char RF_IR;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

unsigned char SendRF(const unsigned char* Code, unsigned char Config, unsigned char size, unsigned char RepeatAmount);
void SendRF_wait(const unsigned char* Code, unsigned char Config, unsigned char size, unsigned char amount);
void SendRF_Channel(unsigned char channel);
unsigned char GetRFstatus(void);

#endif	/* RF_H */
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
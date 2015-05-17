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
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
 * 04/29/15     1.0_DW0d    Create new macro for holding the number of rf codes.
 * 05/14/15     1.0_DW0e    Fixed RF timing so that conf2_channeB works.
 *                          Added support for 433MHz transmitter.
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
#include "UART.h"


/******************************************************************************/
/* RFrepeatAmount
 *
 * This contains the number times to repeat an RF code.
/******************************************************************************/
#define RFrepeatAmount 7

/******************************************************************************/
/* RFnumberOfSavedCodes
 *
 * This contains the number of RF codes/channels.
 * (See amount used in SendRF_Channel).
 *
 * current:
 * 1. Conf1_ChannelD
 * 2. Conf1_ChannelE
 * 3. Conf1_ChannelF
 * 4. Conf2_ChannelB
 * 5. Conf2_ChannelD
 * 6. Conf2_ChannelH 1
 * 7. Conf2_ChannelH 2
 * 8. Conf2_ChannelH 3
 * 9-89. Conf3_Channel0 - F
 *     button 1-5
/******************************************************************************/
#define RFnumberOfSavedCodes 89

/******************************************************************************/
/* NumRfConfigs
 *
 * This contains the number of available RF configs.
/******************************************************************************/
#define NumRfConfigs 3

/******************************************************************************/
/* RF bit Timing for RFConfig = 1
 *
 * 315MHz transmission
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
 * 315MHz transmission
 *
 * Settings:
 * Timer clock = FOSC/4 = 16MHz
 * Prescaler = 16
 * Postscaler = 15 * RF_Postscaler
 * RF_Postscaler = 4
/******************************************************************************/
/* Short bit/pause */
/* 650uS*/
#define Conf2_Short 9 //~660uS

/* Long bit/pause */
/* 1.75mS*/
#define Conf2_Long 29 // ~1740uS

/* Sync bit/pause */
/* 11.89mS*/
#define Conf2_Sync 146 //11220 + 660 = 11880uS

/******************************************************************************/
/* RF bit Timing for RFConfig = 3
 *
 * 433MHz transmission
 *
 * Settings:
 * Timer clock = FOSC/4 = 16MHz
 * Prescaler = 16
 * Postscaler = 15 * RF_Postscaler
 * RF_Postscaler = 2
/******************************************************************************/
/* Short bit/pause */
/* Nominal 128 cycles or about 300uS*/
#define Conf3_Short 8

/* Long bit/pause */
/* Nominal 384 cycles or about 900uS*/
#define Conf3_Long 23

/* Sync bit/pause */
/* Nominal 4096 cycles or about 7.680mS*/
#define Conf3_Sync 200

/******************************************************************************/
/* Conf3NumConfig
 *
 * This is the number of configuration channels available. If this changes then
 * Conf3_Channels[] must also change.
/******************************************************************************/
#define Conf3NumConfig 16

/******************************************************************************/

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
#define RFsendWaitTime 3

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define f 3
#define FAIL 0
#define PASS 1
#define SENDING 2
#define RF 55
#define EndofRFcode 99
#define _315MHz 31
#define _433MHz 43

/******************************************************************************/
/* Rf Codes for RFConfig = 1
 *
 * This contains the RF codes that are sent from the system.
/******************************************************************************/
// RF bit Timing for PT2260a
// f = short long long short
// 0 = short long short long
// 1 = long short long short
const unsigned char Conf1_Channels[] = "DEF";

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

const unsigned char Conf2_Channels[] = "BDH";
/* channel B */
const unsigned char Conf2_ChannelB_STR[] = "Conf2_ChannelB";
const unsigned char Conf2_ChannelB_ON[] =  {0,1,1,0,1,0,0,0,0,0,1,0,0,1,0,0};
const unsigned char Conf2_ChannelB_ON_STR[] = "Conf2_ChannelB_ON";
const unsigned char Conf2_ChannelB_OFF[] = {0,1,1,0,1,0,0,0,0,0,0,1,0,1,0,0};
const unsigned char Conf2_ChannelB_OFF_STR[] = "Conf2_ChannelB_OFF";

/* channel D */
const unsigned char Conf2_ChannelD_STR[] = "Conf2_ChannelD";
const unsigned char Conf2_ChannelD_ON[] =  {0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,1};
const unsigned char Conf2_ChannelD_ON_STR[] = "Conf2_ChannelD_ON";
const unsigned char Conf2_ChannelD_OFF[] = {0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,1};
const unsigned char Conf2_ChannelD_OFF_STR[] = "Conf2_ChannelD_OFF";

/* channel H */
const unsigned char Conf2_ChannelH_1_STR[] = "Conf2_ChannelH_1";
const unsigned char Conf2_ChannelH_1_ON[] =  {0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_1_ON_STR[] = "Conf2_ChannelH_1_ON";
const unsigned char Conf2_ChannelH_1_OFF[] = {0,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_1_OFF_STR[] = "Conf2_ChannelH_1_OFF";

const unsigned char Conf2_ChannelH_2_STR[] = "Conf2_ChannelH_2";
const unsigned char Conf2_ChannelH_2_ON[] =  {0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0};
const unsigned char Conf2_ChannelH_2_ON_STR[] = "Conf2_ChannelH_2_ON";
const unsigned char Conf2_ChannelH_2_OFF[] = {0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,0};
const unsigned char Conf2_ChannelH_2_OFF_STR[] = "Conf2_ChannelH_2_OFF";

const unsigned char Conf2_ChannelH_3_STR[] = "Conf2_ChannelH_3";
const unsigned char Conf2_ChannelH_3_ON[] =  {0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_3_ON_STR[] = "Conf2_ChannelH_3_ON";
const unsigned char Conf2_ChannelH_3_OFF[] = {0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,0};
const unsigned char Conf2_ChannelH_3_OFF_STR[] = "Conf2_ChannelH_3_OFF";

/******************************************************************************/
/* Rf Codes for RFConfig = 3
 *
 * This contains the RF codes that are sent from the system.
/******************************************************************************/
// RF bit Timing for hs2260a
// Product is the Zap 5L
// This uses the same bit configuration as config 1 but it is timed different
// f = short long long short
// 0 = short long short long
// 1 = long short long short
// this remote allows the factory to modify the top 5 bits of the adress.
// The first bit can be a 0 ,1, or f.
// The 2,3, 4, 5 bits can either be floating or ground.
//

const unsigned char Conf3_Channels[] = "0123456789abcdef"; // channels
const unsigned char Conf3_Buttons[] = "12345"; // buttons

const unsigned char Conf3[Conf3NumConfig][5][2][12] =
{
    {
        /******** Channel 0 ********/
        //           ON                      OFF
        { {f,f,f,f,f,f,f,f,0,1,0,1},{f,f,f,f,f,f,f,f,0,1,1,0} }, //button 1
        { {f,f,f,f,f,f,f,f,1,0,0,1},{f,f,f,f,f,f,f,f,1,0,1,0} }, //button 2
        { {f,f,f,f,f,f,f,1,0,0,0,1},{f,f,f,f,f,f,f,1,0,0,1,0} }, //button 3
        { {f,f,f,f,f,f,1,f,0,0,0,1},{f,f,f,f,f,f,1,f,0,0,1,0} }, //button 4
        { {f,f,f,f,f,1,f,f,0,0,0,1},{f,f,f,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 1 ********/ 
        //           ON                      OFF
        { {0,f,f,f,f,f,f,f,0,1,0,1},{0,f,f,f,f,f,f,f,0,1,1,0} }, //button 1
        { {0,f,f,f,f,f,f,f,1,0,0,1},{0,f,f,f,f,f,f,f,1,0,1,0} }, //button 2
        { {0,f,f,f,f,f,f,1,0,0,0,1},{0,f,f,f,f,f,f,1,0,0,1,0} }, //button 3
        { {0,f,f,f,f,f,1,f,0,0,0,1},{0,f,f,f,f,f,1,f,0,0,1,0} }, //button 4
        { {0,f,f,f,f,1,f,f,0,0,0,1},{0,f,f,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 2 ********/
        //           ON                      OFF
        { {f,0,f,f,f,f,f,f,0,1,0,1},{f,0,f,f,f,f,f,f,0,1,1,0} }, //button 1
        { {f,0,f,f,f,f,f,f,1,0,0,1},{f,0,f,f,f,f,f,f,1,0,1,0} }, //button 2
        { {f,0,f,f,f,f,f,1,0,0,0,1},{f,0,f,f,f,f,f,1,0,0,1,0} }, //button 3
        { {f,0,f,f,f,f,1,f,0,0,0,1},{f,0,f,f,f,f,1,f,0,0,1,0} }, //button 4
        { {f,0,f,f,f,1,f,f,0,0,0,1},{f,0,f,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 3 ********/
        //           ON                      OFF
        { {0,0,f,f,f,f,f,f,0,1,0,1},{0,0,f,f,f,f,f,f,0,1,1,0} }, //button 1
        { {0,0,f,f,f,f,f,f,1,0,0,1},{0,0,f,f,f,f,f,f,1,0,1,0} }, //button 2
        { {0,0,f,f,f,f,f,1,0,0,0,1},{0,0,f,f,f,f,f,1,0,0,1,0} }, //button 3
        { {0,0,f,f,f,f,1,f,0,0,0,1},{0,0,f,f,f,f,1,f,0,0,1,0} }, //button 4
        { {0,0,f,f,f,1,f,f,0,0,0,1},{0,0,f,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 4 ********/
        //           ON                      OFF
        { {f,f,0,f,f,f,f,f,0,1,0,1},{f,f,0,f,f,f,f,f,0,1,1,0} }, //button 1
        { {f,f,0,f,f,f,f,f,1,0,0,1},{f,f,0,f,f,f,f,f,1,0,1,0} }, //button 2
        { {f,f,0,f,f,f,f,1,0,0,0,1},{f,f,0,f,f,f,f,1,0,0,1,0} }, //button 3
        { {f,f,0,f,f,f,1,f,0,0,0,1},{f,f,0,f,f,f,1,f,0,0,1,0} }, //button 4
        { {f,f,0,f,f,1,f,f,0,0,0,1},{f,f,0,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 5 ********/
        //           ON                      OFF
        { {0,f,0,f,f,f,f,f,0,1,0,1},{0,f,0,f,f,f,f,f,0,1,1,0} }, //button 1
        { {0,f,0,f,f,f,f,f,1,0,0,1},{0,f,0,f,f,f,f,f,1,0,1,0} }, //button 2
        { {0,f,0,f,f,f,f,1,0,0,0,1},{0,f,0,f,f,f,f,1,0,0,1,0} }, //button 3
        { {0,f,0,f,f,f,1,f,0,0,0,1},{0,f,0,f,f,f,1,f,0,0,1,0} }, //button 4
        { {0,f,0,f,f,1,f,f,0,0,0,1},{0,f,0,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 6 ********/
        //           ON                      OFF
        { {f,0,0,f,f,f,f,f,0,1,0,1},{f,0,0,f,f,f,f,f,0,1,1,0} }, //button 1
        { {f,0,0,f,f,f,f,f,1,0,0,1},{f,0,0,f,f,f,f,f,1,0,1,0} }, //button 2
        { {f,0,0,f,f,f,f,1,0,0,0,1},{f,0,0,f,f,f,f,1,0,0,1,0} }, //button 3
        { {f,0,0,f,f,f,1,f,0,0,0,1},{f,0,0,f,f,f,1,f,0,0,1,0} }, //button 4
        { {f,0,0,f,f,1,f,f,0,0,0,1},{f,0,0,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 7 ********/
        //           ON                      OFF
        { {0,0,0,f,f,f,f,f,0,1,0,1},{0,0,0,f,f,f,f,f,0,1,1,0} }, //button 1
        { {0,0,0,f,f,f,f,f,1,0,0,1},{0,0,0,f,f,f,f,f,1,0,1,0} }, //button 2
        { {0,0,0,f,f,f,f,1,0,0,0,1},{0,0,0,f,f,f,f,1,0,0,1,0} }, //button 3
        { {0,0,0,f,f,f,1,f,0,0,0,1},{0,0,0,f,f,f,1,f,0,0,1,0} }, //button 4
        { {0,0,0,f,f,1,f,f,0,0,0,1},{0,0,0,f,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 8 ********/
        //           ON                      OFF
        { {f,f,f,0,f,f,f,f,0,1,0,1},{f,f,f,0,f,f,f,f,0,1,1,0} }, //button 1
        { {f,f,f,0,f,f,f,f,1,0,0,1},{f,f,f,0,f,f,f,f,1,0,1,0} }, //button 2
        { {f,f,f,0,f,f,f,1,0,0,0,1},{f,f,f,0,f,f,f,1,0,0,1,0} }, //button 3
        { {f,f,f,0,f,f,1,f,0,0,0,1},{f,f,f,0,f,f,1,f,0,0,1,0} }, //button 4
        { {f,f,f,0,f,1,f,f,0,0,0,1},{f,f,f,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 9 ********/
        //           ON                      OFF
        { {0,f,f,0,f,f,f,f,0,1,0,1},{0,f,f,0,f,f,f,f,0,1,1,0} }, //button 1
        { {0,f,f,0,f,f,f,f,1,0,0,1},{0,f,f,0,f,f,f,f,1,0,1,0} }, //button 2
        { {0,f,f,0,f,f,f,1,0,0,0,1},{0,f,f,0,f,f,f,1,0,0,1,0} }, //button 3
        { {0,f,f,0,f,f,1,f,0,0,0,1},{0,f,f,0,f,f,1,f,0,0,1,0} }, //button 4
        { {0,f,f,0,f,1,f,f,0,0,0,1},{0,f,f,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 10 ********/
        //           ON                      OFF
        { {f,0,f,0,f,f,f,f,0,1,0,1},{f,0,f,0,f,f,f,f,0,1,1,0} }, //button 1
        { {f,0,f,0,f,f,f,f,1,0,0,1},{f,0,f,0,f,f,f,f,1,0,1,0} }, //button 2
        { {f,0,f,0,f,f,f,1,0,0,0,1},{f,0,f,0,f,f,f,1,0,0,1,0} }, //button 3
        { {f,0,f,0,f,f,1,f,0,0,0,1},{f,0,f,0,f,f,1,f,0,0,1,0} }, //button 4
        { {f,0,f,0,f,1,f,f,0,0,0,1},{f,0,f,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 11 ********/
        //           ON                      OFF
        { {0,0,f,0,f,f,f,f,0,1,0,1},{0,0,f,0,f,f,f,f,0,1,1,0} }, //button 1
        { {0,0,f,0,f,f,f,f,1,0,0,1},{0,0,f,0,f,f,f,f,1,0,1,0} }, //button 2
        { {0,0,f,0,f,f,f,1,0,0,0,1},{0,0,f,0,f,f,f,1,0,0,1,0} }, //button 3
        { {0,0,f,0,f,f,1,f,0,0,0,1},{0,0,f,0,f,f,1,f,0,0,1,0} }, //button 4
        { {0,0,f,0,f,1,f,f,0,0,0,1},{0,0,f,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 12 ********/
        //           ON                      OFF
        { {f,f,0,0,f,f,f,f,0,1,0,1},{f,f,0,0,f,f,f,f,0,1,1,0} }, //button 1
        { {f,f,0,0,f,f,f,f,1,0,0,1},{f,f,0,0,f,f,f,f,1,0,1,0} }, //button 2
        { {f,f,0,0,f,f,f,1,0,0,0,1},{f,f,0,0,f,f,f,1,0,0,1,0} }, //button 3
        { {f,f,0,0,f,f,1,f,0,0,0,1},{f,f,0,0,f,f,1,f,0,0,1,0} }, //button 4
        { {f,f,0,0,f,1,f,f,0,0,0,1},{f,f,0,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 13 ********/
        //           ON                      OFF
        { {0,f,0,0,f,f,f,f,0,1,0,1},{0,f,0,0,f,f,f,f,0,1,1,0} }, //button 1
        { {0,f,0,0,f,f,f,f,1,0,0,1},{0,f,0,0,f,f,f,f,1,0,1,0} }, //button 2
        { {0,f,0,0,f,f,f,1,0,0,0,1},{0,f,0,0,f,f,f,1,0,0,1,0} }, //button 3
        { {0,f,0,0,f,f,1,f,0,0,0,1},{0,f,0,0,f,f,1,f,0,0,1,0} }, //button 4
        { {0,f,0,0,f,1,f,f,0,0,0,1},{0,f,0,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 14 ********/
        //           ON                      OFF
        { {f,0,0,0,f,f,f,f,0,1,0,1},{f,0,0,0,f,f,f,f,0,1,1,0} }, //button 1
        { {f,0,0,0,f,f,f,f,1,0,0,1},{f,0,0,0,f,f,f,f,1,0,1,0} }, //button 2
        { {f,0,0,0,f,f,f,1,0,0,0,1},{f,0,0,0,f,f,f,1,0,0,1,0} }, //button 3
        { {f,0,0,0,f,f,1,f,0,0,0,1},{f,0,0,0,f,f,1,f,0,0,1,0} }, //button 4
        { {f,0,0,0,f,1,f,f,0,0,0,1},{f,0,0,0,f,1,f,f,0,0,1,0} }  //button 5
    },
    {
        /******** Channel 15 ********/
        //           ON                      OFF
        { {0,0,0,0,f,f,f,f,0,1,0,1},{0,0,0,0,f,f,f,f,0,1,1,0} }, //button 1
        { {0,0,0,0,f,f,f,f,1,0,0,1},{0,0,0,0,f,f,f,f,1,0,1,0} }, //button 2
        { {0,0,0,0,f,f,f,1,0,0,0,1},{0,0,0,0,f,f,f,1,0,0,1,0} }, //button 3
        { {0,0,0,0,f,f,1,f,0,0,0,1},{0,0,0,0,f,f,1,f,0,0,1,0} }, //button 4
        { {0,0,0,0,f,1,f,f,0,0,0,1},{0,0,0,0,f,1,f,f,0,0,1,0} }  //button 5
    }
};
//{ channel { buttons {on / off {code} } } }
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
extern unsigned char RFPause;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

unsigned char SendRF(const unsigned char* Code, unsigned char Config, unsigned char size, unsigned char RepeatAmount);
void SendRF_wait(const unsigned char* Code, unsigned char Config, unsigned char size, unsigned char amount);
void SendRF_Channel(unsigned char channel);
unsigned char GetRFstatus(void);
void DisplayRF_Channel(unsigned char channel);
inline void RFon();
inline void RFoff();

#endif	/* RF_H */

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
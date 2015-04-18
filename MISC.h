/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_PS2_to_UART'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef MISC_H
#define	MISC_H

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

#include "system.h"

/******************************************************************************/
/* Delay constant
 *
 * This parameter is set to calibrate the delayUS() function. Parameter found
 *   from guess and check.
/******************************************************************************/

#define delayConst 2

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define SYS_FREQ_US 64
#define Even        2
#define Odd         3
#define NOCOMMA     1
#define DOUBLECOMMA 2
#define NOVALUE     3
#define TOOBIG      4
#define NEGATIVE    5
#define NoError     0


/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void delayUS(long US);
unsigned char IsCharacter(unsigned char data);
void cleanBuffer(unsigned char* data, int count);
void cleanBuffer16bit(unsigned int* data, int count);
double Round(double input);
void BufferCopy(unsigned char* from,unsigned char* to, unsigned char count, unsigned char shift);
unsigned char StringMatch(const unsigned char* This, const unsigned char* That);
unsigned char StringMatchCaseInsensitive(const unsigned char* This, const unsigned char* That);
unsigned char StringContains(const unsigned char* This, const unsigned char* That);
unsigned char StringContainsCaseInsensitive(const unsigned char* This, const unsigned char* That);
void CalibrateDelay(void);
void lowercaseChar(unsigned char* Input);
void lowercaseString(unsigned char* Input);
void delayNOP(unsigned char NOPs);
unsigned char Reverse_Byte(unsigned char This);
unsigned int Reverse_2Byte(unsigned int This);
unsigned long Reverse_4Byte(unsigned long This);
unsigned char GetNumber(unsigned char* This, unsigned char CommaNumber, long* result);
unsigned char StringAddEqual(unsigned char* Input);
unsigned char GetStringAfterComma(unsigned char* This, unsigned char CommaNumber, unsigned char* result);
unsigned char GetNumberUnsigned(unsigned char* This, unsigned char CommaNumber, unsigned long* result);

#endif	/* MISC_H */
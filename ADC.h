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
#ifndef ADC_H
#define	ADC_H

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Ratio of voltage divider
 *
 * Set this to the ratio of R2 to R1. The voltage divider = ((R15 + R16) / R16)
/******************************************************************************/
#define ratio 11

/******************************************************************************/
/* Acquisition Time
 *
 * Set this parameter is the required settling time for an ADC measurement
/******************************************************************************/
#define ACQtime 50

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define OK 1
#define ERROR 0
#define FAILlow 34
#define FAILhigh 98

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* DisableADC
 *
 * The function turns off the internal ADC module.
/******************************************************************************/
#define DisableADC() (ADCON0bits.ADON = FALSE)

/******************************************************************************/
/* EnableADC
 *
 * The function turns on the internal ADC module.
/******************************************************************************/
#define EnableADC() (ADCON0bits.ADON = TRUE)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
double ReadVoltage(void);
unsigned char InternalADC_Read(unsigned char channel, unsigned int *ADCcounts);
void ResetADC(void);
void InitADC(void);
void VoltageDividerON(void);
void VoltageDividerOFF(void);

#endif	/* ADC_H */
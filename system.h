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
#ifndef SYSTEM_H
#define	SYSTEM_H

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

/******************************************************************************/
/* System frequency
 *
 * This is the CPU clock frequency.
 *
 * For this system the clock is 32MHz and the internal PLL is used.
 *
/******************************************************************************/

#define SYS_FREQ        64000000L

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define FCY             SYS_FREQ/4

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void ConfigureOscillator(void); /* Handles clock switching/osc initialization */


#endif	/* SYSTEM_H */

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef Bluetooth_H
#define	Bluetooth_H

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
/* CommandWait
 *
 * The amount of time between writes in command mode.
/******************************************************************************/
#define CommandWait 50000

/******************************************************************************/
/* BlueFWbuf
 *
 * This is the size of the array that holds the first and second line of the
 *  Bluetooth Firmware Version.
/******************************************************************************/
#define BlueFWbuf 30

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitBluetooth(void);
unsigned char BlueConnected(void);
void ResetBlue(void);
unsigned char EnterCommandMode(void);
unsigned char BluetoothInitialSetup(void);
unsigned char BluetoothConfigured(void);
unsigned char UseBluetooth(unsigned char *String, unsigned char StringPos);
unsigned char WaitForIRsignal(void);

#endif	/* Bluetooth_H */
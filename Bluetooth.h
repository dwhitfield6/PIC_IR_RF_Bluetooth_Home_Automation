/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
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
unsigned char UseBluetooth(void);

#endif	/* Bluetooth_H */
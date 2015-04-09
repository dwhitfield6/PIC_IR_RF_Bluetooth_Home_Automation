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
/******************************************************************************/

/******************************************************************************/
/* Contains Functions for PIC initialization
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
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
#include <stdio.h>         /* For sprintf definition */

#endif

#include "user.h"
#include "UART.h"
#include "EEPROM.h"
#include "MISC.h"
#include "Timer.h"
#include "ADC.h"
#include "Bluetooth.h"
#include "IR.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern const unsigned char Version[];

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* InitApp
 *
 * The function initializes the application. It sets the pins and pull-ups.
/******************************************************************************/
void InitApp(void)
{
    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATA |= BLUE_Reset;     // Do not hold bluetooth in reset on power-up

    InputVoltageTris        = INPUT;
    BLUE_ConnectedTris      = INPUT;
    BLUE_ResetTris          = OUTPUT;
    BLUE_AutoDiscoveryTris  = OUTPUT;
    BLUE_AutoMasterTris     = OUTPUT;
    BLUE_FactoryTris        = OUTPUT;
    PushbuttonTris          = INPUT;
    BiRedTris               = OUTPUT;
    BiGreenTris             = OUTPUT;
    IRLEDTris               = OUTPUT;
    SWcodePwrTris           = OUTPUT;
    RFtransTris             = OUTPUT;
    BLUE_TxTris             = OUTPUT;
    BLUE_RxTris             = INPUT;
    Swcode4Tris             = INPUT;
    Swcode3Tris             = INPUT;
    Swcode2Tris             = INPUT;
    Swcode1Tris             = INPUT;
    IRreceiverTris          = INPUT;
    VoltageDividerTris      = INPUT;

    /* set all pins to digital except pin RA0 which is the voltage input */
    ANSEL = 0x01;
    ANSELH = 0x00;

    /* Individualy set the pull-up pins */
    INTCON2bits.RBPU = 0;
    /* Turn on weak pull-ups on pins RB0 - RB3 */
    WPUB = 0x0F;
}

/******************************************************************************/
/* Init_System
 *
 * The function initializes the system
/******************************************************************************/
void Init_System (void)
{
    cleanBuffer(&ReceivedString, RXbufsize);

    /* set up interrupt priorities */
    IPR1bits.RCIP       = OFF;  // UART receive is Low priority
    INTCON2bits.RBIP    = ON;   // KBI0 is High priority
    INTCON2bits.TMR0IP  = ON;   // Timer0 overflow is High priority
    IPR1bits.TMR2IP     = ON;   // Timer2 compare is High priority
    IPR1bits.TMR1IP     = OFF;  // Timer1 overflow is Low priority
    IPR2bits.TMR3IP     = ON;   // Timer3 overflow is High priority

    /* Enable interrupts */
    RCONbits.IPEN   = ON; //Priority interrupts
    INTCONbits.PEIE = ON; //Peripheral interrupts
    INTCONbits.GIE  = ON; //Global interrupts

    SyncEEPROMToGlobal();
    if(!EEPROMinitialized())
    {
        SetEEPROMdefault();
        Global.EEPROMinitFlag = EEPROMinitilized;
        SyncGlobalToEEPROM();
    }
    InitADC();
    InitIR();
    InitUART(BAUD);
    UARTstring(CRLN);
    UARTchar('>');
    InitBluetooth();
    InitTimers();
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
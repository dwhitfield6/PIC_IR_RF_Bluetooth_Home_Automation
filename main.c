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
/* Contains main function.
 *
/******************************************************************************/

/******************************************************************************/
/* Pick Which System!!!
 *
 * Go to user.h and define if the system is the RS232 model or TTL model
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_OR_MASKS

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "MISC.h"
#include "UART.h"
#include "FLASH.h"
#include "ADC.h"
#include "IR.h"

/******************************************************************************/
/* Version number                                                             */
/******************************************************************************/

const unsigned char Version[] = {"1.0_DW0a"};

/******************************************************************************/
/* Defines                                           */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
double BatteryVoltage = 0.0;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    unsigned char i=0;

    ConfigureOscillator();
    InitApp();
    Init_System();

    //blink LED
    for(i =0;i<10;i++)
    {
       RedLEDON();
       GreenLEDOFF();
       delayUS(10000);
       RedLEDOFF();
       GreenLEDON();
       delayUS(10000);
    }
    GreenLEDOFF();

    BatteryVoltage = ReadVoltage();
    if(BatteryVoltage < VoltageLow )
    {
        NOP();
    }
    else if (BatteryVoltage > VoltageHigh)
    {
        NOP();
    }

    while(1)
    {
        if(IR_New_Code)
        {
            UseIRCode(&IR_New_Code, IR_NEC);
        }
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/

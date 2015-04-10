/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Added IR, Bluetooth, and RF capabilities.
 * 04/09/15     1.0_DW0b    Cleanded up project.
 *                          Added macro to pick if the system has a bluetooth
 *                            module.
 *                          Fixed bugs.
/******************************************************************************/

/******************************************************************************/
/* Contains main function.
 *
/******************************************************************************/

/******************************************************************************/
/* Pick Which System!!!
 *
 * Go to user.h and define if the system has a bluetooth module or not.
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

#include "system.h"
#include "user.h"
#include "MISC.h"
#include "UART.h"
#include "ADC.h"
#include "IR.h"
#include "Bluetooth.h"

/******************************************************************************/
/* Version number                                                             */
/******************************************************************************/

const unsigned char Version[] = {"1.0_DW0b"};

/******************************************************************************/
/* Defines                                                                    */
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
    unsigned char IRtask        = FALSE;
    unsigned char Bluetoothtask = FALSE;
    unsigned char VoltageStatus = PASS;
    unsigned char BluetoothString[RXbufsize];
    unsigned char BluetoothStringPos = 0;


    ConfigureOscillator();
    InitApp();
    Init_System();

    BatteryVoltage = ReadVoltage();
    if(BatteryVoltage < VoltageLow )
    {
        VoltageStatus = FAILlow;
    }
    else if (BatteryVoltage > VoltageHigh)
    {
        VoltageStatus = FAILhigh;
    }
    if(VoltageStatus == PASS)
    {
        /* Voltage is in range */
        for(i =0;i<10;i++)
        {
            RedLEDON();
            GreenLEDOFF();
            delayUS(50000);
            RedLEDOFF();
            GreenLEDON();
            delayUS(50000);
        }
        GreenLEDOFF();
    }
    else
    {
        for(i =0;i<10;i++)
        {
            RedLEDON();
            delayUS(100000);
            RedLEDOFF();
            delayUS(100000);
        }
    }

    while(1)
    {
        IRtask          = IR_New_Code;
        Bluetoothtask   = NewReceivedString;
        BatteryVoltage = ReadVoltage();
        if(VoltageStatus == FAILlow)
        {
            /* Require voltage drastic change to avoid oscillation */
            BatteryVoltage -= 0.5;
        }
        else if(VoltageStatus == FAILhigh)
        {
            /* Require voltage drastic change to avoid oscillation */
            BatteryVoltage += 0.5;
        }

        if(BatteryVoltage < VoltageLow )
        {
            VoltageStatus = FAILlow;
        }
        else if(BatteryVoltage > VoltageHigh)
        {
            VoltageStatus = FAILhigh;
        }
        else
        {
            VoltageStatus = PASS;
        }

        if(IRtask && IR_NEC)
        {
            UseIRCode(&IR_New_Code, IR_NEC);
        }
#ifdef BLUETOOTH
        if(Bluetoothtask >= TRUE)
        {
            cleanBuffer(BluetoothString, RXbufsize);
            BufferCopy(ReceivedString,BluetoothString, ReceivedStringPos, 0);
            BluetoothStringPos = ReceivedStringPos;
            cleanBuffer(ReceivedString, ReceivedStringPos);
            ReceivedStringPos = 0;
            NewReceivedString = FALSE;
            UseBluetooth(&BluetoothString, BluetoothStringPos);
            UARTstring(CRLN);
            UARTchar('>');
            if(IR_New_Code)
            {
                IR_New_Code = Old;
            }
            /* Make sure that the IR reveicer is active */
            IRpinOLD = ReadIRpin();
            INTCONbits.RBIF = FALSE;
            IRreceiverIntOn();
            INTCONbits.RBIE = TRUE;
            ClearUSART();
            PIR1bits.RCIF = FALSE;
            PIE1bits.RCIE   = TRUE;
        }
#endif
        if(IRtimeout < IRtimeoutLoops)
        {
            IRtimeout++;
        }
        else if(IRtimeout == IRtimeoutLoops)
        {
            IRtimeout++;
            IR_NEC = 0;
            IR_New_Code = 0;
            IRpinOLD = ReadIRpin();
            INTCONbits.RBIF = FALSE;
            IRreceiverIntOn();
            INTCONbits.RBIE = TRUE;
        }
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/

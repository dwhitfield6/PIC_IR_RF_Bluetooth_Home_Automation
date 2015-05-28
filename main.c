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
 *                          Print project title when the bluetooth gets
 *                            connected.
 *                          Fixed bug in conf2 channel H parsing.
 *                          Check bluetooth status in initiation.
 * 04/18/15     1.0_DW0c    Added a header to be printed when bluetooth is
 *                            is connected.
 *                          Dont allow full duplex UART. (It caused eratic
 *                            behavior)
 *                          Fix several high priority bugs.
 *                          Add Serial number algorithem and command.
 * 04/24/15     1.0_DW0d    Reestablish full duplex workaround.
 *                          Add command to change Bluetooth Broadcast name.
 *                          Turn on RedLed when voltage is out of range.
 *                          Print a warning when voltage is out of range.
 *                          Added Flash.c/.h to program and read the flash
 *                            memory.
 *                          Added commands to send/save all rf codes.
 *                          Add new command rf set all, rf clear all,
 *                            rf clear system.
 * 05/14/15     1.0_DW0e    Fixed "AddEqual" parsing bugs.
 *                          Added support for 433MHz transmitter.
 *                          Fixed RF timing so that conf2_channeB works.
 *                          Fixed IR receive to sart teh record when the start
 *                            bit is found.
 *                          Added support for pioneer (modified NEC) remote.
 *                          Only sample ADC if the rf code is not sendng and
 *                            the IR code is not being recieved.
 *                          Fixed wierd bug where the Global2 struct was not
 *                            being passed in to a function correctly. This is
 *                            due to the lard size of the structure.
 * 05/27/15     1.0_DW0f    Moved IR receive to low priority interrupt. This
 *                            seems to have fixed alot of weird bugs where the
 *                            stack pointer would be corrupt.
 *                          Created a new function to handle the sending of one
 *                            IR or RF bit.
 *                          Fixed bug in detecting the IR start bit
 *                            (Timing bug).
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
#include <stdio.h>       /* For true/false definition */

#include "system.h"
#include "user.h"
#include "MISC.h"
#include "UART.h"
#include "ADC.h"
#include "IR.h"
#include "Bluetooth.h"
#include "Timer.h"

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
    unsigned char VoltageStatusOLD;
    unsigned char BluetoothString[RXbufsize];
    unsigned char BluetoothStringPos = 0;
    unsigned char Connected, ConnectedOLD;
    unsigned char BroadcastTEMP[BroadcastSize];
    unsigned char buf[100];
    double tempVoltage = 0.0;

    cleanBuffer(buf, 100);
    cleanBuffer(BluetoothString, RXbufsize);
    cleanBuffer(BroadcastTEMP, BroadcastSize);

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
    Connected = BlueConnected();
    ConnectedOLD = FALSE;
    VoltageStatusOLD = VoltageStatus;
    while(1)
    {
        IRtask          = IR_New_Code;
        Bluetoothtask   = NewReceivedString;
        /* read voltage */
        if(IRstarted == FALSE && (GetRFstatus() != SENDING))
        {
            tempVoltage = ReadVoltage();
            if(tempVoltage > 0.1)
            {
                BatteryVoltage = tempVoltage;
            }
            if(VoltageStatus == FAILlow)
            {
                BatteryVoltage -= 0.1;
            }
            else if(VoltageStatus == FAILhigh)
            {
                BatteryVoltage += 0.1;
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
        }

        if(VoltageStatus != PASS)
        {
            RedLEDON();
        }
        if(VoltageStatus == PASS && VoltageStatusOLD != PASS)
        {
            LEDTimerON();
        }

        /* IR */
        if(IRtask && IR_NEC)
        {
            UseIRCode(&IR_New_Code, IR_NEC);
        }

        /* Bluetooth */
#ifdef BLUETOOTHMODULE
        Connected = BlueConnected();

        if(Connected != ConnectedOLD)
        {
            if(Connected)
            {
                cleanBuffer(ReceivedString, ReceivedStringPos);
                ReceivedStringPos = 0;
                delayUS(1000000);
                UARTstringCRLN_CONST(ProjectName);
                UARTstring_CONST(CRLN);
                if(BluetoothChangeStatus)
                {
                    UARTstring_CONST(CRLN);
                    BufferCopy(BluetoothBroadcast,BroadcastTEMP, BroadcastSize, 0);
                    sprintf(buf,"Bluetooth Name was successfully changed to \" %s \"",BroadcastTEMP);
                    UARTstringCRLN(buf);
                    UARTstring_CONST(CRLN);
                }
                if(VoltageStatus != PASS && VoltageStatusOLD != PASS)
                {
                    UARTstringCRLN_CONST("Voltage out of Range!!!");
                    UARTstring_CONST(CRLN);
                }
                ClearUSART();
                PIR1bits.RCIF = FALSE;
                PIE1bits.RCIE   = TRUE;
                UARTchar_CONST('>');
            }
            else
            {
                PIE1bits.RCIE   = FALSE;
            }
        }
        if(Bluetoothtask >= TRUE && Connected)
        {
            cleanBuffer(BluetoothString, RXbufsize);
            BufferCopy(ReceivedString,BluetoothString, ReceivedStringPos, 0);
            BluetoothStringPos = ReceivedStringPos;
            cleanBuffer(ReceivedString, ReceivedStringPos);
            ReceivedStringPos = 0;
            NewReceivedString = FALSE;
            if(BluetoothString[0] != 0)
            {
                UseBluetooth(BluetoothString, BluetoothStringPos);
            }
            cleanBuffer(ReceivedString, ReceivedStringPos);
            ReceivedStringPos = 0;
            NewReceivedString = FALSE;
            UARTstring_CONST(CRLN);
            UARTchar_CONST('>');
            if(IR_New_Code)
            {
                IR_New_Code = Old;
            }
            /* Make sure that the IR receiver is active */
            IRpinOLD = ReadIRpin();
            INTCONbits.RBIF = FALSE;
            IRreceiverIntOn();
            INTCONbits.RBIE = TRUE;
            ClearUSART();
            PIR1bits.RCIF = FALSE;
            PIE1bits.RCIE   = TRUE;
        }
#endif
        /* maintainance */
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
        if(BufferOverflow)
        {
            /* Clean up an overflowed buffer */
            cleanBuffer(ReceivedString,RXbufsize);
            ReceivedStringPos = 0;
            UARTstring_CONST(CRLN);
            UARTstringCRLN_CONST("Buffer Overflow");
            UARTstring_CONST(CRLN);
            UARTchar('>');
            BufferOverflow = FALSE;
            ClearUSART();
            PIR1bits.RCIF = FALSE;
            PIE1bits.RCIE   = TRUE;
        }
        ConnectedOLD = Connected;
        VoltageStatusOLD = VoltageStatus;
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/

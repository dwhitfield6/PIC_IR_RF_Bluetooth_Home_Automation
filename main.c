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

const unsigned char FirmVersion[] = {"1.0_DW0b"};
const unsigned char PCBVersion[] = {"RevA"};
const unsigned char Frequency[] = {"315"};
const unsigned char ProjectName[] = {"Home Automation IR/RF Hub"};
unsigned char BluetoothFirmware1[BlueFWbuf];
unsigned char BluetoothFirmware2[BlueFWbuf];

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
    unsigned char Connected, ConnectedOLD;

    cleanBuffer(BluetoothString, RXbufsize);

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
    while(1)
    {
        IRtask          = IR_New_Code;
        Bluetoothtask   = NewReceivedString;
        /* read voltage */
        BatteryVoltage = ReadVoltage();

        if(VoltageStatus == FAILlow)
        {            
            BatteryVoltage -= 0.5;
        }
        else if(VoltageStatus == FAILhigh)
        {
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
                UARTchar_CONST('>');
                ClearUSART();
                PIR1bits.RCIF = FALSE;
                PIE1bits.RCIE   = TRUE;
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
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/

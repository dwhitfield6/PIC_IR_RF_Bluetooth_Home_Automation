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
/* Contains functions that control RN41 bluetooth module.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
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
#include <stdio.h>       /* For true/false definition */

#endif

#include "Bluetooth.h"
#include "user.h"
#include "UART.h"
#include "MISC.h"
#include "EEPROM.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* InitBluetooth
 *
 * The function takes the module out of reset.
/******************************************************************************/
void InitBluetooth(void)
{
    cleanBuffer(&ReceivedString,RXbufsize);
    ReceivedStringPos = 0;
    cleanBuffer(&CommandString,RXCommandsize);
    CommandStringPos = 0;

    LATA &= ~BLUE_Factory; // turn off factory reset
    ResetBlue();
    LATA |= BLUE_AutoDiscovery; // Turn on Autodiscovery
    LATA &= ~BLUE_AutoMaster; //turn off auto master
    if(!BluetoothConfigured())
    {
        BluetoothInitialSetup();
    }
}

/******************************************************************************/
/* BlueConnected
 *
 * The function puts the module in command mode.
/******************************************************************************/
unsigned char BlueConnected(void)
{
    //READ RA1
    if((PORTA & BLUE_Connected) == BLUE_Connected)
    {
        return ON;
    }
    return OFF;
}

/******************************************************************************/
/* ResetBlue
 *
 * The function resets the Bluetooth module.
/******************************************************************************/
void ResetBlue(void)
{
    LATA &= ~BLUE_Reset; // Reset module
    delayUS(CommandWait);
    LATA |= BLUE_Reset; // Take module out of reset
    delayUS(CommandWait);
}

/******************************************************************************/
/* EnterCommandMode
 *
 * The function puts the module in command mode.
/******************************************************************************/
unsigned char EnterCommandMode(void)
{
    unsigned char count = 0;
    while(BlueConnected())
    {
        ResetBlue();
        count++;
        if(count > 4)
        {
            /* Wouldn't enter command mode after 4 tries. */
            return FAIL;
        }
    }
    
    delayUS(CommandWait);
    UARTstring("$$$");
    delayUS(CommandWait);
    if(StringContains(CommandString,"CMD"))
    {
        cleanBuffer(&CommandString,CommandStringPos);
        CommandStringPos = 0;
        return PASS;
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;
    return FAIL;
}

/******************************************************************************/
/* BluetoothInitialSetup
 *
 * The function configures up the bluetooth module.
/******************************************************************************/
unsigned char BluetoothInitialSetup(void)
{
    unsigned char buf[50];
    unsigned char ok = TRUE;
    
    cleanBuffer(buf,50);

    if(!EnterCommandMode())
    {
        return FAIL;
    }
    sprintf(buf,"SN,Home Automation %ld\r",SN);
    UARTstring(buf); // Service name to home automation and serial number
    delayUS(CommandWait);
    if(!StringContains(CommandString,"AOK"))
    {
        ok = FALSE;
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring("SS, Home Automation\r"); // Service name to home automation
        delayUS(CommandWait);
        if(!StringContains(CommandString,"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring("SY,0010\r"); // Set Power to 16dBM
        delayUS(CommandWait);
        if(!StringContains(CommandString,"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring("S~,0\r"); // SPP profile
        delayUS(CommandWait);
        if(!StringContains(CommandString,"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring("S~,0\r"); // SPP profile
        delayUS(CommandWait);
        if(!StringContains(CommandString,"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring("D,0\r"); // Check for name
        delayUS(CommandWait);
        sprintf(buf,"BTName=Home Automation %ld\r",SN);
        if(!StringContains(CommandString, buf))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    UARTstring("---\r"); // exit command mode
    delayUS(CommandWait);
    if(!StringContains(CommandString, "END"))
    {
        ok = FALSE;
    }
    cleanBuffer(&CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        return PASS;
    }
    return FALSE;
}

/******************************************************************************/
/* BluetoothConfigured
 *
 * The function checks to see if the module has been configured yet.
/******************************************************************************/
unsigned char BluetoothConfigured(void)
{
    if(Global.BlueToothFlag != 1)
    {
        Global.BlueToothFlag = TRUE;
        SyncGlobalToEEPROM();
        return FALSE;
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
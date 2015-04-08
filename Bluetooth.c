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
#include <stdio.h>             

#endif

#include "Bluetooth.h"
#include "user.h"
#include "UART.h"
#include "MISC.h"
#include "EEPROM.h"
#include "RF.h"
#include "IR.h"

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
        UARTstring("SU, 115K\r"); // Baud rate to 115200
        delayUS(CommandWait);
        if(!StringContains(CommandString,"AOK"))
        {
            ok = FALSE;
        }
    }

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
/******************************************************************************/
/* UseBluetooth
 *
 * The function is called after the UART receiver gets a line feed or a
 *  carriage return.
/******************************************************************************/
unsigned char UseBluetooth(void)
{
    unsigned char ok = TRUE;

    unsigned long EnteredNEC = 0;
    unsigned char NecAddress = 0;
    unsigned char NecCommand = 0;
    unsigned long temp = 0;
    unsigned char buf[50];
    unsigned char timeout =0;

    cleanBuffer(buf,50);

    if(StringContainsCaseInsensitive(ReceivedString,"Conf1"))
    {
        if(StringMatchCaseInsensitive(ReceivedString,Conf1_ChannelD_STR))
        {
            // Configuration 1 channel D
            SendRF_wait(Conf1_ChannelD,1,12,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf1_ChannelE_STR))
        {
            // Configuration 1 channel E
            SendRF_wait(Conf1_ChannelE,1,12,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf1_ChannelF_STR))
        {
            // Configuration 1 channel F
            SendRF_wait(Conf1_ChannelF,1,12,8);
        }
        else
        {
            /* Invalid command */
            ok = FALSE;
        }
        if(ok)
        {
            UARTstringCRLN("Configuration 1 RF code sent");
            return PASS;
        }
    }
    else if(StringContainsCaseInsensitive(ReceivedString,"Conf2"))
    {
        if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelB_ON_STR))
        {
            // Configuration 2 channel B ON
            SendRF_wait(Conf2_ChannelB_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelB_OFF_STR))
        {
            // Configuration 2 channel B OFF
            SendRF_wait(Conf2_ChannelB_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelD_ON_STR))
        {
            // Configuration 2 channel D ON
            SendRF_wait(Conf2_ChannelD_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelD_OFF_STR))
        {
            // Configuration 2 channel D OFF
            SendRF_wait(Conf2_ChannelD_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelH_1_ON_STR))
        {
            // Configuration 2 channel H device 1 ON
            SendRF_wait(Conf2_ChannelH_1_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelH_1_OFF_STR))
        {
            // Configuration 2 channel H device 1 OFF
            SendRF_wait(Conf2_ChannelH_1_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelH_2_ON_STR))
        {
            // Configuration 2 channel H device 2 ON
            SendRF_wait(Conf2_ChannelH_2_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelH_2_OFF_STR))
        {
            // Configuration 2 channel H device 2 OFF
            SendRF_wait(Conf2_ChannelH_2_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelH_3_ON_STR))
        {
            // Configuration 2 channel H device 3 ON
            SendRF_wait(Conf2_ChannelH_3_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(ReceivedString,Conf2_ChannelH_3_OFF_STR))
        {
            // Configuration 2 channel H device 3 OFF
            SendRF_wait(Conf2_ChannelH_2_OFF,3,16,8);
        }
        else
        {
            /* Invalid command */
            ok = FALSE;
        }
        if(ok)
        {
            UARTstringCRLN("Configuration 2 RF code sent");
            return PASS;
        }
    }
    else if(StringContainsCaseInsensitive(ReceivedString,"NEC"))
    {
        if(!GetNumber(ReceivedString, 2, &temp))
        {
            NecCommand = (unsigned char)temp;
            if(!GetNumber(ReceivedString, 1, &temp))
            {
                NecAddress = (unsigned char) temp;
                sprintf(buf,"NEC address: %X ", NecAddress);
                UARTstringCRLN(buf);
                sprintf(buf,"NEC command: %X ", NecCommand);
                UARTstringCRLN(buf);
                EnteredNEC = EncodeNEC(NecAddress, NecCommand);
                SendNEC_wait(EnteredNEC,0);
                sprintf(buf,"Sent NEC code = 0x%lX ", EnteredNEC);
                UARTstringCRLN(buf);
                UARTstring(CRLN);
            }
            else
            {
                UARTstringCRLN("Error: could not decode Address");
            }
        }
        else if(!GetNumber(ReceivedString, 1, &EnteredNEC))
        {
            SendNEC_wait(EnteredNEC,0);
            sprintf(buf,"sent NEC code %lX", EnteredNEC);
            UARTstringCRLN(buf);
            if(DecodeNEC(EnteredNEC, &NecAddress, &NecCommand))
            {
                sprintf(buf,"address: %X ", NecAddress);
                UARTstringCRLN(buf);
                sprintf(buf,"command: %X ", NecCommand);
                UARTstringCRLN(buf);
            }
            else
            {
                UARTstringCRLN("Error: could not decode NEC code");
            }
            UARTstring(CRLN);
            return PASS;
        }
        else
        {
            UARTstring(CRLN);
            UARTstringCRLN("NEC code not entered correctly");
            UARTstring(CRLN);
            UARTstringCRLN("Usage:");
            UARTstringCRLN("NEC = (32 bit code) ");
            UARTstringCRLN("        or");
            UARTstringCRLN("NEC = address,command ");
            UARTstring(CRLN);
            UARTstringCRLN("Example:");
            UARTstringCRLN("NEC = 0x1CE350af ");
            UARTstringCRLN("      or");
            UARTstringCRLN("NEC = 0x38, 0xA ");
            UARTstring(CRLN);
            return FAIL;
        }
    }
    else if(StringContainsCaseInsensitive(ReceivedString,"RemoteButton"))
    {
        if(StringAddEqual(ReceivedString))
        {
            if(!GetNumber(ReceivedString, 1, &temp))
            {
                UARTstring(CRLN);
                UARTstring("Hit Remote Control button");
                timeout = 0;
                while(IR_New_Code != New)
                {
                    UARTchar('.');
                    timeout++;
                    if(timeout > 20)
                    {
                        UARTstring(CRLN);
                        UARTstringCRLN("You took too long hit a button on the remote");
                        return FAIL;
                    }
                }

                sprintf(buf,"RemoteButton%d = ox%lX ",temp, IR_NEC);
                IR_New_Code = Old;
                UARTstringCRLN(buf);
            }
        }
        UARTstring(CRLN);
        UARTstringCRLN("Error: Remote button not specified");
        UARTstring(CRLN);
        UARTstringCRLN("Usage:");
        UARTstringCRLN("RemoteButton'x'");
        UARTstring(CRLN);
        UARTstringCRLN("Example:");
        UARTstringCRLN("RemoteButton1");
        UARTstring(CRLN);
    }
    else
    {
        UARTstringCRLN(SYNTAX_ERR);
        UARTstringCRLN("Enter '???' Command Menu");
        UARTstring(CRLN);
        return FAIL;
    }
    if(ok == FALSE)
    {
        /* Conf1 of Conf2 was in the string but syntax error exists */
        UARTstring(SYNTAX_ERR);
        UARTstring(CRLN);
    }

    if(StringMatch(ReceivedString,"???") || ok == FALSE)
    {
        UARTstring(CRLN);
        UARTstringCRLN("~~~~~~~~~~~~~~~~~~~~~~~~~Command Menu~~~~~~~~~~~~~~~~~~~~~~~~~~");
        UARTstring(CRLN);
        UARTstringCRLN("System commands:");
        UARTcommandMenu("???", "Help Menu");
        UARTstring(CRLN);
        UARTstringCRLN("RF commands:");
        UARTcommandMenu(Conf1_ChannelD_STR, "RFK100LC Channel D");
        UARTcommandMenu(Conf1_ChannelE_STR, "RFK100LC Channel E");
        UARTcommandMenu(Conf1_ChannelF_STR, "RFK100LC Channel F");
        UARTcommandMenu(Conf2_ChannelB_ON_STR, "RFK306LC Channel B ON");
        UARTcommandMenu(Conf2_ChannelB_OFF_STR, "RFK306LC Channel B OFF");
        UARTcommandMenu(Conf2_ChannelD_ON_STR, "TR009 Channel D ON");
        UARTcommandMenu(Conf2_ChannelD_OFF_STR, "TR009 Channel D OFF");
        UARTcommandMenu(Conf2_ChannelH_1_ON_STR, "RC-015*3 Channel H device 1 ON");
        UARTcommandMenu(Conf2_ChannelH_1_OFF_STR, "RC-015*3 Channel H device 1 OFF");
        UARTcommandMenu(Conf2_ChannelH_2_ON_STR, "RC-015*3 Channel H device 2 ON");
        UARTcommandMenu(Conf2_ChannelH_2_OFF_STR, "RC-015*3 Channel H device 2 OFF");
        UARTcommandMenu(Conf2_ChannelH_3_ON_STR, "RC-015*3 Channel H device 3 ON");
        UARTcommandMenu(Conf2_ChannelH_3_OFF_STR, "RC-015*3 Channel H device 3 OFF");
        UARTstring(CRLN);
    }

    return FAIL;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
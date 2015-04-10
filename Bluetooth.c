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
#include "Timer.h"
#include "user.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern const unsigned char Version[];
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
    cleanBuffer(ReceivedString,RXbufsize);
    ReceivedStringPos = 0;
    cleanBuffer(CommandString,RXCommandsize);
    CommandStringPos = 0;

    LATA &= ~BLUE_Factory; // turn off factory reset
    LATA |= BLUE_AutoDiscovery; // Turn on Autodiscovery
    LATA &= ~BLUE_AutoMaster; //turn off auto master
    if(!BluetoothConfigured())
    {
        BluetoothInitialSetup();
        ResetBlue();
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
    UARTstring((unsigned char *)"$$$");
    delayUS(CommandWait);
    if(StringContains(CommandString,(unsigned char *)"CMD"))
    {
        cleanBuffer(CommandString,CommandStringPos);
        CommandStringPos = 0;
        return PASS;
    }
    cleanBuffer(CommandString,CommandStringPos);
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
    unsigned char RFchannel[10];
    
    cleanBuffer(buf,50);
    cleanBuffer(RFchannel,10);

    if(!EnterCommandMode())
    {
        return FAIL;
    }
    sprintf(buf,"SN,Home Automation %ld\r",SN);
    UARTstring(buf); // Service name to home automation and serial number
    delayUS(CommandWait);
    if(!StringContains(CommandString,(unsigned char *)"AOK"))
    {
        ok = FALSE;
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring((unsigned char *)"SU, 115K\r"); // Baud rate to 115200
        delayUS(CommandWait);
        if(!StringContains(CommandString,(unsigned char *)"AOK"))
        {
            ok = FALSE;
        }
    }

    if(ok)
    {
        UARTstring((unsigned char *)"SS, Home Automation\r"); // Service name to home automation
        delayUS(CommandWait);
        if(!StringContains(CommandString,(unsigned char *)"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring((unsigned char *)"SY,0010\r"); // Set Power to 16dBM
        delayUS(CommandWait);
        if(!StringContains(CommandString,(unsigned char *)"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring((unsigned char *)"S~,0\r"); // SPP profile
        delayUS(CommandWait);
        if(!StringContains(CommandString,(unsigned char *)"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring((unsigned char *)"S~,0\r"); // SPP profile
        delayUS(CommandWait);
        if(!StringContains(CommandString,(unsigned char *)"AOK"))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        UARTstring((unsigned char *)"D,0\r"); // Check for name
        delayUS(CommandWait);
        sprintf(buf,"BTName=Home Automation %ld\r",SN);
        if(!StringContains(CommandString, buf))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    UARTstring((unsigned char *)"---\r"); // exit command mode
    delayUS(CommandWait);
    if(!StringContains(CommandString,(unsigned char *)"END"))
    {
        ok = FALSE;
    }
    cleanBuffer(CommandString,CommandStringPos);
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
unsigned char UseBluetooth(unsigned char *String, unsigned char StringPos)
{
    unsigned char ok = TRUE;

    unsigned long EnteredNEC = 0;
    unsigned char NecAddress = 0;
    unsigned char NecCommand = 0;
    unsigned long temp = 0;
    unsigned long tempNec = 0;
    unsigned char buf[100];
    unsigned char timeout =0;
    unsigned char rfchannelSTR[10];
    unsigned char rfconf, i, tempRFArray;
    unsigned char device = 0;
    unsigned char EmptyPlace = 0;
    unsigned char set = FALSE;

    cleanBuffer(buf,100);
    cleanBuffer(rfchannelSTR,10);

    if(StringContainsCaseInsensitive(String,(unsigned char *)"Conf1"))
    {
        if(StringMatchCaseInsensitive(String,Conf1_ChannelD_STR))
        {
            // Configuration 1 channel D
            SendRF_wait(Conf1_ChannelD,1,12,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf1_ChannelE_STR))
        {
            // Configuration 1 channel E
            SendRF_wait(Conf1_ChannelE,1,12,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf1_ChannelF_STR))
        {
            // Configuration 1 channel F
            SendRF_wait(Conf1_ChannelF,1,12,8);
        }
        else
        {
            /* Invalid command */
            UARTstring((unsigned char *)SYNTAX_ERR);
            UARTstring((unsigned char *)CRLN);
            return FAIL;
        }
        if(ok)
        {
            UARTstringCRLN((unsigned char *)"Configuration 1 RF code sent");
            return PASS;
        }
    }
    else if(StringContainsCaseInsensitive(String,(unsigned char *)"Conf2"))
    {
        if(StringMatchCaseInsensitive(String,Conf2_ChannelB_ON_STR))
        {
            // Configuration 2 channel B ON
            SendRF_wait(Conf2_ChannelB_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelB_OFF_STR))
        {
            // Configuration 2 channel B OFF
            SendRF_wait(Conf2_ChannelB_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelD_ON_STR))
        {
            // Configuration 2 channel D ON
            SendRF_wait(Conf2_ChannelD_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelD_OFF_STR))
        {
            // Configuration 2 channel D OFF
            SendRF_wait(Conf2_ChannelD_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelH_1_ON_STR))
        {
            // Configuration 2 channel H device 1 ON
            SendRF_wait(Conf2_ChannelH_1_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelH_1_OFF_STR))
        {
            // Configuration 2 channel H device 1 OFF
            SendRF_wait(Conf2_ChannelH_1_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelH_2_ON_STR))
        {
            // Configuration 2 channel H device 2 ON
            SendRF_wait(Conf2_ChannelH_2_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelH_2_OFF_STR))
        {
            // Configuration 2 channel H device 2 OFF
            SendRF_wait(Conf2_ChannelH_2_OFF,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelH_3_ON_STR))
        {
            // Configuration 2 channel H device 3 ON
            SendRF_wait(Conf2_ChannelH_3_ON,2,16,8);
        }
        else if(StringMatchCaseInsensitive(String,Conf2_ChannelH_3_OFF_STR))
        {
            // Configuration 2 channel H device 3 OFF
            SendRF_wait(Conf2_ChannelH_2_OFF,3,16,8);
        }
        else
        {
            /* Invalid command */
            UARTstring((unsigned char *)SYNTAX_ERR);
            UARTstring(CRLN);
            return FAIL;
        }
        if(ok)
        {
            UARTstringCRLN((unsigned char *)"Configuration 2 RF code sent");
            return PASS;
        }
    }
    else if(StringContainsCaseInsensitive(String,(unsigned char *)"NEC"))
    {
        if(!GetNumber(String, 2, &temp))
        {
            if(temp > 0xFF)
            {
                UARTstringCRLN((unsigned char *)"Error: Command larger that 255");
                UARTstring(CRLN);
                return FAIL;
            }
            NecCommand = (unsigned char)temp;
            if(!GetNumber(String, 1, &temp))
            {
                if(temp > 0xFF)
                {
                    UARTstringCRLN((unsigned char *)"Error: Address larger that 255");
                    UARTstring(CRLN);
                    return FAIL;
                }
                NecAddress = (unsigned char) temp;
                sprintf(buf,"NEC address: 0x%02X ", NecAddress);
                UARTstringCRLN(buf);
                sprintf(buf,"NEC command: 0x%02X ", NecCommand);
                UARTstringCRLN(buf);
                EnteredNEC = EncodeNEC(NecAddress, NecCommand);
                SendNEC_wait(EnteredNEC,0);
                sprintf(buf,"Sent NEC code = 0x%lX ", EnteredNEC);
                UARTstringCRLN(buf);
                UARTstring(CRLN);
            }
            else
            {
                UARTstringCRLN((unsigned char *)"Error: could not decode Address");
            }
        }
        else if(!GetNumber(String, 1, &EnteredNEC))
        {
            SendNEC_wait(EnteredNEC,0);
            sprintf(buf,"sent NEC code 0x%lX", EnteredNEC);
            UARTstringCRLN(buf);
            if(DecodeNEC(EnteredNEC, &NecAddress, &NecCommand))
            {
                sprintf(buf,"address: 0x%02X ", NecAddress);
                UARTstringCRLN(buf);
                sprintf(buf,"command: 0x%02X ", NecCommand);
                UARTstringCRLN(buf);
            }
            else
            {
                UARTstringCRLN((unsigned char *)"Error: could not decode NEC code");
            }
            UARTstring(CRLN);
            return PASS;
        }
        else
        {
            UARTstring(CRLN);
            UARTstringCRLN((unsigned char *)"NEC code not entered correctly");
            UARTstring(CRLN);
            UARTstringCRLN((unsigned char *)"Usage:");
            UARTstringCRLN((unsigned char *)"NEC = (32 bit code)");
            UARTstringCRLN((unsigned char *)"        or");
            UARTstringCRLN((unsigned char *)"NEC = address,command");
            UARTstring(CRLN);
            UARTstringCRLN((unsigned char *)"Example:");
            UARTstringCRLN((unsigned char *)"NEC = 0x1CE350af ");
            UARTstringCRLN((unsigned char *)"      or");
            UARTstringCRLN((unsigned char *)"NEC = 0x38, 0x0A");
            UARTstring(CRLN);
            return FAIL;
        }
    }
    else if(StringContainsCaseInsensitive(String,(unsigned char *)"RemoteButton"))
    {
        if(StringAddEqual(String))
        {
            StringPos++; // we added one place by adding an equal sine
            if(!GetNumber(String, 1, &temp))
            {
                if(temp <= ButtonAmount && temp > 0)
                {
                    if(!StringContainsCaseInsensitive(String,(unsigned char *)"set"))
                    {
                        tempNec =  EncodeNEC(Global.RemoteButtonNEC[temp-1][0], Global.RemoteButtonNEC[temp-1][1]);
                        if(tempNec != 0x00FF00FF)
                        {
                            SendNEC_wait(tempNec,0);
                            sprintf(buf,"Sent NEC code 0x%lX ", tempNec);
                            UARTstringCRLN(buf);
                            sprintf(buf,"NEC Code address: 0x%02X , command: 0x%02X ",Global.RemoteButtonNEC[temp-1][0], Global.RemoteButtonNEC[temp-1][1]);
                            UARTstringCRLN(buf);
                            UARTstring(CRLN);
                            return PASS;
                        }
                        UARTstring(CRLN);
                        UARTstringCRLN((unsigned char *)"Error: No NEC code saved for specified command");
                        return FAIL;
                    }
                    else
                    {
                        if(!WaitForIRsignal())
                        {
                            return FAIL;
                        }

                        /* Save NEC code */
                        DecodeNEC(IR_NEC, &NecAddress, &NecCommand);
                        Global.RemoteButtonNEC[temp - 1][0] = NecAddress;
                        Global.RemoteButtonNEC[temp - 1][1] = NecCommand;
                        SyncGlobalToEEPROM();
                        UARTstring(CRLN);
                        sprintf(buf,"RemoteButton%ld now transmits NEC code 0x%lX ",temp, IR_NEC);
                        UARTstringCRLN(buf);
                        sprintf(buf,"NEC Code address: 0x%02X , command: 0x%02X ",NecAddress, NecCommand);
                        UARTstringCRLN(buf);
                        UARTstring(CRLN);
                        /* successful EEPROM burn */
                        for(i =0;i<10;i++)
                        {
                           GreenLEDON();
                           delayUS(50000);
                           GreenLEDOFF();
                           delayUS(50000);
                        }
                        return PASS;
                    }
                }
                else
                {
                    UARTstring(CRLN);
                    UARTstringCRLN((unsigned char *)"Error: Remote button number too high");
                    UARTstring(CRLN);
                    return FAIL;
                }
            }
        }
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"Error: Remote button not specified");
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"Usage:");
        UARTstringCRLN((unsigned char *)"RemoteButton'x'");
        UARTstringCRLN((unsigned char *)"RemoteButton'x' set");
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"Example:");
        UARTstringCRLN((unsigned char *)"RemoteButton1");
        UARTstringCRLN((unsigned char *)"RemoteButton1 set");
        UARTstring(CRLN);
        return FAIL;
    }
    else if(StringContainsCaseInsensitive(String,(unsigned char *)"RF set") || StringContainsCaseInsensitive(String,(unsigned char *)"RF clear"))
    {
        if(StringContainsCaseInsensitive(String,(unsigned char *)"RF set"))
        {
            set = TRUE;
        }
        else
        {
            set = FALSE;
        }
        if(!StringAddEqual(String))
        {
            UARTstringCRLN((unsigned char *)"Error: No RF configuration specified");
            UARTstring(CRLN);
            return FAIL;
        }
        StringPos++; // we added one place by adding an equal sine
        UARTstring(CRLN);
        if(!GetNumber(String, 1, &temp))
        {
            rfconf = (unsigned char) temp;
        }
        else
        {
            UARTstringCRLN((unsigned char *)"Error: No RF configuration specified");
            UARTstring(CRLN);
            return FAIL;
        }
        if(rfconf <= 0 || rfconf > NumRfConfigs)
        {
            UARTstringCRLN((unsigned char *)"Error: RF configuration out of range");
            UARTstring(CRLN);
            return FAIL;
        }
        if(GetStringAfterComma(String, 1, rfchannelSTR))
        {
            UARTstringCRLN((unsigned char *)"Error: No RF channel specified");
            UARTstring(CRLN);
            return FAIL;
        }
        else
        {
            lowercaseString(rfchannelSTR);
            if(rfchannelSTR[1] == 0 || rfchannelSTR[1] == ' ')
            {
                rfchannelSTR[1] = 0; // this is needed for function 'StringContainsCaseInsensitive'
                if(rfconf == 1)
                {
                    if(!StringContainsCaseInsensitive(Conf1_Channels, rfchannelSTR))
                    {
                        ok = FALSE;
                    }
                }
                else if(rfconf == 2)
                {
                    if(!StringContainsCaseInsensitive(Conf2_Channels, rfchannelSTR))
                    {
                        ok = FALSE;
                    }
                }
                if(!ok)
                {
                    UARTstringCRLN((unsigned char *)"Error: RF channel out of range");
                    UARTstring(CRLN);
                    return FAIL;
                }

                if(rfconf == 1)
                {
                    if(rfchannelSTR[0] == 'd')
                    {
                        tempRFArray = 0;
                    }
                    else if(rfchannelSTR[0] == 'e')
                    {
                        tempRFArray = 1;
                    }
                    else if(rfchannelSTR[0] == 'f')
                    {
                        tempRFArray = 2;
                    }
                    else
                    {
                        // Got here by mistake
                        return FAIL;
                    }
                }
                else if(rfconf ==2)
                {
                    if(rfchannelSTR[0] == 'b')
                    {
                        tempRFArray = 3;
                    }
                    else if(rfchannelSTR[0] == 'd')
                    {
                        tempRFArray = 4;
                    }
                    else if(rfchannelSTR[0] == 'h')
                    {
                        cleanBuffer(ReceivedString, ReceivedStringPos);
                        ReceivedStringPos = 0;
                        NewReceivedString = FALSE;
                        UARTstring((unsigned char *)"Which device?");
                        UARTstring(CRLN);
                        UARTchar('>');
                        timeout = 0;
                        ClearUSART();
                        PIR1bits.RCIF = FALSE;
                        PIE1bits.RCIE   = TRUE;
                        while(!NewReceivedString)
                        {
                            delayUS(300000);
                            timeout++;
                            if(timeout > 25)
                            {
                                UARTstring(CRLN);
                                UARTstringCRLN((unsigned char *)"Error: No RF config 2, channel H device specified");
                                return FAIL;
                            }
                        }
                        PIE1bits.RCIE   = FALSE;
                        ok = TRUE;
                        if(ReceivedString[0] < '1' || ReceivedString[0] > '3')
                        {
                            UARTstringCRLN((unsigned char *)"Error: RF config 2, channel H device out of range");
                            ok = FALSE;
                        }
                        cleanBuffer(ReceivedString, ReceivedStringPos);
                        ReceivedStringPos = 0;
                        NewReceivedString = FALSE;
                        if(!ok)
                        {
                            return FAIL;
                        }
                        device = String[0] - '0';
                        tempRFArray = device + 4;
                    }
                    else
                    {
                        // Got here by mistake
                        return FAIL;
                    }
                }
                if(set)
                {
                    if(!WaitForIRsignal())
                    {
                        return FAIL;
                    }
                    UARTstring(CRLN);
                    DecodeNEC(IR_NEC, &NecAddress, &NecCommand);
                    ok = FALSE;
                    for(i=0; i < MirrorButtonsAmount; i++)
                    {
                        if(Global.RemoteButtonRF[tempRFArray][i][0] == 0 && Global.RemoteButtonRF[tempRFArray][i][1] == 0)
                        {
                            EmptyPlace = i;
                            ok = TRUE;
                            break;
                        }
                    }
                    if(!ok)
                    {
                        UARTstringCRLN((unsigned char *)"Error: No space available to this RF code");
                        return FAIL;
                    }
                    ok = TRUE;
                    if(EmptyPlace)
                    {
                        /* Check to see if this NEC is already saved */
                        for(i=0; i < EmptyPlace; i++)
                        {
                            if(NecAddress == Global.RemoteButtonRF[tempRFArray][i][0])
                            {
                                if(NecCommand == Global.RemoteButtonRF[tempRFArray][i][1])
                                {
                                    ok = FALSE;
                                    break;
                                }
                            }
                        }
                    }
                    if(!ok)
                    {
                        UARTstringCRLN((unsigned char *)"Error: NEC code already saved to this RF code");
                        return FAIL;
                    }

                    /* save new code to eeprom */
                    Global.RemoteButtonRF[tempRFArray][EmptyPlace][0] = NecAddress;
                    Global.RemoteButtonRF[tempRFArray][EmptyPlace][1] = NecCommand;
                    SyncGlobalToEEPROM();

                    if(device)
                    {
                        sprintf(buf,"NEC code 0x%lX now transmits RF config %d channel %c device %d",IR_NEC, rfconf, rfchannelSTR[0], device);
                    }
                    else
                    {
                        sprintf(buf,"NEC code 0x%lX now transmits RF config %d channel %c ",IR_NEC, rfconf, rfchannelSTR[0]);
                    }
                    UARTstringCRLN(buf);
                    UARTstring(CRLN);
                    /* successful EEPROM burn */
                    for(i =0;i<10;i++)
                    {
                       GreenLEDON();
                       delayUS(50000);
                       GreenLEDOFF();
                       delayUS(50000);
                    }
                    return PASS;
                }
                else
                {
                    /* RF clear */
                    for(i=0; i < MirrorButtonsAmount; i++)
                    {
                        /* clear codes from eeprom */
                        Global.RemoteButtonRF[tempRFArray][i][0] = 0;
                        Global.RemoteButtonRF[tempRFArray][i][1] = 0;
                    }
                    SyncGlobalToEEPROM();
                    if(device)
                    {
                        sprintf(buf,"RF config %d channel %c device %d successfully erased",rfconf, rfchannelSTR[0], device);
                    }
                    else
                    {
                        sprintf(buf,"RF config %d channel %c successfully erased",rfconf, rfchannelSTR[0]);
                    }
                    UARTstringCRLN(buf);
                    UARTstring(CRLN);
                    /* successful EEPROM burn */
                    for(i =0;i<10;i++)
                    {
                       GreenLEDON();
                       delayUS(50000);
                       GreenLEDOFF();
                       delayUS(50000);
                    }
                    return PASS;
                }
            }
            else
            {
                UARTstringCRLN((unsigned char *)"Error: RF channel needs to be one character");
                UARTstring(CRLN);
                return FAIL;
            }
        }
    }
    else if(StringMatchCaseInsensitive(String,(unsigned char *)"Reset"))
    {
        cleanBuffer(ReceivedString, ReceivedStringPos);
        ReceivedStringPos = 0;
        NewReceivedString = FALSE;
        UARTstring((unsigned char *)"Enter Y to reset or N to cancel");
        UARTstring(CRLN);
        UARTchar('>');
        timeout = 0;
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
        while(!NewReceivedString)
        {
            delayUS(300000);
            timeout++;
            if(timeout > 25)
            {
                UARTstring(CRLN);
                UARTstringCRLN((unsigned char *)"Device resumes without reset");
                return FAIL;
            }
        }
        PIE1bits.RCIE   = FALSE;
        if(ReceivedString[0] == 'Y' || ReceivedString[0] == 'y' && ReceivedString[1] == 0)
        {
            /* Erase EEPROM flag and reset device */
            UARTstringCRLN((unsigned char *)"System will reset");
            Global.EEPROMinitFlag = FAIL;
            SyncGlobalToEEPROM();
            RESET();
            return PASS;
        }
        else
        {
            UARTstring(CRLN);
            UARTstringCRLN((unsigned char *)"Device resumes without reset");
        }
        cleanBuffer(ReceivedString, ReceivedStringPos);
        ReceivedStringPos = 0;
        NewReceivedString = FALSE;
        return FAIL;
    }
    else if(StringMatchCaseInsensitive(String,(unsigned char *)"Voltage"))
    {
        UARTstring(CRLN);
        sprintf(buf,"Supply Voltage = %f", (double) ReadVoltage());
        UARTstringCRLN(buf);
        UARTstring(CRLN);
    }
    else if(StringMatchCaseInsensitive(String,(unsigned char *)"Version"))
    {
        UARTstring(CRLN);
        sprintf(buf,"Firmware Version: %s", Version);
        UARTstringCRLN(buf);
        UARTstring(CRLN);
    }
    else if(StringMatch(String,(unsigned char *)"???"))
    {
        UARTstring(CRLN);
        for(i=0; i< 34; i++ )
        {
            UARTchar('~');
        }
        UARTstring((unsigned char *)"Command Menu");
        for(i=0; i< 34; i++ )
        {
            UARTchar('~');
        }
        UARTstring(CRLN);
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"System commands:");
        UARTcommandMenu((unsigned char *)"???", (unsigned char *)"Help Menu");
        UARTcommandMenu((unsigned char *)"Reset", (unsigned char *)"Clears memory and resets device");
        UARTcommandMenu((unsigned char *)"Version", (unsigned char *)"Displays Firmware Version");
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"Diagnostic Commands:");
        UARTcommandMenu((unsigned char *)"Voltage", (unsigned char *)"Displays the supply voltage");
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"IR commands:");
        for(i=1; i <= ButtonAmount; i++)
        {
            cleanBuffer(buf,50);
            sprintf(buf,"RemoteButton%d",i);
            UARTcommandMenu(buf, "Transmits saved NEC code");
        }
        for(i=1; i <= ButtonAmount; i++)
        {
            cleanBuffer(buf,50);
            sprintf(buf,"RemoteButton%d set",i);
            UARTcommandMenu(buf,(unsigned char *) "Allows saving NEC codes");
        }
        UARTcommandMenu("RF set 1,D", "Sets Remote button to send RF Config 1 channel D");
        UARTcommandMenu("RF set 1,E", "Sets Remote button to send RF Config 1 channel E");
        UARTcommandMenu("RF set 1,F", "Sets Remote button to send RF Config 1 channel F");
        UARTcommandMenu("RF set 2,B", "Sets Remote button to send RF Config 2 channel B");
        UARTcommandMenu("RF set 2,D", "Sets Remote button to send RF Config 2 channel D");
        UARTcommandMenu("RF set 2,H", "Sets Remote button to send RF Config 2 channel H");
        UARTcommandMenu("RF clear 1,D", "Clears saved Button for RF Config 1 channel D");
        UARTcommandMenu("RF clear 1,E", "Clears saved Button for RF Config 1 channel E");
        UARTcommandMenu("RF clear 1,F", "Clears saved Button for RF Config 1 channel F");
        UARTcommandMenu("RF clear 2,B", "Clears saved Button for RF Config 2 channel B");
        UARTcommandMenu("RF clear 2,D", "Clears saved Button for RF Config 2 channel D");
        UARTcommandMenu("RF clear 2,H", "Clears saved Button for RF Config 2 channel H");
        UARTcommandMenu((unsigned char *)"NEC?", (unsigned char *)"NEC description for Arbitrary code send");
        UARTstring(CRLN);
        UARTstringCRLN((unsigned char *)"RF commands:");
        UARTcommandMenu(Conf1_ChannelD_STR, (unsigned char *)"RFK100LC Channel D");
        UARTcommandMenu(Conf1_ChannelE_STR, (unsigned char *)"RFK100LC Channel E");
        UARTcommandMenu(Conf1_ChannelF_STR, (unsigned char *)"RFK100LC Channel F");
        UARTcommandMenu(Conf2_ChannelB_ON_STR, (unsigned char *)"RFK306LC Channel B ON");
        UARTcommandMenu(Conf2_ChannelB_OFF_STR, (unsigned char *)"RFK306LC Channel B OFF");
        UARTcommandMenu(Conf2_ChannelD_ON_STR, (unsigned char *)"TR009 Channel D ON");
        UARTcommandMenu(Conf2_ChannelD_OFF_STR, (unsigned char *)"TR009 Channel D OFF");
        UARTcommandMenu(Conf2_ChannelH_1_ON_STR, (unsigned char *)"RC-015*3 Channel H device 1 ON");
        UARTcommandMenu(Conf2_ChannelH_1_OFF_STR, (unsigned char *)"RC-015*3 Channel H device 1 OFF");
        UARTcommandMenu(Conf2_ChannelH_2_ON_STR, (unsigned char *)"RC-015*3 Channel H device 2 ON");
        UARTcommandMenu(Conf2_ChannelH_2_OFF_STR, (unsigned char *)"RC-015*3 Channel H device 2 OFF");
        UARTcommandMenu(Conf2_ChannelH_3_ON_STR, (unsigned char *)"RC-015*3 Channel H device 3 ON");
        UARTcommandMenu(Conf2_ChannelH_3_OFF_STR, (unsigned char *)"RC-015*3 Channel H device 3 OFF");
        UARTstring(CRLN);
    }
    else
    {
        UARTstringCRLN(SYNTAX_ERR);
        UARTstringCRLN((unsigned char *)"Enter ??? for Command Menu");
        UARTstring(CRLN);
        return FAIL;
    }

    return FAIL;
}

/******************************************************************************/
/* WaitForIRsignal
 *
 * This function is called inorder to allow for user input.
/******************************************************************************/
unsigned char WaitForIRsignal(void)
{
    unsigned char timeout = 0;

    UARTstring(CRLN);
    UARTstring((unsigned char *)"Press Remote Control button");
    timeout = 0;
    while(IR_New_Code != New)
    {
        UARTchar('.');
        delayUS(300000);
        timeout++;
        if(timeout > 25)
        {
            UARTstring(CRLN);
            UARTstringCRLN((unsigned char *)"Error: No remote button was pressed");
            return FAIL;
        }
    }
    GreenLEDON();
    LEDTimerON();
    return TRUE;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
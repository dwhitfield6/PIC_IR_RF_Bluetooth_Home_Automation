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
 *                          Fixed bug in conf2 channel H parsing.
 *                          Only set bluetooth configured flag if it is indeed
 *                            configured.
 *                          Add new command rf set all, rf clear all,
 *                            rf clear system.
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
#include "ADC.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
extern const unsigned char FirmVersion[];
extern const unsigned char PCBVersion[];
extern const unsigned char Frequency[];
extern unsigned char BluetoothFirmware1[BlueFWbuf];
extern unsigned char BluetoothFirmware2[BlueFWbuf];
extern unsigned char BluetoothBroadcast[BroadcastSize];
unsigned char BluetoothChangeStatus = FALSE;

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
        if(BluetoothInitialSetup(TRUE))
        {
            Global1.BlueToothFlag = TRUE;
            SyncGlobalToEEPROM();
        }
        ResetBlue();
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
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
        return TRUE;
    }
    return FALSE;
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
    delayUS(CommandWait);
    delayUS(CommandWait);
    delayUS(CommandWait);
    delayUS(CommandWait);
    LATA |= BLUE_Reset; // Take module out of reset
    delayUS(CommandWait);
    delayUS(CommandWait);
    delayUS(CommandWait);
    delayUS(CommandWait);
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
    unsigned char buf[50];
    while(BlueConnected())
    {
        ResetBlue();
        count++;
        if(count > 4)
        {
            /* Wouldn't enter command mode after 4 tries. */
            return FAIL;
        }
        delayUS(CommandWait);
        delayUS(CommandWait);
        delayUS(CommandWait);
        delayUS(CommandWait);
        delayUS(CommandWait);
        delayUS(CommandWait);
    }

    ClearUSART();
    PIR1bits.RCIF = FALSE;
    PIE1bits.RCIE   = TRUE;
    UARTstring_CONST("$$$");
    delayUS(CommandWait);
    PIE1bits.RCIE   = FALSE;
    sprintf(buf,"CMD");
    if(StringContains(CommandString,buf))
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
unsigned char BluetoothInitialSetup(unsigned char Default)
{
    unsigned char buf[50];
    unsigned char ok = TRUE;
    unsigned char BluetoothVersionLine1[BlueFWbuf];
    unsigned char BluetoothVersionLine2[BlueFWbuf];
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char first = TRUE;
    unsigned char BroadcastTEMP[BroadcastSize];

    cleanBuffer(buf,50);
    cleanBuffer(BroadcastTEMP,BroadcastSize);
    cleanBuffer(BluetoothVersionLine1,BlueFWbuf);
    cleanBuffer(BluetoothVersionLine2,BlueFWbuf);

    if(!EnterCommandMode())
    {
        return FAIL;
    }

    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;
    ClearUSART();
    PIR1bits.RCIF = FALSE;
    PIE1bits.RCIE   = TRUE;
    UARTstring_CONST("V\r"); // Print bluetooth firmware version
    delayUS(CommandWait);
    PIE1bits.RCIE   = FALSE;
    for(i=0; i < (BlueFWbuf * 2); i++)
    {
        if(first == TRUE)
        {
            if(CommandString[i] == '\n' || CommandString[i] == '\r')
            {
                first = FALSE;
            }
            else
            {
                if( i < (BlueFWbuf - 1))
                {
                    BluetoothVersionLine1[i] = CommandString[i];
                }
            }
        }
        else
        {
            if(CommandString[i] != '\n' && CommandString[i] != '\r')
            {
                if( j < (BlueFWbuf - 1))
                {
                    BluetoothVersionLine2[j] = CommandString[i];
                }
                j++;
            }
            else
            {
                if(j > 2)
                {
                    /* greater than 2 characters displayed in second line */
                    break;
                }
            }
        }
    }

    if(BluetoothVersionLine1[0] == 0 || BluetoothVersionLine2[0] == 0)
    {
        ok = FALSE;
    }
    else
    {
        cleanBuffer(BluetoothFirmware1,BlueFWbuf);
        cleanBuffer(BluetoothFirmware2,BlueFWbuf);
        BufferCopy(BluetoothVersionLine1, BluetoothFirmware1, BlueFWbuf, 0);
        BufferCopy(BluetoothVersionLine2, BluetoothFirmware2, BlueFWbuf, 0);
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(Default)
    {
        if(ok)
        {
            sprintf(buf,"SN,Home Automation %lu\r",Global2.SerialNumber);
            ClearUSART();
            PIR1bits.RCIF = FALSE;
            PIE1bits.RCIE   = TRUE;
            UARTstring(buf); // Service name to home automation and serial number
            delayUS(CommandWait);
            PIE1bits.RCIE   = FALSE;
            sprintf(buf,"AOK");
            if(!StringContains(CommandString,buf))
            {
                ok = FALSE;
            }
        }
    }
    else
    {
        if(ok)
        {
            if(BluetoothBroadcast[BroadcastSize-1] == 0 )
            {
                BufferCopy(BluetoothBroadcast,BroadcastTEMP, BroadcastSize, 0);
                sprintf(buf,"SN,%s\r",BroadcastTEMP);
                ClearUSART();
                PIR1bits.RCIF = FALSE;
                PIE1bits.RCIE   = TRUE;
                UARTstring(buf); // Service name to home automation and serial number
                delayUS(CommandWait);
                PIE1bits.RCIE   = FALSE;
                sprintf(buf,"AOK");
                if(!StringContains(CommandString,buf))
                {
                    ok = FALSE;
                }
            }
            else
            {
                ok = FALSE;
            }
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
        UARTstring_CONST("SU,115K\r"); // Baud rate to 115200
        delayUS(CommandWait);
        PIE1bits.RCIE   = FALSE;
        if(!StringContains(CommandString,buf))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
        UARTstring_CONST("SS,Home Automation\r"); // Service name to home automation
        delayUS(CommandWait);
        PIE1bits.RCIE   = FALSE;
        if(!StringContains(CommandString,buf))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
        UARTstring_CONST("SY,0010\r"); // Set Power to 16dBM
        delayUS(CommandWait);
        PIE1bits.RCIE   = FALSE;
        if(!StringContains(CommandString,buf))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
        UARTstring_CONST("S~,0\r"); // SPP profile
        delayUS(CommandWait);
        PIE1bits.RCIE   = FALSE;
        if(!StringContains(CommandString,buf))
        {
            ok = FALSE;
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    if(ok)
    {
        ClearUSART();
        PIR1bits.RCIF = FALSE;
        PIE1bits.RCIE   = TRUE;
        UARTstring_CONST("D\r"); // Check for name
        delayUS(CommandWait);
        PIE1bits.RCIE   = FALSE;
        if(Default)
        {
            sprintf(buf,"BTName=Home Automation %lu\r",Global2.SerialNumber);
        }
        else
        {
            sprintf(buf,"BTName=%s\r",BroadcastTEMP);
        }
        if(!StringContainsCaseInsensitive(CommandString, buf))
        {
            if(Global2.SerialNumber != 0xFFFFFFFF)
            {
                ok = FALSE;
            }
        }
    }
    cleanBuffer(CommandString,CommandStringPos);
    CommandStringPos = 0;

    ClearUSART();
    PIR1bits.RCIF = FALSE;
    PIE1bits.RCIE   = TRUE;
    UARTstring_CONST("---\r"); // exit command mode
    delayUS(CommandWait);
    sprintf(buf,"END");
    PIE1bits.RCIE   = FALSE;
    if(!StringContains(CommandString,buf))
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
    if(Global1.BlueToothFlag != TRUE)
    {
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
    unsigned char rfconf, i, j, k, tempRFArray;
    unsigned char device = 0;
    unsigned char EmptyPlace = 0;
    unsigned char set = FALSE;
    long SerialNumberTEMP = 0;
    unsigned char ReceivedStringPosOLD = FALSE;
    unsigned char system;

    cleanBuffer(buf,100);
    cleanBuffer(rfchannelSTR,10);

    if(StringMatchCaseInsensitive(String,"Rf All"))
    {
        for(i=0; i < RFnumberOfSavedCodes; i++)
        {
            SendRF_Channel(i);
            UARTstring_CONST("Sent ");
            DisplayRF_Channel(i);
            UARTstring_CONST(CRLN);
        }
        return PASS;
    }
    else if(StringContainsCaseInsensitive(String,"Conf1"))
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
            UARTstring_CONST(SYNTAX_ERR);
            UARTstring_CONST(CRLN);
            return FAIL;
        }
        UARTstringCRLN_CONST("Configuration 1 RF code sent");
        return PASS;
    }
    else if(StringContainsCaseInsensitive(String,"Conf2"))
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
            UARTstring_CONST(SYNTAX_ERR);
            UARTstring_CONST(CRLN);
            return FAIL;
        }
        UARTstringCRLN_CONST("Configuration 2 RF code sent");
        return PASS;
    }
    else if(StringContainsCaseInsensitive(String,"NEC"))
    {
        if(!GetNumberUnsigned(String, 2, &temp))
        {
            if(temp > 0xFF)
            {
                UARTstringCRLN_CONST("Error: Command larger that 255");
                UARTstring_CONST(CRLN);
                return FAIL;
            }
            NecCommand = (unsigned char)temp;
            if(!GetNumberUnsigned(String, 1, &temp))
            {
                if(temp > 0xFF)
                {
                    UARTstringCRLN_CONST("Error: Address larger that 255");
                    UARTstring_CONST(CRLN);
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
                UARTstring_CONST(CRLN);
                return PASS;
            }
            else
            {
                UARTstringCRLN_CONST("Error: could not decode Address");
                UARTstring_CONST(CRLN);
                return FAIL;
            }
        }
        else if(!GetNumberUnsigned(String, 1, &EnteredNEC))
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
                UARTstringCRLN_CONST("Error: could not decode NEC code");
                UARTstring_CONST(CRLN);
                return FAIL;
            }
            UARTstring_CONST(CRLN);
            return PASS;
        }
        else
        {
            if(!StringContains(String,"?"))
            {
                UARTstring_CONST(CRLN);
                UARTstringCRLN_CONST("NEC code not entered correctly");
            }
            UARTstring_CONST(CRLN);
            UARTstringCRLN_CONST("Usage:");
            UARTstringCRLN_CONST("NEC = (32 bit code)");
            UARTstringCRLN_CONST("        or");
            UARTstringCRLN_CONST("NEC = address,command");
            UARTstring_CONST(CRLN);
            UARTstringCRLN_CONST("Example:");
            UARTstringCRLN_CONST("NEC = 0x1CE350AF ");
            UARTstringCRLN_CONST("      or");
            UARTstringCRLN_CONST("NEC = 0x38, 0x0A");
            UARTstring_CONST(CRLN);
            return FAIL;
        }
    }
    else if(StringContainsCaseInsensitive(String,"RemoteButton"))
    {
        if(StringAddEqual(String))
        {
            StringPos++; // we added one place by adding an equal sine
            if(!GetNumberUnsigned(String, 1, &temp))
            {
                if(temp <= ButtonAmount && temp > 0)
                {
                    if(!StringContainsCaseInsensitive(String,"set"))
                    {
                        tempNec =  EncodeNEC(Global1.RemoteButtonNEC[temp-1][0], Global1.RemoteButtonNEC[temp-1][1]);
                        if(tempNec != 0x00FF00FF)
                        {
                            SendNEC_wait(tempNec,0);
                            sprintf(buf,"Sent NEC code 0x%lX ", tempNec);
                            UARTstringCRLN(buf);
                            sprintf(buf,"NEC Code address: 0x%02X , command: 0x%02X ",Global1.RemoteButtonNEC[temp-1][0], Global1.RemoteButtonNEC[temp-1][1]);
                            UARTstringCRLN(buf);
                            UARTstring_CONST(CRLN);
                            return PASS;
                        }
                        UARTstring_CONST(CRLN);
                        UARTstringCRLN_CONST("Error: No NEC code saved for specified command");
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
                        Global1.RemoteButtonNEC[temp - 1][0] = NecAddress;
                        Global1.RemoteButtonNEC[temp - 1][1] = NecCommand;
                        SyncGlobalToEEPROM();
                        UARTstring_CONST(CRLN);
                        sprintf(buf,"RemoteButton%lu now transmits NEC code 0x%lX ",temp, IR_NEC);
                        UARTstringCRLN(buf);
                        sprintf(buf,"NEC Code address: 0x%02X , command: 0x%02X ",NecAddress, NecCommand);
                        UARTstringCRLN(buf);
                        UARTstring_CONST(CRLN);
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
                    UARTstring_CONST(CRLN);
                    UARTstringCRLN_CONST("Error: Remote button number too high");
                    UARTstring_CONST(CRLN);
                    return FAIL;
                }
            }
        }
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("Error: Remote button not specified");
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("Usage:");
        UARTstringCRLN_CONST("RemoteButton'x'");
        UARTstringCRLN_CONST("RemoteButton'x' set");
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("Example:");
        UARTstringCRLN_CONST("RemoteButton1");
        UARTstringCRLN_CONST("RemoteButton1 set");
        UARTstring_CONST(CRLN);
        return FAIL;
    }
    else if(StringContainsCaseInsensitive(String,"RF set") || StringContainsCaseInsensitive(String,"RF clear"))
    {
        system = FALSE;
        if(StringContainsCaseInsensitive(String,"RF set"))
        {
            set = TRUE;
        }
        else
        {
            set = FALSE;
            if(StringContainsCaseInsensitive(String,"system"))
            {
                system = TRUE;
            }
        }
        if(StringContainsCaseInsensitive(String,"All"))
        {
            if(!WaitForIRsignal())
            {
                return FAIL;
            }
            UARTstring_CONST(CRLN);
            DecodeNEC(IR_NEC, &NecAddress, &NecCommand);
            for(j=0; j < RFnumberOfSavedCodes; j++)
            {
                ok = FALSE;
                for(i=0; i < MirrorButtonsAmount; i++)
                {
                    if(Global2.RemoteButtonRF[j][i][0] == 0 && Global2.RemoteButtonRF[j][i][1] == 0)
                    {
                        EmptyPlace = i;
                        ok = TRUE;
                        break;
                    }
                }
                if(set == TRUE)
                {
                    if(!ok)
                    {
                        UARTstring_CONST("Error: No space available to save ");
                        DisplayRF_Channel(j);
                        UARTstring_CONST(CRLN);
                    }
                    else
                    {
                        if(EmptyPlace)
                        {
                            /* Check to see if this NEC is already saved */
                            for(i=0; i < EmptyPlace; i++)
                            {
                                if(NecAddress == Global2.RemoteButtonRF[j][i][0])
                                {
                                    if(NecCommand == Global2.RemoteButtonRF[j][i][1])
                                    {
                                        ok = FALSE;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!ok)
                        {
                            UARTstring_CONST("Error: NEC code already saved to ");
                            DisplayRF_Channel(j);
                            UARTstring_CONST(CRLN);
                        }
                        else
                        {
                            /* Update Globals to prepare for EEPROM burning */
                            Global2.RemoteButtonRF[j][EmptyPlace][0] = NecAddress;
                            Global2.RemoteButtonRF[j][EmptyPlace][1] = NecCommand;
                        }
                    }
                }
                else
                {
                    if(EmptyPlace)
                    {
                        /* Check to see if this NEC is already saved */
                        for(i=0; i < EmptyPlace; i++)
                        {
                            if(NecAddress == Global2.RemoteButtonRF[j][i][0])
                            {
                                if(NecCommand == Global2.RemoteButtonRF[j][i][1])
                                {
                                    /* Clear the IR code */
                                    Global2.RemoteButtonRF[j][i][0] = 0;
                                    Global2.RemoteButtonRF[j][i][1] = 0;
                                    for(k=i; k < (MirrorButtonsAmount -1); k++)
                                    {
                                        /* back fill the array until there is no more null before data */
                                        Global2.RemoteButtonRF[j][k][0] = Global2.RemoteButtonRF[j][k+1][0];
                                        Global2.RemoteButtonRF[j][k][1] = Global2.RemoteButtonRF[j][k+1][1];
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            /* save new code to eeprom */
            SyncGlobalToEEPROM();

            /* successful EEPROM burn */
            for(i =0;i<10;i++)
            {
                if(set == TRUE)
                {
                    GreenLEDON();
                }
                else
                {
                    RedLEDON();
                }
                delayUS(50000);
                if(set == TRUE)
                {
                    GreenLEDOFF();
                }
                else
                {
                    RedLEDOFF();
                }
                delayUS(50000);
            }
            if(set == TRUE)
            {
                sprintf(buf,"NEC code 0x%lX now transmits all RF codes",IR_NEC);
            }
            else
            {
                sprintf(buf,"NEC code 0x%lX will no longer transmit any RF codes",IR_NEC);
            }
            UARTstringCRLN(buf);
            UARTstring_CONST(CRLN);
            return PASS;
        }
        if(system != TRUE)
        {
            if(!StringAddEqual(String))
            {
                UARTstringCRLN_CONST("Error: No RF configuration specified");
                UARTstring_CONST(CRLN);
                return FAIL;
            }
            StringPos++; // we added one place by adding an equal sine
            UARTstring_CONST(CRLN);
            if(!GetNumberUnsigned(String, 0, &temp))
            {
                rfconf = (unsigned char) temp;
            }
            else
            {
                UARTstringCRLN_CONST("Error: RF configuration Syntax not understood");
                UARTstring_CONST(CRLN);
                return FAIL;
            }
            if(rfconf <= 0 || rfconf > NumRfConfigs)
            {
                UARTstringCRLN_CONST("Error: RF configuration out of range");
                UARTstring_CONST(CRLN);
                return FAIL;
            }
            if(GetStringAfterComma(String, 1, rfchannelSTR))
            {
                UARTstringCRLN_CONST("Error: RF Channel Syntax not understood");
                UARTstring_CONST(CRLN);
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
                        UARTstringCRLN_CONST("Error: RF channel out of range");
                        UARTstring_CONST(CRLN);
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
                            UARTstring_CONST("Which device?");
                            UARTstring_CONST(CRLN);
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
                                    UARTstring_CONST(CRLN);
                                    UARTstringCRLN_CONST("Error: No RF config 2, channel H device specified");
                                    return FAIL;
                                }
                            }
                            PIE1bits.RCIE   = FALSE;
                            ok = TRUE;
                            if(ReceivedString[0] < '1' || ReceivedString[0] > '3')
                            {
                                UARTstringCRLN_CONST("Error: RF config 2, channel H device out of range");
                                ok = FALSE;
                            }
                            device = ReceivedString[0] - '0';
                            tempRFArray = device + 4;
                            cleanBuffer(ReceivedString, ReceivedStringPos);
                            ReceivedStringPos = 0;
                            NewReceivedString = FALSE;
                            if(!ok)
                            {
                                return FAIL;
                            }
                        }
                        else
                        {
                            // Got here by mistake
                            return FAIL;
                        }
                    }
                }
                else
                {
                    UARTstringCRLN_CONST("Error: RF channel needs to be one character");
                    UARTstring_CONST(CRLN);
                    return FAIL;
                }
            }
        }
        if(set)
        {
            if(!WaitForIRsignal())
            {
                return FAIL;
            }
            UARTstring_CONST(CRLN);
            DecodeNEC(IR_NEC, &NecAddress, &NecCommand);
            ok = FALSE;
            for(i=0; i < MirrorButtonsAmount; i++)
            {
                if(Global2.RemoteButtonRF[tempRFArray][i][0] == 0 && Global2.RemoteButtonRF[tempRFArray][i][1] == 0)
                {
                    EmptyPlace = i;
                    ok = TRUE;
                    break;
                }
            }
            if(!ok)
            {
                UARTstringCRLN_CONST("Error: No space available to save this RF code");
                return FAIL;
            }
            if(EmptyPlace)
            {
                /* Check to see if this NEC is already saved */
                for(i=0; i < EmptyPlace; i++)
                {
                    if(NecAddress == Global2.RemoteButtonRF[tempRFArray][i][0])
                    {
                        if(NecCommand == Global2.RemoteButtonRF[tempRFArray][i][1])
                        {
                            ok = FALSE;
                            break;
                        }
                    }
                }
            }
            if(!ok)
            {
                UARTstringCRLN_CONST("Error: NEC code already saved to this RF code");
                return FAIL;
            }

            /* save new code to eeprom */
            Global2.RemoteButtonRF[tempRFArray][EmptyPlace][0] = NecAddress;
            Global2.RemoteButtonRF[tempRFArray][EmptyPlace][1] = NecCommand;
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
            UARTstring_CONST(CRLN);
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
            if(system == TRUE)
            {
                for(i=0; i < MirrorButtonsAmount; i++)
                {
                    for(k=0; k < RFnumberOfSavedCodes; k++)
                    {
                        /* clear all codes from eeprom */
                        Global2.RemoteButtonRF[k][i][0] = 0;
                        Global2.RemoteButtonRF[k][i][1] = 0;
                    }
                }
            }
            else
            {
                for(i=0; i < MirrorButtonsAmount; i++)
                {
                    /* clear specific code from eeprom */
                    Global2.RemoteButtonRF[tempRFArray][i][0] = 0;
                    Global2.RemoteButtonRF[tempRFArray][i][1] = 0;
                }
            }
            SyncGlobalToEEPROM();
            if(system == TRUE)
            {
                sprintf(buf,"All Remote buttons associated with RF codes successfully erased");
            }
            else
            {
                if(device)
                {
                    sprintf(buf,"RF config %d channel %c device %d successfully erased",rfconf, rfchannelSTR[0], device);
                }
                else
                {
                    sprintf(buf,"RF config %d channel %c successfully erased",rfconf, rfchannelSTR[0]);
                }
            }
            UARTstringCRLN(buf);
            UARTstring_CONST(CRLN);
            /* successful EEPROM burn */
            for(i =0;i<10;i++)
            {
               RedLEDON();
               delayUS(50000);
               RedLEDOFF();
               delayUS(50000);
            }
            return PASS;
        }
    }
    else if(StringMatchCaseInsensitive(String,"Reset"))
    {
        cleanBuffer(ReceivedString, ReceivedStringPos);
        ReceivedStringPos = 0;
        NewReceivedString = FALSE;
        UARTstring_CONST("Enter Y to reset or N to cancel");
        UARTstring_CONST(CRLN);
        UARTchar_CONST('>');
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
                UARTstring_CONST(CRLN);
                UARTstringCRLN_CONST("Device resumes without reset");
                return FAIL;
            }
        }
        PIE1bits.RCIE   = FALSE;
        if(ReceivedString[0] == 'Y' || ReceivedString[0] == 'y' && ReceivedString[1] == 0)
        {
            /* Erase EEPROM flag and reset device */
            UARTstringCRLN_CONST("System will reset");
            Global1.EEPROMinitFlag = FAIL;
            SyncGlobalToEEPROM();
            RESET();
            return PASS;
        }
        else
        {
            UARTstring_CONST(CRLN);
            UARTstringCRLN_CONST("Device resumes without reset");
        }
        cleanBuffer(ReceivedString, ReceivedStringPos);
        ReceivedStringPos = 0;
        NewReceivedString = FALSE;
        return FAIL;
    }
    else if(StringMatchCaseInsensitive(String,"Change SN"))
    {
        cleanBuffer(ReceivedString, ReceivedStringPos);
        ReceivedStringPos = 0;
        NewReceivedString = FALSE;
        UARTstring_CONST("Enter special 'Code' to change serial number or N to cancel");
        UARTstring_CONST(CRLN);
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
                UARTstring_CONST(CRLN);
                UARTstringCRLN_CONST("Device resumes without changing serial number");
                return FAIL;
            }
        }
        PIE1bits.RCIE   = FALSE;
        if(StringMatch(ReceivedString,"Code"))
        {
            cleanBuffer(ReceivedString, ReceivedStringPos);
            ReceivedStringPos = 0;
            NewReceivedString = FALSE;
            UARTstring_CONST("Enter new Serial Number");
            UARTstring_CONST(CRLN);
            UARTchar_CONST('>');
            timeout = 0;
            ClearUSART();
            PIR1bits.RCIF = FALSE;
            PIE1bits.RCIE   = TRUE;
            while(!NewReceivedString)
            {
                delayUS(300000);
                timeout++;
                if(ReceivedStringPos != ReceivedStringPosOLD)
                {
                    timeout = 0;
                }
                if(timeout > 25)
                {
                    UARTstring_CONST(CRLN);
                    UARTstringCRLN_CONST("Device resumes without changing serial number");
                    return FAIL;
                }
                ReceivedStringPosOLD = ReceivedStringPos;
            }
            if(StringAddEqual(ReceivedString))
            {
                ReceivedStringPos++;
                if(!GetNumber(ReceivedString, 1, &SerialNumberTEMP))
                {
                    if(SerialNumberTEMP > 0)
                    {
                        Global2.SerialNumber = (unsigned long) SerialNumberTEMP;
                        if(!SetEEPROM2(Global2,0x00000002)) // burn serial number
                        {
                            SyncEEPROMToGlobal();
                            UARTstringCRLN_CONST("Serial Number successfully burned to EEPROM");
                            sprintf(buf,"Serial Number is now: %lu ",Global2.SerialNumber);
                            UARTstringCRLN(buf);
                            UARTstring_CONST(CRLN);
                            /* Erase EEPROM flag and reset device */
                            UARTstringCRLN_CONST("System will reset");
                            Global1.EEPROMinitFlag = FAIL;
                            SyncGlobalToEEPROM();
                            UARTstring_CONST(CRLN);
                            RESET();
                            return PASS;
                        }
                        else
                        {
                            UARTstringCRLN_CONST("Serial Number could not be burned to EEPROM");
                            UARTstring_CONST(CRLN);
                            return FAIL;
                        }

                    }
                }
            }
            UARTstringCRLN_CONST("Serial Number entered incorrectly");
            UARTstring_CONST(CRLN);
            return FAIL;
        }
        else
        {
            UARTstringCRLN_CONST("The 'Code' was wrong!!!");
            UARTstring_CONST(CRLN);
        }
        return FAIL;
    }
    else if(StringMatchCaseInsensitive(String,"Change Bluetooth Name"))
    {
        cleanBuffer(ReceivedString, ReceivedStringPos);
        ReceivedStringPos = 0;
        NewReceivedString = FALSE;
        UARTstring_CONST(CRLN);
        UARTstring_CONST("Enter Y to change Name or N to cancel");
        UARTstring_CONST(CRLN);
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
                UARTstring_CONST(CRLN);
                UARTstringCRLN_CONST("Device resumes without changing Bluetooth Broadcast Name");
                return FAIL;
            }
        }
        PIE1bits.RCIE   = FALSE;
        if(ReceivedString[0] == 'Y' || ReceivedString[0] == 'y' && ReceivedString[1] == 0)
        {
            cleanBuffer(ReceivedString, ReceivedStringPos);
            ReceivedStringPos = 0;
            NewReceivedString = FALSE;
            UARTstring_CONST(CRLN);
            UARTstring_CONST("Enter new Bluetooth Broadcast Name");
            UARTstring_CONST(CRLN);
            UARTchar_CONST('>');
            timeout = 0;
            ClearUSART();
            PIR1bits.RCIF = FALSE;
            PIE1bits.RCIE   = TRUE;
            while(!NewReceivedString)
            {
                delayUS(300000);
                timeout++;
                if(ReceivedStringPos != ReceivedStringPosOLD)
                {
                    timeout = 0;
                }
                if(timeout > 25)
                {
                    UARTstringCRLN_CONST("Device resumes without changing Bluetooth Broadcast Name");
                    return FAIL;
                }
                ReceivedStringPosOLD = ReceivedStringPos;
            }
            PIE1bits.RCIE   = FALSE;
            UARTstring_CONST(CRLN);
            UARTstring_CONST(CRLN);
            if(ReceivedString[0] != 0 && ReceivedString[BroadcastSize-1] == 0)
            {
                if(IsAlphaNumericString(ReceivedString))
                {
                    UARTstringCRLN_CONST("Bluetooth Module will Reset. Please reconnect device");
                    UARTstring_CONST(CRLN);
                    BufferCopy(ReceivedString,BluetoothBroadcast, BroadcastSize, 0);
                    delayUS(CommandWait);
                    if(BluetoothInitialSetup(FALSE))
                    {
                        BluetoothChangeStatus = TRUE;
                        return PASS;
                    }
                    else
                    {
                        BluetoothChangeStatus = FALSE;
                        return FAIL;
                    }
                }
                else
                {
                    UARTstringCRLN_CONST("Name has to be Alphanumeric. No symbols!");
                    UARTstring_CONST(CRLN);
                }
            }
            else
            {
                UARTstringCRLN_CONST("Entered Name is too long. Name must be less than 21 characters!");
                UARTstring_CONST(CRLN);
            }
        }
        else
        {
            UARTstringCRLN_CONST("Device resumes without changing Bluetooth Broadcast Name");
            UARTstring_CONST(CRLN);
        }
        return FAIL;
    }
    else if(StringMatchCaseInsensitive(String,"Voltage"))
    {
        UARTstring_CONST(CRLN);
        sprintf(buf,"Supply Voltage = %f", (double) ReadVoltage());
        UARTstringCRLN(buf);
        UARTstring_CONST(CRLN);
    }
    else if(StringMatchCaseInsensitive(String,"Version"))
    {
        UARTstring_CONST(CRLN);
        sprintf(buf,"Firmware Version: %s", FirmVersion);
        UARTstringCRLN(buf);
        UARTstring_CONST(CRLN);
#ifdef BLUETOOTHMODULE
        UARTstringCRLN_CONST("PCB has Bluetooth capability");
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("Bluetooth firmware is:");
        UARTstringCRLN(BluetoothFirmware1);
        UARTstringCRLN(BluetoothFirmware2);
        UARTstring_CONST(CRLN);
#else
        UARTstringCRLN("PCB does not have Bluetooth capability");
        UARTstring(CRLN);
#endif
        sprintf(buf,"PCB Version: %s", PCBVersion);
        UARTstringCRLN(buf);
        sprintf(buf,"Transmitting Frequency: %s", Frequency);
        UARTstring(buf);
        UARTstringCRLN_CONST(" MHz");
        UARTstring_CONST(CRLN);
        sprintf(buf,"Serial Number: %lu", Global2.SerialNumber);
        UARTstring(buf);
        UARTstring_CONST(CRLN);
        return PASS;
    }
    else if(StringMatch(String,"???"))
    {
        UARTstring_CONST(CRLN);
        for(i=0; i< 34; i++ )
        {
            UARTchar_CONST('~');
        }
        UARTstring_CONST("Command Menu");
        for(i=0; i< 34; i++ )
        {
            UARTchar_CONST('~');
        }
        UARTstring_CONST(CRLN);
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("System commands:");
        UARTcommandMenu("???", "Help Menu");
        UARTcommandMenu("Reset", "Clears memory and resets device");
        UARTcommandMenu("Version", "Displays Firmware/Hardware Version");
        UARTcommandMenu("Change SN", "Changes Serial Number");
        UARTcommandMenu("Change Bluetooth Name", "Changes Bluetooth Broadcast Name");
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("Diagnostic Commands:");
        UARTcommandMenu("Voltage", "Displays the supply voltage");
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("IR commands:");
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
            UARTcommandMenu(buf,"Allows saving NEC codes");
        }
        UARTcommandMenu("RF set all", "Sets Remote button to send all of the RF codes as stated below");
        UARTcommandMenu("RF set 1,D", "Sets Remote button to send RF Config 1 channel D");
        UARTcommandMenu("RF set 1,E", "Sets Remote button to send RF Config 1 channel E");
        UARTcommandMenu("RF set 1,F", "Sets Remote button to send RF Config 1 channel F");
        UARTcommandMenu("RF set 2,B", "Sets Remote button to send RF Config 2 channel B");
        UARTcommandMenu("RF set 2,D", "Sets Remote button to send RF Config 2 channel D");
        UARTcommandMenu("RF set 2,H", "Sets Remote button to send RF Config 2 channel H");
        UARTcommandMenu("RF clear all", "Clears Remote button from all RF codes");
        UARTcommandMenu("RF clear system", "Clears all Remote buttons from all RF codes");
        UARTcommandMenu("RF clear 1,D", "Clears all saved Button for RF Config 1 channel D");
        UARTcommandMenu("RF clear 1,E", "Clears all saved Button for RF Config 1 channel E");
        UARTcommandMenu("RF clear 1,F", "Clears all saved Button for RF Config 1 channel F");
        UARTcommandMenu("RF clear 2,B", "Clears all saved Button for RF Config 2 channel B");
        UARTcommandMenu("RF clear 2,D", "Clears all saved Button for RF Config 2 channel D");
        UARTcommandMenu("RF clear 2,H", "Clears all saved Button for RF Config 2 channel H");
        UARTcommandMenu("NEC?", "NEC description for Arbitrary code send");
        UARTstring_CONST(CRLN);
        UARTstringCRLN_CONST("RF commands:");
        UARTcommandMenu("RF all", "Send all Rf codes as stated below");
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
        UARTstring_CONST(CRLN);
    }
    else
    {
        UARTstringCRLN_CONST(SYNTAX_ERR);
        UARTstringCRLN_CONST("Enter ??? for Command Menu");
        UARTstring_CONST(CRLN);
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

    UARTstring_CONST(CRLN);
    UARTstring_CONST("Press Remote Control button");
    timeout = 0;
    while(IR_New_Code != New)
    {
        UARTchar_CONST('.');
        delayUS(300000);
        timeout++;
        if(timeout > 25)
        {
            UARTstring_CONST(CRLN);
            UARTstringCRLN_CONST("Error: No remote button was pressed");
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
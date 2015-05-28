/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_IR_to_RF_MICRF113_test'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
 * 05/14/15     1.0_DW0e    Added support for 433MHz transmitter.
/******************************************************************************/

/******************************************************************************/
/* Contains RF functions.
 *
 * Chip PT2260a
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>      

#include "RF.h"
#include "Timer.h"
#include "MISC.h"
#include "IR.h"
#include "user.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

unsigned char RFsendCode[RFmaxSize];
volatile unsigned char RFsendFlag = 0;
unsigned char RFcodePlace = 0;
unsigned char RFcodeBit = 0;
volatile unsigned char Sendcount =0;
volatile unsigned char Sent = TRUE;
unsigned char RFConfig = 0;
volatile unsigned char RF_IR_Postscaler = 2;
unsigned char Conf2_ChannelB_Status = OFF;
unsigned char Conf2_ChannelD_Status = OFF;
unsigned char Conf2_ChannelH_1_Status = OFF;
unsigned char Conf2_ChannelH_2_Status = OFF;
unsigned char Conf2_ChannelH_3_Status = OFF;
unsigned char Conf3_Status[16][5] =
{
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0}

};
unsigned char RF_IR = RF;
unsigned char frequency = _315MHz;
unsigned char RFPause = FALSE;

/******************************************************************************/
/* Functions
 *
/******************************************************************************/

/******************************************************************************/
/* SendRF
 *
 * The function sends the RF channel code.
/******************************************************************************/
unsigned char SendRF(const unsigned char* Code, unsigned char Config, unsigned char size, unsigned char RepeatAmount)
{
    unsigned char i;
    if(Sent)
    {
        /* Previous send finished */
        RF_IR = RF;
        if((Config <= 0) || (Config > NumRfConfigs))
        {
            return FAIL;
        }
        RFConfig = Config;
        if(Config == 1)
        {
            RF_IR_Postscaler = 2;
            frequency = _315MHz;
        }
        else if(Config == 2)
        {
            RF_IR_Postscaler = 4;
            frequency = _315MHz;
        }
        #ifndef IR_to_RF_w_bluetooth_revA
        else if(Config == 3)
        {
            RF_IR_Postscaler = 2;
            frequency = _433MHz;
        }
        #endif
        else
        {
            return FAIL;
        }

        Sent = NO;
        Sendcount = RepeatAmount;
        RFsendFlag = 1;
        RFcodeBit = 0;
        RFcodePlace = 1;


        if(size >= RFmaxSize)
        {
            return FAIL;
        }
        for(i=0;i<size;i++)
        {
            RFsendCode[i] = Code[i];
        }
        RFsendCode[i] = EndofRFcode;

        ResetTimer2();
        if(Config == 1)
        {
            SetTimer2(Conf1_Sync);
        }
        else if(Config == 2)
        {
            SetTimer2(Conf2_Sync);
        }
        else if(Config == 3)
        {
            SetTimer2(Conf3_Sync);
        }
        Timer2ON();
        return PASS;        
    }
    return SENDING;
}

/******************************************************************************/
/* GetRFstatus
 *
 * The function returns the status of the RF transmitter.
/******************************************************************************/
unsigned char GetRFstatus(void)
{
    if(!RFsendFlag)
    {
        return PASS;
    }
    return SENDING;
}

/******************************************************************************/
/* SendRF_wait
 *
 * Sends the code and waits here until finished. Send the 'amount' of repeats
 *  passed in as amount.
/******************************************************************************/
void SendRF_wait(const unsigned char* Code, unsigned char Config, unsigned char size, unsigned char amount)
{
    IRreceiverIntOff();
    INTCONbits.RBIE = OFF;
    INTCONbits.PEIE = OFF;
    LEDTimerON();
    GreenLEDON();
    SendRF(Code, Config, size, amount);
    while(!Sent);
    INTCONbits.PEIE = ON;
    IRpinOLD = ReadIRpin();
    INTCONbits.RBIF = FALSE;
    IRreceiverIntOn();
    INTCONbits.RBIE = ON;
}
/******************************************************************************/
/* SendRF_Channel
 *
 * Sends the code associated with the RF channel.
/******************************************************************************/
void SendRF_Channel(unsigned char channel)
{
    /* 
     * RF codes amount macro (RFnumberOfSavedCodes) needs to be one greater than
     * the last value.
     */
    unsigned char Conf3Channel;
    unsigned char Conf3button;

    if(channel < 8)
    {
        switch (channel)
        {
            case 0:
                // Configuration 1 channel D
                SendRF_wait(Conf1_ChannelD,1,12,RFrepeatAmount);
                break;
            case 1:
                // Configuration 1 channel E
                SendRF_wait(Conf1_ChannelE,1,12,RFrepeatAmount);
                break;
            case 2:
                // Configuration 1 channel F
                SendRF_wait(Conf1_ChannelF,1,12,RFrepeatAmount);
                break;
            case 3:
                // Configuration 2 channel B
                if(Conf2_ChannelB_Status)
                {
                    SendRF_wait(Conf2_ChannelB_OFF,2,16,RFrepeatAmount);
                    Conf2_ChannelB_Status = OFF;
                }
                else
                {
                    SendRF_wait(Conf2_ChannelB_ON,2,16,RFrepeatAmount);
                    Conf2_ChannelB_Status = ON;
                }
                break;
            case 4:
                // Configuration 2 channel D
                if(Conf2_ChannelD_Status)
                {
                    SendRF_wait(Conf2_ChannelD_OFF,2,16,RFrepeatAmount);
                    Conf2_ChannelD_Status = OFF;
                }
                else
                {
                    SendRF_wait(Conf2_ChannelD_ON,2,16,RFrepeatAmount);
                    Conf2_ChannelD_Status = ON;
                }
                break;
            case 5:
                // Configuration 2 channel H device 1
                if(Conf2_ChannelH_1_Status)
                {
                    SendRF_wait(Conf2_ChannelH_1_OFF,2,16,RFrepeatAmount);
                    Conf2_ChannelH_1_Status = OFF;
                }
                else
                {
                    SendRF_wait(Conf2_ChannelH_1_ON,2,16,RFrepeatAmount);
                    Conf2_ChannelH_1_Status = ON;
                }
                break;
            case 6:
                // Configuration 2 channel H device 2
                if(Conf2_ChannelH_2_Status)
                {
                    SendRF_wait(Conf2_ChannelH_2_OFF,2,16,RFrepeatAmount);
                    Conf2_ChannelH_2_Status = OFF;
                }
                else
                {
                    SendRF_wait(Conf2_ChannelH_2_ON,2,16,RFrepeatAmount);
                    Conf2_ChannelH_2_Status = ON;
                }
                break;
            case 7:
                // Configuration 2 channel H device 3
                if(Conf2_ChannelH_3_Status)
                {
                    SendRF_wait(Conf2_ChannelH_3_OFF,2,16,RFrepeatAmount);
                    Conf2_ChannelH_3_Status = OFF;
                }
                else
                {
                    SendRF_wait(Conf2_ChannelH_3_ON,2,16,RFrepeatAmount);
                    Conf2_ChannelH_3_Status = ON;
                }
                break;
            default:
                break;
        }
    }
    else if(channel < 88)
    {
        /* conf3 */
        Conf3Channel = channel - 8;
        Conf3Channel /= 5;
        Conf3button = channel - 8;
        Conf3button %= 5;

        if(Conf3_Status[Conf3Channel][Conf3button])
        {
            SendRF_wait(&Conf3[Conf3Channel][Conf3button][1][0],3,12,RFrepeatAmount);
            Conf3_Status[Conf3Channel][Conf3button] = OFF;
        }
        else
        {
            SendRF_wait(&Conf3[Conf3Channel][Conf3button][0][0],3,12,RFrepeatAmount);
            Conf3_Status[Conf3Channel][Conf3button] = ON;
        }
    }
}

/******************************************************************************/
/* DisplayRF_Channel
 *
 * Prints the code associated with the RF channel.
/******************************************************************************/
void DisplayRF_Channel(unsigned char channel)
{
    unsigned char Conf3Channel;
    unsigned char Conf3button;
    unsigned char buf[50];
    
    if(channel < 8)
    {
        switch(channel)
        {
            case 0:
                UARTstring_CONST(Conf1_ChannelD_STR);
                break;
            case 1:
                UARTstring_CONST(Conf1_ChannelE_STR);
                break;
            case 2:
                UARTstring_CONST(Conf1_ChannelF_STR);
                break;
            case 3:
                UARTstring_CONST(Conf2_ChannelB_STR);
                break;
            case 4:
                UARTstring_CONST(Conf2_ChannelD_STR);
                break;
            case 5:
                UARTstring_CONST(Conf2_ChannelH_1_STR);
                break;
            case 6:
                UARTstring_CONST(Conf2_ChannelH_2_STR);
                break;
            case 7:
                UARTstring_CONST(Conf2_ChannelH_3_STR);
                break;
            default:
                break;
        }
    }
    else if(channel < 88)
    {
                /* conf3 */
        Conf3Channel = channel - 8;
        Conf3Channel /= 5;
        Conf3button = channel - 8;
        Conf3button %= 5;
        Conf3button++;

        sprintf(buf,"Conf3_Channel%X%d",Conf3Channel,Conf3button);
        UARTstring(buf);
    }
}

/******************************************************************************/
/* RFon
 *
 * The function turns on the specific Rf transmitter.
/******************************************************************************/
inline void RFon(void)
{
    #ifdef IR_to_RF_w_bluetooth_revA
    LATC |= RF315trans;
    #else
    if(frequency == _315MHz)
    {
        LATC |= RF315trans;
    }
    else
    {
        LATB |= RF433trans;
    }
    #endif
}

/******************************************************************************/
/* RFoff
 *
 * The function turns off the specific Rf transmitter.
/******************************************************************************/
inline void RFoff()
{
    #ifdef IR_to_RF_w_bluetooth_revA
    LATC &= ~RF315trans;
    #else
    if(frequency == _315MHz)
    {
        LATC &= ~RF315trans;
    }
    else
    {
        LATB &= ~RF433trans;
    }
    #endif
}

/******************************************************************************/
/* RFoff
 *
 * The function turns off the specific Rf transmitter.
/******************************************************************************/
void Send_RForIR_bit(void)
{
    Timer2_Postscaler++;
    if(RF_IR == RF)
    {
        /* Sending RF code */
        if(Timer2_Postscaler >= RF_IR_Postscaler)
        {
            Timer2_Postscaler = 0;
            if(RFsendCode[RFcodeBit] != EndofRFcode)
            {
                /* In the middle of sending a code */
                if(RFsendFlag)
                {
                    if(RFsendCode[RFcodeBit] == 0)
                    {

                        if(RFConfig == 1)
                        {
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf1_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf1_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 3)
                            {
                                RFon();
                                SetTimer2(Conf1_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 4)
                            {
                                RFoff();
                                SetTimer2(Conf1_Long);
                                RFcodePlace = 1;
                                RFcodeBit++;
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else if(RFConfig == 2)
                        {
                            /* zero is 650uS high folowed by 1.75mS low */
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf2_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf2_Long);
                                RFcodePlace = 1;
                                RFcodeBit++;
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else if(RFConfig == 3)
                        {
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf3_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf3_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 3)
                            {
                                RFon();
                                SetTimer2(Conf3_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 4)
                            {
                                RFoff();
                                SetTimer2(Conf3_Long);
                                RFcodePlace = 1;
                                RFcodeBit++;
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else
                        {
                            /* Invalid */
                            Timer2OFF();
                            Sent = YES;
                            RFsendFlag = 0;
                        }
                    }
                    else if(RFsendCode[RFcodeBit] == 1)
                    {
                        if(RFConfig == 1)
                        {
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf1_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf1_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 3)
                            {
                                RFon();
                                SetTimer2(Conf1_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 4)
                            {
                                RFoff();
                                SetTimer2(Conf1_Short);
                                RFcodePlace = 1;
                                RFcodeBit++;
                                ResetTimer2();
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else if(RFConfig == 2)
                        {
                            /* one is 1.75mS high followed by 650uS low */
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf2_Long);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf2_Short);
                                RFcodePlace = 1;
                                RFcodeBit++;
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else if(RFConfig == 3)
                        {
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf3_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf3_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 3)
                            {
                                RFon();
                                SetTimer2(Conf3_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 4)
                            {
                                RFoff();
                                SetTimer2(Conf3_Short);
                                RFcodePlace = 1;
                                RFcodeBit++;
                                ResetTimer2();
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else
                        {
                            /* Invalid */
                            Timer2OFF();
                            Sent = YES;
                            RFsendFlag = 0;
                        }
                    }
                    else
                    {
                        if(RFConfig == 1)
                        {
                            /* The bit is an f (floating)*/
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf1_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf1_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 3)
                            {
                                RFon();
                                SetTimer2(Conf1_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 4)
                            {
                                RFoff();
                                SetTimer2(Conf1_Short);
                                RFcodePlace = 1;
                                RFcodeBit++;
                                ResetTimer2();
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else if(RFConfig == 2)
                        {
                            /* Invalid */
                            Timer2OFF();
                            Sent = YES;
                            RFsendFlag = 0;
                        }
                        else if(RFConfig == 3)
                        {
                            /* The bit is an f (floating)*/
                            if(RFcodePlace == 1)
                            {
                                RFon();
                                SetTimer2(Conf3_Short);
                                RFcodePlace++;
                                ResetTimer2();
                            }
                            else if(RFcodePlace == 2)
                            {
                                RFoff();
                                SetTimer2(Conf3_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 3)
                            {
                                RFon();
                                SetTimer2(Conf3_Long);
                                RFcodePlace++;
                            }
                            else if(RFcodePlace == 4)
                            {
                                RFoff();
                                SetTimer2(Conf3_Short);
                                RFcodePlace = 1;
                                RFcodeBit++;
                                ResetTimer2();
                            }
                            else
                            {
                                /* Not supposed to get here */
                                Timer2OFF();
                                Sent = YES;
                                RFsendFlag = 0;
                            }
                        }
                        else
                        {
                            /* Invalid */
                            Timer2OFF();
                            Sent = YES;
                            RFsendFlag = 0;
                        }
                    }
                }
            }
            else
            {
                /* We finished sending one code and now need to sync */
                if(RFcodePlace == 1)
                {
                    if(RFConfig == 1)
                    {
                        RFon();
                        SetTimer2(Conf1_Short);
                    }
                    else if(RFConfig == 2)
                    {
                        RFoff();
                        SetTimer2(Conf2_Short);
                    }
                    else if(RFConfig == 3)
                    {
                        RFon();
                        SetTimer2(Conf3_Short);
                    }
                    else
                    {
                        /* Invalid */
                        Timer2OFF();
                        Sent = YES;
                    }
                    ResetTimer2();
                    RFcodePlace++;
                }
                else if( RFcodePlace == 2)
                {
                    if(RFConfig == 1)
                    {
                        RFoff();
                        SetTimer2(Conf1_Sync);
                    }
                    else if(RFConfig == 2)
                    {
                        SetTimer2(Conf2_Sync);
                    }
                    else if(RFConfig == 3)
                    {
                        RFoff();
                        SetTimer2(Conf3_Sync);
                    }
                    else
                    {
                        /* Invalid */
                        Timer2OFF();
                        Sent = YES;
                    }
                    RFcodePlace++;
                    Sendcount--;
                    if(Sendcount)
                    {
                        /* Send another repeat */
                        RFcodePlace = 1;
                        RFcodeBit = 0;
                    }
                }
                else
                {
                    /* Finished sending all of the repeats */
                    if(RFPause)
                    {
                        ResetTimer3();
                        PIR2bits.TMR3IF = FALSE;//Clear Flag
                        Timer3_Postscaler = 0;
                        Timer3ON();
                    }
                    else
                    {
                        Sent = YES;
                    }
                    Timer2OFF();
                    RFsendFlag = 0;
                }
            }
        }
        if(RFsendFlag)
        {
            /* We are still in the middle of sending */
            Timer2ON();
        }
    }
    else
    {
        /* Sending IR code in NEC protocol */
        if(Timer2_Postscaler >= RF_IR_Postscaler)
        {
            Timer2_Postscaler = 0;
            if(IRcodePlace == Start1)
            {
                IRbitPosition = 32;
                IRLEDmodOFF();
                IRLEDoff();
                IRcodePlace = Start2;
                RF_IR_Postscaler = 4;
                SetTimer2(Scale_StartbitLOWnominal);
            }
            else if(IRcodePlace == Start2 || IRcodePlace == Data2)
            {
                IRLEDmodON();
                RF_IR_Postscaler = 1;
                IRbit = (unsigned char) (IRsendCode & 0x01);
                IRsendCode >>= 1;
                SetTimer2(Scale_DataShortnominal);
                IRcodePlace = Data1;
            }
            else if(IRcodePlace == Data1)
            {
                IRLEDmodOFF();
                IRLEDoff();
                RF_IR_Postscaler = 1;
                if(IRbit)
                {
                    /* bit is a one */
                    SetTimer2(Scale_DataLongnominal);
                }
                else
                {
                    /* bit is a Zero */
                    SetTimer2(Scale_DataShortnominal);
                }
                IRbitPosition--;
                if(IRbitPosition == 0)
                {
                    IRcodePlace = End1;
                }
                else
                {
                    IRcodePlace = Data2;
                }
            }
            else if(IRcodePlace == End1)
            {
                IRLEDmodON();
                SetTimer2(Scale_DataShortnominal);
                IRcodePlace = End2;
            }
            else if(IRcodePlace == End2)
            {
                IRLEDmodOFF();
                IRLEDoff();
                if(!IRrepeatflag)
                {
                    /* Done sending the NEC code and no repeat codes */
                    IRcodePlace = Finished;
                    Timer2OFF();
                    Sent = YES;
                }
                else
                {
                    RF_IR_Postscaler = 11;
                    SetTimer2(Scale_PauseBurstnominal);
                    IRcodePlace = Repeat1;
                }
            }
            else if(IRcodePlace == Repeat1  || IRcodePlace == Repeat5)
            {
                IRLEDmodON();
                RF_IR_Postscaler = 4;
                SetTimer2(Scale_StartbitHIGHnominal);
                IRcodePlace = Repeat2;
            }
            else if(IRcodePlace == Repeat2)
            {
                IRLEDmodOFF();
                IRLEDoff();
                RF_IR_Postscaler = 1;
                SetTimer2(Scale_PauseSpacenominal);
                IRcodePlace = Repeat3;
            }
            else if(IRcodePlace == Repeat3)
            {
                IRLEDmodON();
                RF_IR_Postscaler = 1;
                SetTimer2(Scale_DataShortnominal);
                IRcodePlace = Repeat4;
            }
            else if(IRcodePlace == Repeat4)
            {
                IRLEDmodOFF();
                IRLEDoff();
                RF_IR_Postscaler = 26;
                SetTimer2(Scale_Repeatnominal);
                IRrepeatAmount--;
                if(IRrepeatAmount)
                {
                    IRcodePlace = Repeat5;
                }
                else
                {
                    IRcodePlace = Finished;
                }
            }
            else if(IRcodePlace == Finished)
            {
                /* Done sending the NEC code and a number of repeat codes */
                Timer2OFF();
                Sent = YES;
            }
            else
            {
                /* Invalid */
                IRLEDmodOFF();
                IRLEDoff();
                IRcodePlace = Error;
                Timer2OFF();
                Sent = YES;
            }
        }
    }
    PIR1bits.TMR2IF = FALSE; //Clear Flag
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
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
unsigned char RF_IR = RF;
unsigned char frequency = _315MHz;

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
        else
        {
            SetTimer2(Conf2_Sync);
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

/******************************************************************************/
/* DisplayRF_Channel
 *
 * Prints the code associated with the RF channel.
/******************************************************************************/
void DisplayRF_Channel(unsigned char channel)
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
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
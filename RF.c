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
volatile unsigned char Sent = 1;
unsigned char RFConfig = 0;
volatile unsigned char RF_IR_Postscaler = 2;
unsigned char Conf2_ChannelB_Status = OFF;
unsigned char Conf2_ChannelD_Status = OFF;
unsigned char Conf2_ChannelH_1_Status = OFF;
unsigned char Conf2_ChannelH_2_Status = OFF;
unsigned char Conf2_ChannelH_3_Status = OFF;
unsigned char RF_IR = RF;

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
    if(Sent == YES)
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
        }
        else if(Config == 2)
        {
            RF_IR_Postscaler = 4;
        }
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
    switch (channel)
    {
        case 0:
            // Configuration 1 channel D
            SendRF_wait(Conf1_ChannelD,1,12,8);
            break;
        case 1:
            // Configuration 1 channel E
            SendRF_wait(Conf1_ChannelE,1,12,8);
            break;
        case 2:
            // Configuration 1 channel F
            SendRF_wait(Conf1_ChannelF,1,12,8);
            break;
        case 3:
            // Configuration 2 channel B
            if(Conf2_ChannelB_Status)
            {
                SendRF_wait(Conf2_ChannelB_OFF,2,16,8);
                Conf2_ChannelB_Status = OFF;
            }
            else
            {
                SendRF_wait(Conf2_ChannelB_ON,2,16,8);
                Conf2_ChannelB_Status = ON;
            }
            break;
        case 4:
            // Configuration 2 channel D
            if(Conf2_ChannelD_Status)
            {
                SendRF_wait(Conf2_ChannelD_OFF,2,16,8);
                Conf2_ChannelD_Status = OFF;
            }
            else
            {
                SendRF_wait(Conf2_ChannelD_ON,2,16,8);
                Conf2_ChannelD_Status = ON;
            }
            break;
        case 5:
            // Configuration 2 channel H device 1
            if(Conf2_ChannelH_1_Status)
            {
                SendRF_wait(Conf2_ChannelH_1_ON,2,16,8);
                Conf2_ChannelH_1_Status = OFF;
            }
            else
            {
                SendRF_wait(Conf2_ChannelH_1_OFF,2,16,8);
                Conf2_ChannelH_1_Status = ON;
            }
            break;
        case 6:
            // Configuration 2 channel H device 2
            if(Conf2_ChannelH_2_Status)
            {
                SendRF_wait(Conf2_ChannelH_2_ON,2,16,8);
                Conf2_ChannelH_2_Status = OFF;
            }
            else
            {
                SendRF_wait(Conf2_ChannelH_2_OFF,2,16,8);
                Conf2_ChannelH_2_Status = ON;
            }
            break;
        case 7:
            // Configuration 2 channel H device 3
            if(Conf2_ChannelH_3_Status)
            {
                SendRF_wait(Conf2_ChannelH_3_ON,2,16,8);
                Conf2_ChannelH_3_Status = OFF;
            }
            else
            {
                SendRF_wait(Conf2_ChannelH_3_OFF,2,16,8);
                Conf2_ChannelH_3_Status = ON;
            }
            break;
        default:
            break;
    }
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
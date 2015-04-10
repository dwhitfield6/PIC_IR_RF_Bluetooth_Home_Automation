/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_Smart_Rf'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains ISR
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

#endif

#include "MISC.h"
#include "UART.h"
#include "IR.h"
#include "Timer.h"
#include "Bluetooth.h"
#include "RF.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/******************************************************************************/
/* High-Priority Interrupt Routine                                            */
/******************************************************************************/

#if defined(__XC) || defined(HI_TECH_C)
void interrupt high_isr(void)
#elif defined (__18CXX)
#pragma code high_isr=0x08
#pragma interrupt high_isr
void high_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif
{
    unsigned char IRpin;
    unsigned int Time0;

    INTCONbits.GIE = OFF; //High priority interrupts

    if(INTCONbits.RBIF)
    {
        NOP();
        NOP();
        IRtimeout = 0;
        IRpin = ReadIRpin();
        if(IRpin != IRpinOLD)
        {
            Timer0ON();
            EnableTimer0Int();
            ReceivingIR = Receiving;
            IR_New_Code = FALSE;
            Time0 = TMR0L;
            Time0 += (TMR0H << 8);
            if(IRrawCodeNum < IR_SIZE)
            {
                if(IRrawCodeNum > 0)
                {
                    IRRawCode[IRrawCodeNum - 1] = Time0;
                    if((Time0 <= PauseBurstupper && Time0 >= PauseBurstlower) ||
                            (Time0 <= Repeatupper && Time0 >= Repeatlower))
                    {
                        /* wait for the timer to timeout and then process the raw code */
                        IRreceiverIntOff();
                        INTCONbits.RBIE = FALSE;
                        /* force timeout */
                        INTCONbits.TMR0IF = ON;
                    }
                    else
                    {
                        ResetTimer0();
                    }
                }
                else
                {
                    ResetTimer0();
                }
                IRrawCodeNum++;
            }
            else
            {
                IRreceiverIntOff();
                INTCONbits.RBIE = FALSE;
            }
            IRpinOLD = IRpin;
        }
        else
        {
            /* This is probably an Error */
            NOP();
        }
        /* We had a change on the RB pin */
        INTCONbits.RBIF = FALSE;
    }
    else if(INTCONbits.TMR0IF)
    {
        /* We had a timeout on the IR receiver */
        DisableTimer0Int();
        Timer0OFF();
        IR_New_Code = IRrawToNEC(IRRawCode, &IR_NEC, TRUE);
        if(!IR_New_Code)
        {
            IRpinOLD = ReadIRpin();
            INTCONbits.RBIF = FALSE;
            IRreceiverIntOn();
            INTCONbits.RBIE = TRUE;
        }
        IRrawCodeNum = 0;
        ReceivingIR = Finished;
        INTCONbits.TMR0IF = FALSE;
    }
    else if(PIR1bits.TMR2IF)
    {
        /* Timer 2 interrupt */
        /* This is used for the RF Code */
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
                        ResetTimer3();
                        PIR2bits.TMR3IF = FALSE;//Clear Flag
                        Timer3_Postscaler = 0;
                        Timer3ON();
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
    else if(PIR2bits.TMR3IF)
    {
        Timer3OFF();
        if(Timer3_Postscaler < RFsendWaitTime)
        {
            ResetTimer3();
            Timer3ON();
            Timer3_Postscaler++;
        }
        else
        {
            /*
             * We waited the specified amount of timer to allow
             *  for another RF send
             */
            Sent = YES;
        }
        PIR2bits.TMR3IF = FALSE;
    }
    else
    {
        /* We got here by error */
        NOP();
    }

    INTCONbits.GIE = ON; //High priority interrupts
}

/******************************************************************************/
/* Low-Priority Interrupt Routine                                             */
/******************************************************************************/
#if defined(__XC) || defined(HI_TECH_C)
void low_priority interrupt low_isr(void)
#elif defined (__18CXX)
#pragma code low_isr=0x18
#pragma interruptlow low_isr
void low_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif
{
        char data = 0;

    INTCONbits.PEIE = OFF; //Low priority interrupts

    if(PIR1bits.RCIF)
    {
        /* Uart Receive interrupt*/
        data =  ReadUSART();

        if(!BlueConnected())
        {
            if(CommandStringPos < RXCommandsize)
            {
                CommandString[CommandStringPos] = data;
                CommandStringPos++;
            }
        }
        else
        {
            if(data != '\r' && data != '\n')
            {
                if((data == 127 || data == 8) && (ReceivedStringPos > 0))
                {
                    /* Backspace of delete */
                    EraseScreen(ReceivedStringPos + 1);
                    ReceivedStringPos--;
                    ReceivedString[ReceivedStringPos] = '\0';
                    UARTchar('>');
                    UARTstring(ReceivedString);
                }
                else if(ReceivedStringPos < RXbufsize)
                {
                    if(!NewReceivedString)
                    {
                        if(IsCharacter(data))
                        {
                            UARTchar(data); // Echo character back to sender
                            ReceivedString[ReceivedStringPos] = data;
                            ReceivedStringPos++;
                        }
                    }
                }
                else
                {
                    cleanBuffer(ReceivedString,RXbufsize);
                    ReceivedStringPos = 0;
                    UARTstring(CRLN);
                    UARTstringCRLN((unsigned char *)"Buffer Overflow");
                }
            }
            else if(ReceivedStringPos > 0 && (NewReceivedString == FALSE))
            {
                PIE1bits.RCIE   = FALSE;   // disable RX interrupts
                UARTstring(CRLN);
                NewReceivedString = TRUE;
            }
        }
        PIR1bits.RCIF = FALSE;
    }
    else if(PIR1bits.TMR1IF)
    {
        Timer1_Postscaler++;
        if(Timer1_Postscaler == LEDPostscaler)
        {
            GreenLEDOFF();
            RedLEDOFF();
            Timer1OFF();
        }
        PIR1bits.TMR1IF = FALSE;
    }
    else
    {
        /* We got here by error */
        NOP();
    }
    INTCONbits.PEIE = ON; //Low priority interrupts

}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
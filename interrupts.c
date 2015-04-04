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
        NOP();
        IRpin = ReadIRpin();
        if(IRpin != IRpinOLD)
        {
            Timer0ON();
            EnableTimer0Int();
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
                        INTCONbits.RBIE = FALSE;
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

    if(INTCONbits.TMR0IF)
    {
        /* We had a timeout on the IR receiver */
        DisableTimer0Int();
        Timer0OFF();
        IR_New_Code = IRrawToNEC(&IRRawCode, &IR_NEC, TRUE);
        if(!IR_New_Code)
        {
            INTCONbits.RBIE = TRUE;
        }
        IRrawCodeNum = 0;
        
        INTCONbits.TMR0IF = FALSE;
    }
    if(PIR1bits.TMR2IF)
    {
        /* Timer 2 interrupt */
        /* This is used for the RF Code */
        Timer2_Postscaler++;
        if(Timer2_Postscaler > 1)
        {
            Timer2_Postscaler = 0;
            if(RFsendCode[RFcodeBit] != EndofRFcode)
            {
                /* In the middle of sending a code */
                if(RFsendFlag)
                {
                    if(RFsendCode[RFcodeBit] == 0)
                    {
                        if(RFcodePlace == 1)
                        {
                            RFon();
                            SetTimer2(Short);
                            RFcodePlace++;
                            ResetTimer2();
                        }
                        else if(RFcodePlace == 2)
                        {
                            RFoff();
                            SetTimer2(Long);
                            RFcodePlace++;
                        }
                        else if(RFcodePlace == 3)
                        {
                            RFon();
                            SetTimer2(Short);
                            RFcodePlace++;
                            ResetTimer2();
                        }
                        else if(RFcodePlace == 4)
                        {
                            RFoff();
                            SetTimer2(Long);
                            RFcodePlace = 1;
                            RFcodeBit++;
                        }
                        else
                        {
                            /* Not supposd to get here */
                            RFcodePlace = 1;
                            RFsendFlag = 0;
                        }
                    }
                    else if(RFsendCode[RFcodeBit] == 1)
                    {
                        if(RFcodePlace == 1)
                        {
                            RFon();
                            SetTimer2(Long);
                            RFcodePlace++;
                        }
                        else if(RFcodePlace == 2)
                        {
                            RFoff();
                            SetTimer2(Short);
                            RFcodePlace++;
                            ResetTimer2();
                        }
                        else if(RFcodePlace == 3)
                        {
                            RFon();
                            SetTimer2(Long);
                            RFcodePlace++;
                        }
                        else if(RFcodePlace == 4)
                        {
                            RFoff();
                            SetTimer2(Short);
                            RFcodePlace = 1;
                            RFcodeBit++;
                            ResetTimer2();
                        }
                        else
                        {
                            /* Not supposd to get here */
                            RFcodePlace = 1;
                            RFsendFlag = 0;
                        }
                    }
                    else
                    {
                        /* The bit is an f (floating)*/
                        if(RFcodePlace == 1)
                        {
                            RFon();
                            SetTimer2(Short);
                            RFcodePlace++;
                            ResetTimer2();
                        }
                        else if(RFcodePlace == 2)
                        {
                            RFoff();
                            SetTimer2(Long);
                            RFcodePlace++;
                        }
                        else if(RFcodePlace == 3)
                        {
                            RFon();
                            SetTimer2(Long);
                            RFcodePlace++;
                        }
                        else if(RFcodePlace == 4)
                        {
                            RFoff();
                            SetTimer2(Short);
                            RFcodePlace = 1;
                            RFcodeBit++;
                            ResetTimer2();
                        }
                        else
                        {
                            /* Not supposd to get here */
                            RFcodePlace = 1;
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
                    RFon();
                    SetTimer2(Short);
                    RFcodePlace++;
                    ResetTimer2();
                }
                else if( RFcodePlace == 2)
                {
                    RFoff();
                    SetTimer2(Sync);
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
        PIR1bits.TMR2IF = FALSE; //Clear Flag
        if(RFsendFlag)
        {
            /* We are still in the middle of sending */
            Timer2ON();
        }
    }
    if(PIR2bits.TMR3IF)
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
            if(IsCharacter(data))
            {
                if(data != '\r' && data != '\n')
                {
                    if(ReceivedStringPos < RXbufsize && NewReceivedString == FALSE)
                    {
                        ReceivedString[ReceivedStringPos] = data;
                        ReceivedStringPos++;
                    }
                }
                else if(ReceivedStringPos > 0)
                {
                    NewReceivedString = TRUE;
                }
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
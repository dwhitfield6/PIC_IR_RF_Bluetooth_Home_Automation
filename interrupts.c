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

    INTCONbits.GIE = OFF; //High priority interrupts
   
    if(PIR1bits.TMR2IF)
    {
        /* Timer 2 interrupt */
        /* 
         * This is used for the RF Code or NEC code depending upon if we
         * are sending an RF signal or IR signal. This is dependant on
         * variable RF_IR
         */
        
        Send_RForIR_bit();
    }
    else if(STKPTRbits.STKFUL)
    {
        /* Stack is full or overflowed */
        UARTstringCRLN_CONST("Error: Stack overflow!!!");
        STKPTRbits.STKFUL = 0;
    }
    else if(STKPTRbits.STKUNF)
    {
        /* Stack underflowed */
        UARTstringCRLN_CONST("Error: Stack underflow!!!");
        STKPTRbits.STKUNF = 0;
    }
    else if(PIR2bits.TMR3IF)
    {
        Timer3OFF();
        if(RFPause == 1)
        {
            if(Timer3_Postscaler < RFsendPauseTime)
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
        }
        else
        {
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
        }
        PIR2bits.TMR3IF = FALSE;
    }
    else
    {
        /* We got here by error */
        UARTstringCRLN_CONST("Error: Unknown High priority interrupt!!!");
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
    unsigned char IRpin;
    unsigned int Time0;

    INTCONbits.PEIE = OFF; //Low priority interrupts

    if(PIR1bits.RCIF)
    {
        /* Uart Receive interrupt*/
        data =  ReadUSART();

        if(!BlueConnected())
        {
            if(CommandStringPos < (RXCommandsize - 1)) //have to make room for null
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
                    UARTchar_CONST('>');
                    UARTstring(ReceivedString);
                }
                else if(ReceivedStringPos < (RXbufsize - 3)) //have to make room for optional equal and null
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
                    PIE1bits.RCIE   = FALSE;
                    BufferOverflow = TRUE;
                }
            }
            else if(ReceivedStringPos > 0 && (NewReceivedString == FALSE))
            {
                PIE1bits.RCIE   = FALSE;   // disable RX interrupts
                UARTstring_CONST(CRLN);
                NewReceivedString = TRUE;
            }
        }
        PIR1bits.RCIF = FALSE;
    }
    else if(PIR1bits.TMR1IF)
    {
        Timer1_Postscaler++;
        if(Timer1_Postscaler >= LEDPostscaler)
        {
            GreenLEDOFF();
            RedLEDOFF();
            Timer1OFF();
            Timer1_Postscaler = 0;
        }
        PIR1bits.TMR1IF = FALSE;
    }
    else if(INTCONbits.TMR0IF)
    {
        /* We had a timeout on the IR receiver */
        DisableTimer0Int();
        Timer0OFF();
        IRstarted = FALSE;
        if(IRrawCodeNum >=  (MinNECFlipsRepeat))
        {
            IR_New_Code = IRrawToNEC(&IR_NEC, TRUE);
        }
        cleanBuffer16bit(IRRawCode,IR_SIZE);
        IRrawCodeNum = 0;
        if(!IR_New_Code)
        {
            RedLEDON();
            delayUS(1000);
            LEDTimerON();
            IRpinOLD = ReadIRpin();
            INTCONbits.RBIF = FALSE;
            IRreceiverIntOn();
        }
        ReceivingIR = Finished;
        INTCONbits.TMR0IF = 0;
    }
    else if(INTCONbits.RBIF)
    {
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
            if((Time0 <= StartbitHIGHupper && Time0 >= StartbitHIGHlower))
            {
                IRstarted = TRUE;
            }
            if(IRstarted)
            {
                if(IRrawCodeNum < IR_SIZE)
                {
                    IRRawCode[IRrawCodeNum] = Time0;
                    IRrawCodeNum++;
                    if((Time0 <= PauseBurstupper && Time0 >= PauseBurstlower))
                    {
                        /* wait for the timer to timeout and then process the raw code */
                        IRreceiverIntOff();
                        /* Create a pause for the timeout */
                        SetTimer0(0x8FFF);
                    }
                    else if(Time0 <= Repeatupper && Time0 >= Repeatlower)
                    {
                        /* wait for the timer to timeout and then process the raw code */
                        IRreceiverIntOff();
                        /* Create a pause for the timeout */
                        SetTimer0(0x8FFF);
                    }
                    else
                    {
                        ResetTimer0();
                    }
                }
                else
                {
                    ResetTimer0();
                    IRreceiverIntOff();
                }
            }
            else
            {
                ResetTimer0();
            }
            IRpinOLD = IRpin;
        }
        else
        {
            /* This is probably an Error */
            NOP();
        }
        /* We had a change on the RB pin */
        ReadIRpin();
        INTCONbits.RBIF = FALSE;
    }
    else
    {
        /* We got here by error */
        UARTstringCRLN_CONST("Error: Unknown Low priority interrupt!!!");
        NOP();
    }
    INTCONbits.PEIE = ON; //Low priority interrupts
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
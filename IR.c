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
/* Contains functions for IR receiver.
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

#include "user.h"          /* User funct/params, such as InitApp */
#include "IR.h"          /* User funct/params, such as InitApp */
#include "Timer.h"          /* User funct/params, such as InitApp */
#include "MISC.h"          /* User funct/params, such as InitApp */
#include "UART.h"          /* User funct/params, such as InitApp */
#include "Button.h"          /* User funct/params, such as InitApp */
#include "EEPROM.h"          /* User funct/params, such as InitApp */
#include "RF.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
unsigned char IRpinOLD;
unsigned int IRRawCode[IR_SIZE];
unsigned long IR_NEC;
unsigned char IRrawCodeNum = 0;
unsigned char IR_New_Code  = 0;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* InitIR
 *
 * The function initializes IR receiver.
/******************************************************************************/
void InitIR(void)
{
    /* Turn on interrupt on pin RB4 */
    IOCBbits.IOCB4 = ON;
    /* Initialize compare value of IR pin */
    IRpinOLD = ReadIRpin();
    /* Timer 0 is used for IR timing */
    InitTimer0();
    /* Clean the code buffers */
    cleanBuffer16bit(IRRawCode, IR_SIZE);
    IR_NEC = 0;
    Timer0OFF();
    /* Reset the timer */
    ResetTimer0();
    /*Clear flag and disable interrupts */
    DisableTimer0Int();
    INTCONbits.TMR0IF = 0;
    /* Enable RB port change on interrupt */
    INTCONbits.RBIF = FALSE;
    INTCONbits.RBIE = TRUE;
}

/******************************************************************************/
/* ReadIRpin
 *
 * The function reads the IR pin and return a 0 if low and 1 if High.
/******************************************************************************/
unsigned char ReadIRpin(void)
{
    //READ RB4
    if((PORTB & IRreceiver) == IRreceiver)
    {
        return (1);
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* IRrawToNEC
 *
 * The function converts from raw counts to NEC code.
/******************************************************************************/
unsigned char IRrawToNEC(unsigned int* Raw, unsigned long* NEC, unsigned char Invert)
{
    /* the Protocol is as follows
     *
     * 1. A 9ms leading pulse burst (16 times the pulse burst length used for
     *   a logical data bit)
     *
     * 2. A 4.5ms space
     *
     * 3. The 8-bit address for the receiving device
     *
     * 4. The 8-bit logical inverse of the address
     *
     * 5. The 8-bit command
     *
     * 6. The 8-bit logical inverse of the command
     *
     * 7. A final 562.5µs pulse burst to signify the end of message transmission.
     *
     * 8. A 40 mS pause signifies the end of entire transmission
     *
     * 9. A 9ms leading pulse burst
     *
     * 10. A 2.25ms space
     *
     * 11.A 562.5µs pulse burst to mark the end of the space
     *  (and hence end of the transmitted repeat code).
     */
    unsigned char i;
    unsigned char first =0;
    unsigned char StartBit=IR_SIZE;
    unsigned long NECtemp = 0;

    /* Start Bit begin*/
    for (i =0; i < IR_SIZE; i++)
    {
        if(Raw[i] >= Repeatlower && Raw[i] <= Repeatupper )
        {
            /* This is the Repeat bit*/
            return Repeat;
        }
        else if(Raw[i] >= StartbitHIGHlower && Raw[i] <= StartbitHIGHupper )
        {
            /* This is the Start bit*/
            StartBit = i;
            break;
        }
    }
    if(IR_SIZE - StartBit < MaxScanEdgeChange)
    {
        /* No start bit found  or found too late in buffer */
        return Error;
    }

    StartBit++;
    if(Raw[StartBit] >= PauseSpacelower && Raw[StartBit] <= PauseSpaceupper )
    {
        /* This is a repeat code most likely */
        if(Raw[StartBit] >= DataShortlower && Raw[StartBit] <= DataShortupper )
        {
            /* This is a repeat code most likely */
            if(Raw[StartBit] >= DataShortlower && Raw[StartBit] <= DataShortupper )
            {
                /* This is a repeat code most likely */
                if(Raw[StartBit] >= Repeatlower && Raw[StartBit] <= Repeatupper)
                {
                    /* This is a repeat code */
                    return Repeat;
                }
            }
        }
        
    }
    else if(Raw[StartBit] < StartbitLOWlower || Raw[StartBit] > StartbitLOWupper )
    {
        /* Start bit is wrong */
        return Error;
    }
    
    StartBit++;
    for(i = StartBit; i < IR_SIZE;i++)
    {
        if(Raw[StartBit] >= DataShortlower && Raw[StartBit] <= DataShortupper )
        {
            if(!first)
            {
                /* This is the beginning of the bit*/
                first = 1;
            }
            else
            {
                /* We just got 2 562.5 uS Pulses in a row */
                first = 0;
                NECtemp <<= 1;
                if(!Invert)
                {
                    NECtemp++;
                }
            }
        }
        else if(Raw[StartBit] >= DataLonglower && Raw[StartBit] <= DataLongupper)
        {
            if(first)
            {
                /* This is a logical Zero */
                NECtemp <<= 1;
                if(Invert)
                {
                    NECtemp++;
                }
                first = 0;
            }
            else
            {
                /* we just got 2 longs in a row which should never happen*/
                return Error;
            }
        }
        else if(Raw[StartBit] == 0 && first == 1)
        {
            /*
             * We got the final 562.5µs pulse burst to signify the end of
             *  message transmission and the rest of the buffer is empty.
             */
            *NEC = NECtemp;
            return New;
        }
        else if(Raw[StartBit] >= PauseBurstlower && Raw[StartBit] <= PauseBurstupper)
        {
            /* Pause bit */
            *NEC = NECtemp;
            return New;
        }
        else if(Raw[StartBit] >= Repeatlower && Raw[StartBit] <= Repeatupper)
        {
            /* repeat character */
            return Repeat;
        }
        else
        {
            return Error;
        }
        StartBit++;
    }
    /* Cant get here but do this to make compiler happy */
    return Error;
}

/******************************************************************************/
/* UseIRCode
 *
 * The function is called after the IR receiver either gets a repeat code or
 *  a new code.
/******************************************************************************/
void UseIRCode(unsigned char* Code, unsigned long NEC)
{
    unsigned char i = 0;

    if(*Code == 2)
    {
        /* Repeat character */
        if(NEC == Global.NECcode)
        {
            SendRF_wait(ChannelF,12,8);
            RedLEDON();
            GreenLEDON();
            LEDTimerON();
        }
    }
    else
    {
        /* New Code */
        if(ReadPushButton())
        {
            Global.NECcode = NEC;
            if(SyncGlobalToEEPROM())
            {
                /* Successful EEPROM burn */
                for(i =0;i<10;i++)
                {
                   GreenLEDON();
                   delayUS(10000);
                   GreenLEDOFF();
                   delayUS(10000);
                }
            }
            else
            {
                /* Unsuccessful EEPROM burn */
                for(i =0;i<10;i++)
                {
                   RedLEDON();
                   delayUS(10000);
                   RedLEDOFF();
                   delayUS(10000);
                }
            }
        }
        else
        {
            if(NEC == Global.NECcode)
            {
                SendRF_wait(ChannelF,12,8);
                GreenLEDON();
                LEDTimerON();
            }
            else
            {
                RedLEDON();
                LEDTimerON();
            }
        }
    }

    *Code = 0;
    INTCONbits.RBIE = TRUE;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/